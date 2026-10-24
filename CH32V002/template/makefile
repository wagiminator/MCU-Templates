# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  CH32V002 Example
# Author:   Stefan Wagner
# Year:     2024
# URL:      https://github.com/wagiminator    
# ===================================================================================
# Install toolchain:
#   sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
#   sudo apt install python3 python3-pip
#   pip install rvprog
#
# Provide access permission to WCH-LinkE programmer:
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8010", MODE="666"' | sudo tee /etc/udev/rules.d/99-WCH-LinkE.rules
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8012", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-WCH-LinkE.rules
#   sudo udevadm control --reload-rules
#
# Connect WCH-LinkE programmer to your board. Type "make flash" in the command line.
# ===================================================================================

# Files and Folders
TARGET   = firmware
INCLUDE  = include
SOURCE   = src
BIN      = bin

# Microcontroller Settings (can be overridden on the command line, e.g. "make F_CPU=24000000")
# MCU: ch32v002, ch32v004, ch32v005, ch32v006, ch32v007, ch32m007 (see folder ld)
MCU     ?= ch32v002
F_CPU   ?= 48000000
LDSCRIPT = ld/$(MCU).ld
CPUARCH  = -march=rv32emc -mno-div -mabi=ilp32e

# Toolchain
PREFIX   = riscv64-unknown-elf
CC       = $(PREFIX)-gcc
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
OBJSIZE  = $(PREFIX)-size
NEWLIB  ?= /usr/include/newlib
ISPTOOL ?= rvprog -f $(BIN)/$(TARGET).bin
CLEAN    = rm -f *.lst *.obj *.cof *.list *.map *.eep.hex *.o *.d

# Compiler Flags
CFLAGS   = -g -Os -flto -ffunction-sections -fdata-sections -fno-builtin -nostdlib
CFLAGS  += -fno-unwind-tables -fno-asynchronous-unwind-tables
CFLAGS  += $(CPUARCH) -DF_CPU=$(F_CPU) -I$(NEWLIB) -I$(INCLUDE) -I$(SOURCE) -I. -Wall
LDFLAGS  = -T$(LDSCRIPT) -lgcc -Wl,--gc-sections,--build-id=none
CFILES   = $(wildcard ./*.c) $(wildcard $(SOURCE)/*.c) $(wildcard $(SOURCE)/*.S)
HFILES   = $(wildcard ./*.h) $(wildcard $(SOURCE)/*.h) $(wildcard $(INCLUDE)/*.h)

# Targets are run in the order listed, so parallel builds (-j) are disabled
.NOTPARALLEL:
.PHONY: help all elf bin hex asm flash clean size removetemp removeelf

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all       compile and build $(TARGET).elf/.map/.lst/.bin/.hex/.asm"
	@echo "make hex       compile and build $(TARGET).hex"
	@echo "make asm       compile and disassemble to $(TARGET).asm"
	@echo "make bin       compile and build $(TARGET).bin"
	@echo "make flash     compile and upload to MCU"
	@echo "make clean     remove all build files"

$(BIN)/$(TARGET).elf: $(CFILES) $(HFILES) $(LDSCRIPT) makefile
	@echo "Building $(BIN)/$(TARGET).elf ..."
	@mkdir -p $(BIN)
	@$(CC) -o $@ $(CFILES) $(CFLAGS) $(LDFLAGS) -Wl,-Map=$(BIN)/$(TARGET).map

$(BIN)/$(TARGET).lst: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).lst ..."
	@$(OBJDUMP) -S $^ > $(BIN)/$(TARGET).lst

$(BIN)/$(TARGET).bin: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).bin ..."
	@$(OBJCOPY) -O binary $< $(BIN)/$(TARGET).bin

$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).hex ..."
	@$(OBJCOPY) -O ihex $< $(BIN)/$(TARGET).hex

$(BIN)/$(TARGET).asm: $(BIN)/$(TARGET).elf
	@echo "Disassembling to $(BIN)/$(TARGET).asm ..."
	@$(OBJDUMP) -d $(BIN)/$(TARGET).elf > $(BIN)/$(TARGET).asm

all:	$(BIN)/$(TARGET).lst $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).asm size

elf:	$(BIN)/$(TARGET).elf removetemp size

bin:	$(BIN)/$(TARGET).bin removetemp size removeelf

hex:	$(BIN)/$(TARGET).hex removetemp size removeelf

asm:	$(BIN)/$(TARGET).asm removetemp size removeelf

flash:	$(BIN)/$(TARGET).bin size removeelf
	@echo "Uploading to MCU ..."
	@$(ISPTOOL)

clean:
	@echo "Cleaning all up ..."
	@$(CLEAN)
	@rm -f $(BIN)/$(TARGET).elf $(BIN)/$(TARGET).lst $(BIN)/$(TARGET).map $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).asm

size:
	@echo "------------------"
	@echo "FLASH: $(shell $(OBJSIZE) -d $(BIN)/$(TARGET).elf | awk 'NR==2 {print $$1 + $$2}') bytes"
	@echo "SRAM:  $(shell $(OBJSIZE) -d $(BIN)/$(TARGET).elf | awk 'NR==2 {print $$2 + $$3}') bytes"
	@echo "------------------"

removetemp:
	@echo "Removing temporary files ..."
	@$(CLEAN)

removeelf:
	@echo "Removing $(BIN)/$(TARGET).elf ..."
	@rm -f $(BIN)/$(TARGET).elf $(BIN)/$(TARGET).map
