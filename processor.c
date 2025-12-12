
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
	unsigned char reg_number = mem_pci(p);
	p->reg[reg_number] = mem_pci(p);
}

/* loads into register from address value, ex: `load r0 @5`*/
void load_at_im(struct Processor* p){
	unsigned char reg_number = mem_pci(p);
	p->reg[reg_number] = mem_apci(p);
}

/* loads into register from address from register, ex: `load r1 @r0` */
void load_at_reg(struct Processor* p){
	unsigned char reg_number_a = mem_pci(p);
	unsigned char reg_number_b = mem_pci(p);
	p->reg[reg_number_a] = mem_areg(p, reg_number_b);
}

/* Adds to a register immediate value, ex: `add r0 5` */
void add_im(struct Processor* p){
	unsigned char reg_number = mem_pci(p);
	p->reg[reg_number] += mem_pci(p);
}

/* Adds to a register value of another register, ex: `add r0 r1` */
void add_reg(struct Processor* p){
	unsigned char reg_number_a = mem_pci(p);
	unsigned char reg_number_b = mem_pci(p);
	p->reg[reg_number_a] += p->reg[reg_number_b];
}

/* Jumps to address or named label, ex: jmp 3; jmp my_label*/
void jmp(struct Processor* p) {
	unsigned char new_pc = mem_pci(p);
	p->pc = new_pc;
}

void jnz(struct Processor* p) {}

typedef void(*InstructionFunc)(struct Processor*);

static InstructionFunc _asm_funcs[] = {
	load_im,
	load_at_im,
	load_at_reg,
	add_im,
	add_reg,
	jmp,
	jnz
};

#define EXIT_PROGRAM 127

static struct InstructionInfo _asm_info[] = {
	{"load", 2, REGISTER|IMMEDIATE,    0}, // load_im
	{"load", 2, REGISTER|AT_IMMEDIATE, 1}, // load_at_im
	{"load", 2, REGISTER|AT_REGISTER,  2}, // load_at_reg
	{"add",  2, REGISTER|IMMEDIATE,    3},
	{"add",  2, REGISTER|REGISTER,     4},
	{"jmp",  1, LOCATION,              5},
	{"jnz",  2, REGISTER|LOCATION,     6},
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
		if(fn > 4) {
			printf("INT: Unknow instruction %u\n", fn);
			return 1;
		}

		//execute
		_asm_funcs[fn](p);
	}

	return 0;
}
