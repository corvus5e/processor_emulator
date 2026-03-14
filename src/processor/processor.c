
#include "processor.h"

#include <stdio.h>
#include <string.h>

#define T word
#include <common/vector.h>
#undef T

int init_processor(const char *program, unsigned char len, struct Processor * const processor) {
	for(int i = 0;i < len; ++i)
		processor->mem[i] = program[i];

	for(int i = 0; i < REG_COUNT; ++i)
		processor->reg[i] = 0;

	return 1;
}

void run_processor(struct Processor * const processor) {
	word pc = 0;
	struct DecodedInstruction di = {};
	word instruction = 0;
	do {
		instruction = (word)processor->mem[pc] << 24;
		instruction |= (word)processor->mem[pc + 1] << 16;
		instruction |= (word)processor->mem[pc + 2] << 8;
		instruction |= (word)processor->mem[pc + 3];

		decode_instruction(instruction, &di);
		switch(di.opcode_i_bit) {
		case ADD_OPCODE | I_BIT_MASK:
			processor->reg[di.dst_reg] = processor->reg[di.src_reg_1] + di.imm_val; break;
		case ADD_OPCODE:
			processor->reg[di.dst_reg] = processor->reg[di.src_reg_1] + processor->reg[di.src_reg_2]; break;
		case SUB_OPCODE | I_BIT_MASK:
			processor->reg[di.dst_reg] = processor->reg[di.src_reg_1] - di.imm_val; break;
		case SUB_OPCODE:
			processor->reg[di.dst_reg] = processor->reg[di.src_reg_1] - processor->reg[di.src_reg_2]; break;
		case MUL_OPCODE | I_BIT_MASK:
			processor->reg[di.dst_reg] = processor->reg[di.src_reg_1] * di.imm_val; break;
		case MUL_OPCODE:
			processor->reg[di.dst_reg] = processor->reg[di.src_reg_1] * processor->reg[di.src_reg_2]; break;
		}
		pc += 4;
	}
	while(di.opcode != END_PROGRAM);
}

void load_program_from_mem(struct vec_word *program, struct Processor * const processor) {
	for(int i = 0; i < REG_COUNT; ++i)
		processor->reg[i] = 0;

	const int n = sizeof(word);
	for(int i = 0; i < program->size; ++i) {
		int offset = n*i;
		word instruction = *vec_at_word(program, i);

		//TODO: Big-endian, but SimpleRisc should be little-endian, fix!
		processor->mem[offset    ] = (instruction >> 24) & 0xFF;
		processor->mem[offset + 1] = (instruction >> 16) & 0xFF;
		processor->mem[offset + 2] = (instruction >> 8) & 0xFF;
		processor->mem[offset + 3] = instruction & 0xFF;
	}

	processor->mem[program->size * n] = 0xFF; //TODO: Program end
}

void decode_instruction(word instruction, struct DecodedInstruction *result){
	/* Speculative Decode */
	result->opcode = instruction & OPCODE_MASK;
	result->opcode_i_bit = instruction & OPCODE_AND_I_BIT_MASK;
	result->dst_reg = ((instruction & DST_REG_MASK) >> DST_REG_SHIFT);
	result->src_reg_1 = ((instruction & SRC_REG_1_MASK) >> SRC_REG_1_SHIFT);
	result->src_reg_2 = ((instruction & SRC_REG_2_MASK) >> SRC_REG_2_SHIFT);
	result->offset = ((instruction << OPCODE_LEN) >> OPCODE_LEN);
	result->is_immediate = instruction & I_BIT_MASK;
	word modifiers = instruction & MODIFIERS_BITS_MASK;

	switch (modifiers) {
	case MODIFIERS_BITS_DEFAULT_MASK:
		result->imm_val = ((instruction & IMMEDIATE_3_MASK) << 16) >> 16;
		break;
	case MODIFIERS_BITS_H_MASK:
		result->imm_val = (instruction & IMMEDIATE_3_MASK) << 16;
		break;
	case MODIFIERS_BITS_U_MASK:
		result->imm_val = (instruction & IMMEDIATE_3_MASK);
		break;
	}
}

void disassembly(word instruction, char *buf) {
	struct DecodedInstruction di;
	decode_instruction(instruction, &di);
	const char *inst_name = NULL;
	size_t args_num = 0;

	switch(di.opcode) {
	case NOP_OPCODE: inst_name = "nop"; args_num = 0; break;

	case BEQ_OPCODE: inst_name = "beq"; args_num = 1; break;
	case BGT_OPCODE: inst_name = "bgt"; args_num = 1; break;
	case B_OPCODE:   inst_name = "b";   args_num = 1; break;
	case CALL_OPCODE:inst_name = "call";args_num = 1; break;
	case RET_OPCODE: inst_name = "ret"; args_num = 1; break;

	case CMP_OPCODE: inst_name = "cmp"; args_num = 2; break;
	case NOT_OPCODE: inst_name = "not"; args_num = 2; break;
	case MOV_OPCODE: inst_name = "mov"; args_num = 2; break;

	case ADD_OPCODE: inst_name = "add"; args_num = 3; break;
	case SUB_OPCODE: inst_name = "sub"; args_num = 3; break;
	case MUL_OPCODE: inst_name = "mul"; args_num = 3; break;
	case DIV_OPCODE: inst_name = "div"; args_num = 3; break;
	case MOD_OPCODE: inst_name = "mod"; args_num = 3; break;
	case AND_OPCODE: inst_name = "and"; args_num = 3; break;
	case OR_OPCODE:  inst_name = "or";  args_num = 3; break;
	case LSR_OPCODE: inst_name = "lsr"; args_num = 3; break;
	case ASR_OPCODE: inst_name = "asr"; args_num = 3; break;
	case LD_OPCODE:  inst_name = "ld";  args_num = 3; break;
	case ST_OPCODE:  inst_name = "st";  args_num = 3; break;

	}

	switch (args_num) {
	case 1: {
		sprintf(buf, "%s %d\n", inst_name, di.offset);
	} break;
	case 3: {
		const char* fmt = di.is_immediate ? "%s r%d, r%d, %d\n" : "%s r%d, r%d, r%d\n";
		sprintf(buf, fmt, inst_name, di.dst_reg, di.src_reg_1, di.is_immediate ? di.imm_val : di.src_reg_2);
	} break;
	}

}
