.text
.global context_switch
.type context_switch, %function

/* context switches from caller passed address */
/* should be stack pointer in tcb */
//balls
context_switch: 
    
    mov r4, sp
    ldr r1, [r0]
    mov sp, r1
    push {r0-r3}
    mov sp, r4
    bx lr

.global foo
.type foo, %function

foo: 
    b foo
