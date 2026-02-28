#ifndef _ASM_INFO_H_
#define _ASM_INFO_H_

#include <stdint.h>

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
	char code;
};

#define EXIT_PROGRAM -1

static struct InstructionInfo asm_info[] = {
	{"mov",  2, {REGISTER, REGISTER},  0},
	{"mov",  2, {REGISTER, IMMEDIATE}, 1},

	{"add",  3, {REGISTER,REGISTER,IMMEDIATE}, 2}, // add_im
	{"add",  3, {REGISTER,REGISTER,REGISTER},  3}, // add_reg
	{"sub",  3, {REGISTER,REGISTER,IMMEDIATE}, 4}, // sub_im
	{"sub",  3, {REGISTER,REGISTER,REGISTER},  5}, // sub_reg
	{"mul",  3, {REGISTER,REGISTER,IMMEDIATE}, 6},
	{"mul",  3, {REGISTER,REGISTER,REGISTER},  7},
	{"div",  3, {REGISTER,REGISTER,IMMEDIATE}, 8},
	{"div",  3, {REGISTER,REGISTER,REGISTER},  9},
	{"mod",  3, {REGISTER,REGISTER,IMMEDIATE}, 10},
	{"mod",  3, {REGISTER,REGISTER,REGISTER},  11},

	{"cmp", 2,  {REGISTER,IMMEDIATE},          12},
	{"cmp", 2,  {REGISTER,REGISTER},           13},

	{"and", 3,  {REGISTER,REGISTER,REGISTER},  14},
	{"and", 3,  {REGISTER,REGISTER,IMMEDIATE}, 15},
	{"or",  3,  {REGISTER,REGISTER,REGISTER},  16},
	{"or",  3,  {REGISTER,REGISTER,IMMEDIATE}, 17},
	{"not", 2,  {REGISTER,REGISTER},           18},
	{"not", 2,  {REGISTER,IMMEDIATE},          19},

	{"lsl", 3,  {REGISTER,REGISTER,REGISTER},  20},
	{"lsl", 3,  {REGISTER,REGISTER,IMMEDIATE}, 21},
	{"lsr", 3,  {REGISTER,REGISTER,REGISTER},  22},
	{"lsr", 3,  {REGISTER,REGISTER,IMMEDIATE}, 23},
	{"asr", 3,  {REGISTER,REGISTER,REGISTER},  24},
	{"asr", 3,  {REGISTER,REGISTER,IMMEDIATE}, 25},

	{"ld",  3,  {REGISTER,IMMEDIATE,AT_REGISTER}, 26},
	{"st",  3,  {REGISTER,IMMEDIATE,AT_REGISTER}, 27},

	{"b",   1,  {LOCATION},                    28},

	{"sbn",  3, {REGISTER,IMMEDIATE,LOCATION}, 9}, // sbn_im
	{"sbn",  3, {REGISTER,REGISTER,LOCATION}, 10}, // sbn_reg
	{"exit", 0, {NONE},       EXIT_PROGRAM},
};

#endif
