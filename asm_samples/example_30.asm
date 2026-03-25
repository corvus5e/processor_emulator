# Example 30 Write an assembly program to nd out if the number stored in r1 is a prime
# number. Assume that it is greater than 3. Save the Boolean result in r0.

mov r2, 2
.loop:
	mod r3, r1, r2  # divide number by r2
	cmp r3, 0       # compare the result with 0
	beq .notprime   # if the result is 0, not prime
	add r2, r2, 1   # increment r2
	cmp r1, r2      # compare r2 with the number
	bgt .loop       # iterate if r2 is smaller

mov r0, 1               # number is prime
b .exit                 # exit
.notprime:
	mov r0, 0       # number is not prime
.exit:
