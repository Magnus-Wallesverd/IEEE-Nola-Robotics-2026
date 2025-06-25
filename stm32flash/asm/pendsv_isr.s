.text
.global PendSV_Handler

/* calls the task scheduler for context switching information */

PndSV_Handler:

    
    
    bl taskscheduler
    

    
; context_switch: 
    
    /* move sp into r4 */
    /* load r0 into r1 */
    /* move r1 into sp */
    /*
    mov r4, sp
    ldr r1, [r0]
    mov sp, r1
    push {r0-r3}
    mov sp, r4
    bx lr
    bx lr
