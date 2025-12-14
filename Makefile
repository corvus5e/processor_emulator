CC := gcc
CFLAGS := --std=c17 -g -Wall -D_POSIX_C_SOURCE=200809L
LDFLAGS := -lc

TARGET := emulator.out
SRC := emulator.c asm_compiler.c processor.c
OBJS := $(SRC:.c=.o)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

SAMPLE := ex_7.asm
#SAMPLE := sample.asm

run:
	./emulator.out asm_samples/$(SAMPLE)

debug:
	lldb emulator.out -- sample.asm

clean:
	rm *.o
	rm *.out
