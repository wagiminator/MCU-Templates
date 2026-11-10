# MCU Template for CH32V003 Microcontrollers
## Overview
The CH32V003 is an industrial-grade, ultra-low-cost 32-bit RISC-V microcontroller from WCH. It is based on the QingKe RISC-V2A core (RV32EC instruction set) and runs at up to 48MHz. It has 16KB flash, 2KB SRAM and a supply voltage range of 2.7V to 5.5V. The chip has a 1-wire serial debug interface (SDI) and comes in very small packages. Its peripherals are:
- 7-channel DMA controller
- 10-bit ADC
- operational amplifier/comparator
- 16-bit advanced-control timer and 16-bit general-purpose timer
- USART, I2C and SPI

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or closed-source IDE. It contains:
- a makefile and a PlatformIO configuration
- a linker script
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

A collection of optional libraries for the internal peripherals is also included.

## Supported Microcontrollers

| Part | Package | Flash | SRAM | GPIO | ADC Channels | Peripherals |
|:-|:-|-:|-:|-:|-:|:-|
| CH32V003F4P6 | TSSOP20 | 16KB | 2KB | 18 | 8 | USART, I2C, SPI, OPA, TIM1, TIM2, DMA |
| CH32V003F4U6 | QFN20 | 16KB | 2KB | 18 | 8 | USART, I2C, SPI, OPA, TIM1, TIM2, DMA |
| CH32V003A4M6 | SOP16 | 16KB | 2KB | 14 | 6 | USART, I2C, SPI, OPA, TIM1, TIM2, DMA |
| CH32V003J4M6 | SOP8 | 16KB | 2KB | 6 | 6 | USART, I2C, OPA, TIM1, TIM2, DMA |

Common features of all variants:
- QingKe RISC-V2A core with RV32EC instruction set, 48MHz maximum clock frequency
- 1920 bytes bootloader area, 64 bytes system non-volatile configuration area, 64 bytes user-defined area
- Internal 24MHz RC oscillator (HSI) with PLL, internal 128kHz RC oscillator (LSI), external 4MHz - 25MHz oscillator (HSE)
- 10-bit ADC with up to 8 external channels, operational amplifier (OPA) that can also be used as a comparator
- 16-bit advanced-control timer (TIM1), 16-bit general-purpose timer (TIM2), independent and window watchdog, 32-bit SysTick
- 1x USART, 1x I2C, 1x SPI, 7-channel DMA controller
- 1-wire serial debug interface (SDI) on PD1
- Supply voltage 2.7V - 5.5V

## Pinout and Peripheral Mapping
### CH32V003F4P6 (TSSOP20)

| Pin | Port | ADC | USART1 (UART_MAP) | I2C (I2C_MAP) | SPI | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | PD4 | A7 | | | | UCK, T2CH1, OPO (OPA output) |
| 2 | PD5 | A5 | TX (0), RX (2) | | | T2CH4 |
| 3 | PD6 | A6 | RX (0), TX (2) | | | T2CH3 |
| 4 | PD7 | | | | | NRST (if enabled), T2CH4 |
| 5 | PA1 | A1 | | | | OSCI (HSE), T1CH2 |
| 6 | PA2 | A0 | | | | OSCO (HSE), T1CH2N |
| 7 | VSS | | | | | Ground |
| 8 | PD0 | | TX (1) | SDA (1) | | T1CH1N |
| 9 | VDD | | | | | Supply voltage 2.7V - 5.5V |
| 10 | PC0 | | TX (3) | | | T2CH3 |
| 11 | PC1 | | RX (3) | SDA (0) | NSS | |
| 12 | PC2 | | | SCL (0) | | T1BKIN |
| 13 | PC3 | | | | | T1CH3 |
| 14 | PC4 | A2 | | | | T1CH4, MCO |
| 15 | PC5 | | | SCL (2) | SCK | T1ETR |
| 16 | PC6 | | | SDA (2) | MOSI | NeoPixel data (neo_spi, neo_dma) |
| 17 | PC7 | | | | MISO | |
| 18 | PD1 | | RX (1) | SCL (1) | | SWIO (programming and debugging) |
| 19 | PD2 | A3 | | | | T1CH1 |
| 20 | PD3 | A4 | | | | T2CH2 |

Notes:
- The numbers in brackets are the mapping options that are selected in the parameter section of the corresponding library (e.g. `UART_MAP`, `I2C_MAP`).
- PD1 is the 1-wire programming and debugging interface (SWIO). Using PD1 for other purposes can make it difficult to re-flash the chip; in this case the chip can be recovered with *rvprog -b* (unbrick by power-cycle erase).
- PD7 is configured as a normal GPIO by default. It can be configured as a reset pin via the option bytes, e.g. with *rvprog -R* (back to GPIO with *rvprog -G*) or with the *flash.h* library.
- The internal pull-up resistors are not strong enough for I2C, so external pull-up resistors (4k7 - 10k) are mandatory.
- Additional timer channel mappings for the rotary encoder library (*encoder_tim.h*) can be found in the header of the library.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC RISC-V toolchain, rvprog) |
| template/platformio.ini | Configuration file for PlatformIO |
| template/ld/ch32v003.ld | Linker script |
| template/src/main.c | Example program (blinking LED on PC0) |
| template/src/system.c/h | Startup code, clock, power, watchdog, sleep and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt, ADC and OPA functions |
| template/src/ch32v003.h | Register definitions for the CH32V003 |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheet, reference manual and QingKe V2 core manuals |

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. The parameters (e.g. pin mapping, baud rate, clock rate) can be adjusted in the parameter section at the beginning of the respective header file.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) via USART1 with simple printf |
| encoder_tim | Rotary encoder evaluation using the encoder mode of TIM1 or TIM2 |
| flash | Read, write and erase the flash memory (e.g. for storing data), user option bytes, reset pin configuration |
| i2c | I2C master functions (polling) |
| i2c_dma | I2C master functions with DMA for transmitting |
| i2c_slave | I2C slave functions (interrupt-driven, register-based) |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| i2c_tx | I2C master functions (write only, e.g. for OLEDs) |
| millis | Millisecond counter using the SysTick timer |
| neo_dma | NeoPixel (WS2812) control using hardware SPI and DMA (MOSI pin PC6) |
| neo_spi | NeoPixel (WS2812) control using hardware SPI (MOSI pin PC6) |
| neo_sw | NeoPixel (WS2812) control using software bit-banging on any pin |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| pwm | PWM outputs on 4 channels of TIM2 (PD4, PD3, PC0, PD7) |
| spi | SPI master functions |
| spi_dma | SPI master functions with DMA (background transfers of buffers) |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions (TX only) |
| timer | Periodic timer interrupt (TIM1, microsecond resolution) |
| uart | Basic USART1 functions (no buffer, no interrupt, no DMA) |
| uart_dma | USART1 with DMA-driven RX ring buffer |
| uart_dma_rx | USART1 receive only with DMA-driven RX ring buffer |
| uart_rx | Basic USART1 functions (RX only) |
| v-usb/hid_keyboard | Software USB HID keyboard (low-speed USB via bit-banging) |
| v-usb/hid_mouse | Software USB HID mouse |
| v-usb/hid_consumer | Software USB HID consumer control (multimedia keys) |
| v-usb/hid_joystick | Software USB HID joystick |
| v-usb/hid_composite | Software USB HID composite device (keyboard, consumer and mouse) |

All I2C, SPI and UART libraries provide functions for buffers with the same API (e.g. `I2C_sendBuffer(addr,buf,len)`, `I2C_getBuffer(addr,buf,len)`, `I2C_writeBuffer(buf,len)`, `I2C_readBuffer(buf,len)`, `SPI_writeBuffer(buf,len)`, `SPI_readBuffer(buf,len)`, `UART_writeBuffer(buf,len)`, `UART_readBuffer(buf,len)`). The *_dma* and *_int* variants transfer write buffers in the background; in this case the buffer must not be changed until the respective `..._busy()` function returns 0.

### Notes on the V-USB Libraries
The CH32V003 has no USB peripheral. The libraries in the *v-usb* folder implement a low-speed USB device in software. They are based on [rv003usb](https://github.com/cnlohr/rv003usb) by CNLohr.
- Copy all files of a library (including the assembler file *usb_handler.S*) into the *src* folder.
- Set the USB pins in *config.h*. The default is D+ on PA1 and D- on PA2.
- D- needs a 1.5k pull-up resistor (either to 3V3 or to the GPIO defined by `USB_PIN_DPU`).
- The USB signals require 3.3V levels, so the supply voltage of the MCU must be 3.3V.
- The system clock must be 48MHz.
- A suitable development board is the [CH32V003F4P6 DevBoard V-USB](https://github.com/wagiminator/Development-Boards/tree/main/CH32V003F4P6_DevBoard_VUSB).

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
Edit the *makefile* according to the needs of the project (e.g. `F_CPU`). Use one of the following commands inside the *template* folder to compile/upload:

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
- Edit the *platformio.ini* according to the needs of the project and your programmer.
- Connect the WCH-LinkE to the target board, then click "Upload".

## Uploading Firmware
### WCH-LinkE Programming and Debugging Device
The CH32V003 has no built-in USB bootloader. It is programmed via its 1-wire serial debug interface (SDI). For this you need the [WCH-LinkE](http://www.wch-ic.com/products/WCH-Link.html) (pay attention to the "E" in the name), which can be purchased commercially. This debugging tool is compatible with most WCH RISC-V and ARM-based microcontrollers.

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
- After a reset, the CH32V003 runs from the internal 24MHz RC oscillator divided by 3 (8MHz). The startup code of the template configures the clock defined by `F_CPU` in the makefile (default: 48MHz using HSI and PLL).
- The ADC uses VDD as its reference voltage. The internal reference voltage (Vref, ~1.2V) can be measured to calculate the supply voltage.
- If the chip is no longer accessible via SWIO (e.g. because PD1 was reconfigured or the chip is in a deep sleep mode), it can usually be recovered with `rvprog -b` (unbrick). A read-protected chip can be unlocked with `rvprog -u`.
- Not every peripheral is available on every package. Check the pinout of your package in the datasheet.

## Development Boards and Projects
### Development Boards
- [CH32V003 F4P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH32V003F4P6_DevBoard)
- [CH32V003 F4P6 Development Board V-USB](https://github.com/wagiminator/Development-Boards/tree/main/CH32V003F4P6_DevBoard_VUSB)
- [CH32V003 A4M6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH32V003A4M6_DevBoard)
- [Development Board Adapter](https://github.com/wagiminator/Development-Boards/tree/main/DevBoard_Adapter)

### Projects
- [CH32V003 RISC-V Mini Game Console](https://github.com/wagiminator/CH32V003-GameConsole) (games can be flashed with the [Game Console Flasher web app](https://wagiminator.github.io/apps/gameconsole/))
- [CH32V003 FM Radio Receiver with RDS](https://github.com/wagiminator/CH32V003-FM-Receiver)
- [CH32V003 Stereo FM Transmitter](https://github.com/wagiminator/CH32V003-FM-Transmitter)
- [TinyRemote - IR Remote Control](https://github.com/wagiminator/CH32V003-IR-Remote)
- [IR2USB - USB IR Remote Receiver](https://github.com/wagiminator/CH32V003-USB-IR-Receiver)
- [Rotary Encoder with I²C Interface](https://github.com/wagiminator/CH32V003-I2C-Knob)
- [TinyBling - NeoPixel Pendant](https://github.com/wagiminator/CH32V003-NeoPixel-Pendant)
- [USB Knob - Rotary Encoder with USB Interface](https://github.com/wagiminator/CH32V003-USB-Knob)
- [USB Joystick](https://github.com/wagiminator/CH32V003-USB-Joystick)
- [MacroPad plus](https://github.com/wagiminator/CH32V003-MacroPad-plus)
- [Mouse Wiggler](https://github.com/wagiminator/CH32V003-Mouse-Wiggler)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [rvprog on PyPI](https://pypi.org/project/rvprog/)
- [WCH RISC-V Programmer Web App](https://wagiminator.github.io/apps/rvprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [WCH: CH32V003 Product Page](https://www.wch-ic.com/products/CH32V003.html)
- [WCH: CH32V003 Datasheet](https://www.wch-ic.com/downloads/CH32V003DS0_PDF.html)
- [WCH: CH32V003 Reference Manual](https://www.wch-ic.com/downloads/CH32V003RM_PDF.html)
- [CNLohr: ch32v003fun](https://github.com/cnlohr/ch32v003fun)
- [CNLohr: rv003usb](https://github.com/cnlohr/rv003usb)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
