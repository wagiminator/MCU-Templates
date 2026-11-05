# MCU Template for STC8H1K08 Microcontrollers
## Overview
The STC8H1K08 from STC is a cost-effective 8-bit microcontroller with an ultra-high-speed 1T 8051 core that is compatible with the MCS51 instruction set. The average instruction speed is about 12 times faster than that of the traditional 8051. It runs at up to 36MHz from its internal high-precision RC oscillator, operates at a supply voltage of 1.9V to 5.5V, and has the following peripherals:
- a 10-bit ADC and an analog comparator
- multiple timers and 16-bit PWM
- UART, SPI and I2C interfaces

It has a factory built-in UART bootloader, so firmware can be uploaded with a simple USB-to-serial converter.

This template is a lightweight starting point for bare-metal firmware development in C with the SDCC compiler. It contains:
- a makefile
- the system, GPIO and delay functions
- the register definitions
- an example program

Optional libraries for the internal peripherals are also included.

## Supported Microcontrollers

| Part | Package | Flash (Code) | EEPROM | SRAM | xRAM | GPIO | ADC | UART | `CODE_SIZE` |
|:-|:-|-:|-:|-:|-:|-:|-:|-:|:-|
| STC8H1K08 | TSSOP20/QFN20 | 8KB | 4KB | 256B | 1KB | 17 | 9x 10-bit | 2 | 0x2000 (default) |
| STC8H1K12 | TSSOP20/QFN20 | 12KB | IAP | 256B | 1KB | 17 | 9x 10-bit | 2 | 0x3000 |
| STC8H1K17 | TSSOP20/QFN20 | 17KB | IAP | 256B | 1KB | 17 | 9x 10-bit | 2 | 0x4400 |

"IAP" means that the unused part of the program flash can be used as EEPROM. Other STC8H chips can also be used with the template, but the available pins and peripherals must be checked (e.g. in *gpio.h* and the libraries).

Common features of all variants:
- Ultra-high-speed 1T 8051 core (MCS51 compatible), up to 36MHz
- Internal high-precision RC oscillator (IRC, adjustable, ±0.3%), internal 32kHz oscillator (LSI), external 4MHz - 33MHz oscillator (HSE)
- 10-bit ADC (9 channels), analog comparator
- 3x 16-bit timers (T0 - T2), 8 channels of 16-bit PWM (PWMA/PWMB) with complementary outputs and dead time
- 2x UART, 1x SPI, 1x I2C (master/slave)
- Watchdog, low-voltage detection, power-on reset
- 17 GPIOs: P1.0 - P1.7, P3.0 - P3.7, P5.4

## Pinout and Peripheral Mapping
### STC8H1K08 (TSSOP20)

| Pin | Port | ADC | UART | I2C | SPI | PWM | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|:-|
| 1 | P1.2 |  |  |  |  | ✓ |  |
| 2 | P1.3 |  |  |  | MOSI (0) | ✓ |  |
| 3 | P1.4 |  |  | SDA (0) | MISO (0) | ✓ |  |
| 4 | P1.5 |  |  | SCL (0) | SCK (0) | ✓ |  |
| 5 | P1.6 |  | U1 RX (2) |  |  | ✓ |  |
| 6 | P1.7 |  | U1 TX (2) |  |  | ✓ |  |
| 7 | P5.4 |  |  |  |  |  | RST (if enabled), MCO |
| 8 | VCC |  |  |  |  |  | Supply voltage 1.9V - 5.5V |
| 9 | ADC_VRef+ |  |  |  |  |  | ADC reference voltage (connect to VCC if not used) |
| 10 | GND |  |  |  |  |  | Ground |
| 11 | P3.0 | ADC8 | U1 RX (0) |  |  |  | Bootloader RXD |
| 12 | P3.1 | ADC9 | U1 TX (0) |  |  |  | Bootloader TXD |
| 13 | P3.2 | ADC10 |  | SCL (3) | SCK (3) |  | INT0 |
| 14 | P3.3 | ADC11 |  | SDA (3) | MISO (3) | ✓ | INT1 |
| 15 | P3.4 | ADC12 |  |  | MOSI (3) | ✓ | T0, CMPO, LED (template example) |
| 16 | P3.5 | ADC13 |  |  |  |  | T1 |
| 17 | P3.6 | ADC14 | U1 RX (1) |  |  |  |  |
| 18 | P3.7 |  | U1 TX (1) |  |  |  |  |
| 19 | P1.0 | ADC0 | U2 RX (0) |  |  | ✓ |  |
| 20 | P1.1 | ADC1 | U2 TX (0) |  |  | ✓ |  |

Notes:
- The numbers in brackets are the pin mapping options (`UART1_MAP`/`DEBUG_MAP`, `I2C_MAP`, `SPI_MAP`) that are selected in the parameter section of the respective library. Mappings 1 and 2 of I2C and SPI use the ports P2/P4/P7, which do not exist on the STC8H1K08. U1 = UART1, U2 = UART2.
- *debug_serial* uses UART1 on P3.1 (map 0) with 115200 baud by default.
- The SPI SS pin is not used by the library (software controlled).
- PWM is available on P1.0 - P1.7, P3.3 and P3.4 (`PWM_start()` in *gpio.h*). ADC inputs are P1.0, P1.1 and P3.0 - P3.6.
- The *uart_tx_soft* and *neo* libraries can use any pin.

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
| uart | Basic UART1 and UART2 functions (polling) |
| uart1_int | UART1 with RX interrupt and ring buffer |
| uart_tx_soft | Software UART (TX only) on any pin |

Since SDCC requires interrupt service routines to be visible in the file containing `main()`, the ISR of interrupt-driven libraries (e.g. *uart1_int*) must be defined in *main.c* (see the respective header file).

## Compiling Firmware
To use the *makefile*, some additional software components must be installed to set up the toolchain.

### Install SDCC Toolchain for Compiling
```
sudo apt install build-essential sdcc
```

### Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install stc8isp
```

### Compiling/Uploading
Edit the *makefile* according to the needs of the project (e.g. `F_CPU`, default 36MHz). The internal oscillator is trimmed to `F_CPU` by the programming tool during upload. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile, build and keep all files
make hex       compile and build <firmware>.hex
make bin       compile and build <firmware>.bin
make flash     compile, build and upload to MCU
make clean     remove all build files
```

## Uploading Firmware
### Via Factory built-in UART Bootloader
The MCU has a factory built-in bootloader with UART interface on P3.0 (RXD) and P3.1 (TXD), which is activated after every power-on reset. Connect a USB-to-serial converter to the MCU as follows:

```
USB2SERIAL         STC8G/8H
+--------+         +------+
|     VCC| --/ --> |VCC   |    interruptible (for power cycle)
|     RXD| --|R|-- |P3.1  |    resistor (100R - 1000R)
|     TXD| --|<|-- |P3.0  |    diode (e.g. 1N5819)
|     GND| ------- |GND   |    common ground
+--------+         +------+
```

The resistor and the diode prevent the MCU from being powered via the data lines, so that the power cycle works reliably.

- **stc8isp**: Use the platform-independent open-source command-line tool [stc8isp](https://pypi.org/project/stc8isp/) (`pip install stc8isp`). Run the following command and perform a power cycle of the MCU (reconnect to power) when prompted. The `-t` option trims the internal oscillator to the given frequency, which should match `F_CPU`.
  ```
  stc8isp -p /dev/ttyUSB0 -t 36000000 -f <firmware>.bin
  ```
- **STC-ISP**: STC offers the free but closed-source software [STC-ISP](https://www.stcmicro.com/rjxz.html) for Windows.

There is currently no web app for STC microcontrollers.

## Notes and Tips
- The template uses `F_CPU` = 36MHz by default. The internal oscillator is trimmed to this value during upload by stc8isp (`-t` option). If another tool is used, the IRC must be trimmed accordingly.
- After power-on, the I/O pins of the STC8H are in high-impedance input mode (except P3.0/P3.1), not in the quasi-bidirectional mode of the classic 8051. Use the pin functions in *gpio.h* (e.g. `PIN_output()`, `PIN_all_bidirect()`) to set the pin modes.
- Extended registers (XFR, e.g. pin pull-ups, PWM, I2C) can only be accessed when `P_SW2.7` (EAXFR) is set. The example program does this at the beginning of `main()` (`P_SW2 |= 0x80;`), keep this line in your own programs.
- P5.4 can be used as reset pin or as normal I/O pin. This is set in the option bytes during upload or with `RST_pin_enable()`/`RST_pin_disable()` in *system.h*.

## Development Boards and Projects
- [STC8H1K08 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/STC8H1K08_DevBoard) (STC8H1K08-36I-TSSOP20 with integrated USB-to-serial converter)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [stc8isp on PyPI](https://pypi.org/project/stc8isp/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [STC: STC8H Product Information](https://www.stcmicro.com/)
- [STC-ISP Software](https://www.stcmicro.com/rjxz.html)
- [Arduino Core for STC8H](https://github.com/thevien257/STC_Arduino_Core)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
