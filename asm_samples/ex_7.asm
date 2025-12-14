#*Ex. 7 — Consider the sbn instruction that subtracts the second operand from the first
# operand, and branches to the instruction specified by the label (third operand), if the result is
# negative. Write a small program using only the sbn instruction to compute the factorial of a
# positive number.

#Idea: we will multiply in negatives and then revert result

#Multiply two numbers
sbn r2 r2 a0  # Make r2 zero
a0:
sbn r3 r3 a1 # Make r3 zero
a1:
sbn r2 5 a2 # r2 = -5
a2:
sbn r3 4 a3 # r3 = -4
a3:
exit


