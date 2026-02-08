#ifndef ASM_COMPILER_H_
#define ASM_COMPILER_H_

#include "common/simple_risc_types.h"

#define T word
#include "common/vector.h"
#undef T

#define COMPILE_ERROR -1
#define COMPILE_SUCCESS 0

int compile_program(const char *file_name, struct vec_word *program);

#endif
