# MCU Template for the STC8H1K08 series and other STC8H Microcontrollers
The STC8H1K08 is a cost-effective 8-bit ultra-high speed 8051 core MCU compatible with the MCS51 instruction set. The average instruction speed is about 12 times faster than that of the traditional 8051. It allows a clock frequency of up to 36Mhz, operates in the voltage range from 1.9V to 5.5V, and incorporates all standard peripherals such as 10-bit ADC, analog comparator, multiple timers, PWM, UART, SPI, and I2C. It has a factory built-in bootloader so firmware can be uploaded directly via UART without the need for an additional programming device.

The *template* directory includes a makefile, an illustrative program, a linker script, and fundamental libraries. These elements are intended to serve as a foundation for beginning the development of bare-metal firmware in the C programming language for this particular microcontroller.

Furthermore, inside the *libraries* directory, there are optional libraries available for certain peripherals. If you intend to utilize these libraries, you need to copy them into the *src* (or *include*) folder within the *template* directory.

# Libraries
|Library|Description|
|:-|:-|
|debug_serial|Basic serial debug output via UART1 or UART2 with a simple printf|
|eeprom|Read, write and erase the data flash (EEPROM/IAP)|
|i2c|Basic I2C master functions using the hardware I2C module|
|neo|Control of addressable LEDs (NeoPixel/WS2812), timing is calculated from F_CPU|
|spi|Basic SPI master functions using the hardware SPI module|
|uart|Basic functions for UART1 and UART2 (polling)|
|uart1_int|UART1 with RX interrupt and ring buffer|
|uart_tx_soft|Software UART (TX only) on any pin|

The STC8H1K08 in the TSSOP20/QFN20 package has the pins P1.0 - P1.7, P3.0 - P3.7 and P5.4. Pin mappings of the peripherals to other ports are not available.

# Using the Makefile
To make use of the *makefile*, it's necessary to install some extra software components to fully set up the toolchain:

## Install SDCC Toolchain for Compiling
```
sudo apt install build-essential sdcc
```

## Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install stc8isp
```

## Compiling/Uploading
Edit the *makefile* according to the needs of the project. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile, build and keep all files
make hex       compile and build <firmware>.hex
make bin       compile and build <firmware>.bin
make flash     compile, build and upload to MCU
make clean     remove all build files
```

# Factory built-in UART Bootloader
The MCU has an embedded bootloader with UART interface, which can be used to upload firmware using a simple USB-to-serial adapter. The open-source platform-independent Python tool [stc8isp](https://pypi.org/project/stc8isp/) can be used for this purpose.

Connect your USB-to-serial converter to your STC8H1K08 MCU as follows:

```
USB2SERIAL         STC8G/8H
+--------+         +------+
|     VCC| --/ --> |VCC   |    interruptible (for power cycle)
|     RXD| --|R|-- |P3.1  |    resistor (100R - 1000R)
|     TXD| --|<|-- |P3.0  |    diode (e.g. 1N5819)
|     GND| ------- |GND   |    common ground
+--------+         +------+
```

Then run the following command to upload your firmware (example):

```
stc8isp -p /dev/ttyUSB0 -t 36000000 -f firmware.bin
```

Perform a power cycle of your MCU (reconnect to power) when prompted.

# Development Board and Example Software
Take a look [here](https://github.com/wagiminator/Development-Boards/tree/main/STC8H1K08_DevBoard).

# Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
