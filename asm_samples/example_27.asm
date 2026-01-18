# Example 27 from Chapter 3, page 109
# Write assembly code in SimpleRisc to compute: 31 / 29 - 50, and save the result in r4.

mov r1 31
mov r2 29
div r3 r1 r2
sub r4 r3 50
