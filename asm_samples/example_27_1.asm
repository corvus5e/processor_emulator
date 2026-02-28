# Example 27.1
# Write assembly code in SimpleRisc to compute: 31 / 29 - 50, and save the result in r4.
# And copmares r1 to 0 and compares r1 to r2.

mov r1, 31
mov r2, 29
div r3, r1, r2
sub r4, r3, 50
cmp r1, 0
cmp r1, r2
