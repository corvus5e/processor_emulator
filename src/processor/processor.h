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

struct DecodedInstruction {
	word opcode;
	word opcode_i_bit;
	uint8_t dst_reg;
	uint8_t src_reg_1;
	uint8_t src_reg_2;
	bool is_immediate;
	word imm_val;
	word offset;
};

struct vec_word;

void run_processor(struct Processor * const processor);

void load_program_from_mem(struct vec_word *program, struct Processor * const processor);

void decode_instruction(word instruction, struct DecodedInstruction *result);

void disassembly(word instruction, char *buf);

#endif
