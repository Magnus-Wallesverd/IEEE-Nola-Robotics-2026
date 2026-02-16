.syntax unified
.text
.global SVC_Handler

SVC_Handler:

psp_move:
    ldr r0, = _stcb
    ldr r1, [r0]
    msr psp, r1

 
SysTick_start:
    ldr r0, =0xE000E010
    ldr r1, [r0]
    orr r1, r1, #1
    str r1, [r0]
   
    /* Exception return */
    ldr r0, =0xFFFFFFFD
    bx r0
