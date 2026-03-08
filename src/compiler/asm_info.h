#ifndef _ASM_INFO_H_
#define _ASM_INFO_H_

#include <stdint.h>
#include <common/simple_risc.h>

enum InstructionArgType {
	NONE = 0,
	LOCATION = 1, // it could be number or label name
	IMMEDIATE = 1 << 1,
	AT_REGISTER = 1 << 2, // example: 12[r1], part [r1] - is at register
	REGISTER = 1 << 3,
};

#define MAX_ARGS_NUM 3

typedef enum InstructionArgType arg_types_array[MAX_ARGS_NUM];
typedef int arg_values_array[MAX_ARGS_NUM];

struct InstructionInfo {
	const char *name;
	int args_num;
	arg_types_array arg_types;
	word code;
};

#define EXIT_PROGRAM -1

/* Table 3.10 from Chapter 3 */
static struct InstructionInfo asm_info[] = {

	{"add",  3, {REGISTER,REGISTER,IMMEDIATE},    ADD_OPCODE},
	{"add",  3, {REGISTER,REGISTER,REGISTER},     ADD_OPCODE},

	{"sub",  3, {REGISTER,REGISTER,IMMEDIATE},    SUB_OPCODE},
	{"sub",  3, {REGISTER,REGISTER,REGISTER},     SUB_OPCODE},

	{"mul",  3, {REGISTER,REGISTER,IMMEDIATE},    MUL_OPCODE},
	{"mul",  3, {REGISTER,REGISTER,REGISTER},     MUL_OPCODE},

	{"div",  3, {REGISTER,REGISTER,IMMEDIATE},    DIV_OPCODE},
	{"div",  3, {REGISTER,REGISTER,REGISTER},     DIV_OPCODE},

	{"mod",  3, {REGISTER,REGISTER,IMMEDIATE},    MOD_OPCODE},
	{"mod",  3, {REGISTER,REGISTER,REGISTER},     MOD_OPCODE},

	{"cmp",  2, {REGISTER,IMMEDIATE},             CMP_OPCODE},
	{"cmp",  2, {REGISTER,REGISTER},              CMP_OPCODE},

	{"and",  3, {REGISTER,REGISTER,REGISTER},     AND_OPCODE},
	{"and",  3, {REGISTER,REGISTER,IMMEDIATE},    AND_OPCODE},

	{"or",   3, {REGISTER,REGISTER,REGISTER},     OR_OPCODE},
	{"or",   3, {REGISTER,REGISTER,IMMEDIATE},    OR_OPCODE},

	{"not",  2, {REGISTER,REGISTER},              NOT_OPCODE},
	{"not",  2, {REGISTER,IMMEDIATE},             NOT_OPCODE},

	{"mov",  2, {REGISTER,REGISTER},              MOV_OPCODE},
	{"mov",  2, {REGISTER,IMMEDIATE},             MOV_OPCODE},

	{"lsl",  3, {REGISTER,REGISTER,REGISTER},     LSL_OPCODE},
	{"lsl",  3, {REGISTER,REGISTER,IMMEDIATE},    LSL_OPCODE},

	{"lsr",  3, {REGISTER,REGISTER,REGISTER},     LSR_OPCODE},
	{"lsr",  3, {REGISTER,REGISTER,IMMEDIATE},    LSR_OPCODE},

	{"asr",  3, {REGISTER,REGISTER,REGISTER},     ASR_OPCODE},
	{"asr",  3, {REGISTER,REGISTER,IMMEDIATE},    ASR_OPCODE},

	{"nop",  0, {},                               NOP_OPCODE},

	{"ld",   3, {REGISTER,IMMEDIATE,AT_REGISTER}, LD_OPCODE},

	{"st",   3, {REGISTER,IMMEDIATE,AT_REGISTER}, ST_OPCODE},

	{"beq",  1, {LOCATION},                       BEQ_OPCODE},

	{"bgt",  1, {LOCATION},                       BGT_OPCODE},

	{"b",    1, {LOCATION},                       B_OPCODE},

	{"call", 1, {LOCATION},                       CALL_OPCODE},

	{"ret",  1, {LOCATION},                       RET_OPCODE},


	// {"sbn",  3, {REGISTER,IMMEDIATE,LOCATION}, 29}, // sbn_im
	// {"sbn",  3, {REGISTER,REGISTER,LOCATION}, 30}, // sbn_reg

	//{"exit", 0, {NONE},       EXIT_PROGRAM},
};

#endif
