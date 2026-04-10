#include "asm_compiler.h"

#include <errno.h>
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
static struct vec_LabelEntry label_table;

/* Separators list */
static char* separators = "[]#, \t\n";

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
	int instructions_count = 0;
	int text_line_num = 1;
	vec_init_LabelEntry(&label_table);
	struct InstructionInfo *inst_info;
	arg_values_array arg_value = {0};
	struct Token t;

	for (; (nread = getline(&line, &n, f)) != -1; ++text_line_num) {
		const char *line_begin = line;
		if((t = get_token(&line_begin)).type == COMMENT || t.type == NEW_LINE) {
			continue;
		}

		if (is_declared_label(t.str, t.len)) {
			if ((status = declare_label(t.str, t.len-1, instructions_count))
				.error_code == COMPILE_SUCCESS)
				continue;
			else
				break;
		}

		status = parse_line(line, &inst_info, arg_value, instructions_count);
		if (status.error_code == COMPILE_ERROR)
			break;

		instructions_count += compile_instruction(inst_info, arg_value, program);
	}

	free(line);
	fclose(f);

	if(status.error_code == COMPILE_ERROR) {
		fprintf(stderr, "Error on line %d, col %d: %s\n", text_line_num, status.column, status.msg);
		return COMPILE_ERROR;
	}

	//vec_push_back_word(program, -1); //TODO: MOVE EXIT_PROGRAM to accessible place
	print_label_table(&label_table);

	status = resolve_labels(program, &label_table);

	if(status.error_code == COMPILE_ERROR) {
		fprintf(stderr, "Error while resolving labels: %s\n", status.msg);
		return COMPILE_ERROR;
	}

	return status.error_code;
}

int compile_instruction(struct InstructionInfo *instruction, arg_values_array args, struct vec_word *out_compiled_instruction) {
	int args_count = instruction->args_num;
	int imm_arg = args[args_count - 1];

	if (instruction->arg_types[args_count - 1] == IMMEDIATE &&
	    (imm_arg > (int)MAX_IMMEDIATE || imm_arg < (int)MIN_IMMEDIATE) &&
	    instruction->code != LSL_OPCODE &&
	    instruction->code != LSR_OPCODE &&
	    instruction->code != ASR_OPCODE) {
		int arg_upper_half = imm_arg >> 16;    // h-modifier
		int lower_half = imm_arg & 0x0000FFFF; // u-modifier

		//Generate instructions
		switch(instruction->code) {
			case MOV_OPCODE: {
					//TODO: Refactor
					struct InstructionInfo mov = asm_info[19]; //TODO: Make better
					mov.modifier = UPPER_HALF;
					arg_values_array mov_args =  {args[0], arg_upper_half};
					encode_and_push(&mov, mov_args, out_compiled_instruction);

					struct InstructionInfo add = asm_info[0]; //TODO: Make better
					arg_values_array add_args =  {args[0], args[0], lower_half};
					add.modifier = UNSIGNED;
					encode_and_push(&add, add_args, out_compiled_instruction);

					return 2;
			}
		}
	} else { // non-immediate or immediate argumant fits in 16 bit 2's compement
		encode_and_push(instruction, args, out_compiled_instruction);
		return 1;
	}

	return 0;
}

void encode_and_push(struct InstructionInfo *instruction, arg_values_array args, struct vec_word *out_compiled_instruction) {
	word encoded_instruction = 0;
	encode_instruction(instruction, args, &encoded_instruction);
	vec_push_back_word(out_compiled_instruction, encoded_instruction);
}

struct CompileError encode_instruction(struct InstructionInfo *instruction, arg_values_array args, word *result) {
	struct CompileError status = {.error_code = COMPILE_ERROR, .msg = "Bad encode_instruction call"};
	if (!instruction || !result) 
		return status;

	*result = instruction->code;
	*result |= instruction->modifier;

	switch (instruction->args_num) {
		case 0: break;
		case 1: return encode_instruction_one_arg(result, args[0]);
		case 2: {
			// NOTE: 2-arg instructions will be encoded in the same
			//       format as 3-arg, but one register remains empty,
			//       so putting immediate or second register in the
			//       third place
			SWAP(int, args[1], args[2]);
			if (instruction->code == CMP_OPCODE) {
				// NOTE: CMP is special - destination reg is
				//       empty, reg to compare is placed on the second place
				SWAP(int, args[0], args[1]);
			}
			bool is_immediate = instruction->arg_types[1] == IMMEDIATE;
			return encode_instruction_two_arg(result, is_immediate, args);
		}
		case 3: {
			bool is_immediate = instruction->arg_types[2] == IMMEDIATE;
			if (instruction->code == LD_OPCODE || instruction->code == ST_OPCODE) {
				// NOTE: for st and ld instruction we should
				// swap imm and src_reg_1 as format is `ld r1, 10[r2]` and `st r1, 10[r2]`
				SWAP(int, args[1], args[2]);
				is_immediate = instruction->arg_types[1] == IMMEDIATE;
			}

			return encode_instruction_three_arg(result, is_immediate, args);
		}
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
	*instruction_with_opcode &= (~ONE_ARG_MASK); //clear any previous offset
	*instruction_with_opcode |= offset;
	status.error_code = COMPILE_SUCCESS;
	return status;
}
struct CompileError encode_instruction_two_arg(word *instruction_with_opcode, bool is_immediate, arg_values_array args) {
	//NOTE: For 2-args instruction second parameter is placed in imeediate field no
	//      matter if it immediate or number of register, so it should be packed
	//      as immediate 3-arg instruction, but preserve I-bit
	struct CompileError status = {.error_code = COMPILE_SUCCESS, .msg = "Bad encode_instruction_three_arg call"};
	if(is_immediate) {
		*instruction_with_opcode |= I_BIT_MASK;
	}

	//TODO: What with modifiers bif, for now default is used
	//TODO: Handle for regs to be < 16 during parsing stage
	*instruction_with_opcode |= (args[0] & 0x0000000F) << DST_REG_SHIFT;
	*instruction_with_opcode |= (args[1] & 0x0000000F) << SRC_REG_1_SHIFT;
	*instruction_with_opcode |= (args[2] & 0x0000FFFF);
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
	//TODO: Token could be taken from previous line from buffer, need to clear ?
	const char *curr_pos = line_text;
	struct Token t = get_token(&curr_pos);

	struct InstructionInfo search_pattern = {.name = t.str, .args_num = -1/*Search only by name*/};
	 // It should be a command then
	if (!(*found_instruction = find_instruction_info(search_pattern))) {
		struct CompileError status = {.error_code = COMPILE_ERROR,
					      .msg = "Unknown instruction"};
		return status;
	}

	struct CompileError status = parse_arguments(&curr_pos, &search_pattern, out_args, instruction_num);
	if(status.error_code != COMPILE_SUCCESS) {
		return status;
	}

	// Find instruction again with all parsed arguments
	if (!(*found_instruction = find_instruction_info(search_pattern))) {
		struct CompileError status = {.error_code = COMPILE_ERROR,
					      .msg = "Unexpected arguments types for instruction",
					      .column = curr_pos - line_text};
		return status;
	}

	return status;
}

struct CompileError parse_arguments(const char **curr_pos, struct InstructionInfo *instruction, arg_values_array out_value, int instruction_num) {
	struct CompileError status = {.error_code = COMPILE_ERROR};

	struct Token t;
	int reg, imm;
	int i = 0;

	for (; i < 3 ;) {
		t = get_token(curr_pos);

		if(is_used_label(t.str, t.len)) {
			add_lable_jump(t.str, t.len, instruction_num);
			instruction->arg_types[i] = LOCATION;
			i += 1;
		}
		else if(parse_reg(&t, &reg)) {
			instruction->arg_types[i] = REGISTER;
			out_value[i] = reg;
			i += 1;
		}
		else if(parse_at_imm(&t, &imm, &reg)) {
			instruction->arg_types[i] = IMMEDIATE;
			instruction->arg_types[i+1] = AT_REGISTER;
			out_value[i] = imm;
			out_value[i+1] = reg;
			i += 2;
		}
		else if(parse_imm(&t, &imm)) {
			instruction->arg_types[i] = IMMEDIATE;
			out_value[i] = imm;
			i += 1;
		}
		else if(t.type == NEW_LINE || t.type == COMMENT) {
			instruction->arg_types[i] = NONE;
			break;
		}
		else {
			status.msg = "Unknow agrument format";
			return status;
		}
	
		t = get_token(curr_pos);

		if(t.type == NEW_LINE || t.type == COMMENT)
			break;

		if(t.type != COMMA) {
			status.msg = "Expected comma after and the argument";
			return status;
		}
	}

	instruction->args_num = i;

	status.error_code = COMPILE_SUCCESS;
	return status;
}

bool parse_reg(const struct Token* t, int* out_reg_number) {
	if(t->len < 2) {
		return false;
	}
	if (t->str[0] == 's' && t->str[1] == 'p') {
		*out_reg_number = SP;
		return true;
	}

	if (t->str[0] == 'r' && t->str[1] == 'a') {
		*out_reg_number = RA;
		return true;
	}

	if (t->str[0] != 'r') {
		return false;
	}

	if (!strntoi(t->str + 1, t->len - 1, out_reg_number)) {
		return false;
	}

	return true;
}

bool parse_imm(const struct Token *t, int* out_immediate) {
	return strntoi(t->str, t->len, out_immediate);
}

bool parse_at_imm(const struct Token *t, int* out_immediate, int* out_reg_number) {
	const char *curr_pos = t->str;
	struct Token sub_token = get_token_sb(&curr_pos);
	if(sub_token.type == LBRACKET) {
		*out_immediate = 0;
		unget_token(sub_token);
	}
	else if(!parse_imm(&sub_token, out_immediate)) {
		return false;
	}
	
	if((sub_token = get_token_sb(&curr_pos)).type != LBRACKET) {
		return false;
	}

	sub_token = get_token_sb(&curr_pos);

	if(!parse_reg(&sub_token, out_reg_number)) {
		return false;
	}

	if((sub_token = get_token_sb(&curr_pos)).type != RBRACKET) {
		return false;
	}

	return true;
}

bool is_declared_label(const char *word, size_t len) {
	return word[0] == '.' && word[len-1] == ':' && len > 2;
}

bool is_used_label(const char *word, size_t len) {
	return word[0] == '.' && len > 1;
}

struct Token token_buf[50];
int token_free_pos = 0;

struct Token get_token_until_separator(const char **text, char *separators) {
	if (token_free_pos > 0) {
		return token_buf[--token_free_pos];
	}

	struct Token token = {0, STR_END, NULL};

	// Skip spaces
	for(; **text && **text != '\n' && isspace(**text); ++(*text))
		;

	token.str = *text;
	
	if(is_char_in_str(**text, separators)) {
		token.type = **text;
		token.len = 1;
		++(*text); // Move to next char
	} else {
		token.type = WORD;
		// Count the length of the word
		for (; **text && !is_char_in_str(**text, separators); ++(*text), ++token.len)
			;
	}

	return token;

}

struct Token get_token(const char **curr_pos) {
	return get_token_until_separator(curr_pos, separators + 2);
}

struct Token get_token_sb(const char **curr_pos) {
	return get_token_until_separator(curr_pos, separators);
}

void unget_token(struct Token t) {
	if(token_free_pos > 50) {
		fprintf(stderr, "Token buffer overflow!\n");
		exit(1); //TODO: Migrate to a vector
	}
	token_buf[token_free_pos++] = t;
}

struct CompileError resolve_labels(struct vec_word *out_program, struct vec_LabelEntry *label_table) {
	struct CompileError status = {.error_code = COMPILE_ERROR};
	for(int i = 0; i < label_table->size; ++i) {
		LabelEntry *e = vec_at_LabelEntry(label_table, i);
		if(e->location < 0) {
			status.msg = "Label is not declared!"; //TODO: Add name to msg
			return status;
		}

		if(e->location >= 0 && vec_empty_word(&e->jumps)) { // Label is not used
			break;
		}

		for(int j = 0; j < e->jumps.size; ++j) {
			word from = *vec_at_word(&e->jumps, j);
			word to = e->location;
			word diff = to - from;
			word *instruction = vec_at_word(out_program, from);
			encode_instruction_one_arg(instruction, diff);
		}
	}
	status.error_code = COMPILE_SUCCESS;
	return status;
}

bool strntoi(const char *s, int n, int *out) {
	static char buf[50]; //Should be enough

	strncpy(buf, s, n);
	buf[n] = '\0';
	char *end;
	errno = 0;
	*out = strtol(buf, &end, 0);
	if(end == buf || errno == ERANGE) {
		return false;
	}
	return true;
}

bool is_char_in_str(const char c, const char *str) {
	for(;*str && c != *str; ++str)
		;
	return *str;
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
