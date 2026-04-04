.syntax unified
.text
.global PendSV_Handler

/* calls the thread scheduler for context switching information */

PendSV_Handler:
    
    mrs r2, psp 
    stmdb r2!, {r4-r11}
    LDR r1, =current_t
    LDR r1, [r1]
    STR r2, [r1]


    /* returns next_tcb into r0*/
    bl select_task 
    LDR r1, =current_t
    LDR r1, [r1]
    LDR r0, [r1]
    /* Pop to Stack */
    ldmia r0!, {r4-r11}
    //ldr r2, [r1]
    //str r0, [r2]
    msr psp, r0
    

    /* Exception return */
    ldr r0, =0xFFFFFFFD
    bx r0
