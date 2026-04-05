# Example 33
# Write a function, f oo, in SimpleRisc that adds the values in registers r0, and r1, and saves
# the result in r2. Then write another function that invokes this function. The invoking
# function needs to first set r0 to 3, r1 to 5, and then invoke f oo. After f oo returns, it needs
# to add 10 to the result of f oo, and finally save the sum in r3.

	b .main
.foo:
	add r2, r0, r1
	ret
.main:
	mov r0, 3
	mov r1, 5
	call .foo
	add r3, r2, 10
