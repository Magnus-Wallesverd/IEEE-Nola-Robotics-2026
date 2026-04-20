ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
  CCMRAM  (rwx) : ORIGIN = 0x10000000, LENGTH = 10K
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

  .process_block :
  {
    . = ALIGN(4);
    _process1 = .;
    . += 512;
    _process2 = .;
    . += 512;
    _process3 = .;
    . += 512;
    _process4 = .;
    . += 512;
    _process5 = .;
    . += 512;
    _endblock = .;
    _shareblock = .;
    . += 512;

    _endshare = .;


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
