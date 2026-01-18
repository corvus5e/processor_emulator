#include <stdio.h>
#include "asm_compiler.h"

int main(int argc, char *argv[]) {
	if(argc < 2) {
		printf("Usage: srac.out <path to asm source file>\n");
	}

	struct vec_word program;
	vec_init_word(&program);

	compile_program(argv[1], &program);

	printf("Program:\n");
	for(int i = 0; i < program.size; ++i) {
		printf("0x%08x|", *vec_at_word(&program, i));
	}

	putchar('\n');

	return 0;
}
