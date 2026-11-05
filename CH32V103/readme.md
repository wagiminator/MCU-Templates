# MCU Template for CH32V103 Microcontrollers
## Overview
The CH32V103 is a general-purpose 32-bit RISC-V microcontroller from WCH. It is based on the QingKe RISC-V3A core (RV32IMAC instruction set) and runs at up to 80MHz. It has up to 64KB flash, 20KB SRAM and operates at a supply voltage of 2.7V to 5.5V. The chip has the following peripherals:
- USB 2.0 full-speed host/device controller (USBHD) with integrated transceiver
- 12-bit ADC and touch key detection with up to 16 channels
- advanced-control and general-purpose timers
- up to 3 USARTs, 2 I2C and 2 SPI interfaces
- DMA controller, RTC and clock security system

It has a factory built-in bootloader with USB and UART interface.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or closed-source IDE. It contains:
- a makefile and a PlatformIO configuration
- linker scripts
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

Optional libraries for the internal peripherals including USB are also included.

## Supported Microcontrollers

| Part | Package | Flash | SRAM | GPIO | ADC/TKey Channels | USART | I2C | SPI | Linker Script |
|:-|:-|-:|-:|-:|-:|-:|-:|-:|:-|
| CH32V103C6T6 | LQFP48 | 32KB | 10KB | 37 | 10 | 2 | 1 | 1 | ld/ch32v103x6.ld |
| CH32V103C8T6 | LQFP48 | 64KB | 20KB | 37 | 10 | 3 | 2 | 2 | ld/ch32v103x8.ld |
| CH32V103C8U6 | QFN48 | 64KB | 20KB | 37 | 10 | 3 | 2 | 2 | ld/ch32v103x8.ld |
| CH32V103R8T6 | LQFP64M | 64KB | 20KB | 51 | 16 | 3 | 2 | 2 | ld/ch32v103x8.ld |

Common features of all variants:
- QingKe RISC-V3A core, RV32IMAC instruction set, up to 80MHz
- Internal 8MHz RC oscillator (HSI), internal 40kHz RC oscillator (LSI), external 3MHz - 25MHz oscillator (HSE), external 32.768kHz oscillator (LSE), PLL
- USB 2.0 full-speed host/device controller with integrated PHY (PA11 = D-, PA12 = D+)
- 12-bit ADC (1µs), touch key detection, advanced-control timer (TIM1), general-purpose timers (TIM2, TIM3, TIM4), watchdogs, RTC
- 7-channel DMA controller, CRC calculation unit, 96-bit unique ID
- 2-wire serial debug interface (SWDIO on PA13, SWCLK on PA14)
- Supply voltage 2.7V - 5.5V, 5V-tolerant I/O pins

## Pinout and Peripheral Mapping
The CH32V103 has an STM32F103-compatible pinout. The following table shows the pins that are used by the template and its libraries. The numbers in brackets are the mapping options that are selected in the parameter section of the respective library.

| Function | Pins | Library / Note |
|:-|:-|:-|
| ADC channels 0 - 7 | PA0 - PA7 | gpio.h |
| ADC channels 8, 9 | PB0, PB1 | gpio.h |
| ADC channels 10 - 15 | PC0 - PC5 | gpio.h (CH32V103R8 only) |
| USART1 TX/RX | PA9/PA10 (0), PB6/PB7 (1) | uart.h |
| USART2 TX/RX | PA2/PA3 (0) | uart.h, uart2_dma.h, debug_serial.h (TX: PA2) |
| USART3 TX/RX | PB10/PB11 (0), PC10/PC11 (1, R8 only) | uart.h (not available on CH32V103C6) |
| I2C SDA/SCL | PB7/PB6 (0), PB9/PB8 (1), PB11/PB10 (2, I2C2) | i2c.h |
| SPI SCK/MISO/MOSI | PA5/PA6/PA7 (0), PB3/PB4/PB5 (1), PB13/PB14/PB15 (2, SPI2) | spi.h, spi_tx.h |
| NeoPixel data (SPI MOSI) | PA7 (0), PB5 (1), PB15 (2) | neo_spi.h |
| USB D-/D+ | PA11/PA12 | usb libraries |
| SWDIO/SWCLK | PA13/PA14 | programming and debugging |
| BOOT0, BOOT1 | BOOT0 pin, PB2 | bootloader selection |
| HSE crystal | PD0 (OSC_IN), PD1 (OSC_OUT) | system.h |

Notes:
- External pull-up resistors (4k7 - 10k) are mandatory for I2C.
- Using remapped pins of SPI1 (PB3/PB4) or other JTAG-related pins may require the debug interface to be (partially) disabled.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC RISC-V toolchain, chprog/rvprog) |
| template/platformio.ini | Configuration file for PlatformIO |
| template/ld/ | Linker scripts (ch32v103x6.ld for 32KB, ch32v103x8.ld for 64KB flash) |
| template/src/main.c | Example program (blinking LED on PC13) |
| template/src/system.c/h | Startup code, clock, power, watchdog, sleep and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt and ADC functions |
| template/src/ch32v103.h | Register definitions |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheet, reference manual and QingKe V3 core manual |

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. The parameters (e.g. pin mapping, baud rate, clock rate) can be adjusted in the parameter section at the beginning of the respective header file.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) via USART2 (PA2) with simple printf |
| i2c | I2C master functions (polling) |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| millis | Millisecond counter using the SysTick timer |
| neo_spi | NeoPixel (WS2812) control using hardware SPI (MOSI pin) |
| neo_sw | NeoPixel (WS2812) control using software bit-banging on any pin |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| spi | SPI master functions |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions (TX only) |
| uart | Basic USART1/2/3 functions (no buffer, no interrupt, no DMA) |
| uart2_dma | USART2 with DMA-driven RX ring buffer |
| usb/usb_cdc | USB CDC (virtual serial port) |
| usb/hid_keyboard | USB HID keyboard |
| usb/hid_mouse | USB HID mouse |
| usb/hid_consumer | USB HID consumer control (multimedia keys) |
| usb/hid_composite | USB HID composite device (keyboard, consumer and mouse) |

### Using the USB Libraries
Each USB library consists of the following files:
- the class driver (e.g. *usb_cdc.c/h*)
- the USB descriptors (*usb_descr.c/h*)
- the device configuration (*config.h*)
- the USB handler (*usb_handler.c/h*)
- the USB definitions (*usb.h*)

To use a library, copy all of its files into the *src* folder. Adjust the *config.h* (e.g. vendor and product strings, VID/PID) according to your needs. The library is started with its init function (e.g. `CDC_init()`). The default VID/PID pairs are shared IDs that are only suitable for private projects.
Additionally:
- The interrupt vector table must be enabled (`SYS_USE_VECTORS` = 1 in *system.h*, default).
- The system clock must be 48MHz or 72MHz (`F_CPU`).

## Compiling Firmware
### Using the Makefile
To use the *makefile*, some additional software components must be installed to set up the toolchain.

#### Install GCC Toolchain for Compiling
```
sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
```

#### Install Python3 and Programming Tools
```
sudo apt install python3 python3-pip
pip install chprog rvprog
```

#### Give Access Permission to the Factory built-in USB Bootloader
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-ch55x.rules
sudo udevadm control --reload-rules
```

#### Give Access Permission to the WCH-LinkE Programmer
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8010", MODE="666"' | sudo tee /etc/udev/rules.d/99-WCH-LinkE.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8012", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-WCH-LinkE.rules
sudo udevadm control --reload-rules
```

#### Compiling/Uploading
Edit the *makefile* according to the needs of the project (e.g. `F_CPU`, `LDSCRIPT`). By default, `make flash` uploads the firmware via the USB bootloader using *chprog*. To use the WCH-LinkE instead, change `ISPTOOL` to `rvprog -f $(BIN)/$(TARGET).bin`. Use one of the following commands inside the *template* folder to compile/upload:

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
### Via Factory built-in USB Bootloader
#### Installing Drivers for the Bootloader
On Linux you do not need to install a driver, but you need to grant access permissions (see above). On Windows, you need the [CH372 driver](http://www.wch-ic.com/downloads/CH372DRV_EXE.html). Alternatively, you can use the [Zadig](https://zadig.akeo.ie/) tool: click "Options" -> "List All Devices", select the USB module and install the libusb-win32 driver. To do this, the board must be connected and the microcontroller must be in bootloader mode.

#### Entering Bootloader Mode
Disconnect the board from the USB port and all voltage sources. Pull BOOT0 high (e.g. press and hold the BOOT button) and keep BOOT1 (PB2) low while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, BOOT0 can be released and new firmware can be uploaded via USB within the next couple of seconds. Alternatively, leave the board connected, hold BOOT0 high and press and release the RESET button.

#### Uploading Firmware Binary
- **chprog**: If [Python3](https://www.pythontutorial.net/getting-started/install-python/) is installed, use the platform-independent open-source command-line tool [chprog](https://pypi.org/project/chprog/) (`pip install chprog`):
  ```
  chprog <firmware>.bin
  ```
- **Web app**: Use the [WCH USB Programmer web app](https://wagiminator.github.io/apps/chprog/). It only works in Chrome, Edge or Opera on a desktop computer. On Linux, the udev rules mentioned above are required. On Windows, the web app needs the WinUSB driver (install it with [Zadig](https://zadig.akeo.ie/)); the libusb-win32 and CH372 drivers do not work in browsers.
- **WCHISPTool**: WCH offers the free but closed-source software [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) for Windows. Open the *<firmware>.hex* file and upload it to the microcontroller.

### Via 2-Wire Serial Debug Interface
#### WCH-LinkE Programming and Debugging Device
To program the microcontroller via its serial debug interface (SWD), you need the [WCH-LinkE](http://www.wch-ic.com/products/WCH-Link.html) (pay attention to the "E" in the name), which can be purchased commercially. This debugging tool is compatible with most WCH RISC-V and ARM-based microcontrollers. On Linux, grant access permissions beforehand (see above). On Windows, if necessary, you can install the WinUSB driver for WCH interface 1 using the [Zadig](https://zadig.akeo.ie/) tool.

Make the following connections between the WCH-LinkE and the MCU board:

```
WCH-LinkE      MCU Board
+-------+      +-------+
|  SWCLK| ---> |SWCLK  |
|  SWDIO| <--> |SWDIO  |
|    GND| ---> |GND    |
|    3V3| ---> |VDD    |
+-------+      +-------+
```

If the blue LED on the WCH-LinkE stays on after it is plugged into the USB port, the device is in ARM mode. It must first be switched to RISC-V mode in one of these ways:
- Use the Python command-line tool [rvprog](https://pypi.org/project/rvprog/) with the *-v* option.
- Select "WCH-LinkRV" in the software provided by WCH, such as MounRiver Studio or WCH-LinkUtility.
- Hold down the ModeS button on the device while plugging it into the USB port.

More information can be found in the [WCH-Link User Manual](http://www.wch-ic.com/downloads/WCH-LinkUserManual_PDF.html).

#### Uploading Firmware Binary
- **rvprog**: Use the platform-independent open-source command-line tool [rvprog](https://pypi.org/project/rvprog/) (`pip install rvprog`):
  ```
  rvprog -f <firmware>.bin
  ```
- **Web app**: Use the [WCH RISC-V Programmer web app](https://wagiminator.github.io/apps/rvprog/). It only works in Chrome, Edge or Opera on a desktop computer.
- **WCH-LinkUtility**: WCH offers the free but closed-source software [WCH-LinkUtility](https://www.wch.cn/downloads/WCH-LinkUtility_ZIP.html) for Windows. Select the "WCH-LinkRV" mode, open the *<firmware>.hex* file and upload it.
- **minichlink**: The open-source tool [minichlink](https://github.com/cnlohr/ch32v003fun/tree/master/minichlink) by Charles Lohr (CNLohr) can be used with Windows, Linux and Mac.

## Notes and Tips
- The template uses `F_CPU` = 72MHz by default (HSI with PLL). The maximum clock frequency of the CH32V103 is 80MHz, but USB requires 48MHz or 72MHz.
- The ADC of the CH32V103 has an offset that should be compensated by calibration (`ADC_calibrate()`, see notes in *gpio.h*).
- The factory built-in bootloader also supports uploads via USART1 (PA9/PA10), e.g. with WCHISPTool.

## Development Boards and Projects
There is currently no dedicated development board for the CH32V103. Generic boards ("CH32V103C8T6 Blue Pill" style) and the official WCH evaluation board can be used.

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [chprog on PyPI](https://pypi.org/project/chprog/)
- [rvprog on PyPI](https://pypi.org/project/rvprog/)
- [WCH USB Programmer Web App](https://wagiminator.github.io/apps/chprog/)
- [WCH RISC-V Programmer Web App](https://wagiminator.github.io/apps/rvprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [WCH: CH32V103 Product Page](https://www.wch-ic.com/products/CH32V103.html)
- [WCH: CH32V103 Datasheet](https://www.wch-ic.com/downloads/CH32V103DS0_PDF.html)
- [WCH: CH32V103 Reference Manual](https://www.wch-ic.com/downloads/CH32xRM_PDF.html)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
