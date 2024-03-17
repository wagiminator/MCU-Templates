# MCU Template for STM32C011 and STM32C31 Microcontrollers
The STM32C0x1 product line features an Arm® Cortex®-M0+ core and runs up to 48 MHz. Setting the lowest STM32 price point ever, the STM32C0x1 line makes no compromise on design quality and delivers a performance far beyond the existing 8-bit architectures. Compact, affordable, and reliable, the STM32C0x1 line features a fast 12-bit ADC with hardware resolution up to 16-bit, flexible mapping on the DMA channels, timers with advanced control capability, and several communication peripherals, always including 2 UARTs, even on the smallest microcontroller of the STM32C0x1 line. With STM32C0x1, application designers can start with an entry-level 32-bit MCU that leaves room for future product upgrades, thanks to the broad STM32 portfolio and ecosystem. The STM32C0x1 shares the same technological and design platform as the STM32G0, and benefits from a consistent pinout mapping. The portfolio supports from 16 to 32 Kbytes of on-chip flash memory.

The 'template' directory includes a makefile, an illustrative program, a linker script, fundamental libraries, and a programming tool. These elements are intended to serve as a foundation for beginning the development of bare-metal firmware in the C programming language for this particular microcontroller.

Furthermore, inside the 'libraries' directory, there are optional libraries available for certain peripherals. If you intend to utilize these libraries, you need to copy them into the 'include' folder within the 'template' directory.

To make use of the template, it's necessary to install some extra software components to fully set up the toolchain:

## Install GCC toolchain for compiling
```
sudo apt install build-essential gcc-arm-none-eabi
```

## Install Python3 and PySerial for the programming tool
```
sudo apt install python3 python3-pip
python3 -m pip install pyserial
```

# Using the Template
Use one of the following commands inside the 'template' folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make clean     remove all build files
```

# Development Board and Example Software
Take a look [here](https://github.com/wagiminator/Development-Boards/tree/main/STM32C011F4P6_DevBoard).

# Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
