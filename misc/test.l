%{

#include <stdio.h>
#include <string.h>
#include "test.tab.h"
void showError();
int hextoi(char*);
%}

EQU     (\.equ)
GLOB    (\.global)
SECTION (\.section)
WORD    (\.word)    
END     (\.end)
ASCII   \.ascii
SKIP    (\.skip)
EXTERN  (\.extern)
HALT    (halt)
INT     (int)
IRET    (iret)
CALL    (call)
RET     (ret)
JMP     (jmp)
JEQ     (jeq)
JNE     (jne)
JGT     (jgt)
XCHG    (xchg)
ADD     (add)
SUB     (sub)
MUL     (mul)
DIV     (div)
CMP     (cmp)
NOT     (not)
AND     (and)
OR      (or)
XOR     (xor)
TEST    (test)
SHL     (shl)
SHR     (shr)
LDR     (ldr)
STR     (str)
PUSH    (push)
POP     (pop)
REG     ((R|r)[0-7]|pc|PC|sp|SP)
PSW     (psw|PSW)
numbers     ([0-9])+|(0[xX][0-9a-fA-F]+)
variable    [a-zA-Z_.][a-zA-Z_\-$0-9]*
COMMENT     #.*
LABEL       [a-zA-Z_.][a-zA-Z_\-$0-9]*:
STRING      \s
%%

{EQU}       {return (EQU);}
{GLOB}      {return (GLOB);}
{SECTION}   {return (SECTION);}
{WORD}      {return (WORD);}
{END}       {return (END);}
\.ascii     {printf("Ascii");return (ASCII);}
{SKIP}      {return (SKIP);}
{EXTERN}    {return (EXTERN);}
{HALT}      {yylval.number=0; return (HALT); }
{INT}       {yylval.number=0b00010000; return (INT); }//int 
{IRET}      {yylval.number=0b00100000; return (IRET); }//iret
{CALL}      {yylval.number=0b00110000; return (CALL); }//call operand
{RET}       {yylval.number=0b01000000; return (RET); }//ret
{JMP}       {yylval.number=0b01010000; return (JMP); }
{JEQ}       {yylval.number=0b01010001; return (JEQ); }
{JNE}       {yylval.number=0b01010010; return (JNE); }
{JGT}       {yylval.number=0b01010011; return (JGT); }
{XCHG}      {yylval.number=0b01100000; return (XCHG); }//xchg
{ADD}       {yylval.number=0b01110000; return (ADD); }//add
{SUB}       {yylval.number=0b01110001; return (SUB); }//sub
{MUL}       {yylval.number=0b01110010; return (MUL); }//mul
{DIV}       {yylval.number=0b01110011; return (DIV); }//div
{CMP}       {yylval.number=0b01110100; return (CMP); }//cmp
{NOT}       {yylval.number=0b10000000; return (NOT); }//not
{AND}       {yylval.number=0b10000001; return (AND); }//and
{OR}        {yylval.number=0b10000010; return (OR); }//or
{XOR}       {yylval.number=0b10000011; return (XOR); }//xor
{TEST}      {yylval.number=0b10000100; return (TEST); }//test
{SHL}       {yylval.number=0b10010000; return (SHL); }//shl
{SHR}       {yylval.number=0b10010001; return (SHR); }//shr
{LDR}       {yylval.number=0b10100000; return (LDR); }//load 
{STR}       {yylval.number=0b10110000; return (STR); }//store
{PUSH}      {yylval.number=0b11111110; return (PUSH); }//push
{POP}       {yylval.number=0b11111111; return (POP); }//pop
{COMMENT}   {return (COMMENT);}
{REG}       {
    if ((yytext[0]=='R')||(yytext[0]=='r')) {yylval.number= atoi(&yytext[1]); return (REG);}
    if ((yytext[0]=='p')||(yytext[0]=='P')) {yylval.number= 7; return (REG);}
    if ((yytext[0]=='S')||(yytext[0]=='S')) {yylval.number= 6; return (REG);}
}
{PSW}           {yylval.number=9; return (PSW);} //BINARNO SE PSW GADJA PREKO 9
{variable}	    {sscanf(yytext, "%s", yylval.name); return (VARIABLE);}
{numbers}       {yylval.number = hextoi(yytext); return (NUM);}
{LABEL}         {sscanf(yytext, "%s", yylval.name); return (LABEL);}
"\n"            {return (ENTER);}
";"      		{return (SEMICOLON);}
":"             {return (COLON);}
","             {return (COMMA);}
"$"             {return (DOLLAR);}
"["             {return (L_BRACKET);}
"]"             {return (R_BRACKET);}
"+"             {return (PLUS);}
"-"             {return (MINUS);}
"%"             {return (PERCENT);}
"*"             {return (ASTERISK);}
"\""            {printf("\"");return (QUOTATION);}
{STRING}        {sscanf(yytext, "%s", yylval.name); printf("STRING:%s",yylval.name);return (STRING);}
.               {}
%%


void showError(){
    printf("Other input");
}
int yywrap(){
    return 1;
}

int hextoi(char * c)
{
    int result = 0;
    int i=2;
    if (c[1]=='x')
        while(c[i]!='\0')
        {
            result<<=4;
            if (c[i]-48<10)
                result += c[i]-48;
            else
                if (c[i]-65<10)
                    result += c[i]-65 + 10;
                else
                    result += c[i]-97+10;
            ++i;
        }
    else
        result = atoi(c);
    return result;
}