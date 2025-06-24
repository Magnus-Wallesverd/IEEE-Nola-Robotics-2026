.text
.global context_switch
.type context_switch, %function

/* context switches from caller passed address */
/* should be stack pointer in tcb */
//balls
context_switch: 
    
    movw r0, sp
    push {r0-r12}
    
    bx lr

foo: b foo
