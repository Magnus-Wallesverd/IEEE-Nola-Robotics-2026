#! /bin/zsh
# flash script for stm32303RE board

SRC_FILES="startup.s *.c"

# calls the gcc compiler -mcpu is the processor, mfpu is floating point math unit
# -T is the linker script flag, which needs linker startup and c file
# -nostdlib prevents stdlib stuff from being linked
# -Wl discards unused code
# -o is output 
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
  -T linker/stm32f303.ld src/startup.s src/main.c src/gpio.c src/timx.c \
  -nostdlib -Wl,--gc-sections -o build/firmware.elf \
  -Iinclude

# -O outputs to raw binary 
arm-none-eabi-objcopy -O binary build/firmware.elf build/firmware.bin

# flashes the bin at the address 
st-flash write build/firmware.bin 0x8000000
