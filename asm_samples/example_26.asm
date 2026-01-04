# Example 26 from Chapter 3
# Write assembly code in SimpleRisc to compute: 31 * 29 - 50, and save the result in r4.

mov r1 3 # Was 31 as we have inly 8 bits for now
mov r2 29
mul r3 r1 r2
sub r4 r3 50
