# Simple processor emulator

This simple processor emulator is intended to help create and test own ISA for  
Chapter 1 exercises in 'Basic Computer Architecture' by Smruti R Sarangi.

## SimpleRisc and it's ASM description (from Chapter 3 (3.3.2) )
SimpleRisc assembly follows GNU assembly format  
SimpleRisc has 16 registers numbered r0...r15  
Register _r14_ is a stack pointer (referred as _sp_)  
Register _r15_ is a return address register (referred as _ra_)  
Each register is 32 bit wide  
Special internal register called _flags_ is assumed, which is not visible to the programmer.   
It contains two fields flags.E(equal) and flags.GT(greater than).  
E is set to 1 if the result of a comparison is equality.  
GT is set to 1 if a comparison concludes that the first operand is greater than the second operand.  
The default values of both the fields are 0.  
Each instruction is encoded into a 32-bit value, and it requires 4 bytes of storage in memory.  
Multi-byte data types such as integers are saved in the little endian format.  

```
# This is a comment
# There are four registers in emulated processor: r0 ... r4
#
# Register transfer instruction mov:
mov r1 3  # moves value 3 into register r1
mov r1 r2 # moves value of register r2 into register r1

# Arithmetic instructions:
add r1 r2 r3 # adds value from r2 and r3 and saves result into r1
add r1 r2 10 # adds value from r2 and immediate value 10, saves result into r1

sub r1 r2 r3 # r1 <- r2 - r3
sub r1 r2 10 # r1 <- r2 - 10

mul r1 r2 r3 # r1 <- r2 * r3
mul r1 r2 10 # r1 <- r2 * 10

div r1 r2 r3 # r1 <- r2 / r3
div r1 r2 10 # r1 <- r2 / 10

mod r1 r2 r3 # r1 <- r2 mod r3
mod r1 r2 10 # r1 <- r2 mod 10

cmp r1 r2    # compares values from registers set flags.E(equal) and flags.GT(greater than)
cmp r1 10    # compares value from r1 and immediate value 10

sbn r0 5 label  # Subtructs 5 from r0 and jumps to label if r0 is negative
sbn r0 r1 label # Subtructs r1 from r0 and jumps to label if r0 is negative
sbn r0 5 _      # Subtructs 5 from r0 and jumps to the next commnad if negative (so it no jump actually)
sbn r0 r1 _     # Subtructs r1 from r0 and jumps to the next commnad
```

The `sbn` command is added to do Exercise 1.7 and make sure it works.  
_asm_samples_ contains a program which calculates a factorial of a number (only 5! max as emulator is 8bit) using only `sbn` command

## TODO:
The Chapter 3 introduces the SimpleRisc ISA for 32-bit processor  
- [ ] Change current ISA to _SimpleRisc_ for asm compiler
- [ ] Migrate to 32-bit word
- [ ] Add ',' between arguments to match GNU assembler format
- [ ] Add mandatory '.' before label

### Local TODO:
- [x] Add tests for each group of instructions as in the book
- [ ] Encode 1 arg instruction in 32-bit format

- [x] Finish working on new function parse_arguments to handle ',' and 12[r1], it is not used for now
- [ ] Add more tests with value checks and other expressions
- [ ] Mirgate to this function in translate line
- [ ] Make arguments type information an array!
