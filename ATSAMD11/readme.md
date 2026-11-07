# MCU Template for ATSAMD11C14A and ATSAMD11D14AM/AS/AU Microcontrollers
## Overview
The SAM D11 series from Microchip (formerly Atmel) are low-power 32-bit microcontrollers with an Arm® Cortex®-M0+ core running at up to 48MHz. They operate at a supply voltage of 1.62V to 3.63V. The chips have the following peripherals:
- a 12-bit ADC (350ksps, up to 10 channels, gain stage, hardware oversampling up to 16-bit)
- a 10-bit DAC and two analog comparators
- a full-speed USB 2.0 device interface (crystalless operation possible)
- two (SAMD11C) or three (SAMD11D) serial communication modules (SERCOM), each configurable as USART, SPI or I2C
- a 24-bit timer/counter for control (TCC0), two 16-bit timer/counters (TC1, TC2), a 32-bit real-time counter (RTC) and a watchdog timer (WDT)
- a 6-channel DMA controller, a 6-channel event system and a peripheral touch controller (PTC)

The SAM D11 has no factory built-in bootloader. Firmware is uploaded via the Serial Wire Debug (SWD) interface or via an optional USB bootloader (e.g. SAM-BA), which itself has to be installed via SWD first.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or IDE. It contains:
- a makefile
- linker scripts
- a startup and system configuration
- basic GPIO, external interrupt, ADC, DAC and analog comparator functions
- an example program

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Device | Flash | SRAM | Package | GPIOs | SERCOMs | ADC Channels | Special Features |
|:-|-:|-:|:-|:-:|:-:|:-:|:-|
| ATSAMD11C14A | 16KB | 4KB | SO14 | 12 | 2 | 5 | - |
| ATSAMD11D14AM | 16KB | 4KB | QFN24 | 22 | 3 | 10 | - |
| ATSAMD11D14AS | 16KB | 4KB | SO20 | 18 | 3 | 10 | - |
| ATSAMD11D14AU | 16KB | 4KB | WLCSP20 | 18 | 3 | 10 | - |

The device is selected in the *makefile* (`DEVICE = __SAMD11C14A__`, `__SAMD11D14AM__`, `__SAMD11D14AS__` or `__SAMD11D14AU__`). All variants use the same linker scripts.

Common features of all variants:
- Arm® Cortex®-M0+ core with single-cycle hardware multiplier and micro trace buffer, up to 48MHz
- 48MHz digital frequency locked loop (DFLL48M), internal 8MHz RC oscillator (OSC8M), internal 32.768kHz oscillators (OSC32K, OSCULP32K), external 0.4MHz - 32MHz crystal oscillator (XOSC), external 32.768kHz crystal oscillator (XOSC32K), fractional digital phase locked loop (FDPLL96M), six generic clock generators
- 12-bit ADC with internal temperature sensor (factory calibrated), internal 1V reference, 10-bit DAC, two analog comparators
- USB 2.0 full-speed device with internal pull-up, crystalless operation using USB clock recovery mode
- Serial wire debug (SWCLK on PA30, SWDIO on PA31), 128-bit unique serial number
- Supply voltage 1.62V - 3.63V (the I/O pins are not 5V-tolerant)

## Pinout and Peripheral Mapping
### ATSAMD11C14A (SO14)

| Pin | Port | Analog | UART | SPI | I2C | PWM | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-:|:-|
| 1 | PA5 | AIN3, AC AIN1 | S0 RX (0) | SCK (0), MISO (1) |  | CH1 | EXTINT5, LED on SAMD11C DevBoard |
| 2 | PA8 |  | S0 TX (2), S1 TX (3), DEBUG (2) | MOSI (1, 2) |  | CH2 | EXTINT6, XIN/XIN32, MCO |
| 3 | PA9 |  | S0 RX (2), S1 RX (3) | SCK (1, 2) |  | CH3 | EXTINT7, XOUT/XOUT32 |
| 4 | PA14 | AIN6 | S0 TX (1), DEBUG (1) | MISO (0) | SDA (0) | CH0 | NMI, AC CMP0 output |
| 5 | PA15 | AIN7 | S0 RX (1) |  | SCL (0) | CH1 | EXTINT1, AC CMP1 output |
| 6 | PA28 |  |  |  |  |  | RESET |
| 7 | PA30 |  | S1 TX (5), DEBUG (4) |  | SDA (4) | CH2 | EXTINT2, SWCLK |
| 8 | PA31 |  | S1 RX (5) | MISO (2) | SCL (4) | CH3 | EXTINT3, SWDIO |
| 9 | PA24 |  | S1 TX (4), DEBUG (3) |  |  | CH2 | EXTINT4, USB D- |
| 10 | PA25 |  | S1 RX (4) |  |  | CH3 | EXTINT5, USB D+ |
| 11 | GND |  |  |  |  |  | Ground |
| 12 | VDD |  |  |  |  |  | Supply voltage 1.62V - 3.63V |
| 13 | PA2 | AIN0, DAC VOUT |  |  |  |  | EXTINT2 |
| 14 | PA4 | AIN2, AC AIN0, VREFA | S0 TX (0), DEBUG (0) | MOSI (0) |  | CH0 | EXTINT4 |

### Additional Ports of ATSAMD11D14AM/AS/AU

| Port | Analog | UART | SPI | I2C | PWM | Other Functions |
|:-|:-|:-|:-|:-|:-:|:-|
| PA3 | AIN1, DAC VREFP |  |  |  |  | EXTINT3 |
| PA6 | AIN4, AC AIN2 |  | MOSI (3) |  | CH2 | EXTINT6 |
| PA7 | AIN5, AC AIN3 |  | SCK (3) |  | CH3 | EXTINT7 |
| PA10 | AIN8 | S0 TX (6), DEBUG (5) | MOSI (5) |  | CH2 | EXTINT2, AC CMP0 output |
| PA11 | AIN9 | S0 RX (6) | SCK (5) |  | CH3 | EXTINT3, AC CMP1 output |
| PA14 |  | S2 TX (8) | MISO (5) | SDA (2) |  | (in addition to the SAMD11C functions) |
| PA15 |  | S2 RX (8) |  | SCL (2) |  | (in addition to the SAMD11C functions) |
| PA16 |  |  | MOSI (4) |  | CH2 | EXTINT0, LED on SAMD11D DevBoard |
| PA17 |  |  | SCK (4) |  | CH3 | EXTINT1 |
| PA22 |  | S1 TX (7), S2 TX (9), DEBUG (6) | MISO (4) | SDA (1, 3) | CH0 | EXTINT6 |
| PA23 |  | S1 RX (7), S2 RX (9) |  | SCL (1, 3) | CH1 | EXTINT7 |
| PA27 |  |  |  |  |  | EXTINT7 |

Notes:
- The numbers in brackets are the mapping options (`UART_MAP`, `DEBUG_TX`, `SPI_MAP`, `I2C_MAP`) that are selected in the parameter section of the respective library. S0/S1/S2 = SERCOM0/1/2. Libraries which are used at the same time must use different SERCOMs. The mapping options that are only available on SAMD11D are rejected with a compiler error on SAMD11C.
- PA10, PA11, PA17 and PA27 are missing on the SO20 and WLCSP20 packages (AS/AU).
- NeoPixel data (neo_spi): PA4 (0), PA8 (1, 2), PA6 (3), PA16 (4) or PA10 (5).
- PWM (pwm library, TCC0): pins with the same channel number output the same PWM signal.
- PA24/PA25 are the USB pins, PA30/PA31 are the SWD pins. Using these pins for other purposes disables USB or the debug interface.
- PA8/PA9 are used for an external crystal (`SYS_USE_XOSC32K` in *system.h* or `RTC_init_XOSC32K()`).

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC ARM toolchain) |
| template/ld/ | Linker scripts (samd11.ld: without bootloader, samd11_boot4k.ld: with 4KB USB bootloader) |
| template/src/main.c | Example program (blinking LED on PA5) |
| template/src/system.c/h | Startup code, vector table, clock, RTC, low-power timer, sleep, watchdog, CRC, event system, DMA and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt (EIC), ADC, DAC and analog comparator functions |
| template/src/samd11.h | Register definitions (CMSIS core and SAMD11 device family pack headers) |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheet and errata |

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. Parameters such as pin mapping, baud rate or clock rate can be set in the parameter section at the beginning of the respective header file. Libraries that use the same peripheral (e.g. *uart* and *uart_int*, or two libraries on the same SERCOM) must not be used at the same time.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (SERCOM USART, TX only) with simple printf, for debugging |
| flash | Flash memory functions (NVMCTRL): erase rows, write pages/words, region locks, user row, security bit |
| i2c | I2C master functions (SERCOM I2CM, polling) |
| i2c_slave | I2C slave with register array (SERCOM I2CS, interrupt) |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| millis | Millisecond counter using a timer/counter (TC1 or TC2) |
| neo_spi | NeoPixel (WS2812) control using the hardware SPI (SERCOM, MOSI pin) |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| pwm | PWM outputs using the timer/counter for control (TCC0, 4 channels) |
| spi | SPI master functions (SERCOM SPI) |
| spi_dma | SPI master functions with DMA (background transfers of buffers) |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions, TX only |
| uart | Basic UART functions (SERCOM USART, polling, no buffer) |
| uart_int | UART with RX interrupt and ring buffer |
| uart2 | Second UART (copy of uart with prefix `UART2_`, default SERCOM1) |
| usb/usb_cdc | USB CDC (virtual serial port) |
| usb/hid_keyboard | USB HID keyboard |
| usb/hid_mouse | USB HID mouse |
| usb/hid_consumer | USB HID consumer control (media keys) |
| usb/hid_composite | USB HID composite device (keyboard, mouse and consumer control) |

The USB libraries consist of several files (all files of the respective folder must be copied into the *src* folder). The USB device descriptors and strings are set in *config.h*. The USB libraries have the same structure and API as the USB libraries of the CH32X035 template. They require the DFLL48M as clock source (`F_CPU` = 48MHz, 24MHz, 16MHz, 12MHz or 6MHz). By default, the DFLL48M runs in USB clock recovery mode (`SYS_USB_CRM` in *system.h*), so no crystal is required. Note that the VID/PID in *config.h* are shared IDs for testing purposes only.

The functions of the real-time counter (calendar), low-power timer, watchdog, sleep modes, event system and DMA controller are part of *system.h*. The ADC, DAC, analog comparators and external interrupts are part of *gpio.h*. The peripheral touch controller (PTC) is not supported, since its operation is only documented in combination with Microchip's closed-source QTouch library.

## Compiling Firmware
To use the *makefile*, some additional software components must be installed to set up the toolchain.

### Install GCC Toolchain for Compiling
```
sudo apt install build-essential gcc-arm-none-eabi
```

### Install Python3 and Programming Tools
```
sudo apt install python3 python3-pip
pip install tinyocd
```

### Compiling/Uploading
Edit the *makefile* according to the needs of the project (e.g. `DEVICE`, `F_CPU`, `LDSCRIPT`). By default, `make flash` uploads the firmware via SWD using *tinyocd*. To upload via an installed USB bootloader instead, set `LDSCRIPT = ld/samd11_boot4k.ld` and `ISPTOOL = $(BOOTTOOL)`. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make clean     remove all build files
```

Supported values for `F_CPU`: 48MHz (default), 24MHz, 16MHz, 12MHz and 6MHz (DFLL48M), as well as 8MHz, 4MHz, 2MHz and 1MHz (internal 8MHz oscillator).

## Uploading Firmware
### Via Serial Wire Debug Interface (SWD)
Connect an SWD programmer (e.g. DAPLink/CMSIS-DAP, Raspberry Pi Debug Probe, WCH-LinkE in DAP mode, ST-Link V2 or J-Link) to the SWCLK (PA30), SWDIO (PA31), GND and optionally RESET (PA28) and VDD pins of the SAMD11.

```
SWD Programmer      SAMD11
+------------+      +-------------+
|       SWCLK| ---> |PA30         |
|       SWDIO| <--> |PA31         |
|        NRST| ---> |PA28 (opt.)  |
|         GND| ---> |GND          |
|         3V3| ---> |VDD (opt.)   |
+------------+      +-------------+
```

- **tinyocd**: Use the platform-independent open-source command-line tool [tinyocd](https://pypi.org/project/tinyocd/) (`pip install tinyocd`). Programmer and chip are detected automatically. On Linux, grant access permissions as described in the tinyocd documentation.
  ```
  tinyocd -f <firmware>.bin
  ```
  Further useful options: `-u` removes the security bit (erases the chip), `-e` erases the chip and `-l` sets the security bit. If a bootloader is protected by the BOOTPROT fuse, the protected area cannot be overwritten.
- **Web app**: Use the [SWD Programmer web app](https://wagiminator.github.io/apps/swdprog/). It supports CMSIS-DAP, ST-Link and J-Link programmers and only works in Chrome, Edge or Opera on a desktop computer.
- **pyOCD**: [pyOCD](https://pyocd.io/) can also be used for uploading and debugging (`pip install pyocd`, `pyocd pack install ATSAMD11C14A`, `pyocd load <firmware>.bin -t atsamd11c14a`).
- **edbg / OpenOCD**: Alex Taradov's [edbg](https://github.com/ataradov/edbg) and [OpenOCD](https://openocd.org/) (target *at91samdXX*) also support the SAMD11.

### Via USB Bootloader
A USB bootloader (e.g. the 4KB SAM-BA bootloader of the [SAMD Development Boards](https://github.com/wagiminator/SAMD-Development-Boards)) can be installed via SWD. Afterwards, the firmware can be uploaded via USB with [BOSSA](https://github.com/shumatech/BOSSA) (`bossac`). Use the linker script *ld/samd11_boot4k.ld* in this case, so that the firmware starts at address 0x1000.

```
bossac -i -U true -e -w -v -R --offset=0x1000 <firmware>.bin
```

The bootloader is activated by double-tapping the RESET button (or by calling `BOOT_now()` from the application, which sets the double-tap magic value and resets the MCU).

## Notes and Tips
- The template uses `F_CPU` = 48MHz by default (DFLL48M in USB clock recovery mode with factory calibration, no external crystal required). For precise timing without USB connection, use a 32.768kHz crystal on PA8/PA9 (`SYS_USE_XOSC32K` in *system.h*).
- The flash wait states are set for a supply voltage of 2.7V or more. For lower supply voltages, reduce `F_CPU` or increase the wait states (see datasheet).
- The generic clock generators GCLK0 (main clock), GCLK1 (DFLL reference), GCLK2 (WDT), GCLK3 (RTC/LPT) and GCLK4 (USB, if F_CPU < 48MHz) are used by the template and the libraries, GCLK5 is free for the user.
- The low-power timer (LPT) and the RTC calendar functions both use the RTC and cannot be used at the same time.
- The SAMD11 has no open-drain outputs. They can be emulated by switching the pin direction (see *i2c_soft*).
- Only 16KB of flash and 4KB of SRAM are available, keep an eye on the memory usage (`make all` shows it).

## Development Boards and Projects
- [SAMD11C Development Board](https://github.com/wagiminator/SAMD-Development-Boards/tree/main/SAMD11C_DevBoard)
- [SAMD11D Development Board](https://github.com/wagiminator/SAMD-Development-Boards/tree/main/SAMD11D_DevBoard)
- [SAMD Programmer Adapter](https://github.com/wagiminator/SAMD-Development-Boards/tree/main/SAMD_Programmer_Adapter)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [tinyocd on PyPI](https://pypi.org/project/tinyocd/)
- [SWD Programmer Web App](https://wagiminator.github.io/apps/swdprog/)
- [SAMD Development Boards](https://github.com/wagiminator/SAMD-Development-Boards)
- [Alex Taradov: MCU Starter Projects](https://github.com/ataradov/mcu-starter-projects/tree/master/samd11)
- [Microchip: ATSAMD11C14A](https://www.microchip.com/en-us/product/atsamd11c14)
- [Microchip: Device Family Packs](https://packs.download.microchip.com/)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)). The register definitions in *samd11.h* are based on the CMSIS headers of the Microchip (Atmel) SAMD11 device family pack and Arm CMSIS, both licensed under the Apache License 2.0.
