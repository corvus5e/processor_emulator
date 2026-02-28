#ifndef ASM_COMPILER_H_
#define ASM_COMPILER_H_

#include <stdbool.h>

#include "common/simple_risc_types.h"

#define T word
#include "common/vector.h"
#undef T

#include "asm_info.h"

#define COMPILE_ERROR -1
#define COMPILE_SUCCESS 0
#define LABEL_ARG 1

struct vec_LabelEntry;

enum TokenType {
	WORD,
	COMMA,
	LBRACKET, //[
	RBRACKET, //]
	NEW_LINE,
	COMMENT,
	STR_END,
};

struct Token {
	size_t len;
	enum TokenType type;
	const char *str;
};

struct CompileError {
	int error_code;
	int column;
	const char *msg;
};

int compile_program(const char *file_name, struct vec_word *program);

/*
 * `line_text` should end with newline and null-terminating character
 * */
struct CompileError parse_line(const char *line_text, struct InstructionInfo **found_instruction, arg_values_array out_args);

struct CompileError parse_arg(const char **curr_pos, enum InstructionArgType *out_type, int* out_value);

bool parse_register(int* out_value);

bool isLabel(const char *word, size_t len);

/* Returns `struct InstructionInfo*` using `instruction` 'WHERE' clause.
 * Returns NULL if no such instruction found.
 * If negative value passed into `instruction.args_num`, `arg_types` is not participated in search.
 * */
struct InstructionInfo* find_instruction_info(struct InstructionInfo search_pattern);

struct Token get_token(const char **curr_pos);

void unget_token(struct Token);

/* Converts `n` chars of string `s` to a number that fits char
 * Returns 1 is succeed to convert and stores result in `out`,
 * otherwise returns 0
 * */
bool strntoc(const char *s, int n, int *out);

#endif
