# Simple program that calculates 5 * 4
load r0 5 #summand
load r1 4 #counter
load r2 0 #result
loop:
jnz r1 continue 
jmp return
continue:
add r2 r0
sub r1 1
jmp loop
return:
exit
