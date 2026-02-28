#include "asm_compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include "asm_info.h"

#define T char
#include "common/vector.h"
#undef T

#define MAX_COMMAND_LEN 5
#define SPECIAL_LABEL '_' // Label pointing to the next command (aka "do nothing" label)

typedef struct LabelEntry {
	char *name;
	int location;
	struct vec_char jumps; /* exact positions in compiled code where label is used */
} LabelEntry;

#define T LabelEntry
#include "common/vector.h"
#undef T

/* Global label table*/
struct vec_LabelEntry label_table;


/* Replaces labels with needed addresses for jmp instructions in compiled program
 * Returns 1 if succeeded, 0 otherwise
 * */
int resolve_labels(struct vec_word *out_program, struct vec_LabelEntry *label_table);

/* Tries to add label into the `label_table`.
 * On success returns pointer to newly added entry, otherwise returns NULL.
 * `jump` is a location of where the label is used.
 * */
LabelEntry* handle_label(const char *name, size_t len, int location, int jump);

/* Finds a label with `name` in `label_table`.
 * Returns NULL is not found.
 * */
LabelEntry* find_label(const char *name);

void print_label_table(struct vec_LabelEntry *label_table);


/* --- Implementation ---- */

int compile_program(const char *file_name, struct vec_word *program)
{
	FILE *f = fopen(file_name, "r");
	if(!f){
		printf("ERROR: failed to open file %s\n", file_name);
		return COMPILE_ERROR;
	}
	
	struct CompileError status = {.error_code = COMPILE_ERROR};
	char *line = NULL;
	size_t n;
	ssize_t nread;
	int line_num = 0;
	vec_init_LabelEntry(&label_table);
	struct InstructionInfo *inst_info;
	arg_values_array arg_value;
	struct Token t;

	for (; (nread = getline(&line, &n, f)) != -1; ++line_num) {
		if((t = get_token(&line)).str[0] == '#') {
			continue;
		}
		status = parse_line(line, &inst_info, arg_value);
		if (status.error_code == COMPILE_ERROR)
			break;
	}

	free(line);
	fclose(f);

	if(status.error_code == COMPILE_ERROR) {
		fprintf(stderr, "Error on line %d: %s\n", line_num, status.msg);
		return COMPILE_ERROR;
	}

	vec_push_back_word(program, -1); //TODO: MOVE EXIT_PROGRAM to accessible place
	//print_label_table(&label_table);

	//TODO: Finish label resolve
	int foo = resolve_labels(program, &label_table);
	return 0;
}

struct CompileError parse_line(const char *line_text, struct InstructionInfo **found_instruction, arg_values_array out_args) {
	const char *curr_pos = line_text;
	struct Token t = get_token(&curr_pos);
	if(t.type != WORD) {
		struct CompileError status = {.error_code = COMPILE_ERROR, .msg = "Unexpected token"};
		return status;
	}
	bool is_label = isLabel(t.str, t.len); // inside if implicit declaration with int return type
	if(is_label) {
		//TODO: Handle label
		// if(add_label(...))
		struct CompileError status = {.error_code = COMPILE_SUCCESS};
		return status;
	}

	struct InstructionInfo search_pattern = {.name = t.str, .args_num = -1/*Search only by name*/};
	 // It should be a command then
	if (!(*found_instruction = find_instruction_info(search_pattern))) {
		struct CompileError status = {.error_code = COMPILE_ERROR,
					      .msg = "Unknown instruction"};
		return status;
	}

	// Now we should parse arguments
	search_pattern.args_num = 0;
	struct CompileError status;
	for (; search_pattern.args_num < 3; ++search_pattern.args_num) {
		if ((status = parse_arg(&curr_pos,
					search_pattern.arg_types + search_pattern.args_num,
					out_args + search_pattern.args_num))
			.error_code == COMPILE_ERROR) {
			return status;
		}
		if ((t = get_token(&curr_pos)).type == NEW_LINE) {
			unget_token(t);
			break; // All argument parsed
		} else if(t.type == LBRACKET) {
			unget_token(t); // that might be at_register arg
		} else if(t.type != COMMA) {
			struct CompileError status = {
			    .error_code = COMPILE_ERROR,
			    .msg	= "Expected comma or end of line"};
			return status;
		}
	}

	// Find instruction again with all parsed arguments
	if (!(*found_instruction = find_instruction_info(search_pattern))) {
		struct CompileError status = {.error_code = COMPILE_ERROR,
					      .msg = "Unexpected arguments types for instruction"};
		return status;
	}

	if((t = get_token(&curr_pos)).type != NEW_LINE) {
		status.error_code = COMPILE_ERROR;
		status.msg = "Too many arguments. Expected end of line";
	}

	return status;
}

struct CompileError parse_arg(const char **curr_pos, enum InstructionArgType *out_type, int* out_value) {
	struct CompileError status = {.error_code = COMPILE_ERROR, .msg = ""};
	struct Token t = get_token(curr_pos);
	if (t.type == NEW_LINE) {
		status.msg = "Unexpected end of line. Expected argument.";
		return status;
	}

	if(t.type == LBRACKET) {
		if((t = get_token(curr_pos)).type != WORD) {
			status.msg = "Failed to parse at registet argument. Expected register after '['";
			return status;
		}
		if (t.str[0] != 'r') {
			status.msg = "Failed to parse at registet argument. Not a register";
			return status;
		}
		if (!strntoc(t.str + 1, t.len - 1, out_value)) {
			status.msg = "Failed to parse register argument";
			return status;
		}
		*out_type = AT_REGISTER;

		if ((t = get_token(curr_pos)).type != RBRACKET) {
			status.msg = "Expected closing bracket ']'";
			return status;
		}
	}

	if(t.type == WORD) {
		if(isdigit(t.str[0])) {
			if (!strntoc(t.str, t.len, out_value)) {
				status.msg = "Failed to parse immediate argument";
				return status;
			}
			*out_type = IMMEDIATE;
		} else if (t.str[0] == 'r') {
			if (!strntoc(t.str + 1, t.len - 1, out_value)) {
				status.msg = "Failed to parse register argument";
				return status;
			}
			*out_type = REGISTER;
		} else {
			//TODO: handle label
		}
	}

	status.error_code = COMPILE_SUCCESS;
	return status;
}

bool isLabel(const char *word, size_t len) {
	return false;//TODO: Implement
}

struct Token token_buf[50];
int token_free_pos = 0;

struct Token get_token(const char **text) {
	if (token_free_pos > 0) {
		return token_buf[--token_free_pos];
	}

	struct Token token = {0, STR_END, NULL};

	// Skip spaces
	for(;text && *text && **text != '\0' && **text != '\n' && isspace(**text); ++(*text))
		;

	token.str = *text;

	if(**text == ',') {
		token.type = COMMA;
		token.len = 1;
		++(*text); // Move to next char
		return token;
	}

	if(**text == '\n') {
		token.type = NEW_LINE;
		token.len = 1;
		++(*text); // Move to next char
		return token;
	}

	if(**text == '[') {
		token.type = LBRACKET;
		token.len = 1;
		++(*text); // Move to next char
		return token;
	}

	if(**text == ']') {
		token.type = RBRACKET;
		token.len = 1;
		++(*text); // Move to next char
		return token;
	}

	if(**text == '\0') {
		token.type = STR_END;
		token.len = 0;
		return token;
	}

	token.type = WORD;

	//Count the length of the word
        for (; text && *text && **text != '\0' &&
		!isspace(**text) &&
               **text != ',' &&
	       **text != '[' && **text != ']';
             ++(*text), ++token.len)
          ;

        return token;
}

void unget_token(struct Token t) {
	if(token_free_pos > 50) {
		fprintf(stderr, "Token buffer overflow!\n");
		exit(1); //TODO: Migrate to a vector
	}
	token_buf[token_free_pos++] = t;
}

int resolve_labels(struct vec_word *out_program, struct vec_LabelEntry *label_table) {
	for(int i = 0; i < label_table->size; ++i) {
		LabelEntry *e = vec_at_LabelEntry(label_table, i);
		if(e->location < 0) {
			return COMPILE_ERROR;
		}

		if(e->location >= 0 && vec_empty_char(&e->jumps)) {
			return COMPILE_ERROR;
		}

		for(int j = 0; j < e->jumps.size; ++j) {
			*vec_at_word(out_program, *vec_at_char(&e->jumps, j)) = e->location;
		}
	}
	return COMPILE_SUCCESS;
}

bool strntoc(const char *s, int n, int *out) {
	
	char *tmp = (char*)malloc((n + 1)*sizeof(char));
	if(!tmp) {
		return false;
	}

	strncpy(tmp, s, n);
	tmp[n] = '\0';
	int res = atoi(tmp);
	free(tmp);

	if(res <= UCHAR_MAX) {
		if(res == 0 && s[0] != '0') //TODO: Assume we not going to write -0
			return false;
		*out = res;
		return true;
	}

	return false;
}

struct InstructionInfo* find_instruction_info(struct InstructionInfo instruction)
{
	const bool search_by_args = instruction.args_num >= 0;
	int size = sizeof(asm_info)/sizeof(struct InstructionInfo);
	for(int i = 0; i < size; ++i) {
		int instruction_found = strncmp(instruction.name, asm_info[i].name, strlen(asm_info[i].name)) == 0;
		bool args_same = true;
                if (search_by_args && instruction_found) {
			for(int j = 0; j < asm_info[i].args_num; ++j) {
				args_same &= (instruction.arg_types[j] == asm_info[i].arg_types[j]);
			}
                }
                if(instruction_found && args_same) {
			return asm_info + i;
                }
        }

	return NULL;
}

LabelEntry* handle_label(const char *name, size_t len, int location, int jump) {
	LabelEntry new_entry;
	new_entry.name = (char*)malloc((len + 1) * sizeof(char));
	if(!new_entry.name)
		return NULL;

	strncpy(new_entry.name, name, len);
	new_entry.name[len] = '\0';

	LabelEntry *e = find_label(new_entry.name);
	if(e) {
		free(new_entry.name);
		if(e->location >=0 && location >= 0) { // Attempt to redeclare label
			return NULL;
		}

		if(e->location < 0) { // label was not "declared yet"
			e->location = location;
		}
		else {
			// Label used argument in jmp?
			if(jump < 0) {
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

	vec_push_back_LabelEntry(&label_table, new_entry);

	return vec_back_LabelEntry(&label_table);
}

LabelEntry* find_label(const char *name) {
	for(int i = 0; i < label_table.size; ++i) {
		LabelEntry *e = vec_at_LabelEntry(&label_table, i);
		if(strcmp(name, e->name) == 0)
			return e;
	}
	return NULL;
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
