# MCU Template for CH32V002, V004, V005, V006, V007 and M007 Microcontrollers
## Overview
The CH32V00X series are industrial-grade, low-cost 32-bit RISC-V microcontrollers from WCH. They use the QingKe RISC-V2C core with the RV32EmC instruction set (RV32EC plus the multiplication subset of the M extension), support two levels of interrupt nesting and run at up to 48MHz. They are the successors of the CH32V003 and offer more memory than it, a fast 12-bit ADC and, depending on the model, additional peripherals:
- second USART
- operational amplifier
- comparators
- touch key
- integrated gate driver for motors (CH32M007)

The CH32V002 is pin- and function-compatible with the CH32V003.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or closed-source IDE. It contains:
- a makefile and a PlatformIO configuration
- linker scripts for all variants
- a startup and system configuration
- basic GPIO, ADC and touch key functions
- an example program

A collection of optional libraries for the internal peripherals is also included.

## Supported Microcontrollers

| Series | Flash | SRAM | Linker Script | Packages (examples) | Peripherals (in addition to CH32V002) |
|:-|-:|-:|:-|:-|:-|
| CH32V002 | 16KB | 4KB | ld/ch32v002.ld | F4P6 (TSSOP20), F4U6 (QFN20), A4M6 (SOP16), J4M6 (SOP8), D4U6 (QFN12) | - |
| CH32V004 | 32KB | 6KB | ld/ch32v004.ld | F6P1 (TSSOP20), see datasheet | - |
| CH32V005 | 32KB | 6KB | ld/ch32v005.ld | QFN12, QFN20, QSOP24 | USART2, OPA, GPIO port B |
| CH32V006 | 62KB | 8KB | ld/ch32v006.ld | TSSOP20, QFN20, QSOP24, QFN32 | USART2, OPA, TIM3, touch key, GPIO port B |
| CH32V007 | 62KB | 8KB | ld/ch32v007.ld | see datasheet | OPA, 2x CMP, TIM3, touch key, GPIO port B |
| CH32M007 | 62KB | 8KB | ld/ch32m007.ld | G8R6 (QSOP28), E8U6, E8R6 | like CH32V007, plus 3-phase gate driver and high-voltage supply |

Common features (CH32V002 as reference, see the datasheet of the respective chip for details):
- QingKe RISC-V2C core, RV32EmC instruction set, 48MHz maximum clock frequency
- 3328 bytes bootloader area, 256 bytes non-volatile system configuration area, 256 bytes user-defined area (CH32V002)
- Internal 24MHz RC oscillator (HSI), internal 128kHz RC oscillator (LSI), external 3MHz - 25MHz oscillator (HSE)
- 12-bit ADC with up to 3Msps
- 16-bit advanced-control timer (TIM1), 16-bit general-purpose timer (TIM2), independent and window watchdog, SysTick
- USART, I2C, SPI, 7-channel DMA controller
- 1-wire serial debug interface (SDI) on PD1
- Supply voltage 2.0V - 5.5V (CH32M007: separate high-voltage supply, see datasheet)

## Pinout and Peripheral Mapping
### CH32V002F4P6 (TSSOP20)
The pinout of the CH32V002F4P6 is identical to the CH32V003F4P6.

| Pin | Port | ADC | USART1 (UART_MAP) | I2C (I2C_MAP) | SPI (SPI_MAP) | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | PD4 | IN7 |  |  | SCK (4) |  |
| 2 | PD5 | IN5 | TX (0), RX (1) |  | MISO (4) |  |
| 3 | PD6 | IN6 | TX (1), RX (0) |  | MOSI (4) |  |
| 4 | PD7 |  |  |  |  | NRST (if enabled) |
| 5 | PA1 | IN1 | RX (8) |  | SCK (5) | XI (HSE) |
| 6 | PA2 | IN0 |  |  | MOSI (5) | XO (HSE) |
| 7 | VSS |  |  |  |  | Ground |
| 8 | PD0 |  | TX (2) | SDA (1) |  |  |
| 9 | VDD |  |  |  |  | Supply voltage |
| 10 | PC0 |  | TX (3) |  | MOSI (3) |  |
| 11 | PC1 |  | RX (3) | SDA (0) |  |  |
| 12 | PC2 |  |  | SCL (0) |  |  |
| 13 | PC3 |  |  |  |  |  |
| 14 | PC4 | IN2 | RX (9) |  |  | MCO |
| 15 | PC5 |  | TX (6) | SCL (2) | SCK (0/1) |  |
| 16 | PC6 |  | RX (6) | SDA (2) | MOSI (0/1) |  |
| 17 | PC7 |  |  |  | MISO (0/1/6) |  |
| 18 | PD1 |  | TX (4), RX (2/5) | SDA (4), SCL (1) |  | SWIO (programming and debugging) |
| 19 | PD2 | IN3 |  |  | SCK (2) |  |
| 20 | PD3 | IN4 |  |  | MOSI (2) |  |

Notes:
- The numbers in brackets are the mapping options that are selected in the parameter section of the corresponding library. **Attention:** for this chip family, the mapping number is written directly to the remap bits in the AFIO_PCFR1 register. The USART1 mapping numbers therefore differ from those of the CH32V003 library.
- Mappings that use pins which do not exist on the CH32V002 (e.g. PA0, PB3, PB5, PB6) are only usable on the larger chips of the series.
- CH32V007 and CH32M007 use PC4 instead of PC6 as SDA for `I2C_MAP` 2 (set `I2C_V007` to 1 in the I2C library).
- PD1 is the 1-wire programming and debugging interface (SWIO). PD7 can be used as GPIO or reset pin (option bytes, e.g. *rvprog -G* / *rvprog -R* or *flash.h*).
- External pull-up resistors (4k7 - 10k) are mandatory for I2C.
- Additional mapping options (e.g. for the timer-based rotary encoder) can be found in the header of the respective library.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC RISC-V toolchain, rvprog); select the chip with `MCU` |
| template/platformio.ini | Configuration file for PlatformIO |
| template/ld/ | Linker scripts for CH32V002, V004, V005, V006, V007 and M007 |
| template/src/main.c | Example program (blinking LED on PC0) |
| template/src/system.c/h | Startup code, clock, power, watchdog, sleep and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt, ADC, OPA and touch key functions |
| template/src/ch32v00x.h | Register definitions for the CH32V00X series |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheet and reference manual |

The target chip is selected in the *makefile* with the variable `MCU` (ch32v002, ch32v004, ch32v005, ch32v006, ch32v007, ch32m007). It can also be set on the command line, e.g. `make flash MCU=ch32v006`.

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. The parameters (e.g. pin mapping, baud rate, clock rate) can be adjusted in the parameter section at the beginning of the respective header file.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) via USART1 with simple printf |
| encoder_tim | Rotary encoder evaluation using the encoder mode of TIM1 or TIM2 |
| flash | Read, write and erase the flash memory, user option bytes, reset pin configuration |
| i2c | I2C master functions (polling) |
| i2c_dma | I2C master functions with DMA for transmitting |
| i2c_slave | I2C slave functions (interrupt-driven, register-based) |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| i2c_tx | I2C master functions (write only, e.g. for OLEDs) |
| millis | Millisecond counter using the SysTick timer |
| neo_dma | NeoPixel (WS2812) control using hardware SPI and DMA |
| neo_spi | NeoPixel (WS2812) control using hardware SPI |
| spi | SPI master functions |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions (TX only) |
| uart | Basic USART1 functions (no buffer, no interrupt, no DMA) |
| v-usb/hid_keyboard | Software USB HID keyboard (low-speed USB via bit-banging) |

The V-USB library is based on [rv003usb](https://github.com/cnlohr/rv003usb) by CNLohr. Copy all files of the library (including the assembler file *usb_handler.S*) into the *src* folder and set the USB pins in *config.h* (default: D+ on PA1, D- on PA2). D- requires a 1.5k pull-up resistor, the supply voltage must be 3.3V and the system clock 48MHz.

## Compiling Firmware
### Using the Makefile
To use the *makefile*, some additional software components must be installed to set up the toolchain.

#### Install GCC Toolchain for Compiling
```
sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
```

#### Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install rvprog
```

#### Give Access Permission to the WCH-LinkE Programmer
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8010", MODE="666"' | sudo tee /etc/udev/rules.d/99-WCH-LinkE.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8012", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-WCH-LinkE.rules
sudo udevadm control --reload-rules
```

#### Compiling/Uploading
Edit the *makefile* according to the needs of the project (e.g. `MCU`, `F_CPU`). Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make clean     remove all build files
```

### Using PlatformIO
- Install [PlatformIO](https://platformio.org) and [platform-ch32v](https://github.com/Community-PIO-CH32V/platform-ch32v). Follow [these instructions](https://pio-ch32v.readthedocs.io/en/latest/installation.html) to do so. Linux/Mac users may also need to install [pyenv](https://realpython.com/intro-to-pyenv).
- Click on "Open Project" and select the *template* folder with the *platformio.ini* file.
- Edit the *platformio.ini* according to the needs of the project and your programmer (board and linker script, default: CH32V002F4P6).
- Connect the WCH-LinkE to the target board, then click "Upload".

## Uploading Firmware
### WCH-LinkE Programming and Debugging Device
The CH32V00X chips are programmed via their 1-wire serial debug interface (SDI). For this you need the [WCH-LinkE](http://www.wch-ic.com/products/WCH-Link.html) (pay attention to the "E" in the name), which can be purchased commercially. This debugging tool is compatible with most WCH RISC-V and ARM-based microcontrollers.

On Linux, grant access permissions beforehand as described above. On Windows, if necessary, you can install the WinUSB driver for WCH interface 1 using the [Zadig](https://zadig.akeo.ie/) tool.

Make the following connections between the WCH-LinkE and the MCU board:

```
WCH-LinkE      MCU Board
+-------+      +-------+
|  SWDIO| <--> |PD1    |
|    GND| ---> |GND    |
|    3V3| ---> |VDD    |
+-------+      +-------+
```

If the blue LED on the WCH-LinkE stays on after it is plugged into the USB port, the device is in ARM mode. It must first be switched to RISC-V mode in one of these ways:
- Use the Python command-line tool [rvprog](https://pypi.org/project/rvprog/) with the *-v* option.
- Select "WCH-LinkRV" in the software provided by WCH, such as MounRiver Studio or WCH-LinkUtility.
- Hold down the ModeS button on the device while plugging it into the USB port.

More information can be found in the [WCH-Link User Manual](http://www.wch-ic.com/downloads/WCH-LinkUserManual_PDF.html).

### Uploading Firmware Binary
There are several ways to upload the firmware:
- **rvprog**: If [Python3](https://www.pythontutorial.net/getting-started/install-python/) is installed, use the platform-independent open-source command-line tool [rvprog](https://pypi.org/project/rvprog/) (`pip install rvprog`):
  ```
  rvprog -f <firmware>.bin
  ```
- **Web app**: Use the [WCH RISC-V Programmer web app](https://wagiminator.github.io/apps/rvprog/). It only works in Chrome, Edge or Opera on a desktop computer. On Linux, the udev rules mentioned above are required, on Windows the WinUSB driver for the WCH-LinkE (see above).
- **WCH-LinkUtility**: WCH offers the free but closed-source software [WCH-LinkUtility](https://www.wch.cn/downloads/WCH-LinkUtility_ZIP.html) for Windows. Select the "WCH-LinkRV" mode, open the *<firmware>.hex* file and upload it to the microcontroller.
- **minichlink**: The open-source tool [minichlink](https://github.com/cnlohr/ch32v003fun/tree/master/minichlink) by Charles Lohr (CNLohr) can be used with Windows, Linux and Mac.

## Notes and Tips
- The CH32V002 can be used on boards designed for the CH32V003 in the same package. Firmware has to be compiled with this template, since the peripherals (e.g. ADC, remapping) differ.
- The touch key functions in *gpio.h* are available for CH32V006, V007 and M007 only.
- On CH32V007 and CH32M007, PD7 is a normal GPIO by default.
- If the chip is no longer accessible via SWIO, it can usually be recovered with `rvprog -b` (unbrick). A read-protected chip can be unlocked with `rvprog -u`.
- Not every peripheral is available on every package. Check the pinout of your chip in the datasheet.

## Development Boards and Projects
There is no dedicated development board for this series yet. The CH32V002 in TSSOP20, SOP16 or SOP8 package can be used on the CH32V003 development boards:
- [CH32V003 F4P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH32V003F4P6_DevBoard)
- [CH32V003 F4P6 Development Board V-USB](https://github.com/wagiminator/Development-Boards/tree/main/CH32V003F4P6_DevBoard_VUSB)
- [CH32V003 A4M6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH32V003A4M6_DevBoard)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [rvprog on PyPI](https://pypi.org/project/rvprog/)
- [WCH RISC-V Programmer Web App](https://wagiminator.github.io/apps/rvprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [MCU Template for CH32V003](https://github.com/wagiminator/MCU-Templates/tree/main/CH32V003)
- [WCH: CH32V002 Product Page](https://www.wch-ic.com/products/CH32V002.html)
- [WCH: CH32V006 Product Page](https://www.wch-ic.com/products/CH32V006.html)
- [CNLohr: rv003usb](https://github.com/cnlohr/rv003usb)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
