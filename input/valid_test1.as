TXT1: .string "Check Run"

SIZE: .data  8, -3, 21

    .extern Sample

START:      mov r1 ,    r5

        V:   .data  7, 8388607 , 22, 11
;first comment, &*^%#!~()
CYCLE: jmp LABEL
prn #-7
sub r2    ,      r3
    bne      Sample

LABEL:  inc V

    .entry CYCLE

FINAL: stop