#include "/home/ss/resenje/inc/emulator.h"
#include <stdio.h>
#include <iostream>
#include <stdio.h>

using namespace std;
void disable_echo();            //Disables echo for stdin
void enable_echo();             //Enable echo for stdin

unsigned short hex2char(const char *hex) {
    unsigned short val = 0;
    uint8_t byte = *hex++; 
    // transform hex character to the 4bit equivalent number, using the ascii table indexes
    if (byte >= '0' && byte <= '9') byte = byte - '0';
    else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
    else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
    // shift 4 to make r[6]ace for new digit, and add the 4 bits of the new digit 
    val = byte & 0xF;
    byte = *hex++; 
    // transform hex character to the 4bit equivalent number, using the ascii table indexes
    if (byte >= '0' && byte <= '9') byte = byte - '0';
    else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
    else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
    // shift 4 to make r[6]ace for new digit, and add the 4 bits of the new digit 
    val = (val << 4) | (byte & 0xF);
    return val;
}


Emulator::Emulator()
{
    mem = new unsigned char[0x10000];
    psw=0x2000;
    halted=false;
}

bool Emulator::get_halted()
{
    return halted;
}

void Emulator::set_memory(int location, string values)
{
    int i=0;
    int pos;
    while((pos=values.find(" "))!=string::npos)
    {
        string byte = values.substr(0,pos);
        values.erase(0,pos+1);
        //printf("%d ",(unsigned short)hex2char(byte.c_str()));
        mem[location+i]=hex2char(byte.c_str());
        ++i;
    }
   // cout<<endl;
}

void Emulator::set_start_pc()
{
    t.set_timer();
    r[7]=(mem[0]<<8) | mem[1];
}

unsigned short Emulator::get_psw()
{
    return psw;
}

char Emulator::get_interrupts()
{
    return interrupts;
}
unsigned short Emulator::get_term_out()
{
    unsigned short ret_val=(mem[term_out]<<8)|mem[term_out+1];
    return ret_val;
}

void Emulator::set_term_out(unsigned short val)
{
    mem[term_out]= (val&0xFF00)>>8;
    mem[term_out+1] = val&0x00FF;
}

void Emulator::set_term_in(unsigned short val)
{
   // val-=48;
    mem[term_in]= (val&0xFF00)>>8;
    mem[term_in+1] = val&0x00FF;
    interrupts|=0x0008;
}

void Emulator::serve_interrupts()
{
    char tmp = interrupts;
    char mask = 1;
    for (int i=0;i<8;++i)
    {
        if ((tmp & mask)!=0)
        {
            push(r[7]);
            push(psw);
            setFlag(PSW_I);
            r[7]=((mem[(i<<1)]<<8) | mem[(i<<1)+1]);
            interrupts &= ~(1<<i);
            if(i==2)
                t.set_timer();
        }
        mask<<=1;
    }
}
// 3 4
// r[7] <= start_Addr[3]
// r[7] <= start_Addr[4]

//executes 1 instruction from mem[r[7]]
//defaults=wrong op= send intr
char Emulator::execute_instruction()
{
    char instruction = *(mem+r[7]);
    r[7] = r[7] +1;
    switch(instruction&0xF0)
    {
        //halt
        case 0b00000000: execute_halt(); break;
        //int regD
        case 0b00010000: execute_int(); break;
        //iret
        case 0b00100000: execute_iret(); break;
        //call operand
        case 0b00110000: execute_call(); break;
        //ret
        case 0b01000000: execute_ret(); break;
        //jmps
        case 0b01010000: 
            switch(instruction & 0x0F)//izvlacenje poslednja 4 bita
            {
                //jmp
                case 0b00000000: execute_jmp(); break;
                //jeq
                case 0b00000001: execute_jeq(); break;
                //jne
                case 0b00000010: execute_jne(); break;
                //jgt
                case 0b00000011: execute_jgt(); break;
                default: break;
            }
            break;
        //xchg
        case 0b01100000: execute_xchg(); break;
        //arithmetic
        case 0b01110000: 
            switch(instruction & 0x0F)//izvlacenje poslednja 4 bita
            {
                //add
                case 0b00000000: execute_add(); break;
                //sub
                case 0b00000001: execute_sub(); break;
                //mul
                case 0b00000010: execute_mul(); break;
                //div
                case 0b00000011: execute_div(); break;
                //cmp
                case 0b00000100: execute_cmp(); break;
                default: break;
            }
            break;
        //logic
        case 0b10000000: 
            switch(instruction & 0x0F)//izvlacenje poslednja 4 bita
            {
                //not
                case 0b00000000: execute_not(); break;
                //and
                case 0b00000001: execute_and(); break;
                //or
                case 0b00000010: execute_or(); break;
                //xor
                case 0b00000011: execute_xor(); break;
                //test
                case 0b00000100: execute_test(); break;
                default: break;
            }
            break;
        //shift
        case 0b10010000:
            switch(instruction & 0x0F)//izvlacenje poslednja 4 bita
            {
                //shl
                case 0b00000000: execute_shl(); break;
                //shr
                case 0b00000001: execute_shr(); break;
                default: break;
            }
            break;
        //load 
        case 0b10100000: execute_ldr(); break;
        //store
        case 0b10110000: execute_str(); break;
        default: break;
    }
   // printf("%02X",r[7]);
    if(t.getTimePassed()>timer_len[mem[0xFF11]]*100)
       interrupts|=0x0004;
} 

unsigned short Emulator::formAddress(char reg)
{
    char add_mode = (mem[r[7]]&higher_nib)>>4;
    char mem_mode = mem[r[7]++]&lower_nib;
    unsigned short retaddr;
    switch(mem_mode)
    {
        case Imm:   retaddr= r[7]; r[7]+=2; return retaddr; // citamo prvo DataHigh pa DataLow i postavljamo r[7] da pokazuje na sledecu lokaciju
        case RegInd: 
            switch(add_mode)
            {
                case 0x00:  return r[reg]; break;
                case 0x01:  r[reg]-=2; return r[reg]; break;
                case 0x02:  r[reg]+=2; return r[reg]; break;
                case 0x03:  
                    retaddr= mem[r[reg]<8 | mem[r[reg]+1]];
                    r[reg]-=2;
                    return retaddr; break;
                case 0x04:
                    retaddr = mem[r[reg]]<<8 | mem[r[reg]+1];
                    r[reg]+=2;
                    return retaddr; break;
                default:
                    printf("Wrong register update mode on r[7]: %i, r[6]: %i, PSW: %i",r[7],r[6],psw);
                    exit(0);
            }
        case RegIndPom: 
            switch(add_mode)
                {
                    case 0x00:  return (r[reg] + (mem[r[7]++]<<8 | mem[r[7]++])); break;
                    case 0x01:  r[reg]-=2; return (r[reg] + (mem[r[7]++]<<8 | mem[r[7]++]));break;
                    case 0x02:  r[reg]+=2; return (r[reg] + (mem[r[7]++]<<8 | mem[r[7]++]));break;
                    case 0x03:  
                        retaddr = (r[reg] + (mem[r[7]++]<<8 | mem[r[7]++]));
                        r[reg]-=2;
                        return retaddr;break;
                    case 0x04:
                        retaddr = (r[reg] + (mem[r[7]++]<<8 | mem[r[7]++]));
                        r[reg]+=2;
                        return  retaddr;break;
                    default:
                        printf("Wrong register update mode on r[7]: %i, r[6]: %i, PSW: %i",r[7],r[6],psw);
                        exit(0);
                }
        case Mem:   r[7]+=2;
                    return ((mem[r[7]-2]<<8) | mem[r[7]-1]);
        default:
                printf("Wrong address mode on r[7]: %i, r[6]: %i, PSW: %i",r[7],r[6],psw);
                return 0;
    }
}


short Emulator::formValue(char reg)
{
    char add_mode = (mem[r[7]]&higher_nib)>>4;
    char mem_mode = mem[r[7]++]&lower_nib;
    short retval;
    switch(mem_mode)
    {
        case Imm: return mem[r[7]++]<<8 | mem[r[7]++]; // citamo prvo DataHigh pa DataLow i postavljamo r[7] da pokazuje na sledecu lokaciju
        case RegDir: return r[reg];
        case RegInd: 
            switch(add_mode )
            {
                case 0x00:  return mem[r[reg]]<<8 | mem[r[reg]+1];
                case 0x01:  r[reg]-=2; return mem[r[reg]]<<8 | mem[r[reg]+1];
                case 0x02:  r[reg]+=2; return mem[r[reg]]<<8 | mem[r[reg]+1];
                case 0x03:  
                    retval = mem[r[reg]]<<8 | mem[r[reg]+1];
                    r[reg]-=2;
                    return retval;
                case 0x04:
                    retval = mem[r[reg]]<<8 | mem[r[reg]+1];
                    r[reg]+=2;
                    return retval;
                default:
                    printf("Wrong register update mode on r[7]: %i, r[6]: %i, PSW: %i",r[7],r[6],psw);
                    return 0;

            }
        case RegIndPom: 
            switch(add_mode)
                {
                    case 0x00:  r[7]+=2;
                                return mem[r[reg]+ (mem[r[7]-2]<<8 | mem[r[7]-1])]<<8|mem[r[reg]+ (mem[r[7]-2]<<8 | mem[r[7]-1])+1];
                    case 0x01:  r[reg]-=2; return mem[r[reg]+ (mem[r[7]++]<<8 | mem[r[7]++])]<<8|mem[r[reg]+ (mem[r[7]++]<<8 | mem[r[7]++])+1];
                    case 0x02:  r[reg]+=2; return mem[r[reg]+ (mem[r[7]++]<<8 | mem[r[7]++])]<<8|mem[r[reg]+ (mem[r[7]++]<<8 | mem[r[7]++])+1];
                    case 0x03:  
                        retval = mem[r[reg]+ (mem[r[7]++]<<8 | mem[r[7]++])]<<8|mem[r[reg]+ (mem[r[7]++]<<8 | mem[r[7]++])+1];
                        r[reg]-=2;
                        return retval;
                    case 0x04:
                        retval = mem[r[reg]+ (mem[r[7]++]<<8 | mem[r[7]++])]<<8|mem[r[reg]+ (mem[r[7]++]<<8 | mem[r[7]++])+1];
                        r[reg]+=2;
                        return retval;
                    default:
                        printf("Wrong register update mode on r[7]: %i, r[6]: %i, PSW: %i",r[7],r[6],psw);
                        return 0;

                }
        case Mem:   r[7]+=2; 
                    return ((short)(mem[mem[r[7]-2]<<8 | mem[r[7]-1]]<<8) | mem[mem[r[7]-2]<<8 | mem[r[7]-1]+1]);
        case RegDirPom: r[7]+=2;
                        return (short)(mem[r[reg] + ((short)(mem[r[7]-2]<<8 | mem[r[7]-1]))]<<8)|mem[r[reg] + ((short)(mem[r[7]-2]<<8 | mem[r[7]-1]))+1];
        default:
                printf("Wrong address mode on r[7]: %i, r[6]: %i, PSW: %i",r[7],r[6],psw);
                return 0;
    }
}

//TO DO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1111
void Emulator::execute_halt()
{
    cout<<"------------------------------------------------\nEmulated processor executed halt instruction\nEmulated processor state: psw=0b";
    unsigned short tmp = psw;
    unsigned short mask = 0x8000;
    for (int i=0;i<16;++i)
    {
        cout<<(((tmp&mask)>0)?1:0);
        tmp<<=1;
    }
    printf(" r0=%#06x r1=%#06x r2=%#06x r3=%#06x r4=%#06x r5=%#06x r6=%#06x r7=%#06x\n", r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
    halted=true;
}

void Emulator::execute_int()
{
    char regD = mem[r[7]++] >> 4; //struktura regD : DDDD 1111, gde je DDDD broj interapta koji se poziva
    if (r[regD]>8) 
        {
            printf("IVT No > 8. Exiting. r[7]: %i, r[6]: %i, PSW: %i",r[7],r[6],psw);
            interrupts |= 1 << regD;  // STAVITI DA SE Ir[6]ISUJE NA TERMINAL I PRIJAVITI GRESK\UA, POZVATI INTERAPT 1
        }
    else
        {
            //interrupts |= (1 << r[regD]); 
            push(r[7]);
            push(psw);
            r[7]=((mem[(r[regD]<<1)]<<8) | mem[(r[regD]<<1)+1]);   
        }
}

void Emulator::execute_iret()
{
    psw = pop();
    r[7] = pop();
}

void Emulator::execute_call()
{
    char regD = mem[r[7]++] & lower_nib;
    unsigned short operand;
    char add_mode = (mem[r[7]]&higher_nib)>>4;
    char mem_mode = mem[r[7]]&lower_nib;
    if (mem_mode==RegDir)
        {
            operand= r[regD];
            ++r[7];
            push(r[7]);
            r[7]=operand;
            return;
        }
    else
        if (mem_mode==RegDirPom)
        { 
            r[7]+=3; 
            operand = r[regD] + ((short)(mem[r[7]-2]<<8 | mem[r[7]-1]));
            push(r[7]);
            r[7]=operand;
            return;
        }
        else
            operand = formAddress(regD);
    push(r[7]);
    r[7]=getWord(operand);
}

void Emulator::execute_ret()
{
    r[7] = pop();
}

void Emulator::execute_jmp()
{
    char regD = mem[r[7]++] & lower_nib;
    unsigned short operand;
    char add_mode = (mem[r[7]]&higher_nib)>>4;
    char mem_mode = mem[r[7]]&lower_nib;
        if (mem_mode==RegDir)
        {
            operand= r[regD];
            ++r[7];
            r[7]=operand;
            return;
        }
    else
        if (mem_mode==RegDirPom)
        { 
            r[7]+=3; 
            operand = r[regD] + ((short)(mem[r[7]-2]<<8 | mem[r[7]-1]));
            r[7]=operand;
            return;
        }
        else
            operand = formAddress(regD);
    r[7]=getWord(operand);
}

void Emulator::execute_jeq()
{
    char regD = mem[r[7]++] & lower_nib;
    unsigned short operand;
    char add_mode = (mem[r[7]]&higher_nib)>>4;
    char mem_mode = mem[r[7]]&lower_nib;
    if (mem_mode==RegDir)
        {
            operand= r[regD];
            ++r[7];
            if (getFlag(PSW_Z))
                r[7]=operand;
            return;
        }
    else
        if (mem_mode==RegDirPom)
        { 
            r[7]+=3; 
            operand = r[regD] + ((short)(mem[r[7]-2]<<8 | mem[r[7]-1]));
            if (getFlag(PSW_Z))
                r[7]=operand;
            return;
        }
        else
            operand = formAddress(regD);
    if (getFlag(PSW_Z))
        r[7]=getWord(operand);
}

void Emulator::execute_jne()
{
    char regD = mem[r[7]++] & lower_nib;
    unsigned short operand;
    char add_mode = (mem[r[7]]&higher_nib)>>4;
    char mem_mode = mem[r[7]]&lower_nib;
    if (mem_mode==RegDir)
        {
            operand= r[regD];
            ++r[7];
            if (!getFlag(PSW_Z))
                r[7]=operand;
            return;
        }
    else
        if (mem_mode==RegDirPom)
        { 
            r[7]+=3; 
            operand = r[regD] + ((short)(mem[r[7]-2]<<8 | mem[r[7]-1]));
            if (!getFlag(PSW_Z))
                r[7]=operand;
            return;
        }
        else
            operand = formAddress(regD);
    if (!getFlag(PSW_Z))
        r[7]=getWord(operand);
}

void Emulator::execute_jgt()
{
    char regD = mem[r[7]++] & lower_nib;
    unsigned short operand;
    char add_mode = (mem[r[7]]&higher_nib)>>4;
    char mem_mode = mem[r[7]]&lower_nib;
        if (mem_mode==RegDir)
        {
            operand= r[regD];
            ++r[7];
            if (getFlag(PSW_Z) && getFlag(PSW_N)==getFlag(PSW_O))
                r[7]=operand;
            return;
        }
    else
        if (mem_mode==RegDirPom)
        { 
            r[7]+=3; 
            operand = r[regD] + ((short)(mem[r[7]-2]<<8 | mem[r[7]-1]));
            if (getFlag(PSW_Z) && getFlag(PSW_N)==getFlag(PSW_O))
                r[7]=operand;
            return;
        }
        else
            operand = formAddress(regD);
    if (getFlag(PSW_Z) && getFlag(PSW_N)==getFlag(PSW_O))
        r[7]=getWord(operand);
}

void Emulator::execute_xchg()
{
    short tmp = r[mem[r[7]] & higher_nib];
    r[mem[r[7]] & higher_nib] = r[mem[r[7]] & lower_nib];
    r[mem[r[7]] & lower_nib] = tmp;
    r[7]++;
}

void Emulator::execute_add()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] += r[regs & lower_nib];
}

void Emulator::execute_sub()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] -= r[regs & lower_nib];
}

void Emulator::execute_mul()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] *= r[regs & lower_nib];
}

void Emulator::execute_div()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] /= r[regs & lower_nib];
}

void Emulator::execute_cmp()
{
    char regs = mem[r[7]++];
    short tmp = r[(regs & higher_nib)>>4] - r[regs & lower_nib];
    if (tmp<0)
        setFlag(PSW_N);
    else 
        if (tmp==0)
            setFlag(PSW_Z);
            else
                resetFlag(PSW_N);
    if (r[(regs & higher_nib)>>4] < r[regs & lower_nib])
        setFlag(PSW_C);
    else
        resetFlag(PSW_C);

    short a = r[(regs & higher_nib)>>4];
    short b = r[regs & lower_nib];
    if ((a > 0 && b < 0 && (a - b) < 0) || (a < 0 && b > 0 && (a - b) > 0))
        setFlag(PSW_O);
    else
        resetFlag(PSW_O);

}

void Emulator::execute_not()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] = ~r[(regs & higher_nib)>>4];
}

void Emulator::execute_and()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] &= r[regs & lower_nib];
}

void Emulator::execute_or()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] |= r[regs & lower_nib];
}

void Emulator::execute_xor()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] ^= r[regs & lower_nib];
}

void Emulator::execute_test()
{
    char regs = mem[r[7]++];
    short tmp = r[(regs & higher_nib)>>4] & r[regs & lower_nib];
    if (tmp<0)
        setFlag(PSW_N);
    else
    resetFlag(PSW_N);
    if (tmp==0)
        setFlag(PSW_Z);
    else
    resetFlag(PSW_Z);
}

void Emulator::execute_shl()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] <<= r[regs & lower_nib];
}

void Emulator::execute_shr()
{
    char regs = mem[r[7]++];
    r[(regs & higher_nib)>>4] >>= r[regs & lower_nib];
}

void Emulator::execute_ldr()
{
    char regD = (mem[r[7]] & higher_nib)>>4;
    char regS = mem[r[7]++] & lower_nib;
    unsigned short operand = formValue(regS);
    r[regD]=operand;
}

void Emulator::execute_str()
{
    char regD = (mem[r[7]] & higher_nib)>>4;
    char regS = mem[r[7]++] & lower_nib;
    unsigned short operand;
    char add_mode = (mem[r[7]]&higher_nib)>>4;
    char mem_mode = mem[r[7]]&lower_nib;
    if (mem_mode==RegDir)
        {
            operand= r[regS];
            ++r[7];
        }
    else
        if (mem_mode==RegDirPom)
        { 
            r[7]+=3; 
            operand = r[regS] + ((short)(mem[r[7]-2]<<8 | mem[r[7]-1]));
        }
        else
            operand = formAddress(regS);
    setWord(operand,r[regD]);
}



//r[6] pokazuje na poslednju zauzetu lokaciju
//pomeram se na sledecu slobodnu (stek raste ka nizim lokacijama), pozivam setWord
// i umanjujem stek za jos 1 kako bi sad ponovo pokazivao na slobodni lokaciju
void Emulator::push(unsigned short value) 
{
    r[6]-=2;
    setWord(r[6],value);
 //   printf("push %d on %d\n", value, r[6]);
}

unsigned short Emulator::pop()
{
    unsigned short tmp = getWord(r[6]);
 //   printf("pop %d from %d\n", tmp, r[6]);
    r[6]+=2;
    return tmp;
}

unsigned short Emulator::getWord(unsigned short address)
{
    return (mem[address]<<8)|((unsigned char)mem[address+1]);
}

void Emulator::setWord(unsigned short address, unsigned short value)
{
    mem[address]= (value & 0xFF00)>>8;
    mem[address+1] = value & 0x00FF;
}
