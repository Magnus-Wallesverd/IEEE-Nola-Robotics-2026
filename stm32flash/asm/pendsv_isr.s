.text
.global PendSV_Handler

/* calls the task scheduler for context switching information */

PendSV_Handler:

    /* random pointer value */
    ldr r3, =current_tcb
    
    /* r1 = 0x20000580*/
    ldr r1, [r3]
    
    /*r2 = psp = r2 = 0x20000580*/
    mrs r2, psp
    ISB
    
    /* returns next_tcb into r0*/
    bl taskscheduler 
    
    /* load sp value from r0 tcb.sp address */
    ldr r2, [r0]
    msr psp, r2
    ldr r3, =current_tcb
    str r0, [r3]
    ISB
    ldr r0, =0xFFFFFFFD
    bx r0
