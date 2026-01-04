#include "emulator.h"

#include <stdio.h>
#include <stdlib.h>

#define T char
#include "vector.h"
#undef T

#include "asm_compiler.h"

int compile_program(const char *file_name, struct vec_char *program);
void print_processor(const struct Processor *p);

int compile_and_run_program(const char *file_name, struct Processor *processor) {
	struct vec_char program;
	vec_init_char(&program);

	int status = compile_program(file_name, &program);
	if(status == COMPILE_ERROR){
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
	printf("flag.E = %d\n", p->flag_E);
	printf("flag.GT = %d\n", p->flag_GT);
	// for(int i = 0; i < 20; ++i){
	// 	printf("mem %d = %d\n", i, p->mem[i]);
	// }
}

