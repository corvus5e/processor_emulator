# Example 31 Write an assembly program to find the least common multiple (LCM) of two
# positive numbers stored in r1 and r2. Save the result in r0.

# let the numbers be A(r1) and B(r2)
# iterate
	mov r3, 1      # idx = 1
	mov r4, r1     # L = A
.loop:
	mod r5, r4, r2 # tmp = L % b
	cmp r5, 0      # compare mod with 0
	beq .lcm       # LCM found (L is the LCM)
	add r3, r3, 1  # increment idx
	mul r4, r1, r3 # L = A * idx
	b .loop
.lcm:
	mov r0, r4     # result is equal to L








