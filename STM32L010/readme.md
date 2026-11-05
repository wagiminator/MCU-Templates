# MCU Template for STM32L010 Microcontrollers
## Overview
The STM32L010 from STMicroelectronics is an ultra-low-power value line 32-bit microcontroller with an Arm® Cortex®-M0+ core running at up to 32MHz. It operates at a supply voltage of 1.8V to 3.6V and is designed for battery-powered applications (down to 0.23µA in standby mode, 76µA/MHz in run mode). The chip has the following features:
- a 12-bit ADC
- a low-power UART and a low-power timer
- a calendar RTC
- an embedded data EEPROM
- USART, I2C and SPI interfaces

It has a factory built-in bootloader with USART and SPI interface.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or IDE. It contains:
- a makefile
- linker scripts
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Part | Package | Flash | SRAM | EEPROM | GPIO | Linker Script |
|:-|:-|-:|-:|-:|-:|:-|
| STM32L010F4P6 | TSSOP20 | 16KB | 2KB | 128B | 15 | ld/stm32l010x4.ld |
| STM32L010K4T6 | LQFP32 | 16KB | 2KB | 128B | 26 | ld/stm32l010x4.ld |
| STM32L010C6T6 | LQFP48 | 32KB | 8KB | 256B | see datasheet | ld/stm32l010x6.ld |
| STM32L010K8T6 | LQFP32 | 64KB | 8KB | 256B | see datasheet | ld/stm32l010x8.ld |
| STM32L010R8T6 | LQFP64 | 64KB | 8KB | 256B | see datasheet | ld/stm32l010x8.ld |
| STM32L010RBT6 | LQFP64 | 128KB | 20KB | 512B | see datasheet | ld/stm32l010xb.ld |

Common features of all variants:
- Arm® Cortex®-M0+ core, 32kHz up to 32MHz
- Internal 16MHz RC oscillator (HSI16) with PLL, internal multispeed RC oscillator (MSI, 65kHz - 4.2MHz), internal 37kHz RC oscillator (LSI), external clock input, external 32.768kHz oscillator (LSE)
- 12-bit ADC (1.14Msps), internal voltage reference
- General-purpose timers, low-power timer (LPTIM), watchdogs, calendar RTC
- 5-channel DMA controller (7 on larger variants), CRC calculation unit, 96-bit unique ID
- USART2, LPUART1, I2C1 and SPI1 (larger variants have more)
- Serial wire debug (SWDIO on PA13, SWCLK on PA14), dedicated BOOT0 pin

The STM32L011, L021 and L031 are closely related. They can probably be used with the linker script of the L010 with the same flash and SRAM size (e.g. STM32L011F4/L021F4 with *stm32l010x4.ld*, STM32L031F6 with *stm32l010x6.ld*), but this has not been tested.

## Pinout and Peripheral Mapping
### STM32L010F4P6 (TSSOP20)

| Pin | Port | ADC | USART | I2C | SPI | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | BOOT0 |  |  |  |  | Bootloader selection |
| 2 | PC14 |  |  |  |  | OSC32_IN |
| 3 | PC15 |  |  |  |  | OSC32_OUT |
| 4 | NRST |  |  |  |  | Reset |
| 5 | VDDA |  |  |  |  | Analog supply |
| 6 | PA0 | IN0 | LP RX (3) |  |  |  |
| 7 | PA1 | IN1 | LP TX (3) |  |  |  |
| 8 | PA2 | IN2 | U2 TX (0), LP TX (0) |  |  | debug_serial TX (default) |
| 9 | PA3 | IN3 | U2 RX (0), LP RX (0) |  |  |  |
| 10 | PA4 | IN4 |  | SCL (2) |  |  |
| 11 | PA5 | IN5 |  |  | SCK (0/1) |  |
| 12 | PA6 | IN6 |  |  | MISO (0) |  |
| 13 | PA7 | IN7 |  |  | MOSI (0) | NeoPixel data (neo_spi, map 0) |
| 14 | PB1 | IN9 |  |  | MOSI (3) |  |
| 15 | VSS |  |  |  |  | Ground |
| 16 | VDD |  |  |  |  | Supply voltage 1.8V - 3.6V |
| 17 | PA9 |  | U2 TX (1) | SCL (0) |  |  |
| 18 | PA10 |  | U2 RX (1) | SDA (0) |  |  |
| 19 | PA13 |  | LP RX (1) | SDA (2) | SCK (3) | SWDIO |
| 20 | PA14 |  | U2 TX (2), LP TX (1) |  | MISO (3) | SWCLK |

Notes:
- The numbers in brackets are the mapping options (`UART_MAP`, `LPUART_MAP`, `I2C_MAP`, `SPI_MAP`) that are selected in the parameter section of the respective library. U2 = USART2 (*uart* library), LP = LPUART1 (*lpuart* library).
- The STM32L010 has no USART1. The *uart* libraries and *debug_serial* use USART2.
- PA13/PA14 are the SWD pins. Mappings that use them (e.g. I2C map 2, SPI map 3) disable the debug interface.
- The STM32L010F4 has a pinout compatible with the STM32F030F4 except for pins 1 - 3 (BOOT0, PC14/PC15 instead of BOOT0, PF0/PF1).

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC ARM toolchain) |
| template/ld/ | Linker scripts (stm32l010x4/x6/x8/xb) |
| template/src/main.c | Example program (blinking LED on PB1) |
| template/src/system.c/h | Startup code, vector table, clock, power, sleep, watchdog and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt and ADC functions |
| template/src/stm32l0xx.h | Register definitions |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheets, reference manuals, programming manual and bootloader application notes (STM32L010, RM0451, RM0377, PM0223, AN2606, AN3155) |

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. Parameters such as pin mapping, baud rate or clock rate can be set in the parameter section at the beginning of the respective header file. Libraries that use the same peripheral (e.g. *uart* and *uart_dma*) must not be used at the same time.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) with simple printf, for debugging |
| eeprom | Read/write the internal data EEPROM |
| i2c | I2C master functions (polling) |
| i2c_dma | I2C master with DMA (background transfers) |
| i2c_dma_tx | I2C master with DMA, TX only (e.g. for OLED displays) |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| i2c_tx | I2C master functions, TX only |
| lpuart | Low-power UART (LPUART1) functions |
| neo_spi | NeoPixel (WS2812) control using the hardware SPI (MOSI pin) |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| spi | SPI master functions |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions, TX only |
| uart | Basic USART2 functions (polling, no buffer) |
| uart_dma | USART2 with DMA-driven RX ring buffer |
| uart_dma_rx | USART2 RX only with DMA-driven ring buffer |

## Compiling Firmware
To use the *makefile*, some additional software components must be installed to set up the toolchain.

### Install GCC Toolchain for Compiling
```
sudo apt install build-essential gcc-arm-none-eabi
```

### Install Python3 and Programming Tools
```
sudo apt install python3 python3-pip
pip install stm32isp tinyocd
```

### Compiling/Uploading
Edit the *makefile* according to the needs of the project (e.g. `F_CPU`, `LDSCRIPT`). By default, `make flash` uploads the firmware via the UART bootloader using *stm32isp*. To use an SWD programmer instead, change `ISPTOOL` to `tinyocd -f $(BIN)/$(TARGET).bin`. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make clean     remove all build files
```

## Uploading Firmware
### Via Factory built-in UART Bootloader
The MCU has a factory built-in bootloader with UART interface, which can be used to upload firmware with a simple USB-to-serial converter (3.3V logic level recommended).

```
USB2SERIAL      STM32L010
+--------+      +------------+
|     RXD| <--- |PA2 or PA9  |
|     TXD| ---> |PA3 or PA10 |
|     3V3| ---> |VDD (3V3)   |
|     GND| ---> |GND         |
+--------+      +------------+
```

Set the MCU to bootloader mode using ONE of the following methods:
- Disconnect the board from all power supplies, pull the BOOT0 pin to VDD (or press and hold the BOOT button, if the board has one), then connect the board to your USB port. BOOT0 (or the BOOT button) can be released now.
- Connect the USB-to-serial converter to your USB port. Pull the BOOT0 pin to VDD, then pull NRST shortly to GND (or press and hold the BOOT button, press and release the RESET button, then release the BOOT button).

Then upload the firmware with one of the following tools:
- **stm32isp**: Use the platform-independent open-source command-line tool [stm32isp](https://pypi.org/project/stm32isp/) (`pip install stm32isp`):
  ```
  stm32isp -f <firmware>.bin
  ```
- **Web app**: Use the [STM32 ISP Programmer web app](https://wagiminator.github.io/apps/stm32isp/). It only works in Chrome, Edge or Opera on a desktop computer. With a USB-to-serial converter whose DTR/RTS lines are wired to BOOT0 and NRST, the bootloader is started automatically.

### Via Serial Wire Debug Interface (SWD)
Connect an SWD programmer (e.g. ST-Link V2, DAPLink/CMSIS-DAP, Raspberry Pi Debug Probe, WCH-LinkE in DAP mode or J-Link) to the SWDIO (PA13), SWCLK (PA14), GND and optionally NRST and VDD pins of the STM32L010.

```
SWD Programmer      STM32L010
+------------+      +------------+
|       SWCLK| ---> |PA14        |
|       SWDIO| <--> |PA13        |
|        NRST| ---> |NRST (opt.) |
|         GND| ---> |GND         |
|         3V3| ---> |VDD (opt.)  |
+------------+      +------------+
```

- **tinyocd**: Use the platform-independent open-source command-line tool [tinyocd](https://pypi.org/project/tinyocd/) (`pip install tinyocd`). Programmer and chip are detected automatically. On Linux, grant access permissions as described in the tinyocd documentation.
  ```
  tinyocd -f <firmware>.bin
  ```
  Further useful options: `-u` removes the read protection (erases the chip), `-e` erases the chip.
- **Web app**: Use the [SWD Programmer web app](https://wagiminator.github.io/apps/swdprog/). It supports CMSIS-DAP, ST-Link and J-Link programmers and only works in Chrome, Edge or Opera on a desktop computer.
- **pyOCD**: [pyOCD](https://pyocd.io/) can also be used for uploading and debugging (`pip install pyocd`, `pyocd pack install STM32L010F4Px`, `pyocd load <firmware>.bin -t stm32l010f4px`).

## Notes and Tips
- The template uses `F_CPU` = 32MHz by default (HSI16 with PLL). Without `F_CPU`, *system.h* uses 2.097MHz (MSI, default after reset). The core voltage range and flash wait states are selected automatically depending on `F_CPU`.
- For the lowest power consumption, use a low `F_CPU` (MSI) and the sleep/stop functions in *system.h*.
- The STM32L0 has no mass erase in the bootloader; stm32isp and the web app erase page by page, which takes a moment.

## Development Boards and Projects
- [STM32F030 F4P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/STM32F030F4P6_DevBoard): according to its description, this board can also be used with other STM32F0 and STM32L0 chips in TSSOP20, such as the STM32L010F4P6.
- [Development Board Adapter](https://github.com/wagiminator/Development-Boards/tree/main/DevBoard_Adapter)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [stm32isp on PyPI](https://pypi.org/project/stm32isp/)
- [tinyocd on PyPI](https://pypi.org/project/tinyocd/)
- [STM32 ISP Programmer Web App](https://wagiminator.github.io/apps/stm32isp/)
- [SWD Programmer Web App](https://wagiminator.github.io/apps/swdprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [ST: STM32L0 Series](https://www.st.com/en/microcontrollers-microprocessors/stm32l0-series.html)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
