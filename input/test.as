mcro a_mc
cmp K, #-6
bne &END
lea STR, r6
inc r6
mov r3, K
mcroend
sub r1, r4
bne END
dec K
jmp &LOOP
END: stop
a_mc
STR: .string “abcd”
LIST: .data 6, -9
.data -100
K: .data 31