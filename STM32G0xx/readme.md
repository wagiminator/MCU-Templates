# MCU Template for STM32G030, STM32G031, and STM32G041 Microcontrollers
The new STM32G0 Series is not simply another Arm® Cortex®-M0+ microcontroller. It sets a new definition of what an efficient microcontroller must offer. This is all about best optimization, down to each and every detail, to offer the best value for your money and allow you to achieve your goals with the minimum BOM cost and the maximum flexibility for upgrades. The STM32G0 Series enables the one-architecture-fits-all concept and is ready for tomorrow’s needs.

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
Take a look [here](https://github.com/wagiminator/Development-Boards/tree/main/STM32G030F6P6_DevBoard).

# Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
