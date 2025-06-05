# Makefile to build code
# make variables for relevent commands then script them
# from Command Line Run
# 1. make
# 2. make flash
# 3. make clean

# Tool-chain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy	
CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Wall -Wextra -O0 -nostdlib -ffreestanding -Iinclude -g
FLASH = st-flash

# Linker
LD_SCRIPT = linker/stm32f303.ld
LD_FLAGS = -T$(LD_SCRIPT) -Wl,--gc-sections -g

# Source Files
SRC = $(wildcard src/*.c)
ASM = src/startup.s
OBJ = $(SRC:src/%.c=build/%.o) build/startup.o

# Target path to put build files
TARGET = firmware
ELF = build/$(TARGET).elf
BIN = build/$(TARGET).bin

# Default Target
all: $(BIN)

ld:
	echo $(LD_SCRIPT)

# Compile .c -> .o
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Compile startup.s -> .o
build/startup.o: $(ASM)
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything into ELF
$(ELF): $(OBJ)
# echo $(LD_SCRIPT)
	$(CC) $(CFLAGS) $(LD_FLAGS) $(OBJ) -o $@

# Convert ELF -> BIN
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Flash using st-flash
flash: $(BIN)
	$(FLASH) write $(BIN) 0x8000000

# Clean
clean:
	rm -rf build

.PHONY: all flash clean
