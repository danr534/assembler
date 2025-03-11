; Invalid target operand type
A: lea B, #2

; Comma not between the operands
B: add ,r1 r3  

; Invalid register operand
C: jsr r4

; missing # before immediate
prn 4

; immediate value is too large (2^21)
sub #1048576, A  

; Immediate values cannot be the target operand in the operation 'add'
add r4, #5 

; missing comma between operands
D: mov #1 r2 

; comma not between operands
E: add ,D r1

; Missing ending quotes around string
F: .string "abcdef 

; illegal quotes in the middle of the string
.string "r%$#"&*()"

; Immediate value is too small (-2^21 - 1)
cmp #-1048577, r2

; operand value is too large (2^24)
.data 1, 2, 8388608

; operand value is too small (-2^24 - 1)
G: .data -8388609

; missing comma between operands
.data 5 -14,18

; comma is not between operands
.data ,1, 2

; illegal operand 'r' is not an integer
.data 10, -20, r

; missing space after .data
H: .data55  
