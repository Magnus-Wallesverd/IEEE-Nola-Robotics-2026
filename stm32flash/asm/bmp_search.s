.syntax unified
.text
.global bmp_search
.type bmp_search, %function

/* returns the right shifted bit */
bmp_search:
    mov r2, #0x0 
loop:
    lsrs r0, r0, 0x1
    mrs  r1, APSR
    lsrs r1, r1, 0x1D
    AND  r1, r1, #0x1
    cmp  r1, 0
    itt ne
    addne r2, r2,#0x1
    bne loop
    mov r0, r2
    bx lr
