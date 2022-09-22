// NE MOZE DVA PUTA DODELA VREDNOSTI ISTOM SIMBOLU
%{
#include <stdio.h>
#include <string>
#include <cstring>
#include <stdbool.h>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <stdlib.h>

using namespace  std;

extern "C"
{
        int yyparse(void);
        int yylex(void);  
        int yyerror(char *s);
}
extern FILE * yyin;
FILE *outputFile; 
char *helper;
int addr_mode;
int allocated=0;
int mode; //MEMORY ADDRESS MODE
int end_read=0;
char* itoa_hex(int x)  // IN HEX
{
    int tmp = x;
    int n = 5;
    char* hexadecimalnum = (char*)malloc(n);
    memcpy(hexadecimalnum,"00 00\0",6);
    int i = 0;
    while (tmp != 0)
    {
        if ((tmp & 0x0F) < 10)
            hexadecimalnum[n - 1 - i++]= (char) (48 + (tmp & 0x0F));
        else
            hexadecimalnum[n - 1 - i++] = 55 + (tmp & 0x0F);
        tmp >>= 4;
        if (i == 2)
            ++i;
    }
    return hexadecimalnum;
}
string active_section="";
string global_file="";
multimap<string,int> *m = nullptr;
multimap<string,string> *e=nullptr;
int sec_rel_pos=0;  //section relative position

void insert_position(string variable, int values, int mode)
{
  //ubaci pojavljivanje sekcije
  if (mode==0)
    m->insert(pair<string,int>(global_file+"!"+active_section+"#"+variable,values));
  //stavi vrednost promenljive iz .equ izraza
  if (mode==1)
    m->insert(pair<string,int>(variable,values));
  //ubaci izvoz labele/promenljive
  if (mode==2)
    m->insert(pair<string,int>("!#"+variable,values));
  //ubaci duzinu sekcije
  if (mode==3)
    m->insert(pair<string,int>(active_section+"#",values));
  //ubaci uvoz labele/promenljive
  if (mode==4)
    m->insert(pair<string,int>("#!"+variable,values));
    //ubaci dodelu vrednosti labeli
 if (mode==5)
    m->insert(pair<string,int>(global_file+"!!"+active_section+"#"+variable,values));

}

void insert_expression(string variable)
{
  e->insert(pair<string,string>(global_file+"?"+active_section+"#"+variable, helper));
}
const char* get_data(const char* key)
{
  std::multimap<string,int>::iterator it = m->find(global_file+"!!"+active_section+"#"+key);
  cout<<key<<endl;
  if(it!=m->end())
    {
      const char *tmp = itoa_hex(it->second);
      return tmp;
    }
  else
    if((strlen(key)==5)&&(key[2]==' '))
    {
      cout<<"2";
      return key;
    }
    else
      {
        return "00 00";
      }
}
void print_jump(int op_code)
{
  char dest_reg='F';
  char src_reg='F';
  cout<<"JP: "<<helper<<" "<<mode<<endl;
  if ((mode!=0)&&(mode!=4))
  {
    src_reg=helper[4];
    helper[0]=' ';
    helper[1]=' ';
    helper[2]=' ';
    helper[3]=' ';
    helper[4]=' ';
    helper[5]=' ';
    cout<<src_reg<<endl;
  }
  string tmp = string(helper);
  while((tmp.length()>0)&&(tmp.at(0)==' ') )
    tmp.erase(0,1);
  cout<<"MOD"<<mode;
  switch(mode)
  {
    case 0: fprintf(outputFile, "%02X FF 00 %s",op_code,get_data(tmp.c_str())); break; //neposredno
    case 1: fprintf(outputFile, "%02X %c%c 01",op_code,dest_reg,src_reg); break;                     //regdir
    case 2: fprintf(outputFile, "%02X %c%c 02",op_code,dest_reg,src_reg);  break;                   //regdind
    case 3: fprintf(outputFile, "%02X %c%c 03 %s",op_code,dest_reg,src_reg,get_data(tmp.c_str())); break;  //regind sa pom
    case 4: fprintf(outputFile, "%02X %c%c 04 %s",op_code,dest_reg,src_reg,get_data(tmp.c_str()));  break; //mem 
    case 5: fprintf(outputFile, "%02X %c%c 05 %s",op_code,dest_reg,src_reg,get_data(tmp.c_str())); break;  //regdir sa pom 
  }
}
//PORAVNAJ ISPIS SA MEMORIJSKIM MODOVIMA!!!
//
//
//
//PORAVNAJ ISPIS SA MEMORIJSKIM MODOVIMA!!!

void print_ld_st(int op_code, int dest_reg, int update)
{
  char src_reg='F';
  if ((mode!=0)&&(mode!=4))
  {
    src_reg=helper[4];
    helper[0]=' ';
    helper[1]=' ';
    helper[2]=' ';
    helper[3]=' ';
    helper[4]=' ';
    helper[5]=' ';
    cout<<helper<<endl;
  }
  string tmp = string(helper);
  while((tmp.length()>0)&&(tmp.at(0)==' ') )
    tmp.erase(0,1);
  switch(mode)
  {
    case 0: fprintf(outputFile, "%02X %02X %d0 %s",op_code,((dest_reg<<4)+0x0f),update,get_data(tmp.c_str())); break; //neposredno
    case 1: fprintf(outputFile, "%02X %c%c %d1",op_code,char(dest_reg+48),src_reg,update); break;                     //regdir
    case 2: fprintf(outputFile, "%02X %c%c %d2",op_code,char(dest_reg+48),src_reg,update);  break;                   //regdind
    case 3: fprintf(outputFile, "%02X %c%c %d3 %s",op_code,char(dest_reg+48),src_reg,update,get_data(tmp.c_str())); break;  //regind sa pom
    case 4: fprintf(outputFile, "%02X %c%c %d4 %s",op_code,char(dest_reg+48),src_reg,update,get_data(tmp.c_str()));  break; //mem 
    case 5: fprintf(outputFile, "%02X %c%c %d5 %s",op_code,char(dest_reg+48),src_reg,update,get_data(tmp.c_str())); break;  //regdir sa pom 
  }
}
%}

%token VARIABLE NUM HALT INT IRET CALL RET JMP JEQ JNE JGT XCHG ADD SUB MUL DIV CMP NOT AND OR XOR TEST SHL SHR LDR STR OTHER SEMICOLON  COLON
		PUSH POP REG PSW COMMA DOLLAR L_BRACKET R_BRACKET PLUS MINUS PERCENT EQU GLOB END SECTION ASCII EXTERN SKIP WORD COMMENT STRING LABEL ASTERISK ENTER
    QUOTATION
%union{
	char name[20];
  int number;
}

%type <name> VARIABLE
%type <name> LABEL
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
%type <number> REG
%type <number> PSW
%type <name> EQU	
%type <name> GLOB		
%type <name> EXTERN	
%type <name> SECTION	
%type <name> WORD	
%type <name> SKIP		
%type <name> ASCII		
%type <name> END
%type <name> STRING
%type <number> OTHER



%%

prog:
  stmts
;

stmts:
		| operation stmts


operation:
halt | int  | iret | call | ret  | jmp  | jeq  | jne  | jgt  | xchg | add  | sub  | mul  | div  | cmp  | not  | and  | or   | xor  | 
test | shl  | shr  | ldr  | str  | push  | pop | equ | section | global | word | skip | end |ascii | extern | label |
ENTER       {fprintf(outputFile, "\n");}
| COLON	  	{fprintf(outputFile, ":");}
| VARIABLE 	{fprintf(outputFile, "%s", $1);}
| NUM  		  {fprintf(outputFile, "%02X", $1);}
| COMMA     {fprintf(outputFile, ",");}
| COMMENT   {}
| OTHER

//PC RELATIVNO ADRESIRANJE MODE=5!
label:  LABEL { string tmp = string($1); tmp.erase(tmp.length()-1);insert_position(tmp,sec_rel_pos,5);} 
equ:    EQU   VARIABLE COMMA   NUM { insert_position(string($2),$4,1); } 
      | EQU   VARIABLE COMMA   expr {insert_expression(string($2)); if (allocated) free(helper); allocated=0;}

expr: | VARIABLE   PLUS   expr
        { 
          char *tmp = (char*)malloc(strlen(helper) + strlen($1) + 1+3);
          tmp[0]='\0';
          strcpy(tmp, $1);          
          strcat(tmp, " + ");
          strcat(tmp, helper);
          if (allocated) free(helper);
          allocated=1;
          helper=tmp;
          printf("%s",tmp);
        } | VARIABLE   MINUS   expr 
          { 
            char *tmp = (char*)malloc(strlen(helper) + strlen($1) + 1+3);
            tmp[0]='\0';
            strcpy(tmp, $1);          
            strcat(tmp, " - ");
            strcat(tmp, helper);
            if (allocated) free(helper);
            allocated=1;
            helper=tmp;
            printf("%s",tmp);
          } | NUM   PLUS   expr 
            { 
            char *tmp = (char*)malloc(strlen(helper) + strlen(itoa_hex($1)) + 1+3);
            tmp[0]='\0';
            strcpy(tmp, itoa_hex($1));          
            strcat(tmp, " + ");
            strcat(tmp, helper);
            if (allocated) free(helper);
            allocated=1;
            helper=tmp;
            printf("%s",tmp);
            } | NUM   MINUS   expr 
            { 
            char *tmp = (char*)malloc(strlen(helper) + strlen(itoa_hex($1)) + 1+3);
            tmp[0]='\0';
            strcpy(tmp, itoa_hex($1));          
            strcat(tmp, " - ");
            strcat(tmp, helper);
            if (allocated) free(helper);
            allocated=1;
            helper=tmp;
            printf("%s",tmp);
            }| VARIABLE {  helper= (char*)malloc(strlen($1)+2);helper[0]='\0'; strcpy(helper,$1); strcat(helper," ");allocated=1;printf("x%sx ",helper);}
            | NUM {  helper= (char*)malloc(strlen(itoa_hex($1))+2);helper[0]='\0'; strcpy(helper,itoa_hex($1));strcat(helper," ");allocated=1;}
section:  SECTION   VARIABLE      
{  if (active_section.compare("")!=0)
    insert_position(active_section, sec_rel_pos,3);
  sec_rel_pos=0; 
  active_section=string($2);
  insert_position(string($2),sec_rel_pos,5); 
  fprintf(outputFile, "%s", $2);
  }
global: GLOB   VARIABLE global_promenljive {insert_position(string($2),sec_rel_pos,2);}
word: WORD   hide_promenljive {sec_rel_pos+=2;}
skip: SKIP   NUM {sec_rel_pos+=$2; for (int i=0;i<$2;++i) fprintf(outputFile, "00 ");}
end: END  {end_read=1;}
ascii:  ASCII quoted_text 
{ 
  printf("%d",allocated); 
  if (allocated) 
  { 
    //ispisujem razmak kako bi prvo slovo koje cita emulator bilo t
    fprintf(outputFile, "20 "); sec_rel_pos+=1;
    for (int i=0;i<strlen(helper);++i) 
     {
      fprintf(outputFile, "%02X ",helper[i]);
      printf("%c ",helper[i]);  
     }
    free(helper); 
    allocated=0;
  }
}
quoted_text: QUOTATION text QUOTATION
text: |  VARIABLE text   {
  char *tmp;
  if (allocated)
   tmp= (char*)malloc(strlen($1) + strlen(helper)+1+1);
  else
     tmp= (char*)malloc(strlen($1) +1+1);
  tmp[0]='\0';
  strcpy(tmp, $1);          
  strcat(tmp, " ");
  if (allocated)
   strcat(tmp, helper);
  if (allocated) free(helper);
  allocated=1;
  helper=tmp;
  printf("%s",tmp);
  sec_rel_pos+=strlen($1);
  }| STRING   text
  {
    char *tmp;
    if (allocated)
    tmp= (char*)malloc(strlen($1) + strlen(helper)+1+1);
   else
     tmp= (char*)malloc(strlen($1) +1+1);
    tmp[0]='\0';
    strcpy(tmp, $1);          
    strcat(tmp, " ");
    if (allocated)
      strcat(tmp, helper);
    if (allocated) free(helper);
    allocated=1;
    helper=tmp;
    printf("%s",tmp);
    sec_rel_pos+=strlen($1);
  }| LABEL   text
  {
    char *tmp;
    if (allocated)
    tmp= (char*)malloc(strlen($1) + strlen(helper)+1+1);
    else
     tmp= (char*)malloc(strlen($1) +1+1);
    tmp[0]='\0';
    strcpy(tmp, $1);          
    strcat(tmp, " ");
    if (allocated)
      strcat(tmp, helper);
    if (allocated) free(helper);
    allocated=1;
    helper=tmp;
    printf("%s",tmp);
    sec_rel_pos+=strlen($1);
  }
extern: EXTERN   VARIABLE export_promenljive { insert_position(string($2),sec_rel_pos,4);}

global_promenljive:  | COMMA   VARIABLE global_promenljive { insert_position(string($2),sec_rel_pos,2);}
export_promenljive:  | COMMA   VARIABLE export_promenljive { insert_position(string($2),sec_rel_pos,4);}
hide_promenljive: | NUM { fprintf(outputFile, "%s", itoa_hex($1));} | VARIABLE {insert_position(string($1),sec_rel_pos,0); fprintf(outputFile, "00 00");} | COMMA   VARIABLE hide_promenljive { insert_position(string($2),sec_rel_pos,0); fprintf(outputFile, "00 00");}
| COMMA   NUM hide_promenljive { fprintf(outputFile, "%02X ", $2);}

halt: HALT    	                      {fprintf(outputFile, "%02X", $1); sec_rel_pos+=1;}
int:  INT   REG                   {fprintf(outputFile, "%02X %01XF",$1,$2); sec_rel_pos+=2;}
iret: IRET                            {fprintf(outputFile, "%02X",$1); sec_rel_pos+=1;}
call: CALL   jmp_operand          {print_jump($1);if (allocated)free(helper);allocated=0;} //ODREDITI DA LI JE 3 ILI 5 B
ret:  RET                             {fprintf(outputFile, "%02X",$1);if (allocated) free(helper);allocated=0;sec_rel_pos+=1;}
jmp:  JMP   jmp_operand           {print_jump($1); if (allocated) free(helper);allocated=0;}//ODREDITI DA LI JE 3 ILI 5 B
jeq:  JEQ   jmp_operand           {print_jump($1); if (allocated) free(helper);allocated=0;}//ODREDITI DA LI JE 3 ILI 5 B
jne:  JNE   jmp_operand           {print_jump($1); if (allocated) free(helper);allocated=0;}//ODREDITI DA LI JE 3 ILI 5 B
jgt:  JGT   jmp_operand           {print_jump($1); if (allocated) free(helper);allocated=0;}//ODREDITI DA LI JE 3 ILI 5 B
xchg: XCHG   REG COMMA   REG  {fprintf(outputFile, "%02X %02X %02X",$1,$2,$4);sec_rel_pos+=2;}
add:  ADD   REG COMMA   REG   {fprintf(outputFile, "%02X %d%d",$1,$2,$4);sec_rel_pos+=2;}
sub:  SUB   REG COMMA   REG   {fprintf(outputFile, "%02X %d%d",$1,$2,$4);sec_rel_pos+=2;}
mul:  MUL   REG COMMA   REG   {fprintf(outputFile, "%02X %d%d",$1,$2,$4);sec_rel_pos+=2;}
div:  DIV   REG COMMA   REG   {fprintf(outputFile, "%02X %d%d",$1,$2,$4);sec_rel_pos+=2;}
cmp:  CMP   REG COMMA   REG   {fprintf(outputFile, "%02X %d%d",$1,$2,$4);sec_rel_pos+=2;}
not:  NOT   REG COMMA   REG   {fprintf(outputFile, "%02X %02X %02X",$1,$2,$4);sec_rel_pos+=2;}
and:  AND   REG COMMA   REG   {fprintf(outputFile, "%02X %02X %02X",$1,$2,$4);sec_rel_pos+=2;}
or:   OR   REG COMMA   REG    {fprintf(outputFile, "%02X %02X %02X",$1,$2,$4);sec_rel_pos+=2;}
xor:  XOR   REG COMMA   REG   {fprintf(outputFile, "%02X %02X %02X",$1,$2,$4);sec_rel_pos+=2;}
test: TEST   REG COMMA   REG  {fprintf(outputFile, "%02X %02X %02X",$1,$2,$4);sec_rel_pos+=2;}
shl:  SHL   REG COMMA   REG   {fprintf(outputFile, "%02X %02X %02X",$1,$2,$4);sec_rel_pos+=2;}
shr:  SHR   REG COMMA   REG   {fprintf(outputFile, "%02X %02X %02X",$1,$2,$4);sec_rel_pos+=2;}
ldr:  LDR   REG COMMA   operand             {print_ld_st($1,$2,0);if (allocated) free(helper);allocated=0;}//ODREDITI DA LI JE 3 ILI 5 B
str:  STR   REG COMMA   operand             {print_ld_st($1,$2,0);if (allocated) free(helper); allocated=0;}//ODREDITI DA LI JE 3 ILI 5 B
push: PUSH   REG    //176<=>B0 (str)  
{
  if((helper = (char*)malloc(6)) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,itoa_hex(6+0xF0));
  }
  mode=2;
  print_ld_st(176,$2,1);
  if (allocated) free(helper);allocated=0;
  sec_rel_pos+=3;
  } 
pop:  POP   REG  //160<=>A0 (ldr)
{
  if((helper = (char*)malloc(6)) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,itoa_hex(6+0xF0));
  }
  mode=2;
  print_ld_st(160,$2,4);
  if (allocated) free(helper);allocated=0;
  sec_rel_pos+=3;
  } 

operand: DOLLAR NUM       
{
  sec_rel_pos+=3;
  helper=itoa_hex($2);
  mode=0; 
  sec_rel_pos+=2;
}
| DOLLAR VARIABLE         
{
  sec_rel_pos+=3;
  helper=$2; 
  insert_position(string($2),sec_rel_pos,0); 
  sec_rel_pos+=2;
  mode=0;
}
| NUM                     
{sec_rel_pos+=3; helper=itoa_hex($1); mode=4; sec_rel_pos+=2;}
| VARIABLE                
{sec_rel_pos+=3;helper=$1;insert_position(string($1),sec_rel_pos,0); mode=4;sec_rel_pos+=2;}
| L_BRACKET REG R_BRACKET  
{sec_rel_pos+=3; helper=itoa_hex($2); mode=2;}
| L_BRACKET REG   PLUS   VARIABLE R_BRACKET   
{
  sec_rel_pos+=3;
if((helper = (char*)malloc(strlen(itoa_hex($2))+strlen($4)+2+3)) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,itoa_hex($2+0xF0));
    strcat(helper," ");
//    strcat(helper,itoa_hex(3)); npm zasto ovo
    strcat(helper,$4);
    insert_position(string($4),sec_rel_pos,0);
    sec_rel_pos+=2;
    }
    mode=3;}
| L_BRACKET REG   PLUS   NUM R_BRACKET   
{
  sec_rel_pos+=3;
if((helper = (char*)malloc(strlen(itoa_hex($2))+strlen(itoa_hex($4))+2+3)) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,itoa_hex($2+0xF0));
    strcat(helper," ");
//    strcat(helper,itoa_hex(3)); nmp zasto ovo
    strcat(helper,itoa_hex($4));
    sec_rel_pos+=2;
    }
    mode=3;}
| REG { sec_rel_pos+=3; helper=(char*)malloc(1); allocated = 1; helper = itoa_hex($1); mode=1; }
| PERCENT VARIABLE { 
    sec_rel_pos+=3;
    insert_position(string($2),sec_rel_pos,0); 
if((helper = (char*)malloc(strlen($2)+3)) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,"00 F7");
    strcat(helper," ");
    strcat(helper,$2);
    sec_rel_pos+=2;
    }
    mode=5;}
;

jmp_operand: NUM                     
{sec_rel_pos+=3; helper=itoa_hex($1); mode=0; sec_rel_pos+=2;}
| VARIABLE                
{sec_rel_pos+=3;helper=$1;insert_position(string($1),sec_rel_pos,0); mode=0;sec_rel_pos+=2;}
| PERCENT VARIABLE { 
    sec_rel_pos+=3;
    insert_position(string($2),sec_rel_pos,0); 
if((helper = (char*)malloc(strlen($2)+3)) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,"00 07");
    strcat(helper," ");
    strcat(helper,$2);
    sec_rel_pos+=2;
    }
    mode=5;}
|  ASTERISK NUM       
{
  sec_rel_pos+=3;
  helper=itoa_hex($2);
  mode=4; 
  sec_rel_pos+=2;
}
| ASTERISK VARIABLE         
{
  sec_rel_pos+=3;
  helper=$2; 
  insert_position(string($2),sec_rel_pos,0); 
  sec_rel_pos+=2;
  mode=4;
}
| ASTERISK REG { sec_rel_pos+=3; helper=(char*)malloc(1); allocated = 1; helper = itoa_hex($2); mode=1; }
| ASTERISK L_BRACKET REG R_BRACKET   
{
  sec_rel_pos+=3;
if((helper = (char*)malloc(strlen(itoa_hex($3)))) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,itoa_hex($3+0xF0));
    }
    mode=2;}
| ASTERISK L_BRACKET REG   PLUS   VARIABLE R_BRACKET   
{
  sec_rel_pos+=3;
if((helper = (char*)malloc(strlen(itoa_hex($3))+strlen($5)+2+3)) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,itoa_hex($3+0xF0));
    strcat(helper," ");
//    strcat(helper,itoa_hex(3)); npm zasto ovo
    strcat(helper,$5);
    insert_position(string($5),sec_rel_pos,0);
    sec_rel_pos+=2;
    }
    mode=3;}
| ASTERISK L_BRACKET REG   PLUS   NUM R_BRACKET   
{
  sec_rel_pos+=3;
if((helper = (char*)malloc(strlen(itoa_hex($3))+strlen(itoa_hex($5))+2+3)) != NULL){
    helper[0] = '\0';   // ensures the memory is an empty string
    allocated = 1;
    strcat(helper,itoa_hex($3+0xF0));
    strcat(helper," ");
//    strcat(helper,itoa_hex(3)); npm zasto ovo
    strcat(helper,itoa_hex($5));
    sec_rel_pos+=2;
    }
    mode=3;};

%%

int yyerror(char *s)
{
	printf("Syntax Error on line %s\n", s);
	return 0;
}

void structure_output(string input, string output_file){
  std::ifstream file(input);
  std::string str; 
  std::ofstream output;
  output.open(output_file);
  int i=0;
  bool end_code=false;
  while (std::getline(file, str))
  {
      if (str.length()==0);
      else
        {
          int pos=0;
          if (str.at(0)==' ')
            str.erase(0,1);
          if ((str.compare("00")==0)||(str.compare("20")==0)||(str.compare("40")==0))
          {
            output<<str<<" ";
            ++i;
            continue;
          }
          else
            if ((pos = str.find(' ')) == std::string::npos)
            {
                i=i%8;
                if (i!=0)
                  {
                /*    for (i;i<8;++i)
                      output<<"00 ";*/
                    output<<endl;
                    i=0;
                  }
              output<<str<<endl;
            }
            else
              if (str.at(0)=='{')
                {
                  if (!end_code)
                  { 
                    end_code=true;
           /*         i=i%8;
                    for (i;i<8;i++)
                      output<<"00 ";*/
                  }
                  output<<endl<<str;
                }
              else
              {
    //            output<<str<<endl;
                string token;
                while ((pos = str.find(' ')) != std::string::npos) {
                  if (pos!=2)
                   {
                      str.erase(0, pos + 1);
                      continue;
                  }
                  token = str.substr(0, pos);
                  ++i;
                  output << token << " ";
                  if (i % 8 ==0)
                    output<<endl;
                  str.erase(0, pos + 1);
                }
                if (str.length()>0)
                {
                  token = str;
                  ++i;
                  output << token << " ";
                  if (i % 8 ==0)
                    output<<endl;
                  str.erase(0, pos + 1);
                }
              }
        }
  }
  file.close();
  output.close();
}

int main(int argc, char* argv[])
{
  // open a file handle to a particular file:
  string output_name;
  for (int i=1;i<argc;++i)
  {
    string arg=*new string(argv[i]);
    if (arg.compare("-o")==0)
    {
      output_name=*new string(argv[++i]);
    }
    else
    {
      m=new multimap<string,int>();
      e=new multimap<string,string>();
      active_section="";
      allocated=0;
      sec_rel_pos=0;
      helper=nullptr;
      addr_mode=-1;
      mode=-1;

      FILE *myfile = fopen(argv[i], "r");
      global_file=string(output_name);
      char pera[6];
      //privremeni fajl
      pera[0]=(char)(i+48);
      pera[1]='.'; pera[2]='t'; pera[3]='x'; pera[4]='t'; pera[5]='\0';
      outputFile = fopen(&pera[0], "w");

      // make sure it's valid:
      if ((!myfile)||(!outputFile)) {
        printf("I can't open the file!");
        return -1;
      }
      // set lex to read from it instead of defaulting to STDIN:
      yyin = myfile;
      
      // lex through the input:
      while(yyparse())
        if (end_read)
          break;
      end_read=0;
      insert_position(active_section, sec_rel_pos, 3);
      fclose(myfile);
      fclose(outputFile);
      ofstream tmp_output;
      tmp_output.open (&pera[0],std::ios_base::app);
      for (auto it = m->cbegin(); it != m->cend(); ++it) {
            tmp_output << "{" << (*it).first << ": " << (*it).second << "}\n";
            cout << "{" << (*it).first << ": " << (*it).second << "}\n";
        }
      for (auto it = e->cbegin(); it != e->cend(); ++it) {
        tmp_output << "{" << (*it).first << ": " << (*it).second << " }\n";
        cout << "{" << (*it).first << ": " << (*it).second << " }\n";
      }
      tmp_output.close();
      structure_output(&pera[0],output_name);
   //   remove(&pera[0]);
    }
  }
}
