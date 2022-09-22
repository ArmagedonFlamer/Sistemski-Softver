#include <iostream>
#include <fstream>
#include <unistd.h>

#include "/home/ss/resenje/inc/emulator.h"

using namespace std;

uint32_t hex2int(const char *hex) {
    uint32_t val = 0;
    while (*hex) {
        // get current character then increment
        uint8_t byte = *hex++; 
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
        // shift 4 to make space for new digit, and add the 4 bits of the new digit 
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

int getch() {
    int r;
    unsigned char c; 
    if ((r = read(0, &c, sizeof(c))) > 0) 
      return c;   
    return 0;
    }  

int main(int argc, char* argv[])
{
    Emulator *e = new Emulator();
    string input=*new string(argv[1]);
    std::ifstream file(input);
    std::string str; 
     struct termios new_term,old_term;
    tcgetattr(0, &old_term);

    new_term=old_term;
   // new_term.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
   // new_term.c_iflag = IGNPAR | ICRNL;
   // new_term.c_oflag = 0;
    new_term.c_lflag &= ~ICANON;
    new_term.c_lflag &= ~ECHO;
    new_term.c_cc[VMIN]=0;
    new_term.c_cc[VTIME]=0;
    tcsetattr(0, 0, &new_term);
    while (std::getline(file, str))
    {
        string loc_s= str.substr(0,4);
        str.erase(0,6);
        int location=hex2int(loc_s.c_str());
        e->set_memory(location,str);
    }
    file.close();
    
    e->set_start_pc();
    int ins=0;
    unsigned short c;
    while(!e->get_halted())
    {
        e->execute_instruction();  
        if (e->get_term_out()!=0)
            {
                printf("%c",e->get_term_out());
                fflush(stdout);
                e->set_term_out(0);
            }    
        if ((c=getch())!=0)
            e->set_term_in(c);      

        if ((!((e->get_psw()) & 0x8000))&&(e->get_interrupts()!=0))
            e->serve_interrupts();
        ++ins;
    }   
    tcsetattr(0, 0, &old_term);
    return 0;
}
