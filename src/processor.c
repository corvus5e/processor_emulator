
#include "processor.h"

#include <stdio.h>
#include <string.h>

#define mem_pc(p) (unsigned char)(p->mem[p->pc]) // get memory at program counter
#define mem_pci(p) (unsigned char)(p->mem[p->pc++]) // get memory at program counter and increments program counter, see -
#define mem_apci(p) (p->mem[mem_pci(p)]) // gets memory from memory with address where program counter points, pc incremented after
#define mem_areg(p, reg_num) (p->mem[(int)p->reg[reg_num]])

/* Processos instructions implementation*/

/* moves into register immediate value, ex: `mov r0 5` */
void mov_im(struct Processor* p){
	int reg_number = mem_pci(p);
	p->reg[reg_number] = mem_pci(p);
}

/* moves into register from address value, ex: `mov r0 r1`*/
void mov_reg(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int src_reg_number = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[src_reg_number];
}

/* Adds to a register immediate value, ex: `add r0 r1 5` */
void add_im(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_value = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] + b_value;
}

/* Adds to a register value of another register, ex: `add r0 r1 r2` */
void add_reg(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_reg_number = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] + p->reg[b_reg_number];

}

/* Subtracts from a register immediate value, ex: `sub r0 r1 5` */
void sub_im(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_value = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] - b_value;
}

/* Subtracts from a register value of another register, ex: `sub r0 r1 r2` */
void sub_reg(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_reg_number = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] - p->reg[b_reg_number];
}

/* Multiplies value from a register on an immediate value, ex: `mul r0 r1 5` */
void mul_im(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_value = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] * b_value;
}

/* Multiplies value from a register on value from another register, ex: `mul r0 r1 r2` */
void mul_reg(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_reg_number = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] * p->reg[b_reg_number];
}

/* Divides value from a register on an immediate value, ex: `div r0 r1 5` */
void div_im(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_value = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] / b_value;
}

/* Divides value from a register on value from another register, ex: `div r0 r1 r2` */
void div_reg(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_reg_number = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] / p->reg[b_reg_number];
}

void mod_im(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_value = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] % b_value;
}

void mod_reg(struct Processor* p){
	int dst_reg_number = mem_pci(p);
	int a_reg_number = mem_pci(p);
	int b_reg_number = mem_pci(p);
	p->reg[dst_reg_number] = p->reg[a_reg_number] % p->reg[b_reg_number];
}

void cmp_im(struct Processor* p){
	int reg_number = mem_pci(p);
	int cmp = p->reg[reg_number] - mem_pci(p);
	p->flag_E = cmp == 0;
	p->flag_GT = cmp > 0;
}

/* moves into register from address value, ex: `mov r0 r1`*/
void cmp_reg(struct Processor* p){
	int a_reg_number = mem_pci(p);
	int b_reg_number = mem_pci(p);
	int cmp = p->reg[a_reg_number] - p->reg[b_reg_number];
	p->flag_E = cmp == 0;
	p->flag_GT = cmp > 0;
}


void sbn_im(struct Processor* p) {
	int reg_number = mem_pci(p);
	p->reg[reg_number] -= mem_pci(p);
	int jump_addr = mem_pci(p);
	if(p->reg[reg_number] < 0) {
		p->pc = jump_addr;
	}
}

void sbn_reg(struct Processor* p) {
	int reg_number_a = mem_pci(p);
	int reg_number_b = mem_pci(p);
	p->reg[reg_number_a] -= p->reg[reg_number_b];
	int jump_addr = mem_pci(p);
	if(p->reg[reg_number_a] < 0) {
		p->pc = jump_addr;
	}
}


typedef void(*InstructionFunc)(struct Processor*);

static InstructionFunc _asm_funcs[] = {
	mov_reg,
	mov_im,
	add_im,
	add_reg,
	sub_im,
	sub_reg,
	mul_im,
	mul_reg,
	div_im,
	div_reg,
	mod_im,
	mod_reg,
	cmp_im,
	cmp_reg,

	/*Commands for exercise 1.7 */
	sbn_im,
	sbn_reg,
};

#define INSTRUCTIONS_COUNT sizeof(_asm_funcs)/sizeof(InstructionFunc)
#define EXIT_PROGRAM -1

static struct InstructionInfo _asm_info[] = {
	{"mov", 2, REGISTER|REGISTER, 0},
	{"mov", 2, REGISTER|IMMEDIATE, 1},

	{"add",  3, REGISTER|REGISTER|IMMEDIATE,    2}, // add_im
	{"add",  3, REGISTER|REGISTER|REGISTER,     3}, // add_reg
	{"sub",  3, REGISTER|REGISTER|IMMEDIATE,    4}, // sub_im
	{"sub",  3, REGISTER|REGISTER|REGISTER,     5}, // sub_reg
	{"mul",  3, REGISTER|REGISTER|IMMEDIATE,    6},
	{"mul",  3, REGISTER|REGISTER|REGISTER,     7},
	{"div",  3, REGISTER|REGISTER|IMMEDIATE,    8},
	{"div",  3, REGISTER|REGISTER|REGISTER,     9},
	{"mod",  3, REGISTER|REGISTER|IMMEDIATE,    10},
	{"mod",  3, REGISTER|REGISTER|REGISTER,     11},

	{"cmp", 2, REGISTER|IMMEDIATE, 12},
	{"cmp", 2, REGISTER|REGISTER, 13},

	{"sbn",  3, REGISTER|IMMEDIATE|LOCATION, 9}, // sbn_im
	{"sbn",  3, REGISTER|REGISTER|LOCATION, 10}, // sbn_reg
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
	int fn;
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
