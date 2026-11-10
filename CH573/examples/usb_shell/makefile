# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  CH573F Example - USB Shell
# Author:   Stefan Wagner
# Year:     2026
# URL:      https://github.com/wagiminator    
# ===================================================================================
# Install toolchain:
#   sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
#   sudo apt install python3 python3-pip
#   pip install chprog rvprog
#
# Provide access permission to WCH-LinkE programmer:
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8010", MODE="666"' | sudo tee /etc/udev/rules.d/99-WCH-LinkE.rules
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8012", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-WCH-LinkE.rules
#   sudo udevadm control --reload-rules
#
# Provide access permission to USB bootloader:
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-ch55x.rules
#   sudo udevadm control --reload-rules
#
# Hold the BOOT key (PB22) while connecting the board via USB (USB bootloader),
# then type "make flash" in the command line. Alternatively, connect a WCH-LinkE
# programmer to the 2-wire debug interface (PB14 = SWDIO, PB15 = SWCLK) and type
# "make flash-link".
# ===================================================================================

# Files and Folders
TARGET   = usb_shell
INCLUDE  = include
SOURCE   = src
BIN      = bin

# Microcontroller Settings
PORT    ?= $(firstword $(wildcard /dev/ttyACM*))
F_CPU    = 60000000
LDSCRIPT = ld/ch573.ld

# Toolchain (riscv64-unknown-elf or xPack riscv-none-elf)
PREFIX  ?= $(if $(shell which riscv64-unknown-elf-gcc 2>/dev/null),riscv64-unknown-elf,riscv-none-elf)
CC       = $(PREFIX)-gcc
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
OBJSIZE  = $(PREFIX)-size
NEWLIB   = /usr/include/newlib
ISPTOOL  = chprog $(BIN)/$(TARGET).bin
LNKTOOL  = rvprog -f $(BIN)/$(TARGET).bin
CLEAN    = rm -f *.lst *.obj *.cof *.list *.map *.eep.hex *.o *.d

# GCC >= 12 needs the Zicsr extension named explicitly
ZICSR   := $(shell echo 'int x;' | $(CC) -march=rv32imac_zicsr -mabi=ilp32 -c -x c -o /dev/null - 2>/dev/null && echo _zicsr)
CPUARCH  = -march=rv32imac$(ZICSR) -mabi=ilp32

# Compiler Flags
CFLAGS   = -g -Os -flto -ffunction-sections -fdata-sections -fno-builtin -nostdlib
CFLAGS  += $(CPUARCH) -DF_CPU=$(F_CPU) -I$(NEWLIB) -I$(INCLUDE) -I$(SOURCE) -I. -Wall
LDFLAGS  = -T$(LDSCRIPT) -lgcc -Wl,--gc-sections,--build-id=none
LDFLAGS += $(shell $(CC) -Wl,--no-warn-rwx-segments -Wl,--version >/dev/null 2>&1 && echo -Wl,--no-warn-rwx-segments)
CONFIG   = config.h
CFILES   = $(wildcard $(SOURCE)/*.c)

# Bluetooth LE: link precompiled WCH BLE library if needed (ble_handler.c in src)
BLE      = $(firstword $(wildcard ./ble_handler.c $(SOURCE)/ble_handler.c))
ifneq ($(BLE),)
  BLOBS  = $(firstword $(wildcard ./blobs $(SOURCE)/blobs))
  CFLAGS += -I$(BLOBS)
  LDFLAGS += $(BLOBS)/LIBCH57xBLE.a -lgcc
endif

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all       compile and build $(TARGET).elf/.bin/.hex/.asm"
	@echo "make hex       compile and build $(TARGET).hex"
	@echo "make asm       compile and disassemble to $(TARGET).asm"
	@echo "make bin       compile and build $(TARGET).bin"
	@echo "make flash     compile and upload to MCU via USB bootloader (chprog)"
	@echo "make flash-link compile and upload to MCU via WCH-LinkE (rvprog)"
	@echo "make monitor   open serial monitor (exit with CTRL-])"
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

flash-link: $(BIN)/$(TARGET).bin size removeelf
	@echo "Uploading to MCU via WCH-LinkE ..."
	@$(LNKTOOL)

monitor:
	@python3 -m serial.tools.miniterm --raw --eol LF $(PORT) 115200

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
