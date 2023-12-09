# MCU Template for STM32L010, L011, L021, L031 Microcontrollers
The STM32L0xx is an ultra-low-power value line Arm Cortex-M0+ MCU. It provides all essentials features for low power applications such as real-time clock, autonomous peripherals, low-power UART, low-power time clock, and ultra-low-power 12-bit ADC with 41 µA budget consumption at 10 ksps. The STM32L0xx offers up to 128 Kbytes of Flash memory, up to 20 Kbytes of RAM and up to 512 bytes of embedded EEPROM. It is a perfect match for battery-powered applications requiring simple, affordable and ultra-low-power MCUs including personal medical devices, industrial sensors, and IoT devices such as building controls, weather stations, smart locks, smoke detectors, or fire alarms.

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
Take a look [here](https://github.com/wagiminator/Development-Boards/tree/main/STM32F030F4P6_DevBoard).

# Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
