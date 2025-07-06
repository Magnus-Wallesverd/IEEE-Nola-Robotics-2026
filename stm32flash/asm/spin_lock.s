.syntax unified
.text
.global lock
.type lock, %function

/* Basic ARM implementation spin lock */
lock:
    ldr r2, =flag
    mov r1, #1
try:
    ldrex r0, [r2]
    cmp r0, #0
    itt eq
    strexeq r0, r1, [r2]
    cmpeq r0, #0
    bne try
    bx lr

.global unlock
.type unlock, %function

unlock:
    ldr r2, =flag
    mov r1, #0
    str r1, [r2]
    bx lr

