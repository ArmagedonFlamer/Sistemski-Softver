
%{
#include <stdio.h>

int yylex();
int yyerror(char *s);
extern FILE * yyin;
%}

%token VARIABLE NUM HALT INT IRET CALL RET JMP JEQ JNE JGT XCHG ADD SUB MUL DIV CMP NOT AND OR XOR TEST SHL SHR LDR STR OTHER SEMICOLON SPACE ENTER COLON
		PUSH POP

%type <name> VARIABLE
%type <number> NUM
%type <number> HALT 
%type <number> INT  
%type <number> IRET 
%type <number> CALL 
%type <number> RET  
%type <number> JMP  
%type <number> JEQ  
%type <number> JNE  
%type <number> JGT  
%type <number> XCHG 
%type <number> ADD  
%type <number> SUB  
%type <number> MUL  
%type <number> DIV  
%type <number> CMP  
%type <number> NOT  
%type <number> AND  
%type <number> OR   
%type <number> XOR  
%type <number> TEST 
%type <number> SHL  
%type <number> SHR  
%type <number> LDR  
%type <number> STR  
%type <number> PUSH  
%type <number> POP  
%type <number> OTHER

%union{
	char name[20];
    int number;
}

%%

prog:
  stmts
;

stmts:
		| stmt stmts


stmt:

HALT    	{printf("%d", $1);}
| INT     	{printf("%d", $1);}
| IRET    	{printf("%d", $1);}
| CALL    	{printf("%d", $1);}
| RET     	{printf("%d", $1);}
| JMP     	{printf("%d", $1);}
| JEQ     	{printf("%d", $1);}
| JNE     	{printf("%d", $1);}
| JGT     	{printf("%d", $1);}
| XCHG    	{printf("%d", $1);}
| ADD     	{printf("%d", $1);}
| SUB     	{printf("%d", $1);}
| MUL     	{printf("%d", $1);}
| DIV     	{printf("%d", $1);}
| CMP     	{printf("%d", $1);}
| NOT     	{printf("%d", $1);}
| AND    	{printf("%d", $1);}
| OR     	{printf("%d", $1);}
| XOR     	{printf("%d", $1);}
| TEST    	{printf("%d", $1);}
| SHL     	{printf("%d", $1);}
| SHR     	{printf("%d", $1);}
| LDR     	{printf("%d", $1);}
| STR     	{printf("%d", $1);}
| PUSH		{printf("%d", $1);}
| POP  		{printf("%d", $1);}
| ENTER		{printf("\n");}
| SPACE		{printf(" ");}
| COLON		{printf(":");}
| VARIABLE	{printf("%s", $1);}
| NUM  		{printf("%d", $1);}
| OTHER

;

%%

int yyerror(char *s)
{
	printf("Syntax Error on line %s\n", s);
	return 0;
}

int main()
{
  // open a file handle to a particular file:
  FILE *myfile = fopen("./test.txt", "r");
  // make sure it's valid:
  if (!myfile) {
    printf("I can't open the file!");
    return -1;
  }
  // set lex to read from it instead of defaulting to STDIN:
  yyin = myfile;
  
  // lex through the input:
  while(yyparse());
  fclose(myfile);
}

