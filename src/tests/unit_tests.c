#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "tests_at_home.h"
#include "compiler/asm_info.h"
#include "compiler/asm_compiler.h"

TEST(parce_positive_int)
{
	char *str = "49";
	int value;
	strntoi(str, strlen(str), &value);
	EXPECT_EQ(value, 49);
	return TEST_PASSED;
}

TEST(parce_negative_int)
{
	char *str = "-49";
	int value;
	strntoi(str, strlen(str), &value);
	EXPECT_EQ(value, -49);
	return TEST_PASSED;

}

TEST(parce_positive_hex)
{
	char *str = "0x0000000F";
	int value;
	strntoi(str, strlen(str), &value);
	EXPECT_EQ(value, 0x0000000F);
	return TEST_PASSED;

}

TEST(parce_negative_hex)
{
	char *str = "0xFFFFFFF1";
	int value;
	strntoi(str, strlen(str), &value);
	EXPECT_EQ(value, 0xFFFFFFF1);
	return TEST_PASSED;
}



TEST(parse_tokens_square_brackets)
{

	const char *line = "ld r1, 12[r2]\n";

	enum TokenType expected_types[] = {WORD, WORD, COMMA, WORD, LBRACKET, WORD, RBRACKET, NEW_LINE};
	const int str_types_num = sizeof(expected_types)/sizeof(enum TokenType);

	const char **curr_pos = &line;
	for(int j = 0; j < str_types_num; ++j) {
		struct Token t = get_token_sb(curr_pos);
		EXPECT_EQ(t.type, expected_types[j]);
	}

	return TEST_PASSED;
}

TEST(parse_tokens)
{
	const char *line = "ld r1, 12[r2]\n";

	enum TokenType expected_types[] = {WORD, WORD, COMMA, WORD, NEW_LINE};
	const int str_types_num = sizeof(expected_types)/sizeof(enum TokenType);

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
	struct CompileError status = parse_line(line, &inst_info, out_values, 0);
	if(status.error_code != COMPILE_SUCCESS) {
		printf("%s\n", status.msg);
	}

	EXPECT_EQ(status.error_code, COMPILE_SUCCESS);

	return TEST_PASSED;
}

TEST(parse_add_ins)
{
	const char *line = "add r1, r2, -27\n";

	struct InstructionInfo *inst_info;
	arg_values_array out_values;
	EXPECT_EQ(parse_line(line, &inst_info, out_values, 0).error_code, COMPILE_SUCCESS);
	return TEST_PASSED;
}


TEST(parse_lsr_ins)
{
	const char *line = "lsr r1, r2, r3\n";

	struct InstructionInfo *inst_info;
	arg_values_array out_values;
	struct CompileError status = parse_line(line, &inst_info, out_values, 0);
	if(status.error_code != COMPILE_SUCCESS) {
		printf("%s\n", status.msg);
	}
	EXPECT_EQ(status.error_code, COMPILE_SUCCESS);
	return TEST_PASSED;
}

TEST(parse_arguments_all_registers)
{
	const char *line = "r1, r2, r3\n";

	struct InstructionInfo inst_info = {};
	arg_values_array out_values;
	struct CompileError status = parse_arguments(&line, &inst_info, out_values);
	if(status.error_code != COMPILE_SUCCESS) {
		printf("%s\n", status.msg);
	}

	EXPECT_EQ(status.error_code, COMPILE_SUCCESS);

	EXPECT_EQ(inst_info.arg_types[0], REGISTER);
	EXPECT_EQ(inst_info.arg_types[1], REGISTER);
	EXPECT_EQ(inst_info.arg_types[2], REGISTER);

	EXPECT_EQ(out_values[0], 1);
	EXPECT_EQ(out_values[1], 2);
	EXPECT_EQ(out_values[2], 3);
	return TEST_PASSED;
}

TEST(parse_arguments_reg_and_imm)
{
	const char *line = "r1, r2, 42\n";

	struct InstructionInfo inst_info = {};
	arg_values_array out_values;
	struct CompileError status = parse_arguments(&line, &inst_info, out_values);
	if(status.error_code != COMPILE_SUCCESS) {
		printf("%s\n", status.msg);
	}

	EXPECT_EQ(status.error_code, COMPILE_SUCCESS);

	EXPECT_EQ(inst_info.arg_types[0], REGISTER);
	EXPECT_EQ(inst_info.arg_types[1], REGISTER);
	EXPECT_EQ(inst_info.arg_types[2], IMMEDIATE);

	EXPECT_EQ(out_values[0], 1);
	EXPECT_EQ(out_values[1], 2);
	EXPECT_EQ(out_values[2], 42);
	return TEST_PASSED;
}

TEST(parse_arguments_reg_and_at_reg)
{
	const char *line = "r1, 42[r2]\n";

	struct InstructionInfo inst_info = {};
	arg_values_array out_values;
	struct CompileError status = parse_arguments(&line, &inst_info, out_values);
	if(status.error_code != COMPILE_SUCCESS) {
		printf("%s\n", status.msg);
	}

	EXPECT_EQ(status.error_code, COMPILE_SUCCESS);

	EXPECT_EQ(inst_info.args_num, 3);

	EXPECT_EQ(inst_info.arg_types[0], REGISTER);
	EXPECT_EQ(inst_info.arg_types[1], IMMEDIATE);
	EXPECT_EQ(inst_info.arg_types[2], AT_REGISTER);

	EXPECT_EQ(out_values[0], 1);
	EXPECT_EQ(out_values[1], 42);
	EXPECT_EQ(out_values[2], 2);
	return TEST_PASSED;
}



int main() {
	return test_at_home_run();
}
