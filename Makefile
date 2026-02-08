CC := clang
CFLAGS := --std=c17 -g -Wall -D_POSIX_C_SOURCE=200809L \
	  -I./src
LDFLAGS := -lc

SRC_DIR := src
BUILD_DIR := bin

COMPILER_TARGET := $(BUILD_DIR)/srac.out # Simple Risc Assembly Compiler 😂
COMPILER_SRC_DIR := compiler
COMPILER_SRC := $(COMPILER_SRC_DIR)/asm_compiler.c
COMPILER_SRC_ALL := $(COMPILER_SRC) $(COMPILER_SRC_DIR)/compiler_main.c
COMPILER_OBJS := $(COMPILER_SRC:%.c=$(BUILD_DIR)/%.o)
COMPILER_OBJS_ALL := $(COMPILER_SRC_ALL:%.c=$(BUILD_DIR)/%.o)

TESTS_SRC_DIR := tests

FUN_TEST_TARGET_NAME := functional_tests.out
FUN_TESTS_TARGET := $(BUILD_DIR)/$(FUN_TEST_TARGET_NAME)
FUN_TESTS_SRC := $(TESTS_SRC_DIR)/functional_tests.c
FUN_TESTS_OBJS := $(FUN_TESTS_SRC:%.c=$(BUILD_DIR)/%.o)

UNIT_TEST_TARGET_NAME := unit_tests.out
UNIT_TESTS_TARGET := $(BUILD_DIR)/$(UNIT_TEST_TARGET_NAME)
UNIT_TESTS_SRC := $(TESTS_SRC_DIR)/unit_tests.c
UNIT_TESTS_OBJS := $(UNIT_TESTS_SRC:%.c=$(BUILD_DIR)/%.o)

SAMPLES_DIR := asm_samples
SAMPLES_DIR_PATH = $(abspath $(SAMPLES_DIR))

tests: $(FUN_TESTS_TARGET) $(UNIT_TESTS_TARGET)
compiler: $(COMPILER_TARGET)

$(COMPILER_TARGET): $(COMPILER_OBJS_ALL)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(FUN_TESTS_TARGET): $(FUN_TESTS_OBJS) $(COMPILER_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(UNIT_TESTS_TARGET): $(UNIT_TESTS_OBJS) $(COMPILER_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/functional_tests.o: $(SRC_DIR)/tests/functional_tests.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DSAMPLES_DIR=\"$(SAMPLES_DIR_PATH)\" -c $< -o $@

test:
	./$(BUILD_DIR)/$(FUN_TEST_TARGET_NAME)

clean:
	rm -rd bin
