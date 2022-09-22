#ifndef _emulator_h_
#define _emulator_h_

#include <termios.h>
#include <string>
#include <chrono>

using namespace std;
#define term_out    0XFF00
#define term_in     0XFF02
#define tim_cfg     0XFF10
#define MEM_SIZE    0x10000

#define Imm         0x00
#define RegDir      0x01
#define RegInd      0x02
#define RegIndPom   0x03
#define Mem         0x04
#define RegDirPom   0x05

#define lower_nib   0x0F
#define higher_nib  0xF0
#define lower_word  0x00FF
#define upper_word  0xFF00

#define PSW_I   0x8000
#define PSW_Tl  0x4000
#define PSW_Tr  0x2000
#define PSW_Z   0x0001
#define PSW_O   0x0002
#define PSW_N   0x0008
#define PSW_C   0x0004


class timer
{
    // alias our types for simplicity
    using clock             = std::chrono::system_clock;
    using time_point_type   = std::chrono::time_point < clock, std::chrono::milliseconds > ;
public:
    // default constructor that stores the start time
    void set_timer()
    {
        start = std::chrono::time_point_cast<std::chrono::milliseconds>(clock::now());
    }

    // gets the time elapsed from construction.
    long /*milliseconds*/ getTimePassed()
    {
        // get the new time
        auto end = clock::now();

        // return the difference of the times
        return (end - start).count()/1000000;
    }

private:
    time_point_type start;
};

class Emulator{

    public:
        void disable_echo();            //Disables echo for stdin
        void enable_echo();             //Enable echo for stdin

        Emulator();
        void set_memory(int, string);
        void set_start_pc();

        char execute_instruction();     //executes 1 instruction from mem[pc]. pc points to the next instruction to be executed
        void execute_halt();
        void execute_int();
        void execute_iret();
        void execute_call();
        void execute_ret();
        void execute_jmp();
        void execute_jeq();
        void execute_jne();
        void execute_jgt();
        void execute_xchg();
        void execute_add();
        void execute_sub();
        void execute_mul();
        void execute_div();
        void execute_cmp();
        void execute_not();
        void execute_and();
        void execute_or();
        void execute_xor();
        void execute_test();
        void execute_shl();
        void execute_shr();
        void execute_ldr();
        void execute_str();

        bool getFlag(unsigned short flag)   { return psw & flag;}
        void setFlag(unsigned short flag)   { psw |= flag;}
        void resetFlag(unsigned short flag) { psw &= ~flag;}

        unsigned short formAddress(char);
        short formValue(char);

        void push(unsigned short);
        unsigned short pop ();

        unsigned short getWord(unsigned short address);
        void setWord(unsigned short address, unsigned short value);

        unsigned short get_psw();
        unsigned short get_term_out();
        void set_term_out(unsigned short);
        void set_term_in(unsigned short);
        bool get_halted();
        char get_interrupts();
        void start_clock();

        void serve_interrupts();
    private:
        bool halted;
        timer t;
        unsigned char* mem;    //ZAUZETI MEMOTIJU U KONSTRUKTORU i inicijalizovati tim_cfg
        unsigned short r[8];
        unsigned short psw;
        char interrupts;//NAPISATI RAZDVAJANJE INTERAPTA
        short timer_len[8]={5,10,15,20,50,100,300,6000}; //timer tick lengths depending on tim_cfg
};

#endif