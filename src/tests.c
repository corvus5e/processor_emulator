#include <stdio.h>

#define T char
#include "vector.h"
#undef T

#ifndef SAMPLES_DIR
#define SAMPLES_DIR "" // Make code highlighting happy
#error "SAMPLES_DIR should be defined"
#endif

#include "emulator.h"

#define SAMPLE(x) SAMPLES_DIR "/" x

int test_1() {
	struct Processor p;
	compile_and_run_program(SAMPLE("sample.asm"), &p);
	return 0;
}

int test_2() {
	struct Processor p;
	compile_and_run_program(SAMPLE("example_26.asm"), &p);
	return 0;
}


int main() {

	int (*tests[])() = {test_2};

	for(int i = 0; i < sizeof(tests)/sizeof(*tests); ++i) {
		if(tests[i]() == 0) {
			printf("Test %d [PASSED]\n", i);
		}
		else {
			printf("Test %d [FAILED]\n", i);
		}
	}

	return 0;
}
