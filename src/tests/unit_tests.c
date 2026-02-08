#include <stddef.h>
#include <string.h>

#include "tests_at_home.h"
#include "compiler/asm_info.h"


enum InstructionArgType parse_arguments(const char *args_line, size_t len, char* out_value);

TEST(parse_args, {
	const char *line = " r1, 12[r2]";
	const int MAX_ARGS = 10;
	char values[MAX_ARGS];
	enum InstructionArgType all_types = parse_arguments(line, strlen(line), values);
	return TEST_PASSED;
})

TEST(parse_args_bad, {
	const char *line = " r1, 12[r2]";
	const int MAX_ARGS = 10;
	char values[MAX_ARGS];
	enum InstructionArgType all_types = parse_arguments(line, strlen(line), values);
	return TEST_PASSED;
})

int main() {
	return test_at_home_run();
}
