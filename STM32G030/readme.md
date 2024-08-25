# MCU Template for STM32G030, STM32G031, and STM32G041 Microcontrollers
The new STM32G0 Series is not simply another Arm® Cortex®-M0+ microcontroller. It sets a new definition of what an efficient microcontroller must offer. This is all about best optimization, down to each and every detail, to offer the best value for your money and allow you to achieve your goals with the minimum BOM cost and the maximum flexibility for upgrades. The STM32G0 Series enables the one-architecture-fits-all concept and is ready for tomorrow’s needs.

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

Connect your USB-to-serial converter to your STM32G0xx MCU as follows:

```
USB2SERIAL        STM32G03xx
+--------+      +------------+
|     RXD| <--- |PA2 or PA9  |
|     TXD| ---> |PA3 or PA10 |
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

On STM32G03x/04x microcontrollers, the BOOT0 pin is initially disabled. When the chip is brand new or the main flash memory is erased, this isn't an issue as the embedded bootloader automatically kicks in. By using the stm32isp tool, the BOOT0 pin will be activated for subsequent use. However, if the chip has been previously programmed using a different software tool, the bootloader might not be accessible through the BOOT0 pin anymore. In such cases, the nBOOT_SEL bit in the User Option Bytes must be cleared (set to 0) using an SWD programmer like ST-Link and the appropriate software.

## Via Serial Wire Debug Interface (SWD)
You can use [PyOCD](https://pyocd.io/) for uploading and debugging. Install it via

```
pip install pyocd
```

Then install the Keil.STM32G0xx_DFP pack:

```
pyocd pack install STM32G030F6Px
```

In order to list all supported MCUs of the STM32G0xx series, run:

```
pyocd pack find stm32g0
```

Connect an SWD debug probe (e.g. [ST-Link V2](https://aliexpress.com/w/wholesale-st%2525252dlink-v2.html)) to your board. Upload firmware with the following command (example):

```
pyocd load firmware.bin -t stm32g030f6px
```

If you want to erase the chip, run:

```
pyocd erase -t stm32g030f6px --chip
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
