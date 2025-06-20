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
LIBS = -lc -lm -lgcc

# Source Files
SRC = $(wildcard src/*.c)
ASM = $(wildcard src/*.s)

ASM_OBJ = $(ASM:src/%.s=build/%.o)
C_OBJ = $(SRC:src/%.c=build/%.o)

OBJ := $(C_OBJ) $(ASM_OBJ)

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
build/%.o: src/%.s
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything into ELF
$(ELF): $(OBJ)
# echo $(LD_SCRIPT)
	$(CC) $(CFLAGS) $(LD_FLAGS) $(OBJ) $(LIBS) -o $@

# Convert ELF -> BIN
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Flash using st-flash
flash: $(BIN)
	$(FLASH) write $(BIN) 0x8000000

# debug
debug:
	@tmux new-session -d -s debug -n main
	@tmux send-keys -t debug:main 'nvim' C-m
	@tmux send-keys -t debug:main ':e. ' C-m
	@tmux split-window -h -t debug:main
	@tmux send-keys -t debug:main.1 'openocd -f interface/stlink.cfg -f target/stm32f3x.cfg' C-m
	@tmux split-window -v -t debug:main.1
	@tmux send-keys -t debug:main.2 'arm-none-eabi-gdb $(ELF)' C-m
	@tmux send-keys -t debug:main.2 'c' C-m
	@tmux send-keys -t debug:main.2 'target extended-remote localhost:3333' C-m		# gdb connects to openocd via localhost!
	@tmux send-keys -t debug:main.2 'monitor arm semihosting enable' C-m
#@tmux send-keys -t debug:main.2 'file $(ELF)' C-m
	@tmux attach -t debug

# Clean
clean:
	rm -rf build

.PHONY: all flash debug clean
