.extern XYZ

YBEGIN: sub r3   ,    r6

INIT:   mov r1, COUNT

jmp FLAG
prn #1048575
bne XYZ
add r2, r4
    bne B

FLAG:  inc M
jmp XYZ

stop
.string "abcdef"

COUNT: .data 5,-14,18

M:      .data  27
    .extern B

    ; Another comment 123?><MNB
    LOOP: cmp r4, r5
    not r1
    clr r5
    red r2
    jsr &LOOP
rts

FINAL: stop
STR: .string "stuvwx"
DATA: .data 12,-18,30
N: .data  55
