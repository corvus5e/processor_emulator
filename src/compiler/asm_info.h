#ifndef _ASM_INFO_H_
#define _ASM_INFO_H_

#include <stdint.h>
#include <common/simple_risc_types.h>

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

#define WORD_LEN 32
#define OPCODE_SHIFT 27
#define OPCODE_LEN 5
#define MAX_OFFSET 1 << 26

/* Table 3.10 from Chapter 3 */
static struct InstructionInfo asm_info[] = {

	{"add",  3, {REGISTER,REGISTER,IMMEDIATE},    0}, // add_im
	{"add",  3, {REGISTER,REGISTER,REGISTER},     0}, // add_reg

	{"sub",  3, {REGISTER,REGISTER,IMMEDIATE},    1}, // sub_im
	{"sub",  3, {REGISTER,REGISTER,REGISTER},     1}, // sub_reg

	{"mul",  3, {REGISTER,REGISTER,IMMEDIATE},    2},
	{"mul",  3, {REGISTER,REGISTER,REGISTER},     2},

	{"div",  3, {REGISTER,REGISTER,IMMEDIATE},    3},
	{"div",  3, {REGISTER,REGISTER,REGISTER},     3},

	{"mod",  3, {REGISTER,REGISTER,IMMEDIATE},    4},
	{"mod",  3, {REGISTER,REGISTER,REGISTER},     4},

	{"cmp",  2, {REGISTER,IMMEDIATE},             5},
	{"cmp",  2, {REGISTER,REGISTER},              5},

	{"and",  3, {REGISTER,REGISTER,REGISTER},     6},
	{"and",  3, {REGISTER,REGISTER,IMMEDIATE},    6},

	{"or",   3, {REGISTER,REGISTER,REGISTER},     7},
	{"or",   3, {REGISTER,REGISTER,IMMEDIATE},    7},

	{"not",  2, {REGISTER,REGISTER},              8},
	{"not",  2, {REGISTER,IMMEDIATE},             8},

	{"mov",  2, {REGISTER, REGISTER},             9},
	{"mov",  2, {REGISTER, IMMEDIATE},            9},

	{"lsl",  3, {REGISTER,REGISTER,REGISTER},     10},
	{"lsl",  3, {REGISTER,REGISTER,IMMEDIATE},    10},

	{"lsr",  3, {REGISTER,REGISTER,REGISTER},     11},
	{"lsr",  3, {REGISTER,REGISTER,IMMEDIATE},    11},

	{"asr",  3, {REGISTER,REGISTER,REGISTER},     12},
	{"asr",  3, {REGISTER,REGISTER,IMMEDIATE},    12},

	{"nop",  0, {},                               13},

	{"ld",   3, {REGISTER,IMMEDIATE,AT_REGISTER}, 14},

	{"st",   3, {REGISTER,IMMEDIATE,AT_REGISTER}, 15},

	{"beq",  1, {LOCATION},                       16},

	{"bgt",  1, {LOCATION},                       17},

	{"b",    1, {LOCATION},                       18},

	{"call", 1, {LOCATION},                       19},

	{"ret",  1, {LOCATION},                       20},


	// {"sbn",  3, {REGISTER,IMMEDIATE,LOCATION}, 29}, // sbn_im
	// {"sbn",  3, {REGISTER,REGISTER,LOCATION}, 30}, // sbn_reg

	//{"exit", 0, {NONE},       EXIT_PROGRAM},
};

#endif
