# MCU Template for STM32F030 and STM32F031 Microcontrollers
The STM32F03x microcontrollers feature an Arm® Cortex®-M0 core and run at speeds up to 48 MHz. While achieving the STM32’s lowest ever price point, the STM32F03x comes with a full set of performing peripherals, such as fast 12-bit ADC, advanced and flexible timers, calendar RTC and communication peripherals such as the I²C, USART and SPI. This combination easily outperforms existing 8-bit architectures and allows all application designers to benefit from the simplicity and efficiency of a state-of-the-art 32-bit core. The STM32F03x line covers many memory and pin count combinations with only few competitive devices and thus further enhances the overall cost efficiency of your projects.

The *template* directory includes a makefile, an illustrative program, a linker script, and fundamental libraries. These elements are intended to serve as a foundation for beginning the development of bare-metal firmware in the C programming language for this particular microcontroller.

Furthermore, inside the *libraries* directory, there are optional libraries available for certain peripherals. If you intend to utilize these libraries, you need to copy them into the *src* folder within the *template* directory.

# Compiling Firmware Using the Makefile
To make use of the *makefile*, it's necessary to install some extra software components to fully set up the toolchain:

## Install GCC Toolchain for Compiling
```
sudo apt install build-essential gcc-arm-none-eabi
```

## Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install stm32isp
```

## Compiling/Uploading
Edit the *makefile* according to the needs of the project. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make clean     remove all build files
```

# Uploading Firmware
## Via Factory built-in UART Bootlader
The MCU has an embedded bootloader with UART interface, which can be used to upload firmware using a simple USB-to-serial adapter. The open-source platform-independent Python tool [stm32isp](https://pypi.org/project/stm32isp/) can be used for this purpose.

Connect your USB-to-serial converter to your STM32F0xx MCU as follows:

```
USB2SERIAL      STM32F03xx4/6
+--------+      +------------+
|     RXD| <--- |PA9  or PA14|
|     TXD| ---> |PA10 or PA15|
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

## Via Serial Wire Debug Interface (SWD)
You can use [PyOCD](https://pyocd.io/) for uploading and debugging. Install it via

```
pip install pyocd
```

Then install the Keil.STM32F0xx_DFP pack:

```
pyocd pack install STM32F030F4Px
```

In order to list all supported MCUs of the STM32F0xx series, run:

```
pyocd pack find stm32f0
```

Connect an SWD debug probe (e.g. [ST-Link V2](https://aliexpress.com/w/wholesale-st%2525252dlink-v2.html)) to your board. Upload firmware with the following command (example):

```
pyocd load firmware.bin -t stm32f030f4px
```

If you want to erase the chip, run:

```
pyocd erase -t stm32f030f4px --chip
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
