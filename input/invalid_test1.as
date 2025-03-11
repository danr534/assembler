; Too many operands
A: sub r3   ,    r6, r2

; 'move' is an invalid opcode
B:   move r1, COUNT  

; label can't start with a '@'
@C:   jmp FLAG

; Missing operand
prn 

; A is already defined
.extern A

; A is already defined
A: .data 1, 2, 3

; 'bne' only takes one operand
bne XYZ, r2 

mcro a_mc
; Immediate values cannot be the target operand in the operation 'add'
add r4, #5 

; 'rts' takes no operands
D:  rts M 

; missing colon after label
E stop
mcroend

; Missing quotes around string
F: .string abcdef 

; Too many operands
G: cmp #1, #2 #3

; Missing second operand
cmp r4

; Can't use relative addressing with the operation 'mov'
mov &L

; Immediate values are not allowed with 'red'
red #5 

a_mc

; Label can't have '$'
clr ABC$

; Typo in '.data'
H: .dat  55  
