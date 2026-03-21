#b .label
add r1, r2, -27 #r1 = -27
add r3, r4, r1  #r3 = -27
mul r4, r1, 3   #r4 = -81
sub r5, r4, 50  #r5 = -131
div r6, r5, 10  #r6 = -13
st r6, 40[r6]   #mem[27] = -13
#.label:

