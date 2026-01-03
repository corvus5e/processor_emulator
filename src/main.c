#include "emulator.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("Welcome to simple processor emulator!\n");
	if(argc < 2){
		printf("Usage: emulator.out <program.asm>\n");
	}

	struct Processor p;
	return compile_and_run_program(argv[1], &p);
}
