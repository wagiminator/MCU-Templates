# MCU Template for STM32L010, L011, L021, L031 Microcontrollers
The STM32L0xx is an ultra-low-power value line Arm Cortex-M0+ MCU. It provides all essentials features for low power applications such as real-time clock, autonomous peripherals, low-power UART, low-power time clock, and ultra-low-power 12-bit ADC with 41 µA budget consumption at 10 ksps. The STM32L0xx offers up to 128 Kbytes of Flash memory, up to 20 Kbytes of RAM and up to 512 bytes of embedded EEPROM. It is a perfect match for battery-powered applications requiring simple, affordable and ultra-low-power MCUs including personal medical devices, industrial sensors, and IoT devices such as building controls, weather stations, smart locks, smoke detectors, or fire alarms.

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

Connect your USB-to-serial converter to your STM32L0xx MCU as follows:

```
USB2SERIAL      STM32L01x/02x
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

# Serial Wire Debug Interface (SWD)
You can use [PyOCD](https://pyocd.io/) for uploading and debugging. Install it via

```
pip install pyocd
```

Then install the Keil.STM32L0xx_DFP pack:

```
pyocd pack install STM32L010F4Px
```

In order to list all supported MCUs of the STM32L0xx series, run:

```
pyocd pack find stm32l0
```

Connect an SWD debug probe (e.g. [ST-Link V2](https://aliexpress.com/w/wholesale-st%2525252dlink-v2.html)) to your board. Upload firmware with the following command (example):

```
pyocd load firmware.bin -t stm32l010f4px
```

If you want to erase the chip, run:

```
pyocd erase -t stm32l010f4px --chip
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
