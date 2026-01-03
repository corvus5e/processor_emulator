#include "asm_compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define T char
#include "vector.h"
#undef T

#include "processor.h"

#define MAX_COMMAND_LEN 5
#define MAX_ARGS 10 //TODO: Make dynamic later
#define SPECIAL_LABEL '_' // Label pointing to the next command (aka "do nothing" label)

typedef struct LabelEntry {
	char *name;
	int location;
	struct vec_char jumps; /* exact positions in compiled code where label is used */
} LabelEntry;

#define T LabelEntry
#include "vector.h"
#undef T

/* Translated one line of assembly into machine instructions
 * The line is expected to be in format either:
 * `label_name:` or
 * `<command> [args1 [arg2]]`
 * and writes them into `out_program` starting from `free_pos`
 * Returns the number of written instructions + arguments into `out program`
 * */
int translate_line(const char *line_text, size_t len, struct vec_LabelEntry *label_table, struct vec_char *out_program, int line_num);

/* Returns InstructionArgType and writes value int `out_value`
 * is type is not `NONE`
 * */
enum InstructionArgType translate_argument(const char *arg, size_t len, char* out_value);

/* Replaces labels with needed addresses for jmp instructions in compiled program
 * Returns 1 if succeeded, 0 otherwise
 * */
int resolve_labels(struct vec_char *out_program, struct vec_LabelEntry *label_table);

/* Finds the beginning of the word, stores it's pointer into `word_begin`.
 * `curr_pos` stores pointer to a position where search finished. Useful to search next word.
 * and returns the length of the found word, otherwise returns 0.
 * Skips all blanks.
 * */
int find_word(const char **curr_pos, const char **word_begin);

/* Returns `struct InstructionInfo*` for `instruction`, NULL if no such instruction
 * if zero value passed into `search_type`, `arg_types` is not participated in search
 * */
struct InstructionInfo* find_instruction_info(const char *instuction, size_t len, int search_type, enum InstructionArgType arg_types);

/* Tries to add label into the `label_table`.
 * On success returns pointer to newly added entry, otherwise returns NULL.
 * `jump` is a location of where the label is used.
 * */
LabelEntry* handle_label(const char *name, size_t len, struct vec_LabelEntry *label_table, int location, int jump);

/* Finds a label with `name` in `label_table`.
 * Returns NULL is not found.
 * */
LabelEntry* find_label(const char *name, struct vec_LabelEntry *label_table);

/* Converts `n` chars of string `s` to a number that fits char
 * Returns 1 is succeed to convert and stores result in `out`,
 * otherwise returns 0
 * */
int strntoc(const char *s, int n, char *out);

/* Prints `n` chars of line `s`
 * */
void printn(const char *s, size_t n);

void print_compile_error(const char *msr, int line_num, const char *word, size_t len);

void print_label_table(struct vec_LabelEntry *label_table);


/* --- Implementation ---- */

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
	struct vec_LabelEntry label_table;
	vec_init_LabelEntry(&label_table);

        while ((nread = getline(&line, &n, f)) != -1 &&
               (status = translate_line(line, nread, &label_table, program, ++line_num)) !=
                   COMPILE_ERROR)
          ;

        free(line);
	fclose(f);

	if(status == COMPILE_ERROR)
		return status;

	//print_label_table(&label_table);

	return resolve_labels(program, &label_table) == 1 ? 0 : COMPILE_ERROR;
}

int translate_line(const char *line, size_t line_len, struct vec_LabelEntry *label_table, struct vec_char *out_program, int line_num)
{
	int written = 0;
	const char *curr_pos = line;
	const char *word = NULL; // Beginning of the word

	int word_len = find_word(&curr_pos, &word);
	if( word_len == 0 || word[0] == '#') // Empty line or comment
		return 0;

	if(word[word_len-1] == ':'){ // This is a label, like `foo:`
		if(!handle_label(word, word_len - 1, label_table, out_program->size, -1/*aka no jumps yet*/))
			return COMPILE_ERROR;
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

	const char *inst = word;
	int inst_len = word_len;

	// Pasre arguments
	enum InstructionArgType args_types = NONE;
	char arg_values[MAX_ARGS] = {0};
	for(int i = 0; i < inst_info->args_num; ++i) {
		word_len = find_word(&curr_pos, &word);
		if(word_len == 0) {
			print_compile_error("Expected an argument for", line_num, inst, inst_len);
			return COMPILE_ERROR;
		}

		enum InstructionArgType type = translate_argument(word, word_len, &arg_values[i]);

                if (type == LOCATION) {
			int jump = out_program->size + i + 1; // instruction position + position of current argument
			if(arg_values[i] == SPECIAL_LABEL) {
				arg_values[i] = jump + 1; // jump to the next line
			}
			else {
				if (!handle_label(word, word_len, label_table, -1/*no point/declare location*/, jump))
					return COMPILE_ERROR;
			}
                }

                args_types |= type;
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

	if(len == 1 && arg[0] == SPECIAL_LABEL) {
		*out_value = SPECIAL_LABEL;
		return LOCATION;
	}

	if(len > 0 && isalpha(arg[0])) {
		return LOCATION;
	}

	return NONE;
}

int resolve_labels(struct vec_char *out_program, struct vec_LabelEntry *label_table) {
	for(int i = 0; i < label_table->size; ++i) {
		LabelEntry *e = vec_at_LabelEntry(label_table, i);
		if(e->location < 0) {
			print_compile_error("Undeclared label ", -1, e->name, strlen(e->name));
			return 0;
		}

		if(e->location >= 0 && vec_empty_char(&e->jumps)) {
			print_compile_error("Unused label ", -1, e->name, strlen(e->name));
			return 0;
		}

		for(int j = 0; j < e->jumps.size; ++j) {
			*vec_at_char(out_program, *vec_at_char(&e->jumps, j)) = e->location;
		}
	}
	return 1;
}

int find_word(const char **curr_pos, const char **word_begin)
{
	const char *text = *curr_pos;
	int word_len = 0;

	// Skip spaces
	for(;text && *text != '\0' && isspace(*text); ++text)
		;

	*word_begin = text;

	//Count the length of the word
	for(;text && *text != '\0' && !isspace(*text); ++text, ++word_len)
		;

	*curr_pos = text;

	return word_len;
}

int strntoc(const char *s, int n, char *out) {
	
	char *tmp = (char*)malloc((n + 1)*sizeof(char));
	if(!tmp) {
		return 0;
	}

	strncpy(tmp, s, n);
	tmp[n] = '\0';
	int res = atoi(tmp);
	free(tmp);

	if(res <= UCHAR_MAX) {
		if(res == 0 && s[0] != '0') //TODO: Assume we not going to write -0
			return 0;
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

LabelEntry* handle_label(const char *name, size_t len, struct vec_LabelEntry *label_table, int location, int jump) {
	LabelEntry new_entry;
	new_entry.name = (char*)malloc((len + 1) * sizeof(char));
	if(!new_entry.name)
		return NULL;

	strncpy(new_entry.name, name, len);
	new_entry.name[len] = '\0';

	LabelEntry *e = find_label(new_entry.name, label_table);
	if(e) {
		free(new_entry.name);
		if(e->location >=0 && location >= 0) {
			// Attempt to redeclare label
			print_compile_error("Label is alredy declared", -1, e->name, len);
			return NULL;
		}

		if(e->location < 0) { // label was not "declared yet"
			e->location = location;
		}
		else {
			// Label used argument in jmp?
			if(jump < 0) {
				print_compile_error("Incorrect jump location for ", -1, e->name, len);
			}
			vec_push_back_char(&e->jumps, jump);
		}

		return e;
	}

	// Adding new label
	new_entry.location = location;
	vec_init_char(&new_entry.jumps);

	if(jump >= 0) {
		vec_push_back_char(&new_entry.jumps, jump);
	}

	vec_push_back_LabelEntry(label_table, new_entry);

	return vec_back_LabelEntry(label_table);
}

LabelEntry* find_label(const char *name, struct vec_LabelEntry *label_table) {
	for(int i = 0; i < label_table->size; ++i) {
		LabelEntry *e = vec_at_LabelEntry(label_table, i);
		if(strcmp(name, e->name) == 0)
			return e;
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

	if(line_num >= 0)
		printf(" at line %d\n", line_num);
	else
		puts("\n");
}

void print_label_table(struct vec_LabelEntry *label_table) {
	for(int i = 0; i < label_table->size; ++i) {
		LabelEntry *l = vec_at_LabelEntry(label_table, i);
		printf("Label: %s on %d from: ", l->name, l->location);
		for(int j = 0; j < l->jumps.size; ++j) {
			printf("%d ", *vec_at_char(&l->jumps, j));
		}
		printf("\n");
	}
}
