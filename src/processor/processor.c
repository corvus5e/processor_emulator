
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

void disassembly(word instruction, char *buf) {
	word opcode = instruction & OPCODE_MASK;
	size_t args_num = 0;
	const char *inst_name = NULL;

	switch(opcode) {
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
		// Right shift shoud extend sign
		word offset = ((instruction << OPCODE_LEN) >> OPCODE_LEN);
		sprintf(buf, "%s %d\n", inst_name, offset);
	} break;
	case 3: {
		const bool is_immediate = instruction & I_BIT_MASK;
		word dst_reg = ((instruction & DST_REG_MASK) >> DST_REG_SHIFT);
		word src_reg_1 = ((instruction & SRC_REG_1_MASK) >> SRC_REG_1_SHIFT);
		word src_val_2 = 0;
		const char *fmt = "%s r%d, r%d, r%d\n";

		if (is_immediate) {
			// << 16 + >> 16 shift is to extend sign bits
			src_val_2 = ((instruction & IMMEDIATE_3_MASK) << 16) >> 16;
			fmt = "%s r%d, r%d, %d\n";
		}
		else {
			src_val_2 = ((instruction & SRC_REG_2_MASK) >> SRC_REG_2_SHIFT);
		}

		sprintf(buf, fmt, inst_name, dst_reg, src_reg_1, src_val_2);

	} break;
	}

}

char* opcode_to_str(word opcode) {
	switch(opcode) {
	case ADD_OPCODE: return "add";
	case SUB_OPCODE: return "sub";
	case MUL_OPCODE: return "mul";
	case DIV_OPCODE: return "div";
	case MOD_OPCODE: return "mod";
	case CMP_OPCODE: return "cmp";
	case AND_OPCODE: return "and";
	case OR_OPCODE:  return "or";
	case NOT_OPCODE: return "not";
	case MOV_OPCODE: return "mov";
	case LSL_OPCODE: return "lsl";
	case LSR_OPCODE: return "lsr";
	case ASR_OPCODE: return "asr";
	case NOP_OPCODE: return "nop";
	case LD_OPCODE:  return "ld";
	case ST_OPCODE:  return "st";
	case BEQ_OPCODE: return "beq";
	case BGT_OPCODE: return "bgt";
	case B_OPCODE:   return "b";
	case CALL_OPCODE:return "call";
	case RET_OPCODE: return "ret";
	}

	return "unknown_instruction";
}
