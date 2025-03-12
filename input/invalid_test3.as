.extern A

.entry B

B: .string "hello"

; invalid entry label 'C' because its not defined in this file
.entry C

; the entry label 'C' is not defined in this file
cmp A, C

; label 'Q' is not defined
bne &Q

; invalid realtive addressing with the external label 'A'
jmp &A

; label 'Q' is not defined
mov Q, r1

; label 'Q' is not defined
prn Q

