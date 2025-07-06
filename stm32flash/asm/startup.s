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
  .word WWDG + 1 
  .word 0, 0, 0, 0, 0, 0, 0, 0
  .word 0, 0, 0, 0, 0, 0, 0, 0 
  .word 0, 0, 0, 0, 0, 0, 0, 0 
  .word 0, 0 
  /* .word TIM1_CC + 1*/

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

/* Load up CCMRAM boundary addresses */
  ldr r0, =_ccmram
  ldr r1, =_eccmram

copy_data2:
  cmp r0, r1
  ittt lt
  ldrlt r3, [r2], #4
  strlt r3, [r0], #4
  blt copy_data2

 
  ldr r4, = _thread1_end 
  ldr r5, = _thread10_end
  ldr r6, = _thread1_start
  mov r7, sp
  ldr r8, = _stcb
  ldr r9, = _stcb

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
  add r6, r6, #0x400
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

  /* C initializers */
  bl tcbinit
  /*bl taskinit*/
  bl systeminit

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
WWDG:               b .
/* TIM1_CC:         b .*/
