# MCU Template for STM32C011 and STM32C31 Microcontrollers
The STM32C0x1 product line features an Arm® Cortex®-M0+ core and runs up to 48 MHz. Setting the lowest STM32 price point ever, the STM32C0x1 line makes no compromise on design quality and delivers a performance far beyond the existing 8-bit architectures. Compact, affordable, and reliable, the STM32C0x1 line features a fast 12-bit ADC with hardware resolution up to 16-bit, flexible mapping on the DMA channels, timers with advanced control capability, and several communication peripherals, always including 2 UARTs, even on the smallest microcontroller of the STM32C0x1 line. With STM32C0x1, application designers can start with an entry-level 32-bit MCU that leaves room for future product upgrades, thanks to the broad STM32 portfolio and ecosystem. The STM32C0x1 shares the same technological and design platform as the STM32G0, and benefits from a consistent pinout mapping. The portfolio supports from 16 to 32 Kbytes of on-chip flash memory.

The *template* directory includes a makefile, an illustrative program, a linker script, and fundamental libraries. These elements are intended to serve as a foundation for beginning the development of bare-metal firmware in the C programming language for this particular microcontroller.

Furthermore, inside the *libraries* directory, there are optional libraries available for certain peripherals. If you intend to utilize these libraries, you need to copy them into the *src* folder within the *template* directory.

To make use of the template, it's necessary to install some extra software components to fully set up the toolchain:

## Install GCC toolchain for compiling
```
sudo apt install build-essential gcc-arm-none-eabi
```

## Install Python3 and stm32isp programming tool
```
sudo apt install python3 python3-pip
pip install stm32isp
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

# Factory built-in UART Bootlader
The MCU has an embedded bootloader with UART interface, which can be used to upload firmware using a simple USB-to-serial adapter. The open-source platform-independent Python tool [stm32isp](https://pypi.org/project/stm32isp/) can be used for this purpose.

Connect your USB-to-serial converter to your STM32C0xx MCU as follows:

```
USB2SERIAL      STM32C011/031
+--------+      +------------+
|     RXD| <--- |PA9  (PA11) |
|     TXD| ---> |PA10 (PA12) |
|     3V3| ---> |VDD (3V3)   |
|     GND| ---> |GND         |
+--------+      +------------+
```

Set your MCU to boot mode by using ONE of the following method:
- Disconnect your board from all power supplies, pull BOOT0 pin to VCC (or press and hold the BOOT button if your board has one), then connect the board to your USB port. The BOOT button can be released now.
- Connect your USB-to-serial converter to your USB port. Pull BOOT0 pin to VCC, then pull nRST shortly to GND (or press and hold the BOOT button, then press and release the RESET button and then release the BOOT button, if your board has them).

Then run the following command to upload your firmware (example):

```
stm32isp -f firmware.bin
```

# Serial Wire Debug Interface (SWD)
You can use [PyOCD](https://pyocd.io/) for uploading and debugging. Install it via

```
pip install pyocd
```

Then install the Keil.STM32C0xx_DFP pack:

```
pyocd pack install STM32C011F4Px
```

In order to list all supported MCUs of the STM32C0xx series, run:

```
pyocd pack find stm32c0
```

Connect an SWD debug probe (e.g. [ST-Link V2](https://aliexpress.com/w/wholesale-st%2525252dlink-v2.html)) to your board. Upload firmware with the following command (example):

```
pyocd load firmware.bin -t stm32c011f4px
```

If you want to erase the chip, run:

```
pyocd erase -t stm32c011f4px --chip
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
