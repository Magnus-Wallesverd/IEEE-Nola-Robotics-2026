.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.section .isr_vector, "a", %progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0, 0, 0, 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

  /* Add peripheral ISRs as needed here */

.section .text.Reset_Handler
.global Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
  
  /* Copy .data from flash to RAM */

  ldr r0, = _sdata /* start of RAM */
  ldr r1, = _edata /* end of .data in RAM */
  ldr r2, = _etext /* Source to copy */

copy_data:
  cmp r0, r1
  ittt lt
  ldrlt r3, [r2], #4
  strlt r3, [r0], #4
  blt copy_data

  /* Zero initialize .bss */
  ldr r0, =_sbss
  ldr r1, =_ebss

zero_bss:
  cmp r0,r1
  ittt lt
  movlt r2, #0
  strlt r2, [r0], #4
  blt zero_bss

/* Call main */
  bl main

infinite_loop:
  b infinite_loop

/* Default handlers */
NMI_Handler:       b .
HardFault_Handler: b .
MemManage_Handler: b .
BusFault_Handler:  b .
UsageFault_Handler:b .
SVC_Handler:       b .
DebugMon_Handler:  b .
PendSV_Handler:    b .
SysTick_Handler:   b .

