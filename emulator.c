/*
 * - Reads file with a asm-like program
 * - Compiles it to emulated machine code
 * - Loads the compiled program into "computer" memory
 * - Runs the program
*/

#include <stdio.h>
#include <stdlib.h>

#include "processor.h"

char* compile_program(const char *file_name, size_t *program_len);
void print_processor(const struct Processor *p);

int main(int argc, char *argv[])
{
	printf("Welcome to simple processor emulator!\n");
	if(argc < 2){
		printf("Usage: emulator.out <program.asm>\n");
	}
	size_t program_len = 0;
	char *program = compile_program(argv[1], &program_len);
	printf("Program len: %lu\n", program_len);

	for(size_t i = 0; i < program_len; ++i) {
		printf("%d|", (unsigned char)program[i]);
	}
	printf("\n");

	struct Processor p;
	
	run_program(program, program_len, &p);

	print_processor(&p);

	free(program);

	return 0;
}

void print_processor(const struct Processor *p) {
	printf("Processor state:\n");
	printf("---------------\n");
	printf("pc = %u\n", p->pc);

	for(int i = 0; i < REG_COUNT; ++i){
		printf("reg[%d] = %d\n", i, p->reg[i]);
	}
	printf("---------------\n");
	for(int i = 0; i < 20; ++i){
		printf("mem %d = %d\n", i, p->mem[i]);
	}
}

