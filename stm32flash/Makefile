# Makefile to build code
# make variables for relevent commands then script them
# from Command Line Run
# 1. make
# 2. make flash
# 3. make clean

# Tool-chain
CC = arm-none-eabi-gcc				# C compiler
AS = arm-none-eabi-as				# assembler
OBJCOPY = arm-none-eabi-objcopy		# copies obj files (Machine code)
CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Wall -Wextra -02 -nostdlib -ffreestanding -Iinclude # compiler flags
FLASH = st-flash					# flasher


# Linker
LD_SCRIPT = linker/stm32f303.ld
LD_FLAGS = -T$(LD_SCRIPT) -Wl,--gc-sections

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

# Compile .c -> .o
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Compile startup.s -> .o
build/startup.o: $(ASM)
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything into ELF
$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

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
