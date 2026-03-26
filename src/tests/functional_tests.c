
#ifndef SAMPLES_DIR
#error "SAMPLES_DIR should be defined"
#endif

#include "tests_at_home.h"
#include "compiler/asm_compiler.h"
#include "processor/processor.h"

#define SAMPLE(x) SAMPLES_DIR "/" x

void decode_program(struct vec_word *program) {
	char buf[50]; // Should be enough for one instruction;
	for(int i = 0; i < program->size; ++i) {
		word inst = *vec_at_word(program, i);
		disassembly(inst, buf);
		printf("%s", buf);
	}
}

TEST(sample)
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("sample.asm"), &program), COMPILE_SUCCESS);
	struct Processor p = {};
	load_program_from_mem(&program, &p);
	decode_program(&program);
	run_processor(&p);
	EXPECT_EQ(p.reg[1], -27)
	EXPECT_EQ(p.reg[3], -27)
	EXPECT_EQ(p.reg[4], -81)
	EXPECT_EQ(p.reg[5], -131)
	EXPECT_EQ(p.reg[6], -13)
	EXPECT_EQ(load_word(p.mem + 120), p.reg[6]);
	EXPECT_EQ(p.reg[7], 40);
	EXPECT_EQ(p.reg[8], 11);
	EXPECT_EQ(p.reg[9], p.reg[1]);
	EXPECT_EQ(p.flag_E, 0);
	EXPECT_EQ(p.flag_GT, 1);
	return TEST_PASSED;
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

TEST(exmaple_29) // Factorial of a number
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("example_29.asm"), &program), COMPILE_SUCCESS);
	struct Processor p = {};
	load_program_from_mem(&program, &p);
	p.reg[0] = 5; // Find factorial of 5
	decode_program(&program);
	run_processor(&p);
	EXPECT_EQ(p.reg[1], 120);
	return TEST_PASSED;
}

TEST(exmaple_30) // Is number prime
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("example_30.asm"), &program), COMPILE_SUCCESS);
	decode_program(&program);

	struct Processor p = {};
	load_program_from_mem(&program, &p);

	p.reg[1] = 5; // Is 5 prime ?
	run_processor(&p);

	EXPECT_EQ(p.reg[0], 1);
	return TEST_PASSED;
}

TEST(exmaple_31) // Least common multiple
{
	struct vec_word program;
	vec_init_word(&program);
	EXPECT_EQ(compile_program(SAMPLE("example_31.asm"), &program), COMPILE_SUCCESS);
	decode_program(&program);

	struct Processor p = {};
	load_program_from_mem(&program, &p);

	p.reg[1] = 9; // LCM of 9 and 12
	p.reg[2] = 12;
	run_processor(&p);

	EXPECT_EQ(p.reg[0], 36);
	return TEST_PASSED;
}

int main() {
	return test_at_home_run();
}
