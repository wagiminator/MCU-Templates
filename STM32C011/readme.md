# MCU Template for STM32C011 and STM32C031 Microcontrollers
## Overview
The STM32C0 series from STMicroelectronics are entry-level 32-bit microcontrollers with an Arm® Cortex®-M0+ core running at up to 48MHz. They share the technology platform and the consistent pinout of the STM32G0 series. The chips operate at a supply voltage of 2.0V to 3.6V. They have the following peripherals:
- a fast 12-bit ADC (0.4µs) with hardware oversampling (up to 16-bit resolution)
- DMA with flexible channel mapping
- advanced-control and general-purpose timers
- 2 USARTs, I2C and SPI interfaces

All STM32C0 have a factory built-in UART bootloader.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or IDE. It contains:
- a makefile
- linker scripts
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Part | Package | Flash | SRAM | GPIO | ADC Channels | Linker Script |
|:-|:-|-:|-:|-:|-:|:-|
| STM32C011D6Y6 | WLCSP12 | 32KB | 6KB | 10 | 7 | ld/stm32c011x6.ld |
| STM32C011J4M6/J6M6 | SO8N | 16/32KB | 6KB | 6 | 5 | ld/stm32c011x4.ld / x6.ld |
| STM32C011F4P6/F6P6 | TSSOP20 | 16/32KB | 6KB | 18 | 13 | ld/stm32c011x4.ld / x6.ld |
| STM32C011F4U6/F6U6 | UFQFPN20 | 16/32KB | 6KB | 18 | 13 | ld/stm32c011x4.ld / x6.ld |
| STM32C031F4P6/F6P6 | TSSOP20 | 16/32KB | 12KB | 18 | 13 | ld/stm32c031x4.ld / x6.ld |
| STM32C031G4U6/G6U6 | UFQFPN28 | 16/32KB | 12KB | 24 | 13 | ld/stm32c031x4.ld / x6.ld |
| STM32C031K4T6/K6T6 | LQFP32/UFQFPN32 | 16/32KB | 12KB | 30 | 15 | ld/stm32c031x4.ld / x6.ld |
| STM32C031C4T6/C6T6 | LQFP48/UFQFPN48 | 16/32KB | 12KB | 45 | 15 | ld/stm32c031x4.ld / x6.ld |

Common features of all variants:
- Arm® Cortex®-M0+ core, up to 48MHz
- Internal 48MHz RC oscillator (HSI48), internal 32kHz RC oscillator (LSI), external 4MHz - 48MHz oscillator (HSE), external 32.768kHz oscillator (LSE)
- 12-bit ADC, temperature sensor, internal voltage reference
- Advanced-control timer (TIM1), general-purpose timers (TIM3, TIM14, TIM16, TIM17), watchdogs, RTC
- 3-channel DMA controller with DMAMUX, CRC calculation unit, 96-bit unique ID
- 2x USART, 1x I2C (Fast-mode Plus), 1x SPI/I2S
- Serial wire debug (SWDIO on PA13, SWCLK on PA14)
- Supply voltage 2.0V - 3.6V, 5V-tolerant I/O pins

## Pinout and Peripheral Mapping
### STM32C011F4P6 (TSSOP20)

| Pin | Port | ADC | USART | I2C | SPI | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | PB7 |  | U1 RX (2) | SCL (4), SDA (2) |  |  |
| 2 | PC14 |  |  | SDA (4) |  | OSC32_IN |
| 3 | PC15 |  |  |  |  | OSC32_OUT |
| 4 | VDD |  |  |  |  | Supply voltage 2.0V - 3.6V |
| 5 | VSS |  |  |  |  | Ground |
| 6 | PF2-NRST |  |  |  |  | NRST (can be configured as GPIO) |
| 7 | PA0 | IN0 | U1 TX (3) |  |  |  |
| 8 | PA1 | IN1 | U1 RX (3) |  | SCK (1) |  |
| 9 | PA2 | IN2 | U2 TX (0) |  | MOSI (1) | debug_serial TX (default) |
| 10 | PA3 | IN3 | U2 RX (0) |  |  |  |
| 11 | PA4 | IN4 | U2 TX (2) |  |  |  |
| 12 | PA5 | IN5 | U2 RX (2) |  | SCK (0/2/4) |  |
| 13 | PA6 | IN6 |  |  | MISO (0/1/4) |  |
| 14 | PA7 | IN7 |  |  | MOSI (0) |  |
| 15 | PA8 | IN8 |  |  |  |  |
| 16 | PA11 [PA9] | IN11 | U1 TX (0/1) | SCL (0/1) | MISO (2) |  |
| 17 | PA12 [PA10] | IN12 | U1 RX (0/1) | SDA (0/1) | MOSI (2) |  |
| 18 | PA13 | IN13 |  |  |  | SWDIO |
| 19 | PA14-BOOT0 | IN14 | U2 TX (1) |  |  | SWCLK, BOOT0 |
| 20 | PB6 |  | U1 TX (2) | SCL (2) | MOSI (4) |  |

Notes:
- The numbers in brackets are the mapping options (`UART_MAP`, `UART2_MAP`, `I2C_MAP`, `SPI_MAP`) that are selected in the parameter section of the respective library. U1 = USART1, U2 = USART2.
- PA9/PA10 are not bonded separately on the 20-pin packages. They can be remapped to the pins of PA11/PA12 via SYSCFG (shown as "PA11 [PA9]"); the libraries do this automatically.
- On the STM32C031 (TSSOP20), further pads are bonded to the same pins: PB8 to pin 1, PB9 to pin 2, PA15 to pin 19 and PB3/PB4/PB5 to pin 20. Additional mappings (e.g. I2C on PB8/PB9, SPI on PB3/PB4/PB5) are available there.
- PA14 is also SWCLK and BOOT0. Using it as USART2 TX (mapping 1) disables the debug interface.
- NeoPixel data (neo_spi): PA2 (0), PA7 (1), PA12 (2), PB5 (3, C031 only) or PB6 (4).

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC ARM toolchain) |
| template/ld/ | Linker scripts (stm32c011x4.ld, stm32c011x6.ld, stm32c031x4.ld, stm32c031x6.ld) |
| template/src/main.c | Example program (blinking LED on PB6) |
| template/src/system.c/h | Startup code, vector table, clock, power, sleep, watchdog and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt and ADC functions |
| template/src/stm32c0xx.h | Register definitions |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheets, reference manuals, programming manual and bootloader application notes (STM32C011, STM32C031, RM0490, PM0223, AN2606, AN3155) |

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
USB2SERIAL      STM32C011/031
+--------+      +------------+
|     RXD| <--- |PA9  (PA11) |
|     TXD| ---> |PA10 (PA12) |
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
Connect an SWD programmer (e.g. ST-Link V2, DAPLink/CMSIS-DAP, Raspberry Pi Debug Probe, WCH-LinkE in DAP mode or J-Link) to the SWDIO (PA13), SWCLK (PA14), GND and optionally NRST and VDD pins of the STM32C0.

```
SWD Programmer      STM32C0
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
- **pyOCD**: [pyOCD](https://pyocd.io/) can also be used for uploading and debugging (`pip install pyocd`, `pyocd pack install STM32C011F4Px`, `pyocd load <firmware>.bin -t stm32c011f4px`).

## Notes and Tips
- The template uses `F_CPU` = 48MHz by default (HSI48, no external crystal required).
- The NRST pin (PF2) can be configured as GPIO via the option bytes (e.g. with `tinyocd -G`). In this case, the chip can only be programmed via the bootloader or with an SWD programmer using "connect under reset" before the firmware starts.
- The STM32C011 and STM32C031 are largely pin-compatible with the STM32G030/G031 in the 20-pin packages.

## Development Boards and Projects
- [STM32C011 F4P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/STM32C011F4P6_DevBoard) (also suitable for STM32C011/C031 in TSSOP20)
- [Development Board Adapter](https://github.com/wagiminator/Development-Boards/tree/main/DevBoard_Adapter)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [stm32isp on PyPI](https://pypi.org/project/stm32isp/)
- [tinyocd on PyPI](https://pypi.org/project/tinyocd/)
- [STM32 ISP Programmer Web App](https://wagiminator.github.io/apps/stm32isp/)
- [SWD Programmer Web App](https://wagiminator.github.io/apps/swdprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [ST: STM32C0 Series](https://www.st.com/en/microcontrollers-microprocessors/stm32c0-series.html)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
