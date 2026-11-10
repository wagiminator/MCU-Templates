# MCU Template for PY32F002A, PY32F003 and PY32F030 Microcontrollers
## Overview
The PY32F0xx series from PUYA are very low-cost 32-bit microcontrollers with an Arm® Cortex®-M0+ core. They feature a wide supply voltage range of 1.7V to 5.5V, up to 64KB flash, up to 8KB SRAM and an operating frequency of up to 48MHz. The chips have the following peripherals:
- a 12-bit ADC
- two comparators
- advanced-control, general-purpose and low-power timers
- USART, I2C and SPI interfaces

They have a factory built-in UART bootloader. The PY32F002A, PY32F003 and PY32F030 are closely related and largely register-compatible with the STM32F0/G0 series.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or IDE. It contains:
- a makefile
- linker scripts
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Line | Flash | SRAM | Max. Clock | Packages | Linker Script |
|:-|-:|-:|-:|:-|:-|
| PY32F002Ax5 | 20KB | 3KB | 24MHz | SOP8, ESSOP10, SOP16, QFN16, TSSOP20 | ld/py32f002ax5.ld |
| PY32F002Ax6 | 32KB | 4KB | 24MHz | (not in the official datasheet) | ld/py32f002ax6.ld |
| PY32F002Bx5 | 24KB | 3KB | 24MHz | MSOP10, SOP14, SOP16, QFN20, TSSOP20 | ld/py32f002bx5.ld |
| PY32F003x4/x6/x7/x8 | 16/32/48/64KB | 2/4/6/8KB | 32MHz | SOP8 - QFN20 (see datasheet) | ld/py32f003x4/x6/x7/x8.ld |
| PY32F030x4/x6/x8 | 16/32/64KB | 2/4/8KB | 48MHz | SOP8 - LQFP32 (see datasheet) | ld/py32f030x4/x6/x8.ld |
| PY32F031x6 | 64KB (see notes) | 8KB | 48MHz | see datasheet | ld/py32f031x6.ld |

Common features of the PY32F002A/F003/F030:
- Arm® Cortex®-M0+ core, up to 24/32/48MHz
- Internal RC oscillator (HSI; PY32F002A: 8/24MHz, PY32F003/F030: 4/8/16/22.12/24MHz), internal 32.768kHz RC oscillator (LSI), external crystal oscillator (HSE, 4MHz - 24/32MHz)
- 12-bit ADC (up to 10 external channels), temperature sensor, internal voltage reference, two comparators
- Advanced-control timer (TIM1), general-purpose timers, low-power timer (LPTIM), watchdogs
- DMA controller (PY32F003/F030), CRC calculation unit
- Up to 2x USART, 1x I2C, 1x or 2x SPI
- Serial wire debug (SWDIO on PA13, SWCLK on PA14)
- Supply voltage 1.7V - 5.5V

## Pinout and Peripheral Mapping
### PY32F002AF15P (TSSOP20)

| Pin | Port | ADC | USART | I2C | SPI | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | VSS |  |  |  |  | Ground |
| 2 | PA13 |  | U1 RX (3) |  | MISO (4/5) | SWDIO |
| 3 | PA14 |  | U1 TX (3/4) |  |  | SWCLK |
| 4 | PB3 |  |  |  |  |  |
| 5 | PB6/PF4 |  | U1 TX (5) | SCL (4) |  | BOOT0 |
| 6 | PA6 | IN6 |  |  | MISO (3) |  |
| 7 | PA5 | IN5 |  |  | SCK (3) |  |
| 8 | PF0 |  | U1 RX (6) | SDA (6) |  | OSC_IN (HSE) |
| 9 | PF1 |  | U1 TX (6) | SCL (6) |  | OSC_OUT (HSE) |
| 10 | PF2-NRST |  |  |  |  | NRST (can be configured as GPIO) |
| 11 | PA0 | IN0 |  |  | MISO (0/1/2) |  |
| 12 | PA1 | IN1 |  |  | SCK (0/2), MOSI (1) |  |
| 13 | PA2 | IN2 | U1 TX (0) | SDA (0) | SCK (1), MOSI (2) | debug_serial TX (default) |
| 14 | PA3 | IN3 | U1 RX (0) | SCL (0) |  |  |
| 15 | PA4 | IN4 |  |  |  |  |
| 16 | PA7 | IN7 | U1 TX (1) | SDA (1) | MOSI (0/3) |  |
| 17 | PB0 |  |  |  |  |  |
| 18 | PB2 |  |  |  |  |  |
| 19 | PB1 | IN9 |  |  |  |  |
| 20 | VCC |  |  |  |  | Supply voltage 1.7V - 5.5V |

Notes:
- The numbers in brackets are the mapping options (`UART_MAP`, `I2C_MAP`, `SPI_MAP`) that are selected in the parameter section of the respective library. U1 = USART1.
- The PY32F002A has only USART1 and 9 external ADC channels (PA0 - PA7, PB1). USART2 (*uart2* library) is only available on the PY32F003/F030.
- PB6 and PF4 (BOOT0) share pin 5 in the TSSOP20 package.
- PA13/PA14 are the SWD pins. Mappings that use them disable the debug interface (it can still be accessed by connecting under reset).
- NeoPixel data (neo_spi): PA1 (0), PA2 (1), PA3 (2), PA7 (3), PA8 (4), PA12 (5) or PB5 (6).
- The *debug_serial* TX pin can be set to PA2, PA7, PA9, PA10, PA14, PB6 or PF1.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC ARM toolchain) |
| template/ld/ | Linker scripts (py32f002ax5/x6, py32f002bx5, py32f003x4/x6/x7/x8, py32f030x4/x6/x8, py32f031x6) |
| template/src/main.c | Example program (blinking LED on PB0) |
| template/src/system.c/h | Startup code, vector table, clock, power, sleep, watchdog and interrupt functions |
| template/src/gpio.h | GPIO and ADC functions |
| template/src/py32f0xx.h | Register definitions |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheets and reference manuals (PY32F002A, PY32F002B, PY32F003, PY32F030) |

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
| pwm | PWM outputs on 4 channels of TIM1 (PA3, PB3, PA0, PA1) |
| spi | SPI master functions |
| spi_dma | SPI master functions with DMA (background transfers of buffers) |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions, TX only |
| timer | Periodic timer interrupt (TIM16, microsecond resolution) |
| uart | Basic USART1 functions (polling, no buffer) |
| uart_dma | USART1 with DMA-driven RX ring buffer |
| uart_dma_rx | USART1 RX only with DMA-driven ring buffer |
| uart2 | Basic USART2 functions (polling, no buffer) (PY32F003/F030 only) |
| v-usb/hid_keyboard | Software USB HID keyboard (low-speed USB via bit-banging) |
| v-usb/hid_mouse | Software USB HID mouse |
| v-usb/hid_consumer | Software USB HID consumer control (multimedia keys) |
| v-usb/hid_joystick | Software USB HID joystick |
| v-usb/hid_composite | Software USB HID composite device (keyboard, consumer and mouse) |

All I2C, SPI and UART libraries provide functions for buffers with the same API (e.g. `I2C_sendBuffer(addr,buf,len)`, `I2C_getBuffer(addr,buf,len)`, `I2C_writeBuffer(buf,len)`, `I2C_readBuffer(buf,len)`, `SPI_writeBuffer(buf,len)`, `SPI_readBuffer(buf,len)`, `UART_writeBuffer(buf,len)`, `UART_readBuffer(buf,len)`). The *_dma* and *_int* variants transfer write buffers in the background; in this case the buffer must not be changed until the respective `..._busy()` function returns 0.

The DMA libraries (*i2c_dma*, *i2c_dma_tx*, *uart_dma*, *uart_dma_rx*) need a chip with DMA controller (PY32F003/F030). The PY32F002A has no DMA.

### Notes on the V-USB Libraries
The PY32F0xx has no USB peripheral. The libraries in the *v-usb* folder implement a low-speed USB device in software. They have the same structure and API as the V-USB libraries of the CH32V003 template: the C part is based on [rv003usb](https://github.com/cnlohr/rv003usb) by CNLohr, the timing-critical bit-banging part (*usb_handler.S*) was newly written for the Cortex-M0+ core.
- Copy all files of a library (including the assembler file *usb_handler.S*) into the *src* folder.
- The system clock must be 48MHz (`F_CPU = 48000000` in the makefile, HSI 24MHz x PLL). Officially, the PLL is only available on the PY32F030; on the PY32F002A/F003 it works on most chips, but this is not guaranteed. The PY32F002B is not supported.
- The timing-critical functions are executed from SRAM (about 1KB), where every instruction has a deterministic execution time. The GPIO registers are accessed via the single-cycle IOPORT. Each USB bit takes exactly 32 clock cycles.
- Set the USB pins in *config.h*. The default is D+ on PA3 and D- on PA1, which are available on all packages except ESSOP10. D+ and D- must be on the same port (PORTA, PORTB or PORTF). The interrupt is triggered by D-, other pins of the same EXTI group cannot use interrupts.
- D- needs a 1.5k pull-up resistor (to 3V3 or to the GPIO defined by `USB_PIN_DPU`). The MCU must be supplied with 3.3V, the data lines should have 22R - 68R series resistors.
- The internal oscillator (HSI) is not accurate enough for USB. If `USB_CALIBRATE` is set in *config.h*, it is calibrated with the USB keep-alive pulses (every 1ms) using TIM16, which cannot be used for other purposes then.
- The USB interrupt gets the highest priority, all other interrupts are set to a lower priority. Interrupts must not be disabled for longer than a few microseconds.
- PA3 is also the RX pin of the UART bootloader (PA2/PA3). Disconnect USB while flashing via these pins.
- This implementation of Software USB is still experimental.

## Compiling Firmware
To use the *makefile*, some additional software components must be installed to set up the toolchain.

### Install GCC Toolchain for Compiling
```
sudo apt install build-essential gcc-arm-none-eabi
```

### Install Python3 and Programming Tools
```
sudo apt install python3 python3-pip
pip install puyaisp tinyocd
```

### Compiling/Uploading
Edit the *makefile* according to the needs of the project (e.g. `F_CPU`, `LDSCRIPT`). Select the chip with `TARGET` (name of the linker script, e.g. `py32f002ax5`); the firmware is named after `PROJECT`. By default, `make flash` uploads the firmware via the UART bootloader using *puyaisp*. To use an SWD programmer instead, change `ISPTOOL` to `tinyocd -f $(OUTPUT)/$(PROJECT).bin`. Use one of the following commands inside the *template* folder to compile/upload:

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
USB2SERIAL      PY32F0xx
+--------+      +------------+
|     RXD| <--- |PA2 or PA9  |
|     TXD| ---> |PA3 or PA10 |
|     3V3| ---> |VDD (3V3)   |
|     GND| ---> |GND         |
+--------+      +------------+
```

Set the MCU to bootloader mode using ONE of the following methods:
- Disconnect the board from all power supplies, pull the BOOT0 pin (PF4) to VDD (or press and hold the BOOT button, if the board has one), then connect the board to your USB port. BOOT0 (or the BOOT button) can be released now.
- Connect the USB-to-serial converter to your USB port. Pull the BOOT0 pin to VDD, then pull NRST (PF2) shortly to GND (or press and hold the BOOT button, press and release the RESET button, then release the BOOT button).

Then upload the firmware with one of the following tools:
- **puyaisp**: Use the platform-independent open-source command-line tool [puyaisp](https://pypi.org/project/puyaisp/) (`pip install puyaisp`):
  ```
  puyaisp -f <firmware>.bin
  ```
- **Web app**: Use the [PUYA ISP Programmer web app](https://wagiminator.github.io/apps/puyaisp/). It only works in Chrome, Edge or Opera on a desktop computer. With a USB-to-serial converter whose DTR/RTS lines are wired to BOOT0 and NRST, the bootloader is started automatically.

The PY32F002B has no UART bootloader and can only be programmed via SWD.

### Via Serial Wire Debug Interface (SWD)
Connect an SWD programmer (e.g. ST-Link V2, DAPLink/CMSIS-DAP, Raspberry Pi Debug Probe, WCH-LinkE in DAP mode or J-Link) to the SWDIO (PA13), SWCLK (PA14), GND and optionally NRST and VDD pins of the PY32F0xx.

```
SWD Programmer      PY32F0xx
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
- **pyOCD**: [pyOCD](https://pyocd.io/) can also be used for uploading and debugging (`pip install pyocd`, `pyocd pack install PY32F002Ax5`, `pyocd load <firmware>.bin -t py32f002ax5`).

## Notes and Tips
- The template uses `F_CPU` = 24MHz by default (internal HSI, no external crystal required).
- The NRST pin (PF2) can be configured as GPIO via the option bytes (`puyaisp -G`, `tinyocd -G`). To use it as reset pin again, use `puyaisp -R` or `tinyocd -R`.
- The PY32F002A is said to be a PY32F030 die with limited specification. Many chips work at higher clock rates and with larger memory than specified, but this is not guaranteed.
- The linker script *py32f031x6.ld* defines 64KB flash and 8KB SRAM, check this against the datasheet of the chip used.
- The PY32F002B is a different chip with its own peripheral set; the template and libraries may need adjustments for it.
- The V-USB libraries require `F_CPU` = 48MHz (see above).

## Development Boards and Projects
- [PY32F002A F15P Development Board](https://github.com/wagiminator/Development-Boards/tree/main/PY32F002AF15P_DevBoard)
- [Development Board Adapter](https://github.com/wagiminator/Development-Boards/tree/main/DevBoard_Adapter)

## Links
- [rv003usb (software USB for CH32V003)](https://github.com/cnlohr/rv003usb)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [puyaisp on PyPI](https://pypi.org/project/puyaisp/)
- [tinyocd on PyPI](https://pypi.org/project/tinyocd/)
- [PUYA ISP Programmer Web App](https://wagiminator.github.io/apps/puyaisp/)
- [SWD Programmer Web App](https://wagiminator.github.io/apps/swdprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [PUYA Semiconductor](https://www.puyasemi.com/)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
