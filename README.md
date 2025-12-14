# Simple processor emulator

This simple processor emulator is intended to help create and test own ISA for 
Chapter 1 exercises in 'Basic Computer Architecture' by Smruti R Sarangi.

## ASM description
Description of assembly language for simple emulated processor

```
# This is a comment
# There are four registers in emulated processor: r0 ... r4
#
# Simple program:
load r0 5       # Loads into register 0 the value 5
load r1 @5      # Loads into register 1 the value from the memory address 5
load r2 @r0     # Loads into register 2 the value from memory with address that is in register 0

store r0 5      # store value of register 0 into memory memory with address 5
store r0 @r1    # store value of register 0 into memory address pointed in register 1

label:          # label for jumps
jmp label       # jump to memory location 5 (set program counter to 5)
jnz r0 label    # jump to location 5 if value in register 0 is not zero

add r0 5        # Adds to register 0 the value 5. Result stored in register 0
add r0 r1       # Adds value of register 1 to rigister 0. Result stored in register 0
exit            # The end of program
```
