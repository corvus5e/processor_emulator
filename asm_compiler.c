
#include "processor.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define T char
#include "vector.h"
#undef T

#define MAX_COMMAND_LEN 5
#define COMPILE_ERROR -1

int compile_program(const char *file_name, struct vec_char *program);

/* Translated one line of assembly into machine instructions
 * The line is expected to be in format either:
 * `label_name:` or
 * `<command> [args1 [arg2]]`
 * and writes them into `out_program` starting from `free_pos`
 * Returns the number of written instructions + arguments into `out program`
 * `line_num` is for compile error reporting
 * */
int translate_line(const char *line_text, size_t len, struct vec_char *out_program, int line_num);

/* Returns InstructionArgType and writes value int `out_value`
 * is type is not `NONE`
 * */
enum InstructionArgType translate_argument(const char *arg, size_t len, char* out_value);

/* Finds the beginning of the word, stores it's pointer into `word_begin`.
 * and returns the length of the found word, otherwise returns 0.
 * Skips all blanks.
 * */
int find_word(const char *text, const char **word_begin);

/* Returns `struct InstructionInfo*` for `instruction`, NULL if no such instruction
 * if zero value passed into `search_type`, `arg_types` is not participated in search
 * */
struct InstructionInfo* find_instruction_info(const char *instuction, size_t len, int search_type, enum InstructionArgType arg_types);

/* Converts `n` chars of string `s` to a number that fits char
 * Returns 1 is succeed to convert and stores result in `out`,
 * otherwise returns 0
 * */
int strntoc(const char *s, int n, char *out);

/* Prints `n` chars of line `s`
 * */
void printn(const char *s, size_t n);

void print_compile_error(const char *msr, int line_num, const char *word, size_t len);



/* --- Implementation ---- */

struct LabelEntry {
	char *name;
	size_t length;
	unsigned char declaration_location;
	unsigned char point_location;
};

int compile_program(const char *file_name, struct vec_char *program)
{
	FILE *f = fopen(file_name, "r");
	if(!f){
		printf("ERROR: failed to open file %s\n", file_name);
		return COMPILE_ERROR;
	}
	
	int status = 0;
	char *line = NULL;
	size_t n;
	ssize_t nread;
	int line_num = 0;

        while ((nread = getline(&line, &n, f)) != -1 &&
               (status = translate_line(line, nread, program, ++line_num)) !=
                   COMPILE_ERROR)
          ;

        free(line);
	fclose(f);

	return status;
}

int translate_line(const char *line, size_t line_len, struct vec_char *out_program, int line_num)
{
	int written = 0;
	const char *curr_pos = line;
	const char *word = NULL; // Beginning of the word

	int word_len = find_word(curr_pos, &word);
	if( word_len == 0 || word[0] == '#') // Empty line or comment
		return 0;

	if(word[word_len-1] == ':'){ // This is a label, like `foo:`
		//TODO: handle_label();
		return 0;
	}

	// Parse instruction
	if(word_len > MAX_COMMAND_LEN) {
		print_compile_error("Unknown command ", line_num, word, word_len);
		return COMPILE_ERROR;
	}

	struct InstructionInfo *inst_info = find_instruction_info(word, word_len, 0, 0);
	if(!inst_info){
		print_compile_error("Unknown command ", line_num, word, word_len);
		return COMPILE_ERROR;
	}

	curr_pos = word + word_len;

	const char *inst = word;
	int inst_len = word_len;

	// Pasre arguments
	enum InstructionArgType args_types = NONE;
	char arg_values[2]; // We have no more that two values for now
	for(int i = 0; i < inst_info->args_num; ++i) {
		word_len = find_word(curr_pos, &word);
		if(word_len == 0) {
			print_compile_error("Expected an argument for", line_num, inst, inst_len);
			return COMPILE_ERROR;
		}
		args_types |= translate_argument(word, word_len, &arg_values[i]);
		curr_pos = word + word_len;
	}

	inst_info = find_instruction_info(inst, inst_len, 1, args_types);
	if(!inst_info) {
		print_compile_error("Unexpected argument type", line_num, NULL, 0);
		return COMPILE_ERROR;
	}

	vec_push_back_char(out_program, inst_info->code);
	for(int i = 0; i < inst_info->args_num; ++i){
		vec_push_back_char(out_program, arg_values[i]);
	}

	written += inst_info->args_num + 1;

	return written;
}

enum InstructionArgType translate_argument(const char *arg, size_t len, char* out_value) {
	if(len == 0)
		return NONE;

	if(len == 2 && arg[0] == 'r' && isdigit(arg[1])) {
		*out_value = arg[1] - '0'; // We have less 10 registers for now
		return REGISTER;
	}

	if(len == 3 && arg[0] == '@' && arg[1] == 'r' && isdigit(arg[2])) {
		*out_value = arg[2] - '0';
		return AT_REGISTER;
	}

	if(len > 1 && arg[0] == '@' && strntoc(arg + 1, len - 1, out_value)) {
		return AT_IMMEDIATE;
	}

	if(strntoc(arg, len, out_value)) {
		return IMMEDIATE;
	}

	return NONE;
}

int find_word(const char *text, const char **word_begin)
{
	int word_len = 0;

	// Skip spaces
	for(;text && *text != '\0' && isspace(*text); ++text)
		;

	*word_begin = text;

	//Count the length of the word
	for(;text && *text != '\0' && !isspace(*text); ++text, ++word_len)
		;

	return word_len;
}

int strntoc(const char *s, int n, char *out) {
	--n;
	int mul = 1;
	int res = 0;
	while(n >=0 && isdigit(s[n])) {
		res += (s[n] - '0') * mul;
		mul *= 10;
		--n;
	}

	if(n < 0 && res <= UCHAR_MAX) { // all chars passed check and fits char
		*out = res;
		return 1;
	}

	return 0;
}

struct InstructionInfo* find_instruction_info(const char *instuction, size_t len, int search_type, enum InstructionArgType arg_types)
{
	struct InstructionInfo *asm_info = NULL;
	int size = get_asm_instructions_info(&asm_info);

	for(int i = 0; i < size; ++i) {
          if (strncmp(instuction, asm_info[i].name, len) == 0 &&
			  (!search_type || arg_types == asm_info[i].args_types)) {
		  return asm_info + i;
          }
        }

	return NULL;
}

void printn(const char *s, size_t n) {
	while(n--) putchar(*s++);
}

void print_compile_error(const char *msg, int line_num, const char *word, size_t len) {
	printf("ERROR: %s ", msg);
	if(word)
		printn(word, len);
	printf(" at line %d\n", line_num);
}
