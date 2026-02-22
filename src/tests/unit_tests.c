#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "tests_at_home.h"
#include "compiler/asm_info.h"
#include "compiler/asm_compiler.h"

bool compare_arg_types(arg_types_array res, arg_types_array ref, int args_size) {
	for(int i = 0; i < args_size; ++i) {
		if(res[i] != ref[i]) return false;
	}
	return true;
}

TEST(parse_tokens)
{
	const int str_types_num = 8;

	const char *line = "ld r1, 12[r2]\n";

	enum TokenType expected_types[] = {WORD, WORD, COMMA, WORD, LBRACKET, WORD, RBRACKET, NEW_LINE};

	const char **curr_pos = &line;
	for(int j = 0; j < str_types_num; ++j) {
		struct Token t = next_token(curr_pos);
		printf("(%.*s)\n", (int)t.len, t.str);
		EXPECT_EQ(t.type, expected_types[j]);
	}

	return TEST_PASSED;
}

TEST_DISABLED(parse_args)
{
	const char *line = " r1, 12[r2]";

	char values[MAX_ARGS_NUM];
	arg_types_array result_types;
	arg_types_array expected_types = {REGISTER, IMMEDIATE, AT_REGISTER};
	const int expected_args_count= 3;
	int args_count = 0;
	EXPECT_EQ(COMPILE_SUCCESS, parse_arguments(line, strlen(line), values, result_types, &args_count));
	EXPECT_EQ(expected_args_count, args_count);
	EXPECT_EQ(true, compare_arg_types(result_types, expected_types, args_count));
	return TEST_PASSED;
}

TEST_DISABLED(parse_args_bad)
{
	const char *line = "r1, 12[r2]";

	char values[MAX_ARGS_NUM];
	arg_types_array result_types;
	arg_types_array expected_types = {REGISTER, IMMEDIATE, AT_REGISTER};
	const int expected_args_count= 3;
	int args_count = 0;
	EXPECT_EQ(COMPILE_SUCCESS, parse_arguments(line, strlen(line), values, result_types, &args_count));
	EXPECT_EQ(expected_args_count, args_count);
	EXPECT_EQ(false, compare_arg_types(result_types, expected_types, args_count));
	return TEST_PASSED;
}

int main() {
	return test_at_home_run();
}
