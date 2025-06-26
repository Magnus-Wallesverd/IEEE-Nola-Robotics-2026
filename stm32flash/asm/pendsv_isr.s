.text
.global PendSV_Handler

/* calls the task scheduler for context switching information */

PendSV_Handler:

    /* context save */
    
    push {r4-r7}
    /* scheduler */
    bl taskscheduler
    
    pop {r4-r7}
    
; context_switch: 
    
    /* move sp into r4 */
    /* load r0 into r1 */
    /* move r1 into sp */
    
    mov r4, sp
    ldr r1, [r0]
    mov sp, r1
    push {r0-r3}
    mov sp, r4
    bx lr
