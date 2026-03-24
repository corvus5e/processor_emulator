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


## TODO:
The Chapter 3 introduces the SimpleRisc ISA for 32-bit processor
- [ ] Migrate to 32-bit word

### Local TODO:
- [ ] Decode 2 args instruction
- [ ] Add call and ret functions (page 114)
- [ ] Go back to loading 32 bit number into a register and modifiers (Chapter 3)
- [ ] Generate additional instructions while compilation to deal with immediate larger than 16 bits using modifiers
- [ ] Add tests for these modifiers
- [ ] Add examples 30, 31
