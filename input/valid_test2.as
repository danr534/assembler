    .entry START

XBEGIN: add r2   ,    r5

START:   cmp r4, TOTAL

CYCLE:   jmp MARK

    mcro M3
add r3, r6
    bne FINISH
    mcroend

prn #-1048576
bne CYCLE

M3

MARK:     dec X
bne CYCLE

FINISH:    stop

TEXT:    .string "ghijkl"

TOTAL: .data 9, -12, 20

X:      .data 7    ,    -33,7,7,-8388608,9

    mcro M4
mov temp1, num
sub temp2, temp1
    mcroend

M4
temp1: .data 3,8,-999,77
temp2: .data 3,8,-999,77
num: .string "xyz12   **pqrs"
