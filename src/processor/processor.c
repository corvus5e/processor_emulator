
#include "processor.h"

#include <stdio.h>
#include <string.h>

int init_processor(const char *program, unsigned char len, struct Processor * const processor) {
	for(int i = 0;i < len; ++i)
		processor->mem[i] = program[i];

	for(int i = 0; i < REG_COUNT; ++i)
		processor->reg[i] = 0;

	return 1;
}

bool run_program(char *program, size_t len, struct Processor * const p) {
	init_processor(program, len, p);
	return 0;
}

void decode_instruction(word instruction, struct DecodedInstruction *result){
	/* Speculative Decode */
	result->opcode = instruction & OPCODE_MASK;
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
