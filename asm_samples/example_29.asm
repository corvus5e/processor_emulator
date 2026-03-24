#Example 29
#Write an iterative program to compute the factorial of a number stored in r0. Assume that
#the number is greater than 2. Save the result in r1.
#Answer: Let us first take a look at a small C program to compute the factorial of the variable num.

# int prod = 1;
# int idx;
# for(idx = num; idx > 1; idx --) {
#	prod = prod * idx
# }

mov r1, 1  # prod = 1
mov r2, r0 # idx = num
.loop:
	mul r1, r1, r2 # prod = prod * idx
	sub r2, r2, 1  # idx = idx - 1
	cmp r2, 1      # compare (idx, 1)
	bgt .loop      # if (idx > 1) goto .loop
