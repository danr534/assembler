.extern A

.entry B

; invalid entry label 'C' because its not defined in this file
.entry C

; B is already an entry label and can't be also an external label
.extern B

; label 'Q' is not defined
bne &Q

; B is already declared as an entry label
.entry B

; invalid realtive addressing with the external label 'A'
jmp &A

; A is already an external label and can't be an entry label as well
.entry A

; label 'Q' is not defined
mov Q, r1

; label 'Q' is not defined
prn Q

B: .string "hello"