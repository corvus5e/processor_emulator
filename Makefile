CC := gcc
CFLAGS := --std=c17 -g -Wall -D_POSIX_C_SOURCE=200809L
LDFLAGS := -lc

SRC_DIR := src
OUT_DIR := bin
TARGET := $(OUT_DIR)/emulator.out
SRC := $(SRC_DIR)/emulator.c \
       $(SRC_DIR)/asm_compiler.c \
       $(SRC_DIR)/processor.c
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(SRC))

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

SAMPLE := ex_7.asm
#SAMPLE := sample.asm

run:
	./$(OUT_DIR)/emulator.out asm_samples/$(SAMPLE)

debug:
	lldb $(OUT_DIR)/emulator.out -- asm_samples/sample.asm

clean:
	rm -rd bin
