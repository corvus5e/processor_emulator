
#include "processor.h"

#include <stdio.h>
#include <string.h>

#define mem_pc(p) (unsigned char)(p->mem[p->pc]) // get memory at program counter
#define mem_pci(p) (unsigned char)(p->mem[p->pc++]) // get memory at program counter and increments program counter, see -
#define mem_apci(p) (p->mem[mem_pci(p)]) // gets memory from memory with address where program counter points, pc incremented after
#define mem_areg(p, reg_num) (p->mem[(int)p->reg[reg_num]])

/* Processos instructions implementation*/

/* loads into register immediate value, ex: `load r0 5` */
void load_im(struct Processor* p){
	int reg_number = mem_pci(p);
	p->reg[reg_number] = mem_pci(p);
}

/* loads into register from address value, ex: `load r0 @5`*/
void load_at_im(struct Processor* p){
	int reg_number = mem_pci(p);
	p->reg[reg_number] = mem_apci(p);
}

/* loads into register from address from register, ex: `load r1 @r0` */
void load_at_reg(struct Processor* p){
	int reg_number_a = mem_pci(p);
	int reg_number_b = mem_pci(p);
	p->reg[reg_number_a] = mem_areg(p, reg_number_b);
}

/* Adds to a register immediate value, ex: `add r0 5` */
void add_im(struct Processor* p){
	int reg_number = mem_pci(p);
	p->reg[reg_number] += mem_pci(p);
}

/* Adds to a register value of another register, ex: `add r0 r1` */
void add_reg(struct Processor* p){
	int reg_number_a = mem_pci(p);
	int reg_number_b = mem_pci(p);
	p->reg[reg_number_a] += p->reg[reg_number_b];
}

/* Jumps to a named label (label should be replaced by address) ex: jmp my_label*/
void jmp(struct Processor* p) {
	int address = mem_pci(p);
	p->pc = address;
}

/* Jumps to named label if register is non-zero, ex: jnz r0 my_label*/
void jnz(struct Processor* p) {
	int reg_number = mem_pci(p);
	int address = mem_pci(p);
	if(p->reg[reg_number]){
		p->pc = address;
	}
}

typedef void(*InstructionFunc)(struct Processor*);

static InstructionFunc _asm_funcs[] = {
	load_im,
	load_at_im,
	load_at_reg,
	add_im,
	add_reg,
	jmp,
	jnz,
};

#define INSTRUCTIONS_COUNT sizeof(_asm_funcs)/sizeof(InstructionFunc)
#define EXIT_PROGRAM 127

static struct InstructionInfo _asm_info[] = {
	{"load", 2, REGISTER|IMMEDIATE,    0}, // load_im
	{"load", 2, REGISTER|AT_IMMEDIATE, 1}, // load_at_im
	{"load", 2, REGISTER|AT_REGISTER,  2}, // load_at_reg
	{"add",  2, REGISTER|IMMEDIATE,    3}, // add_im
	{"add",  2, REGISTER|REGISTER,     4}, // add_reg
	{"jmp",  1, LOCATION,              5}, // jmp
	{"jnz",  2, REGISTER|LOCATION,     6}, // jzn
	{"exit", 0, NONE,       EXIT_PROGRAM},
};


int get_asm_instructions_info(struct InstructionInfo **asm_info) {
	*asm_info = _asm_info;
	return sizeof(_asm_info)/sizeof(struct InstructionInfo);
}

int init_processor(const char *program, unsigned char len, struct Processor * const processor) {
	for(int i = 0;i < len; ++i)
		processor->mem[i] = program[i];

	processor->pc = 0;

	for(int i = 0; i < REG_COUNT; ++i)
		processor->reg[i] = 0;

	return 1;
}

int run_program(char *program, size_t len, struct Processor * const p) {
	init_processor(program, len, p);

	//fetch
	unsigned char fn;
	while((fn = p->mem[p->pc++]) != EXIT_PROGRAM) {
		//decode
		if(fn > INSTRUCTIONS_COUNT - 1) {
			printf("INT: Unknow instruction %u\n", fn);
			return 1;
		}

		//execute
		_asm_funcs[fn](p);
	}

	return 0;
}
