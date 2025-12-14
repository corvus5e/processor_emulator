/*
 * - Reads file with a asm-like program
 * - Compiles it to emulated machine code
 * - Loads the compiled program into "computer" memory
 * - Runs the program
*/

#include <stdio.h>
#include <stdlib.h>

#include "processor.h"

#define T char
#include "vector.h"
#undef T

int compile_program(const char *file_name, struct vec_char *program);
void print_processor(const struct Processor *p);

int main(int argc, char *argv[])
{
	printf("Welcome to simple processor emulator!\n");
	if(argc < 2){
		printf("Usage: emulator.out <program.asm>\n");
	}

	struct vec_char program;
	vec_init_char(&program);

	int status = compile_program(argv[1], &program);
	if(status == -1){/*TODO: place COMPILE_ERROR in accessible place*/
		printf("Compilation failed\n");
		return 1;
	}

	printf("Program len: %lu\n", program.size);

	for(size_t i = 0; i < program.size; ++i) {
		printf("%d|", (unsigned char)program.data[i]);
	}
	printf("\n");

	struct Processor p;
	
	run_program(program.data, program.size, &p);

	print_processor(&p);

	vec_free_char(&program);

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
	// for(int i = 0; i < 20; ++i){
	// 	printf("mem %d = %d\n", i, p->mem[i]);
	// }
}

