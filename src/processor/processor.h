#ifndef _SIMLE_PROCESSOR_H_
#define _SIMLE_PROCESSOR_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <common/simple_risc.h>

#define REG_COUNT 16
#define MEM_LEN (4 * 1024)

struct Processor {
	uint8_t mem[MEM_LEN];   // memory
	word    reg[REG_COUNT]; // registers
	word    flag_E;
	word    flag_GT;
};

/* Runs the compiled program
 */
bool run_program(char *program, size_t len, struct Processor * const processor);

char* opcode_to_str(word opcode);

void disassembly(word instruction, char *buf);

#endif
