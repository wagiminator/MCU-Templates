# ===================================================================================
# Project:  Example for tinyAVR 0/1-series
# Author:   Stefan Wagner
# Year:     2021
# URL:      https://github.com/wagiminator    
# ===================================================================================
# Type "make help" in the command line.
# ===================================================================================

# Files and Folders
MAINFILE = example.c
TARGET   = example
INCLUDE  = include
TOOLS    = tools

# Microcontroller Settings
DEVICE  ?= attiny814
F_CPU    = 5000000
FUSE0    = 0x00
FUSE1    = 0x00
FUSE4    = 0x00
FUSE5    = 0xC5
FUSE6    = 0x04
FUSE7    = 0x00
FUSE8    = 0x00

# Toolchain
PREFIX   = avr
CC       = $(PREFIX)-gcc
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
OBJSIZE  = $(PREFIX)-size
ISPTOOL  = python3 $(TOOLS)/tinyupdi.py
CLEAN    = rm -f *.lst *.obj *.cof *.list *.map *.eep.hex *.o *.d

# Compiler Flags
CFLAGS   = -Os -flto -Wall -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -I$(INCLUDE) -I.
LDFLAGS  = -B $(INCLUDE)/dev/$(DEVICE)
CFILES   = $(MAINFILE) $(wildcard $(INCLUDE)/*.c) $(wildcard $(INCLUDE)/*.S)

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all       compile and build $(TARGET).elf/.bin/.hex/.asm"
	@echo "make hex       compile and build $(TARGET).hex"
	@echo "make asm       compile and disassemble to $(TARGET).asm"
	@echo "make bin       compile and build $(TARGET).bin"
	@echo "make flash     compile and upload to MCU"
	@echo "make fuses     write fuses to MCU"
	@echo "make clean     remove all build files"

$(TARGET).elf: $(CFILES)
	@echo "Building $(TARGET).elf ..."
	@$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(TARGET).lst: $(TARGET).elf
	@echo "Building $(TARGET).lst ..."
	@$(OBJDUMP) -S $^ > $(TARGET).lst

$(TARGET).map: $(TARGET).elf
	@echo "Building $(TARGET).map ..."
	@$(OBJDUMP) -t $^ > $(TARGET).map

$(TARGET).bin: $(TARGET).elf
	@echo "Building $(TARGET).bin ..."
	@$(OBJCOPY) -O binary $< $(TARGET).bin

$(TARGET).hex: $(TARGET).elf
	@echo "Building $(TARGET).hex ..."
	@$(OBJCOPY) -O ihex $< $(TARGET).hex

$(TARGET).asm: $(TARGET).elf
	@echo "Disassembling to $(TARGET).asm ..."
	@$(OBJDUMP) -d $(TARGET).elf > $(TARGET).asm

all:	$(TARGET).lst $(TARGET).map $(TARGET).bin $(TARGET).hex $(TARGET).asm size

elf:	$(TARGET).elf removetemp size

bin:	$(TARGET).bin removetemp size removeelf

hex:	$(TARGET).hex removetemp size removeelf

asm:	$(TARGET).asm removetemp size removeelf

flash:	$(TARGET).bin size removeelf
	@echo "Uploading to $(DEVICE) ..."
	@$(ISPTOOL) -d $(DEVICE) -t $(F_CPU) -f $(TARGET).bin -fs 6:$(FUSE6) 7:$(FUSE7) 8:$(FUSE8)

fuses:
	@echo "Writing fuses to $(DEVICE) ..."
	@$(ISPTOOL) -t $(F_CPU) -fs 0:$(FUSE0) 1:$(FUSE1) 4:$(FUSE4) 5:$(FUSE5) 6:$(FUSE6) 7:$(FUSE7) 8:$(FUSE8)

clean:
	@echo "Cleaning all up ..."
	@$(CLEAN)
	@rm -f $(TARGET).elf $(TARGET).bin $(TARGET).hex $(TARGET).asm

size:
	@echo "------------------"
	@echo "FLASH: $(shell $(OBJSIZE) -d $(TARGET).elf | awk '/[0-9]/ {print $$1 + $$2}') bytes"
	@echo "SRAM:  $(shell $(OBJSIZE) -d $(TARGET).elf | awk '/[0-9]/ {print $$2 + $$3}') bytes"
	@echo "------------------"

removetemp:
	@echo "Removing temporary files ..."
	@$(CLEAN)

removeelf:
	@echo "Removing $(TARGET).elf ..."
	@rm -f $(TARGET).elf
