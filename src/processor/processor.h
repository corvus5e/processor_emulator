#ifndef _SIMLE_PROCESSOR_H_
#define _SIMLE_PROCESSOR_H_

#include <stddef.h>
#include <stdint.h>

#define REG_COUNT 16
#define MEM_LEN 256

typedef int32_t WORD;

struct Processor {
	unsigned char pc;    // program counter
	char mem[MEM_LEN];   // memory, 8bit memory
	char reg[REG_COUNT]; // registers
	char flag_E;
	char flag_GT;
};

/* Runs the compiled program
 */
int run_program(char *program, size_t len, struct Processor * const processor);

#endif
