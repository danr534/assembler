; macro name can't be an operation name
mcro add

; macro name can't be a directive name
mcro .extern
mcro .data

; too many operands
mcro a_mc b_mc

mcro addi

cmp r1, r2
prn r1

; too many operands
mcroend w

mcroend

addi
