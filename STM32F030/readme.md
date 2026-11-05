# MCU Template for STM32F030 and STM32F031 Microcontrollers
## Overview
The STM32F030 (value line) and STM32F031 (access line) from STMicroelectronics are low-cost 32-bit microcontrollers with an Arm® Cortex®-M0 core running at up to 48MHz. They operate at a supply voltage of 2.4V to 3.6V (STM32F031: 2.0V to 3.6V). The chips have the following peripherals:
- a fast 12-bit ADC (1µs)
- advanced-control and general-purpose timers
- a calendar RTC
- USART, I2C and SPI interfaces

All variants have a factory built-in UART bootloader.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or IDE. It contains:
- a makefile
- linker scripts
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Line | Flash | SRAM | Packages | USART | I2C | SPI | Linker Script |
|:-|-:|-:|:-|-:|-:|-:|:-|
| STM32F030x4 | 16KB | 4KB | TSSOP20 | 1 | 1 | 1 | ld/stm32f030x4.ld |
| STM32F030x6 | 32KB | 4KB | LQFP32, LQFP48 | 1 | 1 | 1 | ld/stm32f030x6.ld |
| STM32F030x8 | 64KB | 8KB | LQFP32, LQFP48, LQFP64 | 2 | 2 | 2 | ld/stm32f030x8.ld |
| STM32F030xC | 256KB | 32KB | LQFP48, LQFP64 | 6 | 2 | 2 | ld/stm32f030xc.ld |
| STM32F031x4 | 16KB | 4KB | TSSOP20, UFQFPN28/32, LQFP32/48 | 1 | 1 | 1 | ld/stm32f031x4.ld |
| STM32F031x6 | 32KB | 4KB | TSSOP20, UFQFPN28/32, LQFP32/48, WLCSP25 | 1 | 1 | 1 | ld/stm32f031x6.ld |

Common features of all variants:
- Arm® Cortex®-M0 core, up to 48MHz
- Internal 8MHz RC oscillator (HSI) with PLL, internal 40kHz RC oscillator (LSI), external 4MHz - 32MHz oscillator (HSE), external 32.768kHz oscillator (LSE)
- 12-bit ADC (up to 16 channels), temperature sensor (F031), internal voltage reference
- Advanced-control timer (TIM1), general-purpose timers, watchdogs, calendar RTC
- 5-channel DMA controller, CRC calculation unit, 96-bit unique ID
- Serial wire debug (SWDIO on PA13, SWCLK on PA14)
- Dedicated BOOT0 pin

## Pinout and Peripheral Mapping
### STM32F030F4P6 (TSSOP20)

| Pin | Port | ADC | USART | I2C | SPI | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | BOOT0 |  |  |  |  | Bootloader selection |
| 2 | PF0 |  |  |  |  | OSC_IN (HSE) |
| 3 | PF1 |  |  |  |  | OSC_OUT (HSE) |
| 4 | NRST |  |  |  |  | Reset |
| 5 | VDDA |  |  |  |  | Analog supply |
| 6 | PA0 | IN0 |  |  |  |  |
| 7 | PA1 | IN1 |  |  |  |  |
| 8 | PA2 | IN2 | U1 TX (0) |  |  | debug_serial TX (default) |
| 9 | PA3 | IN3 | U1 RX (0) |  |  |  |
| 10 | PA4 | IN4 |  |  |  |  |
| 11 | PA5 | IN5 |  |  | SCK (0) |  |
| 12 | PA6 | IN6 |  |  | MISO (0) |  |
| 13 | PA7 | IN7 |  |  | MOSI (0) | NeoPixel data (neo_spi, map 0) |
| 14 | PB1 | IN9 |  |  |  |  |
| 15 | VSS |  |  |  |  | Ground |
| 16 | VDD |  |  |  |  | Supply voltage 2.4V - 3.6V |
| 17 | PA9 |  | U1 TX (1) | SCL (0) |  |  |
| 18 | PA10 |  | U1 RX (1) | SDA (0) |  |  |
| 19 | PA13 |  |  |  |  | SWDIO |
| 20 | PA14 |  | U1 TX (2) |  |  | SWCLK |

Notes:
- The numbers in brackets are the mapping options (`UART_MAP`, `I2C_MAP`, `SPI_MAP`) that are selected in the parameter section of the respective library. U1 = USART1.
- USART1 on PA2/PA3 (map 0) and PA14/PA15 (map 2) is only available on STM32F030x4/x6 and STM32F031. On STM32F030x8/xC these pins belong to USART2 (*uart2* library).
- PA14 is also SWCLK. Using it as USART1 TX disables the debug interface.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC ARM toolchain) |
| template/ld/ | Linker scripts (stm32f030x4/x6/x8/xc, stm32f031x4/x6) |
| template/src/main.c | Example program (blinking LED on PB1) |
| template/src/system.c/h | Startup code, vector table, clock, power, sleep, watchdog and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt and ADC functions |
| template/src/stm32f03x.h | Register definitions |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheets, reference manuals, programming manual and bootloader application notes (STM32F030, STM32F031, RM0360, RM0091, PM0215, AN2606, AN3155) |

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. Parameters such as pin mapping, baud rate or clock rate can be set in the parameter section at the beginning of the respective header file. Libraries that use the same peripheral (e.g. *uart* and *uart_dma*) must not be used at the same time.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) with simple printf, for debugging |
| i2c | I2C master functions (polling) |
| i2c_dma | I2C master with DMA (background transfers) |
| i2c_dma_tx | I2C master with DMA, TX only (e.g. for OLED displays) |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| i2c_tx | I2C master functions, TX only |
| neo_spi | NeoPixel (WS2812) control using the hardware SPI (MOSI pin) |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| spi | SPI master functions |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions, TX only |
| uart | Basic USART1 functions (polling, no buffer) |
| uart_dma | USART1 with DMA-driven RX ring buffer |
| uart_dma_rx | USART1 RX only with DMA-driven ring buffer |
| uart2 | Basic USART2 functions (polling, no buffer) (STM32F030x8/xC only) |
| uart2_dma | USART2 with DMA-driven RX ring buffer (STM32F030x8/xC only) |

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
USB2SERIAL      STM32F03xx4/6
+--------+      +------------+
|     RXD| <--- |PA9  or PA14|
|     TXD| ---> |PA10 or PA15|
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
Connect an SWD programmer (e.g. ST-Link V2, DAPLink/CMSIS-DAP, Raspberry Pi Debug Probe, WCH-LinkE in DAP mode or J-Link) to the SWDIO (PA13), SWCLK (PA14), GND and optionally NRST and VDD pins of the STM32F030.

```
SWD Programmer      STM32F030
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
- **pyOCD**: [pyOCD](https://pyocd.io/) can also be used for uploading and debugging (`pip install pyocd`, `pyocd pack install STM32F030F4Px`, `pyocd load <firmware>.bin -t stm32f030f4px`).

## Notes and Tips
- The template uses `F_CPU` = 48MHz by default (HSI with PLL, no external crystal required).
- The STM32F030 has a dedicated BOOT0 pin, which should be pulled down with a resistor (e.g. 10k) for normal operation.
- The STM32F030F4P6 is very common and cheap, but there are many remarked or cloned chips on the market (e.g. GD32F130, APM32F030, CKS32F030), which behave slightly differently.

## Development Boards and Projects
- [STM32F030 F4P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/STM32F030F4P6_DevBoard)
- [Development Board Adapter](https://github.com/wagiminator/Development-Boards/tree/main/DevBoard_Adapter)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [stm32isp on PyPI](https://pypi.org/project/stm32isp/)
- [tinyocd on PyPI](https://pypi.org/project/tinyocd/)
- [STM32 ISP Programmer Web App](https://wagiminator.github.io/apps/stm32isp/)
- [SWD Programmer Web App](https://wagiminator.github.io/apps/swdprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [ST: STM32F0 Series](https://www.st.com/en/microcontrollers-microprocessors/stm32f0-series.html)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
