# MCU Template for STM32G030, STM32G031 and STM32G041 Microcontrollers
## Overview
The STM32G0 series from STMicroelectronics are efficient mainstream 32-bit microcontrollers with an Arm® Cortex®-M0+ core running at up to 64MHz. They operate at a supply voltage of 2.0V to 3.6V. The chips have the following peripherals:
- a 12-bit ADC (0.4µs, up to 16-bit with hardware oversampling)
- DMA with flexible channel mapping
- advanced-control, general-purpose and (G031/G041) low-power timers
- up to 2 USARTs (plus LPUART on G031/G041), 2 I2C and 2 SPI interfaces

The STM32G041 adds an AES hardware accelerator and a true random number generator. All STM32G0 have a factory built-in UART bootloader.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or IDE. It contains:
- a makefile
- linker scripts
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Line | Flash | SRAM | Packages | Special Features | Linker Script |
|:-|-:|-:|:-|:-|:-|
| STM32G030x6 | 32KB | 8KB | TSSOP20, SO8N, LQFP32, LQFP48 | - | ld/stm32g030x6.ld |
| STM32G030x8 | 64KB | 8KB | LQFP32, LQFP48 | - | ld/stm32g030x8.ld |
| STM32G031x4 | 16KB | 8KB | SO8N, TSSOP20, UFQFPN28/32/48, LQFP32/48, WLCSP18 | LPUART, LPTIM, VBAT | ld/stm32g031x4.ld |
| STM32G031x6 | 32KB | 8KB | as G031x4 | LPUART, LPTIM, VBAT | ld/stm32g031x6.ld |
| STM32G031x8 | 64KB | 8KB | as G031x4 | LPUART, LPTIM, VBAT | ld/stm32g031x8.ld |
| STM32G041x4/x6/x8 | 16/32/64KB | 8KB | as G031 | as G031 plus AES and RNG | ld/stm32g041x4/x6/x8.ld |

Common features of all variants:
- Arm® Cortex®-M0+ core with MPU, up to 64MHz
- Internal 16MHz RC oscillator (HSI) with PLL, internal 32kHz RC oscillator (LSI), external 4MHz - 48MHz oscillator (HSE), external 32.768kHz oscillator (LSE)
- 12-bit ADC (up to 19 channels), temperature sensor, internal voltage reference
- Advanced-control timer (TIM1), general-purpose timers, watchdogs, calendar RTC
- 5-channel DMA controller with DMAMUX, CRC calculation unit, 96-bit unique ID
- Serial wire debug (SWDIO on PA13, SWCLK on PA14)
- Supply voltage 2.0V - 3.6V, 5V-tolerant I/O pins

## Pinout and Peripheral Mapping
### STM32G030F6P6 (TSSOP20)

| Pin | Port | ADC | USART | I2C | SPI | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | PB7/PB8 | IN11 | U1 RX (2) | SCL (3), SDA (2) |  |  |
| 2 | PB9/PC14 |  |  | SDA (3) |  | OSC32_IN |
| 3 | PC15 |  |  |  |  | OSC32_OUT |
| 4 | VDD |  |  |  |  | Supply voltage 2.0V - 3.6V |
| 5 | VSS |  |  |  |  | Ground |
| 6 | NRST |  |  |  |  | Reset |
| 7 | PA0 | IN0 |  |  |  |  |
| 8 | PA1 | IN1 |  |  | SCK (1) |  |
| 9 | PA2 | IN2 | U2 TX (0) |  | MOSI (1) | debug_serial TX (default) |
| 10 | PA3 | IN3 | U2 RX (0) |  |  |  |
| 11 | PA4 | IN4 |  |  |  |  |
| 12 | PA5 | IN5 |  |  | SCK (0/2) |  |
| 13 | PA6 | IN6 |  |  | MISO (0/1) |  |
| 14 | PA7 | IN7 |  |  | MOSI (0) |  |
| 15 | PB0/PB1/PB2/PA8 | IN8, IN9, IN10 |  |  |  |  |
| 16 | PA11 [PA9] | IN15 | U1 TX (0/1) | SCL (0/1) | MISO (2) |  |
| 17 | PA12 [PA10] | IN16 | U1 RX (0/1) | SDA (0/1) | MOSI (2) |  |
| 18 | PA13 | IN17 |  |  |  | SWDIO |
| 19 | PA14-BOOT0/PA15 | IN18 | U2 TX (1), U2 RX (1) |  |  | SWCLK, BOOT0 |
| 20 | PB3/PB4/PB5/PB6 |  | U1 TX (2) | SCL (2) | SCK (3), MISO (3), MOSI (3) |  |

Notes:
- The numbers in brackets are the mapping options (`UART_MAP`, `UART2_MAP`, `I2C_MAP`, `SPI_MAP`) that are selected in the parameter section of the respective library. U1 = USART1, U2 = USART2.
- Several pads share one pin in the 20-pin package (e.g. pin 15 = PB0/PB1/PB2/PA8). Only one of them should be used as an output at a time, the others must stay inputs.
- PA9/PA10 can be remapped to the pins of PA11/PA12 via SYSCFG (shown as "PA11 [PA9]"); the libraries do this automatically.
- PA14 is also SWCLK and BOOT0. Using it as USART2 TX disables the debug interface.
- LPUART1 (lpuart library): TX/RX on PA2/PA3 (0) or PB11/PB10 (1), only on STM32G031/G041. The STM32G030 has no LPUART.
- NeoPixel data (neo_spi): PA2 (0), PA7 (1), PA12 (2) or PB5 (3).

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC ARM toolchain) |
| template/ld/ | Linker scripts (stm32g030x6/x8, stm32g031x4/x6/x8, stm32g041x4/x6/x8) |
| template/src/main.c | Example program (blinking LED on PB3) |
| template/src/system.c/h | Startup code, vector table, clock, power, sleep, watchdog and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt and ADC functions |
| template/src/stm32g0xx.h | Register definitions |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheets, reference manuals, programming manual and bootloader application notes (STM32G030, STM32G031, STM32G041, RM0454, RM0444, PM0223, AN2606, AN3155) |

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
| lpuart | Low-power UART (LPUART1) functions (STM32G031/G041 only) |
| neo_spi | NeoPixel (WS2812) control using the hardware SPI (MOSI pin) |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| spi | SPI master functions |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions, TX only |
| uart | Basic USART1 functions (polling, no buffer) |
| uart_dma | USART1 with DMA-driven RX ring buffer |
| uart_dma_rx | USART1 RX only with DMA-driven ring buffer |
| uart2 | Basic USART2 functions (polling, no buffer) |
| uart2_dma | USART2 with DMA-driven RX ring buffer |

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
USB2SERIAL      STM32G03x/04x
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

On STM32C0 and STM32G0 microcontrollers, the BOOT0 pin is disabled by default (option bit nBOOT_SEL = 1). As long as the chip is brand new or the main flash memory is empty, this is no problem, as the bootloader starts automatically. *stm32isp* and the STM32 ISP web app enable the BOOT0 pin for later use after flashing. If the chip has been programmed with another tool before, the BOOT0 pin may not work anymore. In this case, enable it via SWD with `tinyocd -B` (or the SWD Programmer web app), or clear the nBOOT_SEL bit with another SWD tool. Note that BOOT0 shares its pin with SWCLK (PA14).

Then upload the firmware with one of the following tools:
- **stm32isp**: Use the platform-independent open-source command-line tool [stm32isp](https://pypi.org/project/stm32isp/) (`pip install stm32isp`):
  ```
  stm32isp -f <firmware>.bin
  ```
- **Web app**: Use the [STM32 ISP Programmer web app](https://wagiminator.github.io/apps/stm32isp/). It only works in Chrome, Edge or Opera on a desktop computer. With a USB-to-serial converter whose DTR/RTS lines are wired to BOOT0 and NRST, the bootloader is started automatically.

### Via Serial Wire Debug Interface (SWD)
Connect an SWD programmer (e.g. ST-Link V2, DAPLink/CMSIS-DAP, Raspberry Pi Debug Probe, WCH-LinkE in DAP mode or J-Link) to the SWDIO (PA13), SWCLK (PA14), GND and optionally NRST and VDD pins of the STM32G0.

```
SWD Programmer      STM32G0
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
  Further useful options: `-u` removes the read protection (erases the chip), `-e` erases the chip, `-B` enables the BOOT0 pin and `-G`/`-R` configure NRST as GPIO or reset pin.
- **Web app**: Use the [SWD Programmer web app](https://wagiminator.github.io/apps/swdprog/). It supports CMSIS-DAP, ST-Link and J-Link programmers and only works in Chrome, Edge or Opera on a desktop computer.
- **pyOCD**: [pyOCD](https://pyocd.io/) can also be used for uploading and debugging (`pip install pyocd`, `pyocd pack install STM32G030F6Px`, `pyocd load <firmware>.bin -t stm32g030f6px`).

## Notes and Tips
- The template uses `F_CPU` = 64MHz by default (HSI with PLL, no external crystal required).
- The STM32G030 line is the "value line" without LPUART, LPTIM, VBAT and some other features of the STM32G031.
- The STM32G0 in TSSOP20 is largely pin-compatible with the STM32C011/C031.

## Development Boards and Projects
- [STM32G030 F6P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/STM32G030F6P6_DevBoard) (also suitable for STM32G031/G041 in TSSOP20)
- [Development Board Adapter](https://github.com/wagiminator/Development-Boards/tree/main/DevBoard_Adapter)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [stm32isp on PyPI](https://pypi.org/project/stm32isp/)
- [tinyocd on PyPI](https://pypi.org/project/tinyocd/)
- [STM32 ISP Programmer Web App](https://wagiminator.github.io/apps/stm32isp/)
- [SWD Programmer Web App](https://wagiminator.github.io/apps/swdprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [ST: STM32G0 Series](https://www.st.com/en/microcontrollers-microprocessors/stm32g0-series.html)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
