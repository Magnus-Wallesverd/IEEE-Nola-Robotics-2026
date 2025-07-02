.text
.global SysTick_Handler

/* SCB_ICSR block is 0xE000ED04 */
/* bit 28 write 1 pendsv exception pending <- only way to trigger pendsv. 0 to clear */

SysTick_Handler:
    ldr r0, =global_tick
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]

    bl update_speed

    ldr r0, =0x10000000
    ldr r1, =0xE000ED04
    str r0, [r1]
    bx lr
