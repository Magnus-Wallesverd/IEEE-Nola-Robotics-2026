.syntax unified     /*  linux syntax??? */
.cpu cortex-m4      /*  cpu directive */
.fpu fpv4-sp-d16    /*  fpu directive */
.thumb              /*  thumb mode directive */

/*  gas: "a" = allocatable section. %progbits = "section contains data"  */
.section .isr_vector, "a", %progbits   

/*  .type defines a metadata type attribute g_pfnVectors as data (%object) */
.type g_pfnVectors, %object

/*  declares the size of the symbol (.) means current address. so (.) minus start of g_pfnVectors */
.size g_pfnVectors, .-g_pfnVectors 

/*  declares an external reference that the linker resolves at compile time */
.extern _estack

/*  the '+1' adds a 1 into the LSB indicating the exception handler is thumb mode */
/*  pm0214 pg. 40 */
g_pfnVectors:
  .word _estack
  .word Reset_Handler + 1               /*  stores address to the actual Reset_Handler in vector table */
  .word NMI_Handler + 1
  .word HardFault_Handler + 1
  .word MemManage_Handler  + 1
  .word BusFault_Handler + 1
  .word UsageFault_Handler + 1
  .word 0, 0, 0, 0
  .word SVC_Handler + 1
  .word DebugMon_Handler + 1
  .word 0
  .word PendSV_Handler + 1
  .word SysTick_Handler + 1
  .word 0, 0, 0, 0, 0, 0, 0, 0
  .word 0, 0, 0
  .word DMA1_CH1_IRQHandler
  .word DMA1_CH2_IRQHandler
  .word DMA1_CH3_IRQHandler
  .word DMA1_CH4_IRQHandler
  .word DMA1_CH5_IRQHandler
  .word DMA1_CH6_IRQHandler
  .word DMA1_CH7_IRQHandler
  .word 0, 0, 0, 0, 0, 0 
  .word 0 
  .word TIM1_UP_TIM16_IRQHandler + 1
  .word TIM1_TRG_TIM17_IRQHandler + 1
  .word 0, 0, 0, 0
  .word I2C1_EV_IRQHandler + 1
  .word I2C1_ER_IRQHandler + 1
  .word I2C2_EV_IRQHandler + 1
  .word I2C2_ER_IRQHandler + 1
  .word SPI1_IRQHandler + 1
  .word SPI2_IRQHandler + 1
  .word USART1_IRQHandler + 1
  .word USART2_IRQHandler + 1
  .word USART3_IRQHandler + 1
  .word 0, 0, 0, 0, 0, 0, 0, 0
  .word 0, 0, 0, 0
  .word UART4_IRQHandler+ 1 
  .word UART5_IRQHandler+ 1

  /* Add peripheral ISRs as needed here */

/*  defines Reset_Handler's memory location */
.section .text.Reset_Handler, "ax", %progbits

/*  allows the linker to find it */
.global Reset_Handler

/*  declares Reset_Handler as a %function */
.type Reset_Handler, %function

/*  Actual function representation */
Reset_Handler:
  
  /* Copy .data from flash to RAM */
  /* = _name sets up a pointer */
  ldr r0, = _sdata /* start of RAM */
  ldr r1, = _edata /* end of .data in RAM */
  ldr r2, = _etext /* Source to copy */

copy_data:              /*  This function uses the N flag to keep looping */
  cmp r0, r1            /*  compare pointers to see if start < end */
  ittt lt               /*  if start < end */
  ldrlt r3, [r2], #4    /*  load r3 from address in [r2] then increment register 2 address #4 bytes */
  strlt r3, [r0], #4    /*  store r3 to address pointed to by [r0] then increment register 0 address #4 bytes */
  blt copy_data         /*  branch to copy_data if N is set  */


  ldr r0, = _spadding
  ldr r1, = _epadding

fill_padding:
  cmp   r0, r1
  ittt  lt
  movlt  r2, #0xdead
  strlt r2, [r0], #4
  blt fill_padding

copy_data2:
  cmp r0, r1
  ittt lt
  ldrlt r3, [r2], #4
  strlt r3, [r0], #4
  blt copy_data2

 
  ldr r4, = _thread2_start 
  ldr r5, = _thread_block_end
  ldr r6, = worker_function
  mov r7, sp
  ldr r8, = _stcb

init_frames:
  cmp  r4, r5
  orr  r2, r6, #1
  mov  r3, #0x01000000
  mov r12, #0
  mov  sp, r4
  push {r3}
  push {r2}
  push {r12,lr}
  mov r0, #0
  mov r1, #0
  mov r2, #0
  mov r3, #0
  push {r0-r3}
  
  str sp, [r8], #0x10
  add r4, r4, #0x400
  blt init_frames
  
  mov sp, r7

  /* Zero initialize .bss*/
  ldr r0, =_sbss
  ldr r1, =_ebss

zero_bss:
  cmp r0,r1             /*  zero out the bss section */
  ittt lt               /*  if start < end */
  movlt r2, #0          /*  move 0 into r2 */
  strlt r2, [r0], #4    /*  store r2 = 0 into address pointed to by R0 increment register 0 address #4 bytes */
  blt zero_bss          /*  branch back to zero if N is set */

  /*bl bmp_search*/

  /* C initializers */
  bl set_pc
  bl tcbinit
  bl task_queue_init
  bl systeminit
  bl lcd_init

set_global:
  ldr r0, =_stcb
  ldr r1, =current_tcb 
  ldr r2, =next_tcb
  str r0, [r1]
  str r0, [r2]

service:
  SVC #0


infinite_loop:
  b infinite_loop
  
/* Default handlers */
NMI_Handler:        b .
HardFault_Handler:  b .
MemManage_Handler:  b .
BusFault_Handler:   b .
UsageFault_Handler: b .
DebugMon_Handler:   b .

.section .text.Default_Handler, "ax", %progbits
.thumb_func

Default_Handler:
1: b 1b

/* Weak aliases */

.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler

.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler

.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler

.weak BusFault_Handler_Handler
.thumb_set BusFault_Handler, Default_Handler

.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler

.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler

.weak DMA1_CH1_IRQHandler
.thumb_set DMA1_CH1_IRQHAndler, Default_Handler

.weak DMA1_CH2_IRQHandler
.thumb_set DMA1_CH2_IRQHAndler, Default_Handler

.weak DMA1_CH3_IRQHandler
.thumb_set DMA1_CH3_IRQHAndler, Default_Handler

.weak DMA1_CH4_IRQHandler
.thumb_set DMA1_CH4_IRQHAndler, Default_Handler

.weak DMA1_CH5_IRQHandler
.thumb_set DMA1_CH5_IRQHAndler, Default_Handler

.weak DMA1_CH6_IRQHandler
.thumb_set DMA1_CH6_IRQHAndler, Default_Handler

.weak DMA1_CH7_IRQHandler
.thumb_set DMA1_CH7_IRQHAndler, Default_Handler

.weak TIM1_UP_TIM16_IRQHandler
.thumb_set TIM1_UP_TIM16_IRQHandler, Default_Handler

.weak TIM1_TRG_TIM17_IRQHandler
.thumb_set TIM1_TRG_TIM17_IRQHandler, Default_Handler

.weak I2C1_EV_IRQHandler
.thumb_set I2C1_EV_IRQHandler, Default_Handler

.weak I2C1_ER_IRQHandler
.thumb_set I2C1_ER_IRQHandler, Default_Handler

.weak I2C2_EV_IRQHandler
.thumb_set I2C2_EV_IRQHandler, Default_Handler

.weak I2C2_ER_IRQHandler
.thumb_set I2C2_ER_IRQHandler, Default_Handler

.weak SPI1_IRQHandler
.thumb_set SPI1_IRQHandler, Default_Handler 

.weak SPI2_IRQHandler
.thumb_set SPI2_IRQHandler, Default_Handler

.weak USART1_IRQHandler
.thumb_set USART1_IRQHandler, Default_Handler

.weak USART2_IRQHandler
.thumb_set USART2_IRQHandler, Default_Handler

.weak USART3_IRQHandler
.thumb_set USART3_IRQHandler, Default_Handler

.weak UART4_IRQHandler
.thumb_set UART4_IRQHandler, Default_Handler

.weak UART5_IRQHandler
.thumb_set UART5_IRQHandler, Default_Handler
