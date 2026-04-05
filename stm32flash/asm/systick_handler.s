.syntax unified
.text
.global SysTick_Handler

/* SCB_ICSR block is 0xE000ED04 */
/* bit 28 write 1 pendsv exception pending <- only way to trigger pendsv. 0 to clear */
//r4 hold _process1
//r5 hold current_t pointer
SysTick_Handler:
    ldr r4, =_process1

    ldr r0, =global_tick
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]


    ldr r0, =wakeup
    ldr r3, [r0]
    cmp r1, r3
    blo buhn 

    ldr r2, [r4, #160]   // grabs the priority register
    orr r2, r2, #(1<<31)  // activate timer task
    str r2, [r4, #160]

buhn:
    ldr r5, =current_t
    ldr r5, [r5]
    ldrb r2, [r5, #9] // grab current task priroirty store in r2

    ldr r1, [r4, #160] //check priorityBit queue
    clz  r3, r1
    rsb r3, r3, #31

    //compare
    cmp r3, r2
    bgt switch


    ldrb r1, [r5, #12] //r0 + 12 is alloc 
    subs r1, r1, #1
    strb r1, [r5, #12]

    bne Skip_Switch

    ldrb r1, [r5, #13]
    strb r1, [r5, #12]


switch:
    ldr r0, =0x10000000
    ldr r1, =0xE000ED04
    str r0, [r1]



Skip_Switch:
    bx lr
