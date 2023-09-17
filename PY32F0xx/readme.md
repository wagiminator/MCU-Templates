# MCU Template for PY32F0xx Microcontrollers
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

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
