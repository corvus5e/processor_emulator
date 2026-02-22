#ifndef ASM_COMPILER_H_
#define ASM_COMPILER_H_

#include "common/simple_risc_types.h"

#define T word
#include "common/vector.h"
#undef T

#include "asm_info.h"

#define COMPILE_ERROR -1
#define COMPILE_SUCCESS 0

struct vec_LabelEntry;

enum TokenType {
	WORD,
	COMMA,
	LBRACKET, //[
	RBRACKET, //]
	NEW_LINE,
	STR_END,
};

struct Token {
	size_t len;
	enum TokenType type;
	const char *str;
};

int compile_program(const char *file_name, struct vec_word *program);

/* Translated one line of assembly into machine instructions
 * The line is expected to be in format either:
 * `label_name:` or
 * `<command> [args1 [arg2]]`
 * and writes them into `out_program` starting from `free_pos`
 * Returns the number of written instructions + arguments into `out program`
 * */
int translate_line(const char *line_text, size_t len, struct vec_LabelEntry *label_table, struct vec_word *out_program, int line_num);

/* Returns 0 on success
 * */
int parse_arguments(const char *args_line, size_t len, char* out_value, arg_types_array out_arg_types, int *out_args_count);

/* Returns InstructionArgType and writes value int `out_value`
 * is type is not `NONE`
 * */
enum InstructionArgType translate_argument(const char *arg, size_t len, char* out_value);

/* Returns InstructionArgType and writes value int `out_value`
 * is type is not `NONE`
 * */
enum InstructionArgType translate_argument2(const struct Token *tokem, arg_values_array out_values);

struct Token next_token(const char **curr_pos);

#endif
