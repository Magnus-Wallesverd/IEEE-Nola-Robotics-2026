#! /bin/zsh
# flash script for stm32303RE board

SRC_FILES="startup.s *.c"

# calls the gcc compiler -mcpu is the processor, mfpu is floating point math unit
# -T is the linker script flag, which needs linker startup and c file
# -nostdlib prevents stdlib stuff from being linked
# -Wl discards unused code
# -o is output 
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
  -T stm32f303.ld $SRC_FILES \
  -nostdlib -Wl,--gc-sections -o firmware.elf \
  -I.

# -O outputs to raw binary 
arm-none-eabi-objcopy -O binary firmware.elf firmware.bin

# flashes the bin at the address 
st-flash write firmware.bin 0x8000000

