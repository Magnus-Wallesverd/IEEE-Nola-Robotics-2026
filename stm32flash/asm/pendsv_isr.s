.text
.global PendSV_Handler

/* calls the task scheduler for context switching information */

PendSV_Handler:
    bl taskscheduler
    ldr r1, [r0]
    sub r1, r1, #0x20
    msr psp,r1
    ISB
    ldr r0, =0xFFFFFFFD
    bx r0
