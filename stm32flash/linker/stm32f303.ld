ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Define top-of-stack symbol globally so it's usable in startup.s */
_estack = ORIGIN(RAM) + LENGTH(RAM);
_sidata = LOADADDR(.data);

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vector))   /* Vector table */
    *(.text*)              /* Code */
    *(.rodata*)            /* Constants */
    _etext = .;            /* End of code */
  } > FLASH

  .data : AT(_etext)
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM
}
