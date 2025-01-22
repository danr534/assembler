# Compiler Flags
CFLAGS = -Wall -ansi -pedantic -std=c89


assembler: assembler.o pre_assembler.o
	gcc $(CFLAGS) assembler.o pre_assembler.o -o assembler

assembler.o: assembler.c pre_assembler.h
	gcc $(CFLAGS) -c assembler.c -o assembler.o

pre_assembler.o: pre_assembler.c pre_assembler.h
	gcc $(CFLAGS) -c pre_assembler.c -o pre_assembler.o
