# Simple program
load r0 0
my_label:
load r1 @r0
jnz r0 my_label
#load r1 @5
#load r2 @r1
#foobarbie r0 3
#add r1 r2
#add r0 r1
exit
