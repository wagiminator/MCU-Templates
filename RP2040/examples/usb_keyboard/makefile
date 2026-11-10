# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  Raspberry Pi Pico Example - USB Keyboard
# Author:   Stefan Wagner
# Year:     2026
# URL:      https://github.com/wagiminator
# ===================================================================================
# Install toolchain:
#   sudo apt install build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi
#   sudo apt install python3 python3-pip
#   pip install rpprog
#
# Provide access permission to the RP2040 USB bootloader (for rpprog):
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="2e8a", ATTR{idProduct}=="0003", MODE="666"' | sudo tee /etc/udev/rules.d/99-rp2040.rules
#   sudo udevadm control --reload-rules
#
# Settings are made in config.h. Hold the BOOTSEL button while connecting the board
# via USB (USB bootloader), then type "make flash" in the command line.
# ===================================================================================

# Files and Folders
TARGET   = usb_keyboard
INCLUDE  = include
SOURCE   = src
BIN      = bin

# Microcontroller Settings
CONFIG   = config.h
F_CPU   ?= $(shell sed -n 's/^\#define[ \t]\+F_CPU[ \t]\+\([0-9]\+\).*/\1/p' $(CONFIG))
LDSCRIPT = ld/rp2040_2mb.ld
CPUARCH  = -mcpu=cortex-m0plus -mthumb

# Toolchain
PREFIX   = arm-none-eabi
CC       = $(PREFIX)-gcc
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
OBJSIZE  = $(PREFIX)-size
ISPTOOL  = rpprog -f $(BIN)/$(TARGET).bin
SWDTOOL  = openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program $(BIN)/$(TARGET).elf verify reset exit"
CLEAN    = rm -f *.lst *.obj *.cof *.list *.map *.eep.hex *.o *.d

# Compiler Flags
CFLAGS   = -g -Os -flto $(CPUARCH) -DF_CPU=$(F_CPU) -I$(INCLUDE) -I$(SOURCE) -I.
CFLAGS  += -fdata-sections -ffunction-sections -fno-builtin -fno-common -Wall
LDFLAGS  = -T$(LDSCRIPT) -static -nostartfiles -lc -lm
LDFLAGS += -Wl,--gc-sections,--build-id=none --specs=nano.specs --specs=nosys.specs
LDFLAGS += $(shell $(CC) -Wl,--no-warn-rwx-segments -Wl,--version >/dev/null 2>&1 && echo -Wl,--no-warn-rwx-segments)
CFILES   = $(wildcard $(SOURCE)/*.c)

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all       compile and build $(TARGET).elf/.bin/.hex/.asm"
	@echo "make hex       compile and build $(TARGET).hex"
	@echo "make asm       compile and disassemble to $(TARGET).asm"
	@echo "make bin       compile and build $(TARGET).bin"
	@echo "make flash     compile and upload to MCU (USB bootloader, rpprog)"
	@echo "make flash-swd compile and upload to MCU via SWD (CMSIS-DAP, OpenOCD)"
	@echo "make clean     remove all build files"

$(BIN)/$(TARGET).elf: $(CFILES) $(CONFIG)
	@echo "Building $(BIN)/$(TARGET).elf ..."
	@mkdir -p $(BIN)
	@$(CC) -o $@ $(CFILES) $(CFLAGS) $(LDFLAGS)

$(BIN)/$(TARGET).lst: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).lst ..."
	@$(OBJDUMP) -S $^ > $(BIN)/$(TARGET).lst

$(BIN)/$(TARGET).map: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).map ..."
	@$(OBJDUMP) -t $^ > $(BIN)/$(TARGET).map

$(BIN)/$(TARGET).bin: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).bin ..."
	@$(OBJCOPY) -O binary $< $(BIN)/$(TARGET).bin

$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).hex ..."
	@$(OBJCOPY) -O ihex $< $(BIN)/$(TARGET).hex

$(BIN)/$(TARGET).asm: $(BIN)/$(TARGET).elf
	@echo "Disassembling to $(BIN)/$(TARGET).asm ..."
	@$(OBJDUMP) -d $(BIN)/$(TARGET).elf > $(BIN)/$(TARGET).asm

all:	$(BIN)/$(TARGET).lst $(BIN)/$(TARGET).map $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).asm size

elf:	$(BIN)/$(TARGET).elf removetemp size

bin:	$(BIN)/$(TARGET).bin removetemp size removeelf

hex:	$(BIN)/$(TARGET).hex removetemp size removeelf

asm:	$(BIN)/$(TARGET).asm removetemp size removeelf

flash:	$(BIN)/$(TARGET).bin size removeelf
	@echo "Uploading to MCU ..."
	@$(ISPTOOL)

flash-swd: $(BIN)/$(TARGET).elf size
	@echo "Uploading to MCU via SWD ..."
	@$(SWDTOOL)

clean:
	@echo "Cleaning all up ..."
	@$(CLEAN)
	@rm -f $(BIN)/$(TARGET).elf $(BIN)/$(TARGET).lst $(BIN)/$(TARGET).map $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).asm

size:
	@echo "------------------"
	@echo "FLASH: $(shell $(OBJSIZE) -d $(BIN)/$(TARGET).elf | awk '/[0-9]/ {print $$1 + $$2}') bytes"
	@echo "SRAM:  $(shell $(OBJSIZE) -d $(BIN)/$(TARGET).elf | awk '/[0-9]/ {print $$2 + $$3}') bytes"
	@echo "------------------"

removetemp:
	@echo "Removing temporary files ..."
	@$(CLEAN)

removeelf:
	@echo "Removing $(BIN)/$(TARGET).elf ..."
	@rm -f $(BIN)/$(TARGET).elf
