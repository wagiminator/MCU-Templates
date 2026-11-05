# MCU Template for STC8H8K64U Microcontrollers
## Overview
The STC8H8K64U from STC is a cost-effective 8-bit microcontroller with an ultra-high-speed 1T 8051 core that is compatible with the MCS51 instruction set, and a native USB 2.0 full-speed device interface. The average instruction speed is about 12 times faster than that of the traditional 8051. The chip operates at a supply voltage of 1.9V to 5.5V. Its internal high-precision RC oscillator can be adjusted up to 45MHz; the specified maximum main frequency is 40MHz. The chip has the following peripherals:
- a 12-bit ADC and an analog comparator
- 16-bit PWM, DMA and an RTC
- an LCM interface for TFT displays
- 4 UARTs, SPI and I2C

It has a factory built-in USB bootloader (HID), so firmware can be uploaded directly via USB without an additional programming device or driver.

This template is a lightweight starting point for bare-metal firmware development in C with the SDCC compiler. It contains:
- a makefile
- the system, GPIO and delay functions
- the register definitions
- an example program

Optional libraries for the internal peripherals and USB devices are also included.

## Supported Microcontrollers

| Part | Packages | Flash (Code) | SRAM | xRAM | USB RAM | GPIO | ADC | UART | `CODE_SIZE` |
|:-|:-|-:|-:|-:|-:|-:|-:|-:|:-|
| STC8H8K64U | LQFP64, QFN64, LQFP48, QFN48, LQFP32, TSSOP20 | 64KB | 256B | 8KB | 1.25KB | up to 61 | 15x 12-bit | 4 | 0xFE00 (default) |
| STC8H8K60U | as STC8H8K64U | 60KB | 256B | 8KB | 1.25KB | up to 61 | 15x 12-bit | 4 | 0xF000 |
| STC8H8K48U | as STC8H8K64U | 48KB | 256B | 8KB | 1.25KB | up to 61 | 15x 12-bit | 4 | 0xC000 |

The program flash not used for code can be used as EEPROM (IAP, size configurable). The number of available pins and peripherals depends on the package.

Common features of all variants:
- Ultra-high-speed 1T 8051 core (MCS51 compatible), max. 40MHz main frequency, hardware 16-bit multiplier/divider (MDU16)
- Internal high-precision RC oscillator (IRC, adjustable up to 45MHz, ±0.3%), internal 48MHz oscillator for USB (IRC48M), internal 32kHz oscillator (LSI), external 4MHz - 33MHz oscillator (HSE)
- USB 2.0/1.1 full-speed device with 6 bidirectional endpoints (64-byte FIFO each)
- 12-bit ADC (up to 15 channels, 800ksps) with internal 1.19V reference, analog comparator
- 5x 16-bit timers, 8 channels of 16-bit PWM (PWMA/PWMB) with complementary outputs and dead time
- 4x UART, 1x SPI, 1x I2C (master/slave), LCM interface (8080/6800), DMA, RTC with calendar
- Watchdog, low-voltage detection, power-on reset

## Pinout and Peripheral Mapping
The STC8H8K64U is available in several packages with different pinouts. The following table shows the port pins of the peripherals used by the template and its libraries; check the reference manual or the [pinout of the development board](https://github.com/wagiminator/Development-Boards/tree/main/STC8H8K64U_DevBoard) for the pin numbers of your package.

| Function | Pins | Library / Note |
|:-|:-|:-|
| USB D-/D+ | P3.0/P3.1 | usb libraries |
| Bootloader entry | P3.2 | pull low at power-on to start the USB bootloader |
| ADC channels 0 - 7 | P1.0, P1.1, P5.4, P1.3 - P1.7 | gpio.h (ADC2 is on P5.4) |
| ADC channels 8 - 14 | P0.0 - P0.6 | gpio.h |
| UART1 RX/TX | P3.0/P3.1 (0), P3.6/P3.7 (1), P1.6/P1.7 (2), P4.3/P4.4 (3) | uart.h, uart1_int.h, debug_serial.h (map 0 shares the USB pins!) |
| UART2 RX/TX | P1.0/P1.1 (0), P4.6/P4.7 (1) | uart.h, debug_serial.h |
| UART3 RX/TX | P0.0/P0.1 (0), P5.0/P5.1 (1) | uart.h |
| UART4 RX/TX | P0.2/P0.3 (0), P5.2/P5.3 (1) | uart.h |
| I2C SDA/SCL | P1.4/P1.5 (0), P2.4/P2.5 (1), P7.6/P7.7 (2), P3.3/P3.2 (3) | i2c.h |
| SPI MOSI/MISO/SCK | P1.3/P1.4/P1.5 (0), P2.3/P2.4/P2.5 (1), P4.0/P4.1/P4.3 (2), P3.4/P3.3/P3.2 (3) | spi.h |
| NeoPixel, software UART | any pin | neo.h, uart_tx_soft.h |
| LED (template example) | P3.4 | main.c |

Notes:
- The numbers in brackets are the pin mapping options that are selected in the parameter section of the respective library.
- UART1 map 0 uses P3.0/P3.1, which are also the USB pins. When using USB, select another UART1 mapping or use UART2 for debugging.
- I2C/SPI map 3 uses P3.2, the bootloader entry pin.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (SDCC toolchain) |
| template/src/main.c | Example program (blinking LED on P3.4) |
| template/src/system.h | Clock, sleep, watchdog, reset, interrupt and other system functions |
| template/src/gpio.h | GPIO, PWM, ADC and comparator functions |
| template/src/delay.c/h | Delay functions (µs and ms) |
| template/src/stc8h.h | Register definitions (SFR and XFR) for the STC8H series |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | STC8H reference manual |

There is no linker script: memory sizes are passed to SDCC by the makefile (`IRAM_SIZE`, `XRAM_SIZE`, `CODE_SIZE`).

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* (or *include*) folder of the template and include the header file in your program. Parameters such as pin mapping, baud rate or clock rate can be set at the beginning of the respective header file. The *neo* library needs a *config.h* file with `PIN_NEO`, `NEO_GRB` and `NEO_COUNT`, which must be created in the *src* folder if it does not exist yet.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) via UART1 or UART2 with a simple printf |
| eeprom | Read, write and erase the data flash (EEPROM/IAP) |
| i2c | I2C master functions using the hardware I2C module |
| neo | NeoPixel (WS2812) control on any pin, timing is calculated from `F_CPU` |
| spi | SPI master functions using the hardware SPI module |
| uart | Basic UART1 - UART4 functions (polling) |
| uart1_int | UART1 with RX interrupt and ring buffer |
| uart_tx_soft | Software UART (TX only) on any pin |
| usb/usb_cdc | USB CDC (virtual serial port) |
| usb/hid_keyboard | USB HID keyboard |
| usb/hid_mouse | USB HID mouse |
| usb/hid_composite | USB HID composite device (keyboard, consumer keys and mouse) |
| usb/hid_multitouch | USB HID multitouch digitizer |
| usb/hid_data | USB HID raw data transfer (64-byte reports) |

Since SDCC requires interrupt service routines to be visible in the file containing `main()`, the ISR of interrupt-driven libraries (e.g. *uart1_int*) must be defined in *main.c* (see the respective header file).

### Using the USB Libraries
Each USB library consists of the class driver, the USB descriptors (*usb_descr.c/h*), the device configuration (*config.h*) and the USB handler (*usb_handler.c/h*), which is identical for all libraries. All accesses to the USB peripheral (the SIE registers are accessed indirectly via `USBADR`/`USBDAT`) take place exclusively within the USB handler; the class drivers only use the endpoint functions `USB_EP_init()`, `USB_EP_send()` and `USB_EP_receive()`. Copy all files of the selected library into the *src* (or *include*) folder and adjust the *config.h* according to your needs.

The USB interrupt service routine must be defined in *main.c*:

```c
#include "usb_cdc.h"                      // or any other USB library

void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

void main(void) {
  P_SW2 |= 0x80;                          // enable access to extended registers
  CDC_init();                             // init USB (also starts IRC48M)
  ...
}
```

The USB peripheral is clocked by the internal 48MHz oscillator (IRC48M), which is enabled by the USB init function. The system clock (`F_CPU`) can be selected independently. The default VID/PID pairs are shared IDs that are only suitable for private projects.

## Compiling Firmware
To use the *makefile*, some additional software components must be installed to set up the toolchain.

### Install SDCC Toolchain for Compiling
```
sudo apt install build-essential sdcc
```

### Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install stc8usb
```

### Give Access Permission to the Factory built-in USB Bootloader
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="34bf", ATTR{idProduct}=="1001", MODE="666"' | sudo tee /etc/udev/rules.d/99-STC-ISP.rules
sudo udevadm control --reload-rules
```

### Compiling/Uploading
Edit the *makefile* according to the needs of the project (e.g. `F_CPU`, default 24MHz). The internal oscillator is trimmed to `F_CPU` by the programming tool during upload. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile, build and keep all files
make hex       compile and build <firmware>.hex
make bin       compile and build <firmware>.bin
make flash     compile, build and upload to MCU
make clean     remove all build files
```

## Uploading Firmware
### Via Factory built-in USB Bootloader
The bootloader works as a USB HID device, so no driver is required (only the udev rule on Linux, see above). To start the bootloader, disconnect the board from the USB port and all voltage sources. Pull P3.2 to GND (e.g. press and hold the BOOT button of the development board) while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode and P3.2 can be released.

- **stc8usb**: Use the platform-independent open-source command-line tool [stc8usb](https://pypi.org/project/stc8usb/) (`pip install stc8usb`). The `-t` option sets the system frequency (IRC trimming), which should match `F_CPU`:
  ```
  stc8usb -t 24000000 -f <firmware>.bin
  ```
- **STC-ISP**: STC offers the free but closed-source software [STC-ISP](https://www.stcmicro.com/rjxz.html) for Windows.

There is currently no web app for STC microcontrollers.

### Via UART Bootloader
The chip can also be programmed via UART1 (P3.0/P3.1) with [stc8isp](https://pypi.org/project/stc8isp/) and a USB-to-serial converter (see the STC8H1K08 template). Since these are also the USB pins, this is rarely useful on USB boards.

## Notes and Tips
- The template uses `F_CPU` = 24MHz by default. The internal oscillator is trimmed to this value during upload (`-t` option).
- After power-on, the I/O pins of the STC8H are in high-impedance input mode (except P3.0/P3.1), not in the quasi-bidirectional mode of the classic 8051. Use the pin functions in *gpio.h* to set the pin modes.
- Extended registers (XFR, e.g. pin pull-ups, PWM, USB) can only be accessed when `P_SW2.7` (EAXFR) is set. The example program does this at the beginning of `main()` (`P_SW2 |= 0x80;`), keep this line in your own programs.
- According to the reference manual, USB direct download without pulling P3.2 low ("USB direct emulation") is only supported by the STC8H8K64U-B version.

## Development Boards and Projects
- [STC8H8K64U Development Board](https://github.com/wagiminator/Development-Boards/tree/main/STC8H8K64U_DevBoard) (STC8H8K64U-45I-TSSOP20)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [stc8usb on PyPI](https://pypi.org/project/stc8usb/)
- [stc8isp on PyPI](https://pypi.org/project/stc8isp/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [STC: STC8H8K64U Product Page](https://stcmicro.com/stc/stc8h8k64u.html)
- [STC-ISP Software](https://www.stcmicro.com/rjxz.html)
- [Arduino Core for STC8H](https://github.com/thevien257/STC_Arduino_Core)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
