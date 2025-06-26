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

  .data : AT(_etext)
  {
    /* sdata is start of data and edata is the end of data */
    _sdata = .; 
    *(.data*)
    _edata = .;
  } > RAM
  
  .ccmram :
  {
    _ccmram = .;
    *(.ccmram*)
    _eccmram = .;

  } > CCMRAM AT> FLASH



  _ltask_flash = _etext + (_edata - _sdata);

  .tcb :
  {
    _stcb = .;
    . = . + 0xA0;
    _etcb = .;
  } > RAM

  .padding :
  {
    _spadding = .;
    . = . + 0x100;
    _epadding = .;
  } > RAM


  /* STATIC SUB BLOCK MANAGEMENT*/
  . = ALIGN(1024);
  .task_load : AT(_ltask_flash)
  {
    _stask_load = .;
   
    _task1_start = .;
    KEEP(*(.task1))
    _task1_end = _task1_start + 0x400;  
    . = _task1_end;

    _task2_start = .;
    KEEP(*(.task2))
    _task2_end = _task2_start + 0x400;  
    . = _task2_end;

    _task3_start = .;
    KEEP(*(.task3))
    _task3_end = _task3_start + 0x400;  
    . = _task3_end;

    _task4_start = .;
    KEEP(*(.task4))
    _task4_end = _task4_start + 0x400;  
    . = _task4_end;

    _task5_start = .;
    KEEP(*(.task5))
    _task5_end = _task5_start + 0x400;  
    . = _task5_end;

    _task6_start = .;
    KEEP(*(.task6))
    _task6_end = _task6_start + 0x400;  
    . = _task6_end;

    _task7_start = .;
    KEEP(*(.task7))
    _task7_end = _task7_start + 0x400;  
    . = _task7_end;

    _task8_start = .;
    KEEP(*(.task8))
    _task8_end = _task8_start + 0x400;  
    . = _task8_end;

    _task9_start = .;
    KEEP(*(.task9))
    _task9_end = _task9_start + 0x400;  
    . = _task9_end;

    _task10_start = .;
    KEEP(*(.task10))
    _task10_end = _task10_start + 0x400;  
    . = _task10_end;


    _etask_load = .;
  } > FLASH 

  .taskspace (NOLOAD) :
  {
    _staskspace = .;
    . = . + 0x2800;
    _etaskspace = .;
  } > RAM

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM
}
