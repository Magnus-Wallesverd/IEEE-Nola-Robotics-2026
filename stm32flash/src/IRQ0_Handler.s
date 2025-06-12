/* global allows the linker to find it */
.global IRQ0_Handler
.type IRQ0_Handler, %function

IRQ0_Handler:
    ldr R4, [R13], #4   /* load r4 from the address in r13 */

