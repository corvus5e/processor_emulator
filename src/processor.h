#ifndef _SIMLE_PROCESSOR_H_
#define _SIMLE_PROCESSOR_H_

#include <stddef.h>

#define REG_COUNT 16
#define MEM_LEN 256

struct Processor {
	unsigned char pc;    // program counter
	char mem[MEM_LEN];   // memory, 8bit memory
	char reg[REG_COUNT]; // registers
	int flag_E;
	int flag_GT;
};

enum InstructionArgType {
	NONE = 0,
	LOCATION = 1, // it could be number or label name
	IMMEDIATE = 1 << 1,
	AT_IMMEDIATE = 1 << 2,
	AT_REGISTER = 1 << 3,
	REGISTER = 1 << 4,
};

struct InstructionInfo {
	const char *name;
	char args_num;
	int args_types; // Mask of InstructionArgType
	char code;
};

int get_asm_instructions_info(struct InstructionInfo **asm_info);

/* Runs the compiled program
 */
int run_program(char *program, size_t len, struct Processor * const processor);

#endif
