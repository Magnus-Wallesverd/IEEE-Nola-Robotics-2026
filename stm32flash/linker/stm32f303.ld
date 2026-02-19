ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
  CCMRAM  (rwx) : ORIGIN = 0x10000000, LENGTH = 16K
}

/* Define top-of-stack symbol globally so it's usable in startup.s */
_estack = ORIGIN(RAM) + LENGTH(RAM);
_sidata = LOADADDR(.data);

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vector))   /* Vector table that holds addresses*/
    *(.text*)              /* Code */
    *(.rodata*)            /* Constants */
    _etext = .;            /* End of code */
  } > FLASH

  .data : 
  {
    /* sdata is start of data and edata is the end of data */
    _sdata = . ; 
    *(.data*)
    _stcb = . ;
    . = . + 0xA0;
    
    _spadding = . ;
    . = . + 0x100 ;
    _epadding = . ;

    _edata = .;
  } > RAM AT > FLASH

  .task_stacks (NOLOAD) : 
  { 
   
    . = ALIGN(8);

    _thread_block_start = .;

    _thread1_start = .;
    . += 0x400;
    _thread2_start = .;
    . += 0x400;
    _thread3_start = .;
    . += 0x400;
    _thread4_start = .;
    . += 0x400;
    _thread5_start = .;
    . += 0x400;
    _thread6_start = .;
    . += 0x400;
    _thread7_start = .;
    . += 0x400;
    _thread8_start = .;
    . += 0x400;
    _thread9_start = .;
    . += 0x400;
    _thread10_start = .;
    . += 0x400;

    _thread_block_end = .;
    . = ALIGN(8);
  
  } > RAM 
 
  /* .node_heap (NOLOAD) : */
  /* { */
  /*   _node_heap_start = .; */
  /*   KEEP(*(.node_heap)) */
  /*   _node_heap_end = _node_heap_start + 0x400; */
  /*   . = _node_heap_end; */
  /* } > RAM */

  /*.ccmram :*/
  /*{*/
  /*  _ccmram = .;*/
  /*  *(.ccmram*)*/
  /*  _eccmram = .;*/
  /**/
  /*} > CCMRAM AT> FLASH*/

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM
}
