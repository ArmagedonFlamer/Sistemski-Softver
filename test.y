
%{
#include <stdio.h>

int yylex();
int yyerror(char *s);
extern FILE * yyin;
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
		| HALT    {	printf("UNEO SI HALT: %d", $1);}
		| INT     {	printf("UNEO SI INT: %d", $1);}
		| IRET    {	printf("UNEO SI IRET: %d", $1);}
		| CALL    {	printf("UNEO SI CALL: %d", $1);}
		| RET     {	printf("UNEO SI RET: %d", $1);}
		| JMP     {	printf("UNEO SI JMP: %d", $1);}
		| JEQ     {	printf("UNEO SI JEQ: %d", $1);}
		| JNE     {	printf("UNEO SI JNE: %d", $1);}
		| JGT     {	printf("UNEO SI JGT: %d", $1);}
		| XCHG    {	printf("UNEO SI XCHG: %d", $1);}
		| ADD     {	printf("UNEO SI ADD: %d", $1);}
		| SUB     {	printf("UNEO SI SUB: %d", $1);}
		| MUL     {	printf("UNEO SI MUL: %d", $1);}
		| DIV     {	printf("UNEO SI DIV: %d", $1);}
		| CMP     {	printf("UNEO SI CMP: %d", $1);}
		| NOT     {	printf("UNEO SI NOT: %d", $1);}
		| AND     {	printf("UNEO SI AND: %d", $1);}
		| OR      {	printf("UNEO SI OR: %d", $1);}
		| XOR     {	printf("UNEO SI XOR: %d", $1);}
		| TEST    {	printf("UNEO SI TEST: %d", $1);}
		| SHL     {	printf("UNEO SI SHL: %d", $1);}
		| SHR     {	printf("UNEO SI SHR: %d", $1);}
		| LDR     {	printf("UNEO SI LDR: %d", $1);}
		| STR     {	printf("UNEO SI STR: %d", $1);}
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
  yyparse();
  fclose(myfile);
}

