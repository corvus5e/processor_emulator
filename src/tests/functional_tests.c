
#ifndef SAMPLES_DIR
#define SAMPLES_DIR "" // Make code highlighting happy
#error "SAMPLES_DIR should be defined"
#endif

#include "tests_at_home.h"
#include "compiler/asm_compiler.h"

#define SAMPLE(x) SAMPLES_DIR "/" x

void print_program(struct vec_word *program) {
	for(int i = 0; i < program->size; ++i) {
		word inst = *vec_at_word(program, i);
		//TODO: Aka decode:
		int opcode = (inst >> OPCODE_SHIFT) & 0b00000000000000000000000000011111;
		int tmp = (inst << OPCODE_LEN);
		int mask = tmp < 0 ? 0xFFFFFFFF : 0x00000;
		int offset = (tmp >> OPCODE_LEN) | (mask << OPCODE_LEN);
		printf("[%d|%d]", opcode, offset);
	}
	printf("\n");
}

TEST(sample)
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("sample.asm"), &program), COMPILE_SUCCESS);
	print_program(&program);
	return 0;
}

TEST(exmaple_26) // mov, mul, sub instructions
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("example_26.asm"), &program), COMPILE_SUCCESS);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
}

TEST(exmaple_27) // mov, mul, div instructions
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("example_27.asm"), &program), COMPILE_SUCCESS);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
}

TEST(exmaple_27_1) // + cmp
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("example_27_1.asm"), &program), COMPILE_SUCCESS);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
}

TEST(exmaple_28) // and, or, not
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("example_28.asm"), &program), COMPILE_SUCCESS);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
}

TEST(exmaple_28_1) // lsl, lsr, asr
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("example_28_1.asm"), &program), COMPILE_SUCCESS);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
}


int main() {
	return test_at_home_run();
}
