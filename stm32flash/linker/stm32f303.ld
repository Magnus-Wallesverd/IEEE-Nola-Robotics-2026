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


    _sthread_load = .;
   
    _thread1_start = .;
    KEEP(*(.thread1))
    _thread1_end = _thread1_start + 0x400;  
    . = _thread1_end;

    _thread2_start = .;
    KEEP(*(.thread2))
    _thread2_end = _thread2_start + 0x400;  
    . = _thread2_end;

    _thread3_start = .;
    KEEP(*(.thread3))
    _thread3_end = _thread3_start + 0x400;  
    . = _thread3_end;

    _thread4_start = .;
    KEEP(*(.thread4))
    _thread4_end = _thread4_start + 0x400;  
    . = _thread4_end;

    _thread5_start = .;
    KEEP(*(.thread5))
    _thread5_end = _thread5_start + 0x400;  
    . = _thread5_end;

    _thread6_start = .;
    KEEP(*(.thread6))
    _thread6_end = _thread6_start + 0x400;  
    . = _thread6_end;

    _thread7_start = .;
    KEEP(*(.thread7))
    _thread7_end = _thread7_start + 0x400;  
    . = _thread7_end;

    _thread8_start = .;
    KEEP(*(.thread8))
    _thread8_end = _thread8_start + 0x400;  
    . = _thread8_end;

    _thread9_start = .;
    KEEP(*(.thread9))
    _thread9_end = _thread9_start + 0x400;  
    . = _thread9_end;

    _thread10_start = .;
    KEEP(*(.thread10))
    _thread10_end = _thread10_start + 0x400;  
    . = _thread10_end;

    _ethread_load = .;

    _task_queue = .;
    KEEP(*(.task_queue))
    _task_queue_end = .;


    _edata = .;
  } > RAM AT>FLASH
  
  .ccmram :
  {
    _ccmram = .;
    *(.ccmram*)
    _eccmram = .;

  } > CCMRAM AT> FLASH


  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM
}
