.text
.global context_switch
.type context_switch, %function

/* context switches from caller passed address */
/* should be stack pointer in tcb */
/* TODO investigate mov r0 r0 */
context_switch: 
    
    /* move sp into r4 */
    /* load r0 into r1 */
    /* move r1 into sp */
    mov r4, sp
    ldr r1, [r0]
    mov sp, r1
    push {r0-r3}
    mov sp, r4
    bx lr

