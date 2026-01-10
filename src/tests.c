
#ifndef SAMPLES_DIR
#define SAMPLES_DIR "" // Make code highlighting happy
#error "SAMPLES_DIR should be defined"
#endif

#include "emulator.h"
#include "tests_at_home.h"

#define SAMPLE(x) SAMPLES_DIR "/" x

TEST_DISABLED(sample,
{
	struct Processor p;
	compile_and_run_program(SAMPLE("sample.asm"), &p);
	return 0;
})

TEST(exmaple_26,
{
	struct Processor p = {0};
	compile_and_run_program(SAMPLE("example_26.asm"), &p);
	EXPECT_EQ(p.reg[4], 37);
	return TEST_PASSED;
})

int main() {
	return test_at_home_run();
}
