# Example 28
# Compute not(a or b). Assume that a is stored in r0, and b is stored in r1. Store the result in r2
or r3, r0, r1
and r1, r0, r1
not r2, r3
