%{

#include <stdio.h>
#include <string.h>
#include "test.tab.h"
void showError();
%}


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

numbers     ([0-9])+
variable    ^[a-zA-Z_$][a-zA-Z_$0-9]*

%%

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

{variable}	    {sscanf(yytext, "%s", yylval.name); return (STRING);}
{numbers}       {yylval.number = atoi(yytext); return (NUM);}
";"      		{return (SEMICOLON);}
.      		    {showError(); return(OTHER);}
%%


void showError(){
    printf("Other input");
}
int yywrap(){
    return 1;
}