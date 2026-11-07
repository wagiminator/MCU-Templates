# MCU Template for ATSAMD21E, ATSAMD21G and ATSAMD21J Microcontrollers
## Overview
The SAM D21 series from Microchip (formerly Atmel) are low-power 32-bit microcontrollers with an Arm® Cortex®-M0+ core running at up to 48MHz. They operate at a supply voltage of 1.62V to 3.63V. The chips have the following peripherals:
- a 12-bit ADC (350ksps, up to 20 channels, gain stage, hardware oversampling up to 16-bit)
- a 10-bit DAC and two analog comparators
- a full-speed USB 2.0 device/host interface (crystalless device operation possible)
- four (SAMD21E) or six (SAMD21G/J) serial communication modules (SERCOM), each configurable as USART, SPI or I2C
- an I2S interface
- three 24/16-bit timer/counters for control (TCC0..TCC2), three (SAMD21E/G) or five (SAMD21J) 16-bit timer/counters (TC3..TC7), a 32-bit real-time counter (RTC) and a watchdog timer (WDT)
- a 12-channel DMA controller, a 12-channel event system and a peripheral touch controller (PTC)

The SAM D21 has no factory built-in bootloader. Firmware is uploaded via the Serial Wire Debug (SWD) interface or via an optional USB bootloader (e.g. SAM-BA or UF2, as used by Arduino and Adafruit boards), which itself has to be installed via SWD first.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or IDE. It contains:
- a makefile
- linker scripts
- a startup and system configuration
- basic GPIO, external interrupt, ADC, DAC and analog comparator functions
- an example program

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Device | Flash | SRAM | Packages | GPIOs | SERCOMs | TCs | ADC Channels | Linker Script |
|:-|-:|-:|:-|:-:|:-:|:-:|:-:|:-|
| ATSAMD21E15/16/17/18 | 32/64/128/256KB | 4/8/16/32KB | TQFP32, QFN32, WLCSP35 | 26 | 4 | 3 | 10 | ld/samd21x15/16/17/18.ld |
| ATSAMD21G15/16/17/18 | 32/64/128/256KB | 4/8/16/32KB | TQFP48, QFN48, WLCSP45 | 38 | 6 | 3 | 14 | ld/samd21x15/16/17/18.ld |
| ATSAMD21J15/16/17/18 | 32/64/128/256KB | 4/8/16/32KB | TQFP64, QFN64, UFBGA64 | 52 | 6 | 5 | 20 | ld/samd21x15/16/17/18.ld |

The device is selected in the *makefile* (`DEVICE`, e.g. `__SAMD21E18A__`, `__SAMD21G18A__`, `__SAMD21J17A__`) together with the linker script for the flash size (`LDSCRIPT`, e.g. `ld/samd21x18.ld`). Revision B and D devices (e.g. ATSAMD21G17D) are register-compatible and can be selected with their own name (e.g. `__SAMD21G17D__`). The SAMD21 "L" variants (e.g. ATSAMD21E16L) and the SAMDA1 automotive devices are not supported.

Common features of all variants:
- Arm® Cortex®-M0+ core with single-cycle hardware multiplier, micro trace buffer, up to 48MHz
- 48MHz digital frequency locked loop (DFLL48M), internal 8MHz RC oscillator (OSC8M), internal 32.768kHz oscillators (OSC32K, OSCULP32K), external 0.4MHz - 32MHz crystal oscillator (XOSC, PA14/PA15), external 32.768kHz crystal oscillator (XOSC32K, PA0/PA1), fractional digital phase locked loop (FDPLL96M), nine generic clock generators
- 12-bit ADC with internal temperature sensor (factory calibrated), internal 1V reference, 10-bit DAC, two analog comparators
- USB 2.0 full-speed with internal pull-up, crystalless device operation using USB clock recovery mode
- Serial wire debug (SWCLK on PA30, SWDIO on PA31), dedicated RESET pin, 128-bit unique serial number
- Supply voltage 1.62V - 3.63V (the I/O pins are not 5V-tolerant)

## Pinout and Peripheral Mapping

| Port | Device | Analog | UART | SPI | I2C | PWM | Other Functions |
|:-|:-:|:-|:-|:-|:-|:-|:-|
| PA0 | EGJ |  | S1 TX (10) |  |  | T2C0 | EXTINT0, XIN32 |
| PA1 | EGJ |  | S1 RX (10) |  |  | T2C1 | EXTINT1, XOUT32 |
| PA2 | EGJ | AIN0, DAC VOUT |  |  |  |  | EXTINT2 |
| PA3 | EGJ | AIN1, DAC VREFP |  |  |  |  | EXTINT3 |
| PA4 | EGJ | AIN4, AC AIN0, VREFA | S0 TX (1) | MISO (0) |  | T0C0 | EXTINT4 |
| PA5 | EGJ | AIN5, AC AIN1 | S0 RX (1) |  |  | T0C1 | EXTINT5 |
| PA6 | EGJ | AIN6, AC AIN2 |  | MOSI (0) |  | T1C0 | EXTINT6 |
| PA7 | EGJ | AIN7, AC AIN3 |  | SCK (0) |  | T1C1 | EXTINT7, I2S SD0 |
| PA8 | EGJ | AIN16 | S2 TX (3) | MISO (2) | SDA (2), SDA (3) | T0C0 | NMI |
| PA9 | EGJ | AIN17 | S2 RX (3) |  | SCL (2), SCL (3) | T0C1 | EXTINT9 |
| PA10 | EGJ | AIN18 | S0 TX (0) |  |  | T0C2 | EXTINT10, I2S SCK0 |
| PA11 | EGJ | AIN19 | S0 RX (0) |  |  | T0C3 | EXTINT11, I2S FS0 |
| PA12 | GJ |  |  | MISO (4) | SDA (6), SDA (7) | T2C0 | EXTINT12, AC CMP0 |
| PA13 | GJ |  |  |  | SCL (6), SCL (7) | T2C1 | EXTINT13, AC CMP1 |
| PA14 | EGJ |  | S2 TX (4) | MOSI (2) |  | T0C0 | EXTINT14, XIN |
| PA15 | EGJ |  | S2 RX (4) | SCK (2) |  | T0C1 | EXTINT15, XOUT |
| PA16 | EGJ |  | S1 TX (2) | MOSI (1), MOSI (3), MISO (6) | SDA (1), SDA (4) | T2C0 | EXTINT0 |
| PA17 | EGJ |  | S1 RX (2) | SCK (1), SCK (3) | SCL (1), SCL (4) | T2C1 | EXTINT1, LED (Arduino Zero) |
| PA18 | EGJ |  |  | MOSI (6) |  | T0C2 | EXTINT2, AC CMP0 |
| PA19 | EGJ |  |  | MISO (1), MISO (3), SCK (6) |  | T0C3 | EXTINT3, I2S SD0, AC CMP1 |
| PA20 | GJ |  |  |  |  | T0C2 | EXTINT4, I2S SCK0 |
| PA21 | GJ |  |  |  |  | T0C3 | EXTINT5, I2S FS0 |
| PA22 | EGJ |  | S3 TX (5) |  | SDA (0), SDA (5) | T0C0 | EXTINT6 |
| PA23 | EGJ |  | S3 RX (5) |  | SCL (0), SCL (5) | T0C1 | EXTINT7 |
| PA24 | EGJ |  |  |  |  | T1C0 | EXTINT12, USB D- |
| PA25 | EGJ |  |  |  |  | T1C1 | EXTINT13, USB D+ |
| PA27 | EGJ |  |  |  |  |  | EXTINT15, MCO |
| PA28 | EGJ |  |  |  |  |  | EXTINT8 |
| PA30 | EGJ |  |  |  |  | T1C0 | EXTINT10, SWCLK |
| PA31 | EGJ |  |  |  |  | T1C1 | EXTINT11, SWDIO |
| PB0 | J | AIN8 |  |  |  |  | EXTINT0 |
| PB1 | J | AIN9 |  |  |  |  | EXTINT1 |
| PB2 | GJ | AIN10 | S5 TX (9) | MISO (5) |  |  | EXTINT2 |
| PB3 | GJ | AIN11 | S5 RX (9) |  |  |  | EXTINT3 |
| PB4 | J | AIN12 |  |  |  |  | EXTINT4 |
| PB5 | J | AIN13 |  |  |  |  | EXTINT5 |
| PB6 | J | AIN14 |  |  |  |  | EXTINT6 |
| PB7 | J | AIN15 |  |  |  |  | EXTINT7 |
| PB8 | GJ | AIN2 | S4 TX (6) |  |  |  | EXTINT8 |
| PB9 | GJ | AIN3 | S4 RX (6) |  |  |  | EXTINT9 |
| PB10 | GJ |  | S4 TX (7) | MOSI (4) |  | T0C0 | EXTINT10 |
| PB11 | GJ |  | S4 RX (7) | SCK (4) |  | T0C1 | EXTINT11 |
| PB12 | J |  |  |  | SDA (8) | T0C2 | EXTINT12 |
| PB13 | J |  |  |  | SCL (8) | T0C3 | EXTINT13 |
| PB14 | J |  |  |  |  |  | EXTINT14 |
| PB15 | J |  |  |  |  |  | EXTINT15 |
| PB16 | J |  |  |  | SDA (9) | T0C0 | EXTINT0 |
| PB17 | J |  |  |  | SCL (9) | T0C1 | EXTINT1 |
| PB22 | GJ |  | S5 TX (8) | MOSI (5) |  |  | EXTINT6 |
| PB23 | GJ |  | S5 RX (8) | SCK (5) |  |  | EXTINT7 |
| PB30 | J |  |  |  | SDA (10) | T0C0 | EXTINT14 |
| PB31 | J |  |  |  | SCL (10) | T0C1 | EXTINT15 |


Notes:
- Device column: port available on SAMD21E (E), SAMD21G (G) and SAMD21J (J).
- The numbers in brackets are the mapping options (`UART_MAP`, `SPI_MAP`, `I2C_MAP`) that are selected in the parameter section of the respective library. S0..S5 = SERCOM0..5. Libraries which are used at the same time must use different SERCOMs. Mapping options that are not available on the selected device are rejected with a compiler error. The *debug_serial* library (`DEBUG_TX`) uses the TX pins of the UART mappings 0..9, the *neo_spi* library (`NEO_MAP`) the MOSI pins of the SPI mappings.
- PWM: TxCy = timer/counter for control x (TCCx), channel y (see *pwm* library). Pins of the same channel output the same PWM signal.
- Default mappings: uart: SERCOM0 (PA10/PA11), debug_serial: SERCOM1 (PA16), spi: SERCOM2 (PA14/PA15/PA8), i2c: SERCOM3 (PA22/PA23).
- PA24/PA25 are the USB pins, PA30/PA31 are the SWD pins. Using these pins for other purposes disables USB or the debug interface.
- PA0/PA1 are used for an external 32.768kHz crystal (`SYS_USE_XOSC32K` in *system.h* or `RTC_init_XOSC32K()`).
- The I2S pins are described in the *i2s* library, AC CMP0/CMP1 are the analog comparator outputs.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC ARM toolchain) |
| template/ld/ | Linker scripts for 32/64/128/256KB flash (samd21x15..x18.ld: without bootloader, samd21x15..x18_boot8k.ld: with 8KB USB bootloader) |
| template/src/main.c | Example program (blinking LED on PA8) |
| template/src/system.c/h | Startup code, vector table, pin designators, clock, RTC, low-power timer, sleep, watchdog, CRC, event system, DMA and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt (EIC), ADC, DAC and analog comparator functions |
| template/src/samd21.h | Register definitions (CMSIS core and SAMD21 device family pack headers) |
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
| i2s | I2S master transmitter (stereo, 16/32-bit) for I2S DACs |
| millis | Millisecond counter using a timer/counter (TC3, TC4 or TC5) |
| neo_spi | NeoPixel (WS2812) control using the hardware SPI (SERCOM, MOSI pin) |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| pwm | PWM outputs using the timers/counters for control (TCC0..TCC2, 8 channels) |
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

The USB libraries consist of several files (all files of the respective folder must be copied into the *src* folder). The USB device descriptors and strings are set in *config.h*. The USB libraries have the same structure and API as the USB libraries of the CH32X035 and ATSAMD11 templates (device mode only). They require the DFLL48M as clock source (`F_CPU` = 48MHz, 24MHz, 16MHz, 12MHz or 6MHz). By default, the DFLL48M runs in USB clock recovery mode (`SYS_USB_CRM` in *system.h*), so no crystal is required. Note that the VID/PID in *config.h* are shared IDs for testing purposes only.

The functions of the real-time counter (calendar), low-power timer, watchdog, sleep modes, event system and DMA controller are part of *system.h*. The ADC, DAC, analog comparators and external interrupts are part of *gpio.h*. The peripheral touch controller (PTC) is not supported, since its operation is only documented in combination with Microchip's closed-source QTouch library. The USB host mode and the FDPLL96M are not used by the template.

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
Edit the *makefile* according to the needs of the project (e.g. `DEVICE`, `F_CPU`, `LDSCRIPT`). By default, `make flash` uploads the firmware via SWD using *tinyocd*. To upload via an installed USB bootloader instead, use the *_boot8k* linker script (e.g. `LDSCRIPT = ld/samd21x18_boot8k.ld`) and set `ISPTOOL = $(BOOTTOOL)`. Use one of the following commands inside the *template* folder to compile/upload:

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
Connect an SWD programmer (e.g. DAPLink/CMSIS-DAP, Raspberry Pi Debug Probe, WCH-LinkE in DAP mode, ST-Link V2 or J-Link) to the SWCLK (PA30), SWDIO (PA31), GND and optionally RESET and VDD pins of the SAMD21.

```
SWD Programmer      SAMD21
+------------+      +-------------+
|       SWCLK| ---> |PA30         |
|       SWDIO| <--> |PA31         |
|        NRST| ---> |RESET (opt.) |
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
- **pyOCD**: [pyOCD](https://pyocd.io/) can also be used for uploading and debugging (e.g. `pip install pyocd`, `pyocd pack install ATSAMD21G18A`, `pyocd load <firmware>.bin -t atsamd21g18a`).
- **edbg / OpenOCD**: Alex Taradov's [edbg](https://github.com/ataradov/edbg) and [OpenOCD](https://openocd.org/) (target *at91samdXX*) also support the SAMD21.

### Via USB Bootloader
A USB bootloader (e.g. the 8KB SAM-BA bootloader of the [SAMD Development Boards](https://github.com/wagiminator/SAMD-Development-Boards), the Arduino Zero bootloader or the [UF2 bootloader](https://github.com/microsoft/uf2-samdx1)) can be installed via SWD. Afterwards, the firmware can be uploaded via USB with [BOSSA](https://github.com/shumatech/BOSSA) (`bossac`). Use the *_boot8k* linker script in this case, so that the firmware starts at address 0x2000.

```
bossac -i -U true -e -w -v -R --offset=0x2000 <firmware>.bin
```

With the UF2 bootloader, the firmware can also be converted into a *.uf2* file (e.g. with *uf2conv.py*) and copied to the USB mass storage device. The bootloader is activated by double-tapping the RESET button (or by calling `BOOT_now()` from the application, which sets the double-tap magic value and resets the MCU).

## Notes and Tips
- The template uses `F_CPU` = 48MHz by default (DFLL48M in USB clock recovery mode with factory calibration, no external crystal required). For precise timing without USB connection, use a 32.768kHz crystal on PA0/PA1 (`SYS_USE_XOSC32K` in *system.h*).
- The flash wait states are set for a supply voltage of 2.7V or more. For lower supply voltages, reduce `F_CPU` or increase the wait states (see datasheet).
- The generic clock generators GCLK0 (main clock), GCLK1 (DFLL reference), GCLK2 (WDT), GCLK3 (RTC/LPT), GCLK4 (USB, if F_CPU < 48MHz) and GCLK5 (I2S) are used by the template and the libraries, GCLK6..GCLK8 are free for the user.
- The low-power timer (LPT) and the RTC calendar functions both use the RTC and cannot be used at the same time.
- The SAMD21 has no open-drain outputs. They can be emulated by switching the pin direction (see *i2c_soft*).
- Pins are defined as PA0..PA31 and PB0..PB31 (e.g. `#define PIN_LED PA17`), the pin designators are defined in *system.h*.

## Development Boards and Projects
- [SAMD21E Development Board](https://github.com/wagiminator/SAMD-Development-Boards/tree/main/SAMD21E_DevBoard)
- [SAMD Programmer Adapter](https://github.com/wagiminator/SAMD-Development-Boards/tree/main/SAMD_Programmer_Adapter)
- Arduino Zero, Arduino MKR boards, Adafruit Feather M0, Seeed XIAO SAMD21 and many other boards with ATSAMD21

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [tinyocd on PyPI](https://pypi.org/project/tinyocd/)
- [SWD Programmer Web App](https://wagiminator.github.io/apps/swdprog/)
- [SAMD Development Boards](https://github.com/wagiminator/SAMD-Development-Boards)
- [Alex Taradov: MCU Starter Projects](https://github.com/ataradov/mcu-starter-projects/tree/master/samd21)
- [Microchip: ATSAMD21G18](https://www.microchip.com/en-us/product/atsamd21g18)
- [Microchip: Device Family Packs](https://packs.download.microchip.com/)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)). The register definitions in *samd21.h* are based on the CMSIS headers of the Microchip (Atmel) SAMD21 device family pack and Arm CMSIS, both licensed under the Apache License 2.0.
