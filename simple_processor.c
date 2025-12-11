#include "processor.h"

struct InstructionInfo asm_info[] = {
	{"write", 2},
	{"load",  2},
	{"add",   1},
};

void write(struct Processor *p){
}

void load(struct Processor *p){
}

void add(struct Processor *p){
}

InstructionFunc asm_fnc[] = {write, load, add};

