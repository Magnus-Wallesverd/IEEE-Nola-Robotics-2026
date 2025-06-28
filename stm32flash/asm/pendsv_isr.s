.text
.global PendSV_Handler

/* calls the task scheduler for context switching information */

PendSV_Handler:
    
    ldr r0, =_stcb

    mrs r1, psp 
    str r1, [r0]

    bl taskscheduler
    ldr r1, [r0]
    msr psp,r1
    ISB
    ldr r0, =0xFFFFFFFD
    bx r0
