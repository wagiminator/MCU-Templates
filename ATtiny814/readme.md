# MCU Template for tinyAVR 0-Series and 1-Series Microcontrollers
## Overview
The tinyAVR 0-series and 1-series from Microchip (formerly Atmel) are low-power, low-cost 8-bit AVR microcontrollers with the modern AVR peripheral set known from the megaAVR 0-series. They run at up to 20MHz from the internal oscillator and operate at a supply voltage of 1.8V to 5.5V. The chips have the following peripherals:
- a 10-bit ADC and an analog comparator
- a 16-bit timer/counter (TCA) and up to two TCB timers
- an RTC with periodic interrupt
- configurable custom logic (CCL) and an event system
- USART, SPI and TWI (I2C) interfaces

The 1-series adds a 12-bit timer for control applications (TCD), a DAC and a peripheral touch controller (PTC). The ATtiny1614/1616/3216 (1-series with 16KB or more) additionally have a second ADC, three comparators/DACs and a second TCB. All chips are programmed via the 1-wire UPDI interface, which only needs a USB-to-serial converter and a resistor.

This template is a lightweight starting point for bare-metal firmware development in C with AVR-GCC without any IDE. It contains:
- a makefile, a PlatformIO configuration and a wrapper file for the Arduino IDE
- the system and GPIO functions
- an example program
- the device support files for all supported chips

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Part | Series | Pins | Flash | SRAM | EEPROM | TCD/DAC/PTC | `DEVICE` |
|:-|:-:|:-|-:|-:|-:|:-:|:-|
| ATtiny202 | 0 | 8 (SOIC8) | 2KB | 128B | 64B | - | attiny202 |
| ATtiny204 | 0 | 14 (SOIC14) | 2KB | 128B | 64B | - | attiny204 |
| ATtiny212 | 1 | 8 (SOIC8) | 2KB | 128B | 64B | TCD, DAC | attiny212 |
| ATtiny214 | 1 | 14 (SOIC14) | 2KB | 128B | 64B | TCD, DAC | attiny214 |
| ATtiny402 | 0 | 8 (SOIC8) | 4KB | 256B | 128B | - | attiny402 |
| ATtiny404 | 0 | 14 (SOIC14) | 4KB | 256B | 128B | - | attiny404 |
| ATtiny406 | 0 | 20 (SOIC20, VQFN20) | 4KB | 256B | 128B | - | attiny406 |
| ATtiny412 | 1 | 8 (SOIC8) | 4KB | 256B | 128B | TCD, DAC | attiny412 |
| ATtiny414 | 1 | 14 (SOIC14) | 4KB | 256B | 128B | TCD, DAC, PTC | attiny414 |
| ATtiny416 | 1 | 20 (SOIC20, VQFN20) | 4KB | 256B | 128B | TCD, DAC, PTC | attiny416 |
| ATtiny804 | 0 | 14 (SOIC14) | 8KB | 512B | 128B | - | attiny804 |
| ATtiny806 | 0 | 20 (SOIC20, VQFN20) | 8KB | 512B | 128B | - | attiny806 |
| ATtiny814 | 1 | 14 (SOIC14) | 8KB | 512B | 128B | TCD, DAC, PTC | attiny814 (default) |
| ATtiny816 | 1 | 20 (SOIC20, VQFN20) | 8KB | 512B | 128B | TCD, DAC, PTC | attiny816 |
| ATtiny1604 | 0 | 14 (SOIC14) | 16KB | 1KB | 256B | - | attiny1604 |
| ATtiny1606 | 0 | 20 (SOIC20, VQFN20) | 16KB | 1KB | 256B | - | attiny1606 |
| ATtiny1614 | 1 | 14 (SOIC14) | 16KB | 2KB | 256B | TCD, 3x DAC, PTC | attiny1614 |
| ATtiny1616 | 1 | 20 (SOIC20, VQFN20) | 16KB | 2KB | 256B | TCD, 3x DAC, PTC | attiny1616 |
| ATtiny3216 | 1 | 20 (SOIC20, VQFN20) | 32KB | 2KB | 256B | TCD, 3x DAC, PTC | attiny3216 |

The I/O count is 6 (8-pin), 12 (14-pin) or 18 (20-pin), including the UPDI pin. The PTC is not available on the 8-pin and 2KB devices. The ATtiny x07/x17 (24-pin) and the tinyAVR 2-series are not supported by the included device files.

Common features of all variants:
- AVR CPU with hardware multiplier, up to 20MHz
- Internal 16/20MHz RC oscillator, internal 32kHz ultra-low-power oscillator, external 32.768kHz crystal (1-series, 14/20-pin) or external clock
- 10-bit ADC (up to 12 channels), analog comparator, internal voltage references
- TCA0 (16-bit, up to 6 PWM channels), TCB0, RTC/PIT, watchdog, brown-out detector
- USART0, SPI0, TWI0 (I2C master/slave), CCL, event system, CRC scan
- 1-wire UPDI programming and debugging interface (PA0)
- Supply voltage 1.8V - 5.5V

## Pinout and Peripheral Mapping
### ATtiny204/214/404/414/804/814/1604/1614 (SOIC14)

| Pin | Port | ADC | USART0 | I2C | SPI | TCA0 (pwm) | TCB/TCD | CCL/AC | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|:-|:-|:-|
| 1 | VCC |  |  |  |  |  |  |  | Supply voltage 1.8V - 5.5V |
| 2 | PA4 | AIN4 |  |  | SS | WO4 | TCD WOA |  LUT0-OUT |  |
| 3 | PA5 | AIN5 |  |  |  | WO5 | TCB0 WO, TCD WOB | AC OUT | VREFA |
| 4 | PA6 | AIN6 |  |  |  |  |  | AINN0 | DAC0 output (1-series) |
| 5 | PA7 | AIN7 |  |  |  |  |  | AINP0, LUT1-OUT | LED (template example) |
| 6 | PB3 |  | RX (0) |  |  | WO0 (alt) |  |  | TOSC1 (1-series) |
| 7 | PB2 |  | TX (0) |  |  | WO2 |  |  | TOSC2 (1-series), EVOUT1 |
| 8 | PB1 | AIN10 |  | SDA (0) |  | WO1 |  | AINP2 |  |
| 9 | PB0 | AIN11 |  | SCL (0) |  | WO0 |  |  |  |
| 10 | PA0 | AIN0 |  |  |  |  |  | LUT0-IN0 | UPDI (programming), RESET (if configured) |
| 11 | PA1 | AIN1 | TX (1) | SDA (1) | MOSI (0) |  |  | LUT0-IN1 |  |
| 12 | PA2 | AIN2 | RX (1) | SCL (1) | MISO (0) |  |  | LUT0-IN2 | EVOUT0 |
| 13 | PA3 | AIN3 |  |  | SCK (0) | WO3 | TCB1 WO (16KB 1-series) |  | EXTCLK |
| 14 | GND |  |  |  |  |  |  |  | Ground |

Notes:
- The numbers in brackets are the mapping options (`UART_MAP`/`DEBUG_TX`, `I2C_MAP`) that are selected in the parameter section of the respective library. Map 0 uses the default pins, map 1 the alternate pins (PORTMUX).
- SPI0 on the 14-pin devices is only available on the default pins (PA1 - PA4). The alternate SPI pins (PC0 - PC3) only exist on the 20-pin devices.
- On the 8-pin devices (ATtiny202/212/402/412), the default pins are different: USART0 TX/RX = PA6/PA7 (map 0) or PA1/PA2 (map 1), I2C SDA/SCL = PA1/PA2, SPI MOSI/MISO/SCK/SS = PA1/PA2/PA3/PA0, TCA0 WO0/WO1/WO2 = PA3 (alt. PA7)/PA1/PA2, TCB0 WO = PA6, AC OUT = PA3, LUT0/LUT1-OUT = PA6/PA7. The libraries select these pins automatically on the 8-pin devices.
- 8-pin pinout (SOIC8): 1 VCC, 2 PA6, 3 PA7, 4 PA1, 5 PA2, 6 PA0/UPDI, 7 PA3, 8 GND.
- PA0 is the UPDI pin. It can be configured as RESET or GPIO via fuse 5 (SYSCFG0). As GPIO, the chip can only be reprogrammed with a high-voltage UPDI programmer.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling, uploading and setting fuses (AVR-GCC, tinyupdi) |
| template/platformio.ini | Configuration file for PlatformIO |
| template/template.ino | Wrapper file for compiling with the Arduino IDE ([megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)) |
| template/src/main.c | Example program (blinking LED on PA7) |
| template/src/system.h | Clock, RTC/PIT, watchdog, BOD/VLM, sleep, reset, EEPROM, chip ID and other system functions |
| template/src/gpio.h | GPIO, pin interrupt, ADC, DAC and comparator functions |
| template/avr/ | Device support files (headers, startup code and device specs) for all supported chips, so that an older avr-gcc/avr-libc can be used |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheets (ATtiny402 family, ATtiny1604 family, ATtiny3216) and errata |

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. Parameters such as pin mapping, baud rate or clock rate can be set in the parameter section at the beginning of the respective header file.

| Library | Description |
|:-|:-|
| ccl | Configurable Custom Logic (2 LUTs, sequencer, output pins), header only |
| debug_serial | Serial debug output (TX only) via USART0 with simple printf, for debugging |
| evsys | Event system helpers (channels, users, pin events, event outputs), header only |
| i2c | I2C master functions using TWI0 |
| i2c_slave | I2C slave with register-based interface using TWI0 |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| i2c_tx | I2C master functions, TX only (e.g. for OLED displays) |
| millis | Millisecond counter using TCB0 (more accurate) or the PIT of the RTC (less accurate) |
| neo | NeoPixel (WS2812B, SK6812, ...) driver with cycle-accurate bit-banging (F_CPU >= 8MHz) |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| pwm | PWM using TCA0: 3 x 16-bit (single mode) or 6 x 8-bit channels (split mode) |
| spi | SPI master functions using SPI0 |
| spi_soft | Software SPI master (bit-banging) on any pins |
| tcb | TCB0/TCB1: periodic timer, 8-bit PWM, frequency and pulse-width measurement |
| tcd | PWM using the 12-bit TCD0 (1-series only), clocked directly by the 16/20MHz oscillator |
| uart | Basic USART0 functions (polling, no buffer) |
| uart_int | USART0 with RX interrupt and ring buffer |
| userrow | Read and write the 32-byte user row (EEPROM page that survives a chip erase) |
| v-usb/hid_keyboard | Software USB HID keyboard (low-speed USB via bit-banging, V-USB) |
| v-usb/hid_mouse | Software USB HID mouse |
| v-usb/hid_consumer | Software USB HID consumer control (multimedia keys) |
| v-usb/hid_joystick | Software USB HID joystick |
| v-usb/hid_composite | Software USB HID composite device (keyboard, consumer and mouse) |

Libraries that use the same peripheral cannot be combined (e.g. *millis* with `MIL_TIMER = 0` and *tcb* with `TCB_NUM = 0`, or *uart*, *uart_int* and *debug_serial*).

### Notes on the V-USB Libraries
The tinyAVR has no USB peripheral. The libraries in the *v-usb* folder implement a low-speed USB device in software. They are based on [V-USB](https://www.obdev.at/products/vusb/index.html) by Objective Development and its tinyAVR 0/1-series port [V-USB_TinyAvr](https://github.com/12oclocker/V-USB_TinyAvr) by 12oClocker, which compensates the different instruction timings of the tinyAVR core. The API is the same as that of the USB libraries of the other MCU templates (e.g. CH32V003 and CH32X035).
- Copy all files of a library (including the assembler files *usbdrvasm.S* and *\*.inc*) into the *src* folder.
- The system clock must be 16.5MHz (`F_CPU = 16500000` in the makefile). The internal oscillator must be set to 16MHz by fuse 2 (OSCCFG = 0x01). The library disables the clock prescaler and calibrates the oscillator to 16.5MHz after each USB bus reset using the USB frame timing. Since tinyupdi does not know this frequency, replace `-t $(F_CPU)` by `-t 16000000` in the makefile. 12.8MHz (`F_CPU = 12800000`) is also supported, but the oscillator must then be calibrated down by about 20%, which not all chips can achieve.
- Set the USB pins in *config.h*. The default is D+ on PA2 and D- on PA1, which are available on all packages (8/14/20-pin). D+ and D- must be on the same port, D+ should be on one of the fully asynchronous pins Px2 or Px6. All other interrupts of this port cannot be used.
- D- needs a 1.5k pull-up resistor (to VCC or to the GPIO defined by `USB_PIN_DPU`). At 16.5MHz the MCU requires a supply voltage of at least 4V (e.g. 5V from USB), so the data lines need 68R series resistors and 3.6V zener diodes to limit the signal levels (see [VUSB-AVR](https://github.com/wagiminator/VUSB-AVR) for a typical wiring).
- The USB requests are processed in the interrupt of a timer (every 1ms), which is selected with `USB_POLL_TIMER` in *config.h* (default: PIT of the RTC; alternatively TCB0 or TCB1). This timer cannot be used for other purposes. The USB pin interrupt is set to high priority (level 1), so that it can interrupt all other interrupts.
- GPIOR0 and GPIOR1 are used by the V-USB driver.
- The libraries require 4KB flash or more (2.3 - 3.4KB are used by the libraries).
- This implementation of Software USB is still experimental.
- V-USB is licensed under the GNU GPL v2 or v3 (see *License.txt* in the library folders).

## Compiling Firmware
### Using the Makefile
To use the *makefile*, some additional software components must be installed to set up the toolchain.

#### Install GCC Toolchain for Compiling
```
sudo apt install build-essential gcc-avr avr-libc
```

#### Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install tinyupdi
```

#### Compiling/Uploading
Edit the *makefile* according to the needs of the project. Select the chip with `DEVICE` (default: attiny814) and the clock frequency with `F_CPU` (default: 5MHz). The fuses are defined with `FUSE0` ... `FUSE8`. `make flash` sets the oscillator fuse matching `F_CPU` and fuses 6 - 8; `make fuses` writes all fuses. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make fuses     burn fuses of MCU
make clean     remove all build files
```

### Using PlatformIO
- Install [PlatformIO](https://platformio.org). Linux/Mac users may also need to install [pyenv](https://realpython.com/intro-to-pyenv).
- Click on "Open Project" and select the *template* folder with the *platformio.ini* file.
- Edit the *platformio.ini* according to the needs of the project and your programmer.
- Connect your programmer to the target board, then click "Upload".

### Using the Arduino IDE
- Install the [Arduino IDE](https://www.arduino.cc/en/software) and [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore).
- Open the *template.ino* file in the Arduino IDE.
- Select the MCU, the desired clock frequency, the programmer "SerialUPDI - 230400 baud" and the correct port.
- Click on "Upload".

The *template.ino* sketch intentionally contains no code and serves only as an anchor for the Arduino IDE. Ensure that this *.ino* file has the same name as the folder in which it is located. Additionally, the subfolder *src* must be present, as this is where the code and libraries are located.

## Uploading Firmware
### Via UPDI with a USB-to-Serial Converter (SerialUPDI)
Connect a USB-to-serial converter to the UPDI pin of the microcontroller as follows (alternatively, use a pre-assembled [SerialUPDI programmer](https://github.com/wagiminator/AVR-Programmer/tree/master/SerialUPDI_Programmer)):

```
USB2SERIAL                       tinyAVR
+--------+                    +-----------+
|     RXD| <------------+---> |UPDI (PA0) |
|        |              |     |           |
|     TXD| ---|1kOhm|---+     |           |
|        |                    |           |
|     VDD| -----------------> |VDD        |
|     GND| -----------------> |GND        |
+--------+                    +-----------+
```

- **tinyupdi**: Use the platform-independent open-source command-line tool [tinyupdi](https://pypi.org/project/tinyupdi/) (`pip install tinyupdi`). The `-t` option sets the oscillator fuse for the given frequency, `-fs` writes fuses:
  ```
  tinyupdi -f <firmware>.bin -t 5000000
  ```
- **megaupdi**: The tool [megaupdi](https://pypi.org/project/megaupdi/) (`pip install megaupdi`) supports all AVRs with UPDI (tinyAVR 0/1/2, megaAVR 0, AVR Dx/Ex) and can be used in the same way.
- **Web app**: Use the [AVR Serial UPDI Programmer web app](https://wagiminator.github.io/apps/serialupdi/). It can also set the clock and the fuses and only works in Chrome, Edge or Opera on a desktop computer.
- **avrdude**: [AVRDUDE](https://github.com/avrdudes/avrdude) supports SerialUPDI with the programmer `serialupdi`.

## Notes and Tips
- The template uses `F_CPU` = 5MHz by default, which works over the whole supply voltage range. Up to 10MHz requires 2.7V, up to 20MHz 4.5V.
- The clock is set by the oscillator fuse (16 or 20MHz) and the main clock prescaler, which is configured in the system initialization according to `F_CPU`. tinyupdi selects the matching oscillator fuse automatically with the `-t` option.
- Writing fuse 5 (SYSCFG0) with wrong values can disable the UPDI pin. The chip can then only be programmed with a high-voltage UPDI programmer.
- The UART libraries select the double-speed mode automatically if the BAUD rate is too high for a low `F_CPU`. With `UART_OSCCOMP` the factory-measured oscillator error can be compensated, which improves the accuracy of high BAUD rates.
- The NeoPixel library requires `F_CPU` >= 8MHz (e.g. 10MHz with the 20MHz oscillator).
- The V-USB libraries require `F_CPU` = 16.5MHz with the 16MHz oscillator (see above).

## Development Boards and Projects
### Development Boards and Programmers
- [ATtiny814 Development Board](https://github.com/wagiminator/AVR-Development-Boards/tree/master/ATtiny814_DevBoard) (ATtiny x04/x14/x24)
- [ATtiny814 Development Board v2](https://github.com/wagiminator/AVR-Development-Boards/tree/master/ATtiny814_DevBoard_v2)
- [ATtiny3216 Development Board](https://github.com/wagiminator/AVR-Development-Boards/tree/master/ATtiny3216_DevBoard) (ATtiny x06/x16/x26)
- [ATtiny3216 Development Board v2](https://github.com/wagiminator/AVR-Development-Boards/tree/master/ATtiny3216_DevBoard_v2)
- [SerialUPDI Programmer](https://github.com/wagiminator/AVR-Programmer/tree/master/SerialUPDI_Programmer)
- [UPDI Programmer (jtag2updi)](https://github.com/wagiminator/AVR-Programmer/tree/master/UPDI_Programmer)
- [picoAVR - ISP/TPI/UPDI Combo Programmer](https://github.com/wagiminator/AVR-Programmer/tree/master/picoAVR_Programmer)

### Projects
- [Li-Ion, LiPo and LiFePO4 Battery Capacity Tester (ATtiny412)](https://github.com/wagiminator/ATtiny412-BatteryCapacityTester)
- [Hamster Wheel Tracker (ATtiny402/412)](https://github.com/wagiminator/ATtiny412-HamsterTracker)
- [Rotary Encoder with I²C Interface (ATtiny412)](https://github.com/wagiminator/ATtiny412-I2C-Rotary-Encoder)
- [FM Pocket Radio with RDS (ATtiny412)](https://github.com/wagiminator/ATtiny412-PocketRadio)
- [USB PD Inverter (ATtiny212/412)](https://github.com/wagiminator/ATtiny412-USB-PD-Inverter)
- [USB Power Isolator (ATtiny412)](https://github.com/wagiminator/ATtiny412-USB-Power-Isolator)
- [Voice Changer (ATtiny412)](https://github.com/wagiminator/ATtiny412-VoiceChanger)
- [AquaTimer - Programmable Timer for Aquariums (ATtiny414/814/1614)](https://github.com/wagiminator/ATtiny814-AquaTimer)
- [USB2NRF - USB Serial Controlled 2.4GHz Transceiver (ATtiny814)](https://github.com/wagiminator/ATtiny814-NRF2USB)
- [Power Supply Analyzer with USB Interface (ATtiny814)](https://github.com/wagiminator/ATtiny814-Power-Analyzer)
- [USB PD Adapter (ATtiny814)](https://github.com/wagiminator/ATtiny814-USB-PD-Adapter)
- [USB-RTC - Real-Time Clock USB Stick (ATtiny214/414/814)](https://github.com/wagiminator/ATtiny814-USB-RTC)
- [C64 DiskBuddy64 (ATtiny202/212/402/412)](https://github.com/wagiminator/C64-Collection/tree/master/C64_DiskBuddy64)
- [C64 DumpMaster64 (ATtiny804/814/1604/1614)](https://github.com/wagiminator/C64-Collection/tree/master/C64_DumpMaster64)
- [C64 TapeBuddy64 (ATtiny404/414/804/814/1604/1614)](https://github.com/wagiminator/C64-Collection/tree/master/C64_TapeBuddy64)
- [C64 TapeDump64 (ATtiny202/212/402/412)](https://github.com/wagiminator/C64-Collection/tree/master/C64_TapeDump64)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [V-USB](https://www.obdev.at/products/vusb/index.html)
- [V-USB_TinyAvr](https://github.com/12oclocker/V-USB_TinyAvr)
- [VUSB-AVR](https://github.com/wagiminator/VUSB-AVR)
- [tinyupdi on PyPI](https://pypi.org/project/tinyupdi/)
- [megaupdi on PyPI](https://pypi.org/project/megaupdi/)
- [AVR Serial UPDI Programmer Web App](https://wagiminator.github.io/apps/serialupdi/)
- [AVR Programmers](https://github.com/wagiminator/AVR-Programmer)
- [AVR Development Boards](https://github.com/wagiminator/AVR-Development-Boards)
- [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)
- [Microchip: tinyAVR 0-series and 1-series](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus/tinyavr-0-series)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
