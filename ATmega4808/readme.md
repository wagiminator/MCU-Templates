# MCU Template for megaAVR 0-Series Microcontrollers
## Overview
The megaAVR 0-series (ATmega808/809/1608/1609/3208/3209/4808/4809) from Microchip (formerly Atmel) are low-power, low-cost 8-bit AVR microcontrollers with the modern AVR peripheral set that is also used in the tinyAVR 0/1-series. They run at up to 20MHz from the internal oscillator and operate at a supply voltage of 1.8V to 5.5V. Compared to the tinyAVR, they offer more flash, SRAM, I/O pins and peripherals:
- a 10-bit ADC with up to 16 channels and an analog comparator with an 8-bit reference DAC
- a 16-bit timer/counter (TCA) with up to 6 PWM channels and up to four TCB timers
- an RTC with periodic interrupt
- configurable custom logic (CCL) with 4 LUTs and an event system with up to 8 channels
- up to four USARTs, SPI and TWI (I2C) interfaces

All chips are programmed via the 1-wire UPDI interface, which only needs a USB-to-serial converter and a resistor. Unlike on the tinyAVR, the UPDI pin is a dedicated pin and cannot be used as GPIO.

This template is a lightweight starting point for bare-metal firmware development in C with AVR-GCC without any IDE. It contains:
- a makefile, a PlatformIO configuration and a wrapper file for the Arduino IDE
- the system and GPIO functions
- an example program
- the device support files for all supported chips

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Part | Pins (Package) | Flash | SRAM | EEPROM | USART | TCB | `DEVICE` |
|:-|:-|-:|-:|-:|:-:|:-:|:-|
| ATmega808 | 28 (SSOP28), 32 (TQFP32, VQFN32) | 8KB | 1KB | 256B | 3 | 3 | atmega808 |
| ATmega809 | 48 (TQFP48, VQFN48) | 8KB | 1KB | 256B | 4 | 4 | atmega809 |
| ATmega1608 | 28 (SSOP28), 32 (TQFP32, VQFN32) | 16KB | 2KB | 256B | 3 | 3 | atmega1608 |
| ATmega1609 | 48 (TQFP48, VQFN48) | 16KB | 2KB | 256B | 4 | 4 | atmega1609 |
| ATmega3208 | 28 (SSOP28), 32 (TQFP32, VQFN32) | 32KB | 4KB | 256B | 3 | 3 | atmega3208 |
| ATmega3209 | 48 (TQFP48, VQFN48) | 32KB | 4KB | 256B | 4 | 4 | atmega3209 |
| ATmega4808 | 28 (SSOP28), 32 (TQFP32, VQFN32) | 48KB | 6KB | 256B | 3 | 3 | atmega4808 (default) |
| ATmega4809 | 40 (PDIP40), 48 (TQFP48, VQFN48) | 48KB | 6KB | 256B | 4 | 4 | atmega4809 |

All devices also have a 64-byte user row (USERROW) that is not erased by a chip erase.

Available I/O pins (without the dedicated UPDI pin):
- 28-pin: PA0-PA7, PC0-PC3, PD0-PD7, PF0, PF1, PF6 (23 I/O)
- 32-pin: PA0-PA7, PC0-PC3, PD0-PD7, PF0-PF6 (27 I/O)
- 40-pin: 33 I/O (PDIP40, ATmega4809 only; see datasheet for the available port pins)
- 48-pin: PA0-PA7, PB0-PB5, PC0-PC7, PD0-PD7, PE0-PE3, PF0-PF6 (41 I/O)

Common features of all variants:
- AVR CPU with hardware multiplier, up to 20MHz
- Internal 16/20MHz RC oscillator, internal 32kHz ultra-low-power oscillator, external 32.768kHz crystal (PF0/PF1) or external clock (PA0)
- 10-bit ADC (up to 16 channels), analog comparator with 8-bit DACREF, internal voltage references (0.55V, 1.1V, 1.5V, 2.5V, 4.3V)
- TCA0 (16-bit, up to 6 PWM channels on a selectable port), TCB0-TCB2 (TCB3 on x09), RTC/PIT, watchdog, brown-out detector
- USART0-USART2 (USART3 on x09), SPI0, TWI0 (I2C master/slave), CCL (4 LUTs), event system (6 channels, 8 on x09), CRC scan
- Dedicated 1-wire UPDI programming and debugging pin, PF6 can be used as RESET or GPIO
- Supply voltage 1.8V - 5.5V

## Pinout and Peripheral Mapping
The pin functions are the same for all packages; the pin numbers depend on the package (see datasheet).

| Port | ADC | USART | I2C | SPI | TCB WO | CCL | AC | Other Functions |
|:-|:-|:-|:-|:-|:-|:-|:-|:-|
| PA0 |  | U0 TX (0) |  |  |  | LUT0-IN0 |  | EXTCLK |
| PA1 |  | U0 RX (0) |  |  |  | LUT0-IN1 |  |  |
| PA2 |  |  | SDA (0) |  | TCB0 (0) | LUT0-IN2 |  | EVOUTA |
| PA3 |  |  | SCL (0) |  | TCB1 (0) | LUT0-OUT |  |  |
| PA4 |  | U0 TX (1) |  | MOSI (0) |  |  |  |  |
| PA5 |  | U0 RX (1) |  | MISO (0) |  |  |  |  |
| PA6 |  |  |  | SCK (0) |  | LUT0-OUT (alt) |  |  |
| PA7 |  |  |  | SS (0) |  |  | OUT | CLKOUT, EVOUTA (alt), LED (template example) |
| PB0 |  | U3 TX (0) |  |  |  |  |  | x09 only |
| PB1 |  | U3 RX (0) |  |  |  |  |  | x09 only |
| PB2 |  |  |  |  |  |  |  | EVOUTB, x09 only |
| PB4 |  | U3 TX (1) |  |  | TCB2 (1) |  |  | x09 only |
| PB5 |  | U3 RX (1) |  |  | TCB3 (0) |  |  | x09 only |
| PC0 |  | U1 TX (0) |  | MOSI (1) | TCB2 (0) | LUT1-IN0 |  |  |
| PC1 |  | U1 RX (0) |  | MISO (1) | TCB3 (1) | LUT1-IN1 |  |  |
| PC2 |  |  | SDA (1) | SCK (1) |  | LUT1-IN2 |  | EVOUTC |
| PC3 |  |  | SCL (1) | SS (1) |  | LUT1-OUT |  |  |
| PC4 |  | U1 TX (1) |  |  |  |  |  | x09 only |
| PC5 |  | U1 RX (1) |  |  |  |  |  | x09 only |
| PC6 |  |  |  |  |  | LUT1-OUT (alt) |  | 48-pin only |
| PC7 |  |  |  |  |  |  |  | EVOUTC (alt), 48-pin only |
| PD0 | AIN0 |  |  |  |  | LUT2-IN0 |  |  |
| PD1 | AIN1 |  |  |  |  | LUT2-IN1 | AINP3 |  |
| PD2 | AIN2 |  |  |  |  | LUT2-IN2 | AINP0 | EVOUTD |
| PD3 | AIN3 |  |  |  |  | LUT2-OUT | AINN0 |  |
| PD4 | AIN4 |  |  |  |  |  | AINP1 |  |
| PD5 | AIN5 |  |  |  |  |  | AINN1 |  |
| PD6 | AIN6 |  |  |  |  | LUT2-OUT (alt) | AINP2 |  |
| PD7 | AIN7 |  |  |  |  |  | AINN2 | VREFA, EVOUTD (alt) |
| PE0 | AIN8 |  |  | MOSI (2) |  |  |  | 48-pin only |
| PE1 | AIN9 |  |  | MISO (2) |  |  |  | 48-pin only |
| PE2 | AIN10 |  |  | SCK (2) |  |  |  | EVOUTE, 48-pin only |
| PE3 | AIN11 |  |  | SS (2) |  |  |  | 48-pin only |
| PF0 |  | U2 TX (0) |  |  |  | LUT3-IN0 |  | TOSC1 (32.768kHz crystal) |
| PF1 |  | U2 RX (0) |  |  |  | LUT3-IN1 |  | TOSC2 (32.768kHz crystal) |
| PF2 | AIN12 |  |  |  |  | LUT3-IN2 |  | EVOUTF, not on 28-pin |
| PF3 | AIN13 |  |  |  |  | LUT3-OUT |  | not on 28-pin |
| PF4 | AIN14 | U2 TX (1) |  |  | TCB0 (1) |  |  | not on 28-pin |
| PF5 | AIN15 | U2 RX (1) |  |  | TCB1 (1) |  |  | not on 28-pin |
| PF6 |  |  |  |  |  | LUT3-OUT (alt) |  | RESET (if configured) |
| UPDI |  |  |  |  |  |  |  | UPDI programming interface (dedicated pin) |

Notes:
- The numbers in brackets are the mapping options (`UART_MAP`/`DEBUG_TX`, `I2C_MAP`, `SPI_MAP`, `TCB_ALT`, `CCL_ALT`) that are selected in the parameter section of the respective library. The USART is selected with `UART_NUM`/`DEBUG_NUM` (U0 ... U3).
- TCA0 outputs WO0 ... WO5 are on pins 0 ... 5 of one port, which is selected with `PWM_PORT` in the *pwm* library (PORTA ... PORTF).
- PF6 is RESET or GPIO depending on fuse 5 (SYSCFG0). Since UPDI is a dedicated pin, the chip can always be reprogrammed, even if PF6 is used as GPIO.
- The template example blinks an LED on PA7.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling, uploading and setting fuses (AVR-GCC, megaupdi) |
| template/platformio.ini | Configuration file for PlatformIO |
| template/template.ino | Wrapper file for compiling with the Arduino IDE ([MegaCoreX](https://github.com/MCUdude/MegaCoreX)) |
| template/src/main.c | Example program (blinking LED on PA7) |
| template/src/system.h | Clock, RTC/PIT, watchdog, BOD/VLM, CRC scan, sleep, reset, EEPROM, chip ID and other system functions |
| template/src/gpio.h | GPIO, pin interrupt, ADC, comparator and DACREF functions |
| template/avr/ | Device support files (headers, startup code and device specs) for all supported chips, so that an older avr-gcc/avr-libc can be used |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheets and errata |

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. Parameters such as peripheral instance, pin mapping, baud rate or clock rate can be set in the parameter section at the beginning of the respective header file. The APIs are the same as in the ATtiny814 template (and the other MCU templates), so code can easily be ported.

| Library | Description |
|:-|:-|
| ccl | Configurable Custom Logic (4 LUTs, 2 sequencers, output pins), header only |
| debug_serial | Serial debug output (TX only) via USART0 ... USART3 with simple printf, for debugging |
| evsys | Event system helpers (channels, users, pin events, event outputs), header only |
| i2c | I2C master functions using TWI0 |
| i2c_slave | I2C slave with register-based interface using TWI0 |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| i2c_tx | I2C master functions, TX only (e.g. for OLED displays) |
| millis | Millisecond counter using TCB0 (more accurate) or the PIT of the RTC (less accurate) |
| neo | NeoPixel (WS2812B, SK6812, ...) driver with cycle-accurate bit-banging (F_CPU >= 8MHz) |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| pwm | PWM using TCA0: 3 x 16-bit (single mode) or 6 x 8-bit channels (split mode) on a selectable port |
| spi | SPI master functions using SPI0 |
| spi_soft | Software SPI master (bit-banging) on any pins |
| tcb | TCB0 ... TCB3: periodic timer, 8-bit PWM, frequency and pulse-width measurement |
| uart | Basic USART0 ... USART3 functions (polling, no buffer) |
| uart_int | USART0 ... USART3 with RX interrupt and ring buffer |
| userrow | Read and write the 64-byte user row (EEPROM page that survives a chip erase) |
| v-usb/hid_keyboard | Software USB HID keyboard (low-speed USB via bit-banging, V-USB) |
| v-usb/hid_mouse | Software USB HID mouse |
| v-usb/hid_consumer | Software USB HID consumer control (multimedia keys) |
| v-usb/hid_joystick | Software USB HID joystick |
| v-usb/hid_composite | Software USB HID composite device (keyboard, consumer and mouse) |

Libraries that use the same peripheral cannot be combined (e.g. *millis* with `MIL_TIMER = 0` and *tcb* with `TCB_NUM = 0`, or *uart*, *uart_int* and *debug_serial* with the same USART). Since the megaAVR has up to four USARTs, *debug_serial* can run on a different USART than *uart* or *uart_int*.

### Notes on the V-USB Libraries
The megaAVR 0-series has no USB peripheral. The libraries in the *v-usb* folder implement a low-speed USB device in software. They are based on [V-USB](https://www.obdev.at/products/vusb/index.html) by Objective Development and its port [V-USB_TinyAvr](https://github.com/12oclocker/V-USB_TinyAvr) by 12oClocker, which compensates the different instruction timings of the AVRxt CPU core used by the tinyAVR 0/1-series and the megaAVR 0-series. The API is the same as that of the USB libraries of the other MCU templates (e.g. ATtiny814, CH32V003 and CH32X035).
- Copy all files of a library (including the assembler files *usbdrvasm.S* and *\*.inc*) into the *src* folder.
- The system clock must be 16.5MHz (`F_CPU = 16500000` in the makefile). The internal oscillator must be set to 16MHz by fuse 2 (OSCCFG = 0x01). The library disables the clock prescaler and calibrates the oscillator to 16.5MHz after each USB bus reset using the USB frame timing. Since megaupdi does not know this frequency, replace `-t $(F_CPU)` by `-t 16000000` in the makefile. 12.8MHz (`F_CPU = 12800000`) is also supported, but the oscillator must then be calibrated down by about 20%, which not all chips can achieve.
- Set the USB pins in *config.h*. The default is D+ on PA2 and D- on PA1, which are available on all packages. D+ and D- must be on the same port (PORTA ... PORTF), D+ should be on one of the fully asynchronous pins Px2 or Px6. All other interrupts of this port cannot be used. With the default pins, TWI0 must use the alternative pins PC2/PC3 (`I2C_MAP = 1`) and USART0 the alternative pins PA4/PA5 (`UART_MAP = 1`), if needed.
- D- needs a 1.5k pull-up resistor (to VCC or to the GPIO defined by `USB_PIN_DPU`). At 16.5MHz the MCU requires a supply voltage of at least 4V (e.g. 5V from USB), so the data lines need 68R series resistors and 3.6V zener diodes to limit the signal levels (see [VUSB-AVR](https://github.com/wagiminator/VUSB-AVR) for a typical wiring).
- The USB requests are processed in the interrupt of a timer (every 1ms), which is selected with `USB_POLL_TIMER` in *config.h* (default: PIT of the RTC; alternatively TCB0 ... TCB3). This timer cannot be used for other purposes. The USB pin interrupt is set to high priority (level 1), so that it can interrupt all other interrupts.
- GPIOR0 and GPIOR1 are used by the V-USB driver.
- The libraries use 2.4 - 3.5KB flash.
- This implementation of Software USB is still experimental.
- V-USB is licensed under the GNU GPL v2 or v3 (see *License.txt* in the library folders).

## Compiling Firmware
### Using the Makefile
To use the *makefile*, some additional software components must be installed to set up the toolchain.

#### Install GCC Toolchain for Compiling
```
sudo apt install build-essential gcc-avr avr-libc
```

The device support files in the *template/avr* folder allow the use of older avr-gcc versions (tested with avr-gcc 5.4, 7.3 and 14.1), which do not support the megaAVR 0-series natively.

#### Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install megaupdi
```

#### Compiling/Uploading
Edit the *makefile* according to the needs of the project. Select the chip with `DEVICE` (default: atmega4808) and the clock frequency with `F_CPU` (default: 5MHz). The fuses are defined with `FUSE0` ... `FUSE8` (fuse 2 is set automatically according to `F_CPU`). `make flash` sets the oscillator fuse matching `F_CPU` and fuses 6 - 8; `make fuses` writes all fuses. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make fuses     burn fuses of MCU
make clean     remove all build files
```

Fuse 5 (SYSCFG0) defaults to 0xC9 in the makefile: PF6 as RESET pin, EEPROM is preserved during chip erase. Use 0xC1 to use PF6 as GPIO.

### Using PlatformIO
- Install [PlatformIO](https://platformio.org). Linux/Mac users may also need to install [pyenv](https://realpython.com/intro-to-pyenv).
- Click on "Open Project" and select the *template* folder with the *platformio.ini* file.
- Edit the *platformio.ini* according to the needs of the project and your programmer (board, MCU, variant).
- Connect your programmer to the target board, then click "Upload".

### Using the Arduino IDE
- Install the [Arduino IDE](https://www.arduino.cc/en/software) and [MegaCoreX](https://github.com/MCUdude/MegaCoreX).
- Open the *template.ino* file in the Arduino IDE.
- Select the MCU, the pinout, the desired clock frequency, the programmer "SerialUPDI (230400 baud)" and the correct port.
- Click on "Upload".

The *template.ino* sketch intentionally contains no code and serves only as an anchor for the Arduino IDE. Ensure that this *.ino* file has the same name as the folder in which it is located. Additionally, the subfolder *src* must be present, as this is where the code and libraries are located.

## Uploading Firmware
### Via UPDI with a USB-to-Serial Converter (SerialUPDI)
Connect a USB-to-serial converter to the UPDI pin of the microcontroller as follows (alternatively, use a pre-assembled [SerialUPDI programmer](https://github.com/wagiminator/AVR-Programmer/tree/master/SerialUPDI_Programmer)):

```
USB2SERIAL                     megaAVR
+--------+                    +-----------+
|     RXD| <------------+---> |UPDI       |
|        |              |     |           |
|     TXD| ---|1kOhm|---+     |           |
|        |                    |           |
|     VDD| -----------------> |VDD, AVDD  |
|     GND| -----------------> |GND        |
+--------+                    +-----------+
```

- **megaupdi**: Use the platform-independent open-source command-line tool [megaupdi](https://pypi.org/project/megaupdi/) (`pip install megaupdi`). It supports all AVRs with UPDI (tinyAVR 0/1/2, megaAVR 0, AVR Dx/Ex). The `-t` option sets the oscillator fuse for the given frequency, `-fs` writes fuses:
  ```
  megaupdi -d atmega4808 -f <firmware>.bin -t 5000000
  ```
- **Web app**: Use the [AVR Serial UPDI Programmer web app](https://wagiminator.github.io/apps/serialupdi/). It only works in Chrome, Edge or Opera on a desktop computer.
- **avrdude**: [AVRDUDE](https://github.com/avrdudes/avrdude) supports SerialUPDI with the programmer `serialupdi`.

Note: tinyupdi only supports the tinyAVR and cannot be used for the megaAVR 0-series.

## Notes and Tips
- The template uses `F_CPU` = 5MHz by default, which works over the whole supply voltage range. Up to 10MHz requires 2.7V, up to 20MHz 4.5V.
- The clock is set by the oscillator fuse (16 or 20MHz) and the main clock prescaler, which is configured in the system initialization according to `F_CPU`. megaupdi selects the matching oscillator fuse automatically with the `-t` option.
- The megaAVR 0-series has no DAC with output pin and no internal reference as ADC input. `ADC_read_VDD()` measures the 8-bit DACREF of the analog comparator (1.1V reference) with VDD as ADC reference instead.
- The flash is mapped into the data space at 0x4000, so data in flash can also be read with normal pointers (address + 0x4000). The avr-libc functions for PROGMEM (`pgm_read_byte()` etc.) work as usual.
- The UART libraries select the double-speed mode automatically if the BAUD rate is too high for a low `F_CPU`. With `UART_OSCCOMP` the factory-measured oscillator error can be compensated, which improves the accuracy of high BAUD rates.
- The NeoPixel library requires `F_CPU` >= 8MHz (e.g. 10MHz with the 20MHz oscillator).
- The V-USB libraries require `F_CPU` = 16.5MHz with the 16MHz oscillator (see above).
- The AVDD pin(s) must also be connected to the supply voltage.

## Development Boards
- [Microchip ATmega4809 Curiosity Nano](https://www.microchip.com/en-us/development-tool/DM320115) (ATmega4809, on-board debugger)
- [Arduino Nano Every](https://docs.arduino.cc/hardware/nano-every/) (ATmega4809)
- [SerialUPDI Programmer](https://github.com/wagiminator/AVR-Programmer/tree/master/SerialUPDI_Programmer)
- [UPDI Programmer (jtag2updi)](https://github.com/wagiminator/AVR-Programmer/tree/master/UPDI_Programmer)
- [picoAVR - ISP/TPI/UPDI Combo Programmer](https://github.com/wagiminator/AVR-Programmer/tree/master/picoAVR_Programmer)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [V-USB](https://www.obdev.at/products/vusb/index.html)
- [V-USB_TinyAvr](https://github.com/12oclocker/V-USB_TinyAvr)
- [VUSB-AVR](https://github.com/wagiminator/VUSB-AVR)
- [megaupdi on PyPI](https://pypi.org/project/megaupdi/)
- [AVR Serial UPDI Programmer Web App](https://wagiminator.github.io/apps/serialupdi/)
- [AVR Programmers](https://github.com/wagiminator/AVR-Programmer)
- [MegaCoreX](https://github.com/MCUdude/MegaCoreX)
- [Microchip: ATmega4808](https://www.microchip.com/en-us/product/ATmega4808)
- [Microchip: ATmega4809](https://www.microchip.com/en-us/product/ATmega4809)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
