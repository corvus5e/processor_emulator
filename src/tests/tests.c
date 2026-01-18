
#ifndef SAMPLES_DIR
#define SAMPLES_DIR "" // Make code highlighting happy
#error "SAMPLES_DIR should be defined"
#endif

#include "tests_at_home.h"
#include "emulator.h"

#define SAMPLE(x) SAMPLES_DIR "/" x

TEST_DISABLED(sample,
{
	struct Processor p;
	compile_and_run_program(SAMPLE("sample.asm"), &p);
	return 0;
})

TEST(exmaple_26, // mov, mul, sub instructions
{
	struct Processor p = {0};
	EXPECT_EQ(compile_and_run_program(SAMPLE("example_26.asm"), &p), 0); // Compiled OK
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
})

TEST(exmaple_27, // mov, mul, div instructions
{
	struct Processor p = {0};
	EXPECT_EQ(compile_and_run_program(SAMPLE("example_27.asm"), &p), 0);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
})

TEST(exmaple_27_1, // + cmp
{
	struct Processor p = {0};
	EXPECT_EQ(compile_and_run_program(SAMPLE("example_27_1.asm"), &p), 0);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
})

TEST(exmaple_28, // and, or, not
{
	struct Processor p = {0};
	EXPECT_EQ(compile_and_run_program(SAMPLE("example_28.asm"), &p), 0);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
})

TEST(exmaple_28_1, // lsl, lsr, asr
{
	struct Processor p = {0};
	EXPECT_EQ(compile_and_run_program(SAMPLE("example_28_1.asm"), &p), 0);
	//EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
})


int main() {
	return test_at_home_run();
}
