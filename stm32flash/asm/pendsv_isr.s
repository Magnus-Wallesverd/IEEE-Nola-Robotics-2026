.syntax unified
.text
.global PendSV_Handler

/* calls the task scheduler for context switching information */

PendSV_Handler:
    
    ldr r1, =current_tcb
    ldr r0, [r1] /*R0 is tcb address*/
    mrs r2, psp 
    stmdb r2, {r4-r11}
    str r2, [r0]

    /* returns next_tcb into r0*/
    bl taskscheduler 
    
    /* update to next */
    str r0, [r1]
    ldr r0, [r0]
    msr psp, r0
    
    /* Pop to Stack */
    sub r0, r0, #0x20
    ldmia r0!, {r4-r11}
    add r0, r0, #0x20
    ldr r2, [r1]
    str r0, [r2]
    

    /* Exception return */
    ldr r0, =0xFFFFFFFD
    bx r0

