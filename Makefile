
main:
	clang --std=c17 -g -Wall \
		emulator.c \
		asm_compiler.c \
		processor.c \
		-D_POSIX_C_SOURCE=200809L -lc -o emulator.out
run:
	./emulator.out sample.asm

debug:
	lldb emulator.out -- sample.asm
