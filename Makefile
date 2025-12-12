CC := clang
CFLAGS := --std=c17 -g -Wall -D_POSIX_C_SOURCE=200809L
LDFLAGS := -lc

VECTOR_TYPES := char
VECTOR_OBJS := $(foreach T,$(VECTOR_TYPES),vector_$(T).o)

TARGET := emulator.out
SRC := emulator.c asm_compiler.c processor.c
OBJS := $(SRC:.c=.o)

$(TARGET): $(OBJS) $(VECTOR_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# $@ is a target name, $* is what matched in target with %, $< - firs prerequisite
vector_%.o: vector.c
	$(CC) $(CFLAGS) -DT=$* -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./emulator.out sample.asm

debug:
	lldb emulator.out -- sample.asm

clean:
	rm *.o
	rm *.out
