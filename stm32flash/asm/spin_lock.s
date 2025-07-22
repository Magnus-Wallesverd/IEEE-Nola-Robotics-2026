.syntax unified
.text
.global lock
.type lock, %function

/* Basic ARM implementation spin lock */
lock:
    mov r1, #1
try:
    ldrex r2, [r0]
    cmp r2, #0
    itt eq
    strexeq r3, r1, [r0]
    cmpeq r3, #0
    bne fail
    mov r0, #1
    bx lr
fail:
    mov r0, #0
    bx lr

.global unlock
.type unlock, %function

unlock:
    mov r1, #0
    str r1, [r0]
    bx lr

.global yield
.type yield, %function

yield:
    ldr r0, =0x10000000
    ldr r1, =0xE000ED04
    str r0, [r1]
    ISB
    DSB
    wfi
    bx lr
