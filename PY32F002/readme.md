# MCU Template for PY32F002, PY32F003, and PY32F030 Microcontrollers
The PY32F0xx series microcontrollers are cost-effective MCUs with a high performance 32-bit ARM® Cortex®-M0+ core. They feature a wide voltage operating range (1.7V - 5.5V), up to 64 Kbytes flash, up to 8 Kbytes SRAM memory, and an operating frequency of up to 48 MHz. These chips integrate multi-channel I2C, SPI, USART, and other communication peripherals, one channel 12-bit ADC, up to five 16-bit timers, and two-channel comparators.

The *template* directory includes a makefile, an illustrative program, a linker script, and fundamental libraries. These elements are intended to serve as a foundation for beginning the development of bare-metal firmware in the C programming language for this particular microcontroller.

Furthermore, inside the *libraries* directory, there are optional libraries available for certain peripherals. If you intend to utilize these libraries, you need to copy them into the *src* folder within the *template* directory.

To make use of the template, it's necessary to install some extra software components to fully set up the toolchain:

## Install GCC toolchain for compiling
```
sudo apt install build-essential gcc-arm-none-eabi
```

## Install Python3 and puyaisp programming tool
```
sudo apt install python3 python3-pip
pip install puyaisp
```

# Using the Template
Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make clean     remove all build files
```

## Factory built-in UART Bootlader
The MCU has an embedded bootloader with UART interface, which can be used to upload firmware using a simple USB-to-serial adapter. The open-source platform-independent Python tool [puyaisp](https://pypi.org/project/puyaisp/) can be used for this purpose.

Connect your USB-to-serial converter to your PY32F0xx MCU as follows:

```
USB2SERIAL            PY32F0xx
+--------+      +-------------------+
|     RXD| <--- |PA2 or PA9  or PA14|
|     TXD| ---> |PA3 or PA10 or PA15|
|     VDD| ---> |VDD                |
|     GND| ---> |GND                |
+--------+      +-------------------+
```

Set your MCU to bootloader mode by using ONE of the following methods:
- Disconnect your USB-to-serial converter, pull BOOT0 pin (PF4) to VCC (or press and hold the BOOT button, if your board has one), then connect the converter to your USB port. BOOT0 pin (or BOOT button) can be released now.
- Connect your USB-to-serial converter to your USB port. Pull BOOT0 pin (PF4) to VCC, then pull nRST (PF2) shortly to GND (or press and hold the BOOT button, then press and release the RESET button and then release the BOOT button, if your board has them).

Then run the following command to upload your firmware (example):

```
puyaisp -f firmware.bin
```

# Serial Wire Debug Interface (SWD)
You can use [PyOCD](https://pyocd.io/) for uploading and debugging. Install it via

```
pip install pyocd
```

Then install the Puya.PY32F0xx_DFP pack:

```
pyocd pack install PY32F002Ax5
```

In order to list all supported MCUs of the PY32F0xx series, run:

```
pyocd pack find py32f0
```

Connect an SWD debug probe (e.g. [ST-Link V2](https://aliexpress.com/w/wholesale-st%2525252dlink-v2.html)) to your board. Upload firmware with the following command (example):

```
pyocd load firmware.bin -t py32f002ax5
```

If you want to erase the chip, run:

```
pyocd erase -t py32f002ax5 --chip
```

# Development Board and Example Software
Take a look [here](https://github.com/wagiminator/Development-Boards/tree/main/PY32F002AF15P_DevBoard).

# Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
