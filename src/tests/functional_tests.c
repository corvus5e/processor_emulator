
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
	decode_program(&program);
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
