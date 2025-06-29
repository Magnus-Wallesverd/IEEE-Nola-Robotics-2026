.text
.global PendSV_Handler

/* calls the task scheduler for context switching information */

PendSV_Handler:
   
    /* set current to next */
    ldr r0, =next_tcb
    ldr r0, [r0]
    ldr r1, =current_tcb
    str r0, [r1]
    
    /* returns next_tcb into r0*/
    bl taskscheduler 
    
    /* update next. LOOK AHEAD implememtation! careful*/
    ldr r2, [r1]
    ldr r3, [r2]
    msr psp, r3
    ISB
    
    /* Exception return */
    ldr r0, =0xFFFFFFFD
    bx r0

