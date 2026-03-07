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

#define T LabelEntry
#include "common/vector.h"
#undef T

/* Global label table*/
struct vec_LabelEntry label_table;

void print_label_table(struct vec_LabelEntry *label_table);

int compile_program(const char *file_name, struct vec_word *program)
{
	if(!program)
		return COMPILE_ERROR;

	FILE *f = fopen(file_name, "r");
	if(!f){
		printf("ERROR: failed to open file %s\n", file_name);
		return COMPILE_ERROR;
	}
	
	struct CompileError status = {.error_code = COMPILE_ERROR};
	char *line = NULL;
	size_t n;
	ssize_t nread;
	int instruction_num = 0;
	int text_line_num = 1;
	vec_init_LabelEntry(&label_table);
	struct InstructionInfo *inst_info;
	arg_values_array arg_value;
	struct Token t;

	for (; (nread = getline(&line, &n, f)) != -1; ++text_line_num) {
		const char *line_begin = line;
		if((t = get_token(&line_begin)).type == COMMENT || t.type == NEW_LINE) {
			continue;
		}

		if (isLabel(t.str, t.len)) {
			if ((status = declare_label(t.str, t.len-1, instruction_num))
				.error_code == COMPILE_SUCCESS)
				continue;
			else
				break;
		}

		status = parse_line(line, &inst_info, arg_value, instruction_num++);
		if (status.error_code == COMPILE_ERROR)
			break;
		
		word encoded_instruction = 0;
		status = encode_instruction(inst_info, arg_value, &encoded_instruction);
		vec_push_back_word(program, encoded_instruction);
	}

	free(line);
	fclose(f);

	if(status.error_code == COMPILE_ERROR) {
		fprintf(stderr, "Error on line %d, col %d: %s\n", text_line_num, status.column, status.msg);
		return COMPILE_ERROR;
	}

	//vec_push_back_word(program, -1); //TODO: MOVE EXIT_PROGRAM to accessible place
	print_label_table(&label_table);

	return resolve_labels(program, &label_table);
}

struct CompileError encode_instruction(struct InstructionInfo *instruction, arg_values_array args, word *result) {
	struct CompileError status = {.error_code = COMPILE_ERROR, .msg = "Bad encode_instruction call"};
	if (!instruction || !result) 
		return status;

	*result = instruction->code;

	switch (instruction->args_num) {
		case 0: break;
		case 1: return encode_instruction_one_arg(result, args[0]);
		case 2: break;
		case 3: {
			//NOTE: Handle ls, st instructions
			bool is_immediate = instruction->arg_types[2] == IMMEDIATE;
			return encode_instruction_three_arg(result, is_immediate, args);
		};
	}
	status.error_code = COMPILE_SUCCESS;
	return status;
}

struct CompileError encode_instruction_one_arg(word *instruction_with_opcode, word offset) {
	struct CompileError status = {.error_code = COMPILE_ERROR, .msg = "Bad encode_instruction_one_arg call"};
	if (abs(offset) > MAX_OFFSET) {
		status.msg = "Too big offset value";
		return status;
	}
	offset &= ONE_ARG_MASK;
	*instruction_with_opcode |= offset;
	status.error_code = COMPILE_SUCCESS;
	return status;
}

struct CompileError encode_instruction_three_arg(word *instruction_with_opcode, bool is_immediate, arg_values_array args) {
	struct CompileError status = {.error_code = COMPILE_SUCCESS, .msg = "Bad encode_instruction_three_arg call"};
	if(is_immediate) {
		*instruction_with_opcode |= I_BIT_MASK;
		*instruction_with_opcode |= (args[2] & 0x0000FFFF);
	}
	else {
		*instruction_with_opcode |= (args[2] & 0x0000000F) << SRC_REG_2_SHIFT;
	}

	//TODO: What with modifiers bif, for now default is used
	//TODO: Handle for regs to be < 16 during parsing stage
	*instruction_with_opcode |= (args[0] & 0x0000000F) << DST_REG_SHIFT;
	*instruction_with_opcode |= (args[1] & 0x0000000F) << SRC_REG_1_SHIFT;
	return status;
}

struct CompileError parse_line(const char *line_text, struct InstructionInfo **found_instruction, arg_values_array out_args, int instruction_num) {
	const char *curr_pos = line_text;
	struct Token t = get_token(&curr_pos);

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
					out_args + search_pattern.args_num, instruction_num))
			.error_code == COMPILE_ERROR) {
			return status;
		}
		if ((t = get_token(&curr_pos)).type == NEW_LINE || t.type == COMMENT) {
			unget_token(t);
			break; // All argument parsed
		} else if(t.type == LBRACKET) {
			unget_token(t); // that might be at_register arg
		} else if(t.type != COMMA) {
			struct CompileError status = {
			    .error_code = COMPILE_ERROR,
			    .msg	= "Expected comma or end of line",
			    .column = curr_pos - line_text};
			return status;
		}
	}

	// Find instruction again with all parsed arguments
	if (!(*found_instruction = find_instruction_info(search_pattern))) {
		struct CompileError status = {.error_code = COMPILE_ERROR,
					      .msg = "Unexpected arguments types for instruction",
					      .column = curr_pos - line_text};
		return status;
	}

	if((t = get_token(&curr_pos)).type != NEW_LINE && t.type != COMMENT) {
		status.error_code = COMPILE_ERROR;
		status.msg = "Too many arguments. Expected end of line";
		status.column = curr_pos - line_text;
	}

	return status;
}

struct CompileError parse_arg(const char **curr_pos, enum InstructionArgType *out_type, int* out_value, int line_num) {
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
			add_lable_jump(t.str, t.len, line_num);
			*out_type = LOCATION;
		}
	}

	status.error_code = COMPILE_SUCCESS;
	return status;
}

bool isLabel(const char *word, size_t len) {
	return word[0] == '.' && word[len-1] == ':' && len > 2;
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

	if(**text == '#') {
		token.type = COMMENT;
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
	       **text != '[' && **text != ']' &&
	       **text != '#';
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

		if(e->location >= 0 && vec_empty_word(&e->jumps)) {
			return COMPILE_ERROR;
		}

		for(int j = 0; j < e->jumps.size; ++j) {
			word from = *vec_at_word(&e->jumps, j);
			word to = e->location;
			word diff = to - from;
			word instruction = *vec_at_word(out_program, from);
			encode_instruction_one_arg(&instruction, diff);
			*vec_at_word(out_program, from) = instruction;
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

struct CompileError declare_label(const char *name, size_t len, word location) {
	struct CompileError status = {.error_code = COMPILE_SUCCESS};
	struct LabelEntry *e = find_label(name, len);

	if(!e && !(e = add_label(name, len))) {
		status.error_code = COMPILE_ERROR;
		status.msg = "Failed to add label";
		return status;
	}

	if(e->location >= 0) {
		//TODO: add label name to error msg
		status.error_code = COMPILE_ERROR;
		status.msg = "Label already declared";
		return status;
	}

	e->location = location;

	return status;
}

void add_lable_jump(const char *name, size_t len, word jump_from) {
	struct LabelEntry *e = find_label(name, len);
	if(!e) {
		e = add_label(name, len);
	}
	vec_push_back_word(&(e->jumps), jump_from);
}

LabelEntry* add_label(const char *name, size_t len) {
	LabelEntry new_entry;
	new_entry.location = -1;
	new_entry.name = (char*)malloc((len + 1) * sizeof(char));
	if(!new_entry.name)
		return NULL;

	strncpy(new_entry.name, name, len);
	new_entry.name[len] = '\0';
	vec_init_word(&new_entry.jumps);
	vec_push_back_LabelEntry(&label_table, new_entry);

	return vec_back_LabelEntry(&label_table);
}

LabelEntry* find_label(const char *name, size_t len) {
	for(int i = 0; i < label_table.size; ++i) {
		LabelEntry *e = vec_at_LabelEntry(&label_table, i);
		if(strncmp(name, e->name, len) == 0)
			return e;
	}
	return NULL;
}

void print_label_table(struct vec_LabelEntry *label_table) {
	for(int i = 0; i < label_table->size; ++i) {
		LabelEntry *l = vec_at_LabelEntry(label_table, i);
		printf("Label: %s on %d from: ", l->name, l->location);
		for(int j = 0; j < l->jumps.size; ++j) {
			printf("%d ", *vec_at_word(&l->jumps, j));
		}
		printf("\n");
	}
}
