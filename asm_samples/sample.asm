# Simple program that calculates 5 * 4
load r0 5
load r1 4
loop:
sub r1 1 # We already have r0 * 1
jnz r1 continue 
jmp return
continue:
add r0 r0
jmp loop
return:
exit
