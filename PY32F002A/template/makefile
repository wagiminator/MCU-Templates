# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  PY32F002A Example
# Authors:  Stefan Wagner, frak0d
# Year:     2025
# URL:      https://github.com/wagiminator/MCU-Templates
# ===================================================================================
# Install toolchain:
#   sudo apt install build-essential gcc-arm-none-eabi
#   sudo apt install python3 python3-pip
#   pip install puyaisp
#
# Type "make flash" in the command line.
#
# For C++ see the commented CC and CFLAGS lines below
# ===================================================================================

# Files and Folders
PROJECT  = template_app
SOURCE   = src
OUTPUT   = bin
INCLUDE  = -I$(SOURCE)

# Microcontroller Settings
TARGET   = py32f002ax5
F_CPU    = 24000000
LDSCRIPT = ld/$(TARGET).ld
CPUARCH  = -mcpu=cortex-m0plus -mthumb

# Toolchain
PREFIX   = arm-none-eabi
CC       = $(PREFIX)-gcc
# CC       = $(PREFIX)-g++
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
OBJSIZE  = $(PREFIX)-size
ISPTOOL  = puyaisp -f $(OUTPUT)/$(PROJECT).bin
# ISPTOOL  = pyocd load $(OUTPUT)/$(PROJECT).bin -t $(TARGET)

# NOTE: do not use -flto and -nostartfiles together for C++ project
# should be fine for C projects tho, saves about 100 bytes of flash

# Compiler Flags
CFLAGS   = -Os -flto -std=gnu23 -Wall
# CFLAGS   = -Os -flto -std=c++26 -fno-exceptions -fno-rtti -Wall -Wno-volatile -Wno-register -fpermissive
CFLAGS  += $(INCLUDE) $(CPUARCH) -DF_CPU=$(F_CPU) -fdata-sections -ffunction-sections -fno-builtin -fno-common
LDFLAGS  = -T$(LDSCRIPT) --specs=nano.specs --specs=nosys.specs #-nostartfiles #-nostdlib
LDFLAGS += -Wl,--gc-sections,--build-id=none,--no-warn-rwx-segments,--print-memory-usage
CFILES   = $(wildcard $(SOURCE)/*.c) $(wildcard $(SOURCE)/*.cpp) $(wildcard $(SOURCE)/*.S)
CFILES  += $(wildcard $(SOURCE)/*/*.c) $(wildcard $(SOURCE)/*/*.cpp) $(wildcard $(SOURCE)/*/*.S)

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all       compile and build $(PROJECT).elf/.bin/.hex/.asm"
	@echo "make hex       compile and build $(PROJECT).hex"
	@echo "make asm       compile and disassemble to $(PROJECT).asm"
	@echo "make bin       compile and build $(PROJECT).bin"
	@echo "make flash     compile and upload to MCU"
	@echo "make clean     remove all build files"

$(OUTPUT)/$(PROJECT).elf: $(CFILES)
	@echo "Building $(OUTPUT)/$(PROJECT).elf ..."
	@mkdir -p $(OUTPUT)
	@$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(OUTPUT)/$(PROJECT).lst: $(OUTPUT)/$(PROJECT).elf
	@echo "Building $(OUTPUT)/$(PROJECT).lst ..."
	@$(OBJDUMP) -S $^ > $(OUTPUT)/$(PROJECT).lst

$(OUTPUT)/$(PROJECT).map: $(OUTPUT)/$(PROJECT).elf
	@echo "Building $(OUTPUT)/$(PROJECT).map ..."
	@$(OBJDUMP) -t $^ > $(OUTPUT)/$(PROJECT).map

$(OUTPUT)/$(PROJECT).bin: $(OUTPUT)/$(PROJECT).elf
	@echo "Building $(OUTPUT)/$(PROJECT).bin ..."
	@$(OBJCOPY) -O binary $< $(OUTPUT)/$(PROJECT).bin

$(OUTPUT)/$(PROJECT).hex: $(OUTPUT)/$(PROJECT).elf
	@echo "Building $(OUTPUT)/$(PROJECT).hex ..."
	@$(OBJCOPY) -O ihex $< $(OUTPUT)/$(PROJECT).hex

$(OUTPUT)/$(PROJECT).asm: $(OUTPUT)/$(PROJECT).elf
	@echo "Disassembling to $(OUTPUT)/$(PROJECT).asm ..."
	@$(OBJDUMP) -d $(OUTPUT)/$(PROJECT).elf > $(OUTPUT)/$(PROJECT).asm

all:	$(OUTPUT)/$(PROJECT).lst $(OUTPUT)/$(PROJECT).map $(OUTPUT)/$(PROJECT).bin $(OUTPUT)/$(PROJECT).hex $(OUTPUT)/$(PROJECT).asm size

elf:	$(OUTPUT)/$(PROJECT).elf

bin:	$(OUTPUT)/$(PROJECT).bin removeelf

hex:	$(OUTPUT)/$(PROJECT).hex removeelf

asm:	$(OUTPUT)/$(PROJECT).asm removeelf

flash:	$(OUTPUT)/$(PROJECT).bin removeelf
	@echo "Uploading to MCU ..."
	@$(ISPTOOL)

clean:
	@echo "Cleaning all up ..."
	@rm -r $(OUTPUT)

removeelf:
	@echo "Removing $(OUTPUT)/$(PROJECT).elf ..."
	@rm -f $(OUTPUT)/$(PROJECT).elf
