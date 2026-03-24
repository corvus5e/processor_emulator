add r1, r2, -27 #r1 = -27
add r3, r4, r1  #r3 = -27
mul r4, r1, 3   #r4 = -81
sub r5, r4, 50  #r5 = -131
div r6, r5, 10  #r6 = -13
st r6, 133[r6]   #mem[120, 121, 122, 123] = -13
b .label
add r7, r7, 20 #r7 = 20
.label:
add r7, r7, 40 #r7 = 40
mov r8, 11     #r8 = 11
mov r9, r1     #r9 = r1 = -27
cmp r9, -28    # E = 0, GT = 1

