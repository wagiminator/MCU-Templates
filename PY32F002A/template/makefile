# NOTE: For C++, set SYS_SUPPORT_CPP to 1 in system.h and rename main.c to main.cpp

# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  PY32F002A Example
# Author:   Stefan Wagner
# Year:     2023
# URL:      https://github.com/wagiminator
# ===================================================================================
# Install toolchain:
#   sudo apt install build-essential gcc-arm-none-eabi
#   sudo apt install python3 python3-pip
#   pip install puyaisp
#
# Type "make flash" in the command line.
# ===================================================================================

# Files and Folders
PROJECT  = template_app
SOURCE   = src
OUTPUT   = bin

# Microcontroller Settings
TARGET   = py32f002ax5
F_CPU    = 24000000
LDSCRIPT = ld/$(TARGET).ld
CPUARCH  = -mcpu=cortex-m0plus -mthumb

# Toolchain
PREFIX   = arm-none-eabi
CC       = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
#ISPTOOL  = puyaisp -f $(OUTPUT)/$(PROJECT).bin
ISPTOOL  = pyocd load $(OUTPUT)/$(PROJECT).bin -t $(TARGET)
CLEAN    = rm -f *.lst *.obj *.cof *.list *.map *.eep.hex *.o *.d

# Compiler Flags
FLAGS    = -Os -flto -Wall -I$(SOURCE) $(CPUARCH) -DF_CPU=$(F_CPU) -fdata-sections -ffunction-sections -fno-builtin -fno-common
CFLAGS   = $(FLAGS) -std=gnu23
CPPFLAGS = $(FLAGS) -std=gnu++26 -fno-exceptions -fno-rtti -Wno-register
LDFLAGS  = -Wl,--gc-sections,--build-id=none,--no-warn-rwx-segments,--print-memory-usage
LDFLAGS += -T$(LDSCRIPT) --specs=nano.specs --specs=nosys.specs -nostartfiles #-nostdlib

ASM_SRCS := $(wildcard $(SOURCE)/*.S)  	$(wildcard $(SOURCE)/*/*.S)
C_SRCS   := $(wildcard $(SOURCE)/*.c) 	$(wildcard $(SOURCE)/*/*.c)
CPP_SRCS := $(wildcard $(SOURCE)/*.cpp) $(wildcard $(SOURCE)/*/*.cpp)

ASM_OBJS := $(patsubst $(SOURCE)/%.S,   $(OUTPUT)/obj/%.o, $(ASM_SRCS))
C_OBJS   := $(patsubst $(SOURCE)/%.c,   $(OUTPUT)/obj/%.o, $(C_SRCS))
CPP_OBJS := $(patsubst $(SOURCE)/%.cpp, $(OUTPUT)/obj/%.o, $(CPP_SRCS))
OBJS     := $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS)

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all       compile and build $(PROJECT).elf/.bin/.hex/.asm"
	@echo "make hex       compile and build $(PROJECT).hex"
	@echo "make asm       compile and disassemble to $(PROJECT).asm"
	@echo "make bin       compile and build $(PROJECT).bin"
	@echo "make flash     compile and upload to MCU"
	@echo "make clean     remove all build files"

$(OUTPUT)/obj/%.o: $(SOURCE)/%.S
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $< $(CFLAGS)

$(OUTPUT)/obj/%.o: $(SOURCE)/%.c
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $< $(CFLAGS)

$(OUTPUT)/obj/%.o: $(SOURCE)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) -o $@ -c $< $(CPPFLAGS)

$(OUTPUT)/$(PROJECT).elf: $(OBJS)
	@echo "Building $(OUTPUT)/$(PROJECT).elf ..."
	@mkdir -p $(@D)
	@$(CXX) -o $@ $(OBJS) $(LDFLAGS)

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

all:	$(OUTPUT)/$(PROJECT).lst $(OUTPUT)/$(PROJECT).map $(OUTPUT)/$(PROJECT).bin $(OUTPUT)/$(PROJECT).hex $(OUTPUT)/$(PROJECT).asm

elf:	$(OUTPUT)/$(PROJECT).elf removetemp

bin:	$(OUTPUT)/$(PROJECT).bin removetemp removeelf

hex:	$(OUTPUT)/$(PROJECT).hex removetemp removeelf

asm:	$(OUTPUT)/$(PROJECT).asm removetemp removeelf

flash:	$(OUTPUT)/$(PROJECT).bin removeelf
	@echo "Uploading to MCU ..."
	@$(ISPTOOL)

clean:
	@echo "Cleaning all up ..."
	@$(CLEAN)
	@rm -rf $(OUTPUT)

removetemp:
	@echo "Removing temporary files ..."
	@$(CLEAN)

removeelf:
	@echo "Removing $(OUTPUT)/$(PROJECT).elf ..."
	@rm -f $(OUTPUT)/$(PROJECT).elf
