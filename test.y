
%{
#include <stdio.h>

int yylex();
int yyerror(char *s);

%}

%token STRING NUM HALT INT IRET CALL RET JMP JEQ JNE JGT XCHG ADD SUB MUL DIV CMP NOT AND OR XOR TEST SHL SHR LDR STR OTHER SEMICOLON

%type <name> STRING
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
		| stmt SEMICOLON stmts

stmt:
	STRING {
				printf("Your entered a string - %s", $1);
		}
		| NUM {
				printf("The number you entered is - %d", $1);
		}
		| HALT    {	printf("UNEO SI HALT");}
		| INT     {	printf("UNEO SI INT");}
		| IRET    {	printf("UNEO SI IRET");}
		| CALL    {	printf("UNEO SI CALL");}
		| RET     {	printf("UNEO SI RET");}
		| JMP     {	printf("UNEO SI %d", $1);}
		| JEQ     {	printf("UNEO SI JEQ");}
		| JNE     {	printf("UNEO SI JNE");}
		| JGT     {	printf("UNEO SI JGT");}
		| XCHG    {	printf("UNEO SI XCHG");}
		| ADD     {	printf("UNEO SI ADD");}
		| SUB     {	printf("UNEO SI SUB");}
		| MUL     {	printf("UNEO SI MUL");}
		| DIV     {	printf("UNEO SI DIV");}
		| CMP     {	printf("UNEO SI CMP");}
		| NOT     {	printf("UNEO SI NOT");}
		| AND     {	printf("UNEO SI AND");}
		| OR      {	printf("UNEO SI OR");}
		| XOR     {	printf("UNEO SI XOR");}
		| TEST    {	printf("UNEO SI TEST");}
		| SHL     {	printf("UNEO SI SHL");}
		| SHR     {	printf("UNEO SI SHR");}
		| LDR     {	printf("UNEO SI LDR");}
		| STR     {	printf("UNEO SI STR");}
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
    yyparse();
    return 0;
}
