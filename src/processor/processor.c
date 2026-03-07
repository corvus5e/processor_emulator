
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

void decode_instruction(word instruction, word *opcode, word out_args[3], size_t *args_num) {
	*opcode = instruction & OPCODE_MASK;
	switch (*opcode) {
	case BEQ_OPCODE:
	case BGT_OPCODE:
	case B_OPCODE:
	case CALL_OPCODE:
	case RET_OPCODE: {
		// Decode offset from one arg instruction
		// Right shift shoud extend sign
		out_args[0] = ((instruction << OPCODE_LEN) >> OPCODE_LEN);
		*args_num   = 1;
	}
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
