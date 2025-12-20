# Simple processor emulator

This simple processor emulator is intended to help create and test own ISA for  
Chapter 1 exercises in 'Basic Computer Architecture' by Smruti R Sarangi.

## Built in ASM description
Description of already implemented assembly language for simple emulated processor
ISA is described and implemented in processor.c. It could be edited to create own ISA.

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
sub r0 1        # Subtracts from register 0 the value 1. Result stored in register 0
sub r0 r1       # Subtracts from register 1 rigister 1. Result stored in register 01

sbn r0 5 label  # Subtructs 5 from r0 and jumps to label if r0 is negative
sbn r0 r1 label # Subtructs r1 from r0 and jumps to label if r0 is negative
sbn r0 5 _      # Subtructs 5 from r0 and jumps to the next commnad if negative (so it no jump actually)
sbn r0 r1 _     # Subtructs r1 from r0 and jumps to the next commnad

exit            # The end of program
```

The `sbn` command is added to do Exercise 1.7 and make sure it works.  
_asm_samples_ contains a program which calculates a factorial of a number (only 5! max as emulator is 8bit) using only `sbn` command

