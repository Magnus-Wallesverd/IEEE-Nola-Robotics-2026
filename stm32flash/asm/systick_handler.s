.syntax unified
.text
.global SysTick_Handler
#include ""

/* SCB_ICSR block is 0xE000ED04 */
/* bit 28 write 1 pendsv exception pending <- only way to trigger pendsv. 0 to clear */

SysTick_Handler:
    ldr r0, =global_tick
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]

    ldr r0, =current_t
    ldr r0, [r0]
    ldrb r2, [r0, #9] // grab current task priroirty store in r2

    ldr r0, =_process1 //grab priority bit
    ldr r1, [r0, #160]
    clz  r3, r1
    rsb r3, r3, #31

    //compare
    cmp r3, r2
    bgt switch



    ldr r0, =current_t
    ldr r0, [r0]

    ldrb r1, [r0, #12] //r0 + 12 is alloc 
    subs r1, r1, #1
    strb r1, [r0, #12]

    bne Skip_Switch

    ldrb r1, [r0, #13]
    strb r1, [r0, #12]


switch:
    ldr r0, =0x10000000
    ldr r1, =0xE000ED04
    str r0, [r1]



Skip_Switch:
    bx lr
