#*Ex. 7 — Consider the sbn instruction that subtracts the second operand from the first
# operand, and branches to the instruction specified by the label (third operand), if the result is
# negative. Write a small program using only the sbn instruction to compute the factorial of a
# positive number.

sbn r1 r1 _ #r1 - mul counter, make it zero
sbn r2 r2 _ #r2 - summand, make it zero
sbn r1 1, _ # put -1 into counter for first mul

sbn r3 r3 _ #r3 - input factorial counter, make it zero
sbn r3 -5 _ #put 5 into factoial counter input

sbn r2 r3 _ #r2 should be always negative

fac_loop:
sub r0 r0 _ #r0 - result, make it zero

# Multiply two numbers r0 = r2 * r1 
# Requires to set up registers:
# r2 - summand
# r1 - counter
# Result will be in r0
sbn r0 r2 _ # add summand first time, as counter (r1) will be decreased by 1 next line
mul_loop:
sbn r1 -1 mul_continue
sbn r2 0 mul_done
mul_continue:
sbn r0 r2 _
sbn r2 0 mul_loop
mul_done:

sbn r2 r2 _
sbn r2 r0 _
sbn r3 1 fac_done
sbn r1 r3 _
sbn r2 0 fac_loop

fac_done:

exit


