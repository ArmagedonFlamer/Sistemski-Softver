default:
	clear
	flex -l ./misc/test.l
	bison -dv ./misc/test.y 
	yacc --verbose ./misc/test.y -o ./misc/test.cc
	cc -c lex.yy.c -o lex.yy.o
	g++ -o assembler lex.yy.o ./misc/test.cc -lfl
	g++ -g -o linker ./src/linker/main.cpp
	g++ -g -o emulator ./src/emulator/main.cpp ./src/emulator/emulator.cpp
