#ifndef EMULATOR_H_
#define EMULATOR_H_

#include "processor.h"

struct vec_char;

int compile_and_run_program(const char *file_name, struct Processor *processor);

#endif
