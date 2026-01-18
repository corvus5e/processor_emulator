CC := gcc
CFLAGS := --std=c17 -g -Wall -D_POSIX_C_SOURCE=200809L \
	  -I./src
LDFLAGS := -lc

SRC_DIR := src
OUT_DIR := bin

COMMON_SRC :=$(SRC_DIR)/asm_compiler.c \
	     $(SRC_DIR)/processor.c \
	     $(SRC_DIR)/emulator.c

COMPILER_TARGET := $(OUT_DIR)/srac.out # Simple Risc Assembly Compiler 😂
COMPILER_SRC_DIR := $(SRC_DIR)/compiler
COMPILER_SRC := $(COMPILER_SRC_DIR)/asm_compiler.c $(COMPILER_SRC_DIR)/compiler_main.c
COMPILER_OBJS := $(patsubst $(COMPILER_SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(COMPILER_SRC))

EMULATOR_TARGET := $(OUT_DIR)/emulator.out
EMULATOR_SRC := $(COMMON_SRC) $(SRC_DIR)/main.c
EMULATOR_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(EMULATOR_SRC))

TESTS_TARGET := $(OUT_DIR)/tests.out
TESTS_SRC := $(COMMON_SRC) $(SRC_DIR)/tests.c
TESTS_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(TESTS_SRC))
SAMPLES_DIR := asm_samples
SAMPLES_DIR_PATH = $(abspath $(SAMPLES_DIR))

build_all: $(COMPILER_TARGET)

$(COMPILER_TARGET): $(COMPILER_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# $(EMULATOR_TARGET): $(EMULATOR_OBJS)
# 	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
#
# $(TESTS_TARGET): $(TESTS_OBJS)
# 	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(OUT_DIR)/%.o: $(COMPILER_SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# $(OUT_DIR)/tests.o: $(SRC_DIR)/tests.c | $(OUT_DIR)
# 	$(CC) $(CFLAGS) -DSAMPLES_DIR=\"$(SAMPLES_DIR_PATH)\" -c $< -o $@

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

test:
	./$(OUT_DIR)/tests.out

debug:
	lldb $(OUT_DIR)/emulator.out -- asm_samples/example_26.asm

clean:
	rm -rd bin
