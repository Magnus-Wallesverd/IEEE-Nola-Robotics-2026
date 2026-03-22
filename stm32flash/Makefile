CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy	
# Updated CFLAGS for G4 (Cortex-M4 with FPU)
CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Wall -Wextra -O0 -Wno-unused -nostdlib -ffreestanding -Iinclude -g
FLASH = st-flash

# Linker - Update to your new G431RB linker script path
LD_SCRIPT = linker/stm32g431rb.ld
LD_FLAGS = -T$(LD_SCRIPT) -Wl,-Map=build/build.map,--gc-sections -g 
LIBS = -lgcc

# Source Files
SRC = $(wildcard src/*.c)
ASM = $(wildcard asm/*.s)

# Ensure build directory exists for object mapping
ASM_OBJ = $(ASM:asm/%.s=build/%.o)
C_OBJ = $(SRC:src/%.c=build/%.o)
OBJ := $(C_OBJ) $(ASM_OBJ)

TARGET = firmware
ELF = build/$(TARGET).elf
BIN = build/$(TARGET).bin

all: $(BIN)

# Compile .c -> .o
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Compile startup.s -> .o (using CC to handle preprocessor if needed)
build/%.o: asm/%.s
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything into ELF
$(ELF): $(OBJ)
	$(CC) $(CFLAGS) $(LD_FLAGS) $(OBJ) $(LIBS) -o $@

# Convert ELF -> BIN
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Flash using st-flash
flash: $(BIN)
	$(FLASH) write $(BIN) 0x8000000

# Updated debug for STM32G4
debug:
	@tmux new-session -d -s debug -n main
	@tmux send-keys -t debug:main 'nvim' C-m
	@tmux split-window -h -t debug:main
	@tmux send-keys -t debug:main.1 'openocd -f interface/stlink.cfg -f target/stm32g4x.cfg' C-m
	@tmux split-window -v -t debug:main.1
	@tmux send-keys -t debug:main.2 'arm-none-eabi-gdb $(ELF)' C-m
	@tmux send-keys -t debug:main.2 'target extended-remote localhost:3333' C-m
	@tmux send-keys -t debug:main.2 'load' C-m
	@tmux attach -t debug

dump:
	@arm-none-eabi-objdump -S $(ELF) > build/dump.txt

disas:
	@arm-none-eabi-objdump -D $(ELF) > build/out.asm

clean:
	rm -rf build

.PHONY: all flash debug dump disas clean
