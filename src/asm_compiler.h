#ifndef ASM_COMPILER_H_
#define ASM_COMPILER_H_

#define COMPILE_ERROR -1

struct vec_char;

int compile_program(const char *file_name, struct vec_char *program);

#endif
