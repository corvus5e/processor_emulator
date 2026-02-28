#include <stddef.h>
#include <stdbool.h>

#include "tests_at_home.h"
#include "compiler/asm_info.h"
#include "compiler/asm_compiler.h"

TEST(parse_tokens)
{
	const int str_types_num = 8;

	const char *line = "ld r1, 12[r2]\n";

	enum TokenType expected_types[] = {WORD, WORD, COMMA, WORD, LBRACKET, WORD, RBRACKET, NEW_LINE};

	const char **curr_pos = &line;
	for(int j = 0; j < str_types_num; ++j) {
		struct Token t = get_token(curr_pos);
		EXPECT_EQ(t.type, expected_types[j]);
	}

	return TEST_PASSED;
}

TEST(parse_ld_ins)
{
	const char *line = "ld r1, 12[r2]\n";

	struct InstructionInfo *inst_info;
	arg_values_array out_values;
	struct CompileError status = parse_line(line, &inst_info, out_values);
	if(status.error_code != COMPILE_SUCCESS) {
		printf("Error:%s\n", status.msg);
		return TEST_FAILED;
	}

	return TEST_PASSED;
}

TEST(parse_lsr_ins)
{
	const char *line = "lsr r1, r2, r3\n";

	struct InstructionInfo *inst_info;
	arg_values_array out_values;
	struct CompileError status = parse_line(line, &inst_info, out_values);
	if(status.error_code != COMPILE_SUCCESS) {
		printf("Error:%s\n", status.msg);
		return TEST_FAILED;
	}

	return TEST_PASSED;
}

int main() {
	return test_at_home_run();
}
