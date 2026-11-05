# MCU Template for CH32V203 Microcontrollers
## Overview
The CH32V203 is an industrial-grade, low-power, high-performance 32-bit RISC-V microcontroller from WCH. It is based on the QingKe RISC-V4B core (RV32IMAC instruction set) and runs at up to 144MHz with a power consumption as low as 45µA/MHz. It operates at a supply voltage of 2.4V to 3.6V. The chip has the following peripherals:
- two USB 2.0 full-speed controllers: USBD (device) and USBFS (host/device), depending on the package
- CAN interface (2.0B active)
- up to two operational amplifiers
- 12-bit ADC with touch key detection
- up to four USARTs, two I2C and two SPI interfaces
- DMA controller and RTC

It has a factory built-in USB bootloader.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or closed-source IDE. It contains:
- a makefile and a PlatformIO configuration
- linker scripts
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

Optional libraries for the internal peripherals including USB are also included.

## Supported Microcontrollers

| Part | Package | Flash | SRAM | GPIO | ADC Channels | USART | USB | Linker Script |
|:-|:-|-:|-:|-:|-:|-:|:-|:-|
| CH32V203F6P6 | TSSOP20 | 32KB | 10KB | 16 | 9 | 1 | USBD | ld/ch32v203x6.ld |
| CH32V203F8P6 | TSSOP20 | 64KB | 20KB | 17 | 9 | 2 | USBD, USBFS | ld/ch32v203x8.ld |
| CH32V203F8U6 | QFN20 | 64KB | 20KB | 17 | 9 | 2 | USBD, USBFS | ld/ch32v203x8.ld |
| CH32V203G6U6 | QFN28 | 32KB | 10KB | 24 | 10 | 2 | USBD | ld/ch32v203x6.ld |
| CH32V203G8R6 | QSOP28 | 64KB | 20KB | 24 | 10 | 2 | USBD, USBFS | ld/ch32v203x8.ld |
| CH32V203K6T6 | LQFP32 | 32KB | 10KB | 26 | 10 | 2 | USBD | ld/ch32v203x6.ld |
| CH32V203K8T6 | LQFP32 | 64KB | 20KB | 26 | 10 | 2 | USBD | ld/ch32v203x8.ld |
| CH32V203C6T6 | LQFP48 | 32KB | 10KB | 37 | 10 | 2 | USBD | ld/ch32v203x6.ld |
| CH32V203C8T6/U6 | LQFP48/QFN48 | 64KB | 20KB | 37 | 10 | 4 | USBD, USBFS | ld/ch32v203x8.ld |
| CH32V203RBT6 | LQFP64M | 128KB | 64KB | 51 | 16 | 4 | USBD, USBFS | - (not included) |

Common features of all variants:
- QingKe RISC-V4B core, RV32IMAC instruction set, hardware prologue/epilogue, up to 144MHz
- Internal 8MHz RC oscillator (HSI), internal 40kHz RC oscillator (LSI), external 3MHz - 25MHz oscillator (HSE), external 32.768kHz oscillator (LSE), PLL
- 12-bit ADC with touch key detection, up to two operational amplifiers (OPA), RTC
- Advanced-control timer (TIM1), general-purpose timers, watchdogs, 8-channel DMA controller
- 2-wire serial debug interface (SWDIO on PA13, SWCLK on PA14)
- Supply voltage 2.4V - 3.6V, 5V-tolerant I/O pins (FT)

## Pinout and Peripheral Mapping
### CH32V203F6P6 (TSSOP20)

| Pin | Port | ADC | USART2 | SPI1 | USB | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | BOOT0 |  |  |  |  | Bootloader selection (BOOT0) |
| 2 | PD0 |  |  |  |  | OSC_IN (HSE) |
| 3 | PD1 |  |  |  |  | OSC_OUT (HSE) |
| 4 | NRST |  |  |  |  | Reset |
| 5 | VDDA |  |  |  |  | Analog supply |
| 6 | PA0 | IN0 | CTS |  |  | WKUP, TIM2CH1 |
| 7 | PA1 | IN1 | RTS |  |  |  |
| 8 | PA2 | IN2 | TX (0) |  |  | debug_serial TX, OPA2 OUT |
| 9 | PA3 | IN3 | RX (0) |  |  |  |
| 10 | PA4 | IN4 | CK | NSS |  |  |
| 11 | PA5 | IN5 |  | SCK (0) |  |  |
| 12 | PA6 | IN6 |  | MISO (0) |  |  |
| 13 | PA7 | IN7 |  | MOSI (0) |  | NeoPixel data (neo_spi) |
| 14 | PB1 | IN9 |  |  |  |  |
| 15 | VSS |  |  |  |  | Ground |
| 16 | VDD |  |  |  |  | Supply voltage 2.4V - 3.6V |
| 17 | PA11 |  |  |  | USBD D- | CAN RX |
| 18 | PA12 |  |  |  | USBD D+ | CAN TX |
| 19 | PA13 |  |  |  |  | SWDIO |
| 20 | PA14 |  |  |  |  | SWCLK |

Notes:
- The numbers in brackets are the mapping options that are selected in the parameter section of the respective library.
- On the CH32V203F6P6, only USART2 (PA2/PA3), SPI1 and the USBD controller are accessible. There is no hardware I2C, use the *i2c_soft* library instead.
- The CH32V203F8P6 has a different pinout than the F6P6 (e.g. USBFS on PB6/PB7, I2C on PB6/PB7). Check the datasheet.

### Peripheral Mapping of the Libraries

| Function | Pins | Library / Note |
|:-|:-|:-|
| ADC channels 0 - 9 | PA0 - PA7, PB0, PB1 | gpio.h |
| USART1 TX/RX | PA9/PA10 (0), PB6/PB7 (1), PB15/PA8 (2), PA6/PA7 (3) | uart.h (mappings 2 and 3 not on all chips) |
| USART2 TX/RX | PA2/PA3 (0), PD5/PD6 (1, 64-pin only) | uart.h, uart2_dma.h, debug_serial.h (TX: PA2) |
| USART3 TX/RX | PB10/PB11 (0), PC10/PC11 (1, 64-pin only) | uart.h (CH32V203C8 and RB only) |
| I2C SDA/SCL | PB7/PB6 (0), PB9/PB8 (1), PB11/PB10 (2, I2C2) | i2c.h (PB6/PB7 are also the USBFS pins) |
| SPI SCK/MISO/MOSI | PA5/PA6/PA7 (0), PB3/PB4/PB5 (1), PB13/PB14/PB15 (2, SPI2) | spi.h, spi_tx.h |
| NeoPixel data (SPI MOSI) | PA7 (0), PB5 (1), PB15 (2) | neo_spi.h |
| USBD D-/D+ | PA11/PA12 | usbd libraries (all chips) |
| USBFS D-/D+ | PB6/PB7 | usbfs libraries (F8, G8, C8, RB only) |
| SWDIO/SWCLK | PA13/PA14 | programming and debugging |

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC RISC-V toolchain, chprog/rvprog) |
| template/platformio.ini | Configuration file for PlatformIO |
| template/ld/ | Linker scripts (ch32v203x6.ld for 32KB, ch32v203x8.ld for 64KB flash) |
| template/src/main.c | Example program (blinking LED on PB1) |
| template/src/system.c/h | Startup code, clock, power, watchdog, sleep and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt and ADC functions |
| template/src/ch32v203.h | Register definitions |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheet, reference manual and QingKe V4 core manual |

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
| usbd/usb_cdc | USB CDC (virtual serial port) |
| usbd/hid_keyboard | USB HID keyboard |
| usbd/hid_mouse | USB HID mouse |
| usbd/hid_consumer | USB HID consumer control (multimedia keys) |
| usbd/hid_composite | USB HID composite device (keyboard, consumer and mouse) |
| usbfs/usb_cdc | USB CDC (virtual serial port) |
| usbfs/hid_keyboard | USB HID keyboard |
| usbfs/hid_mouse | USB HID mouse |
| usbfs/hid_consumer | USB HID consumer control (multimedia keys) |
| usbfs/hid_composite | USB HID composite device (keyboard, consumer and mouse) |

The CH32V203 has two USB controllers. The libraries in the *usbd* folder use the USBD peripheral (PA11/PA12) which is available on all chips. The libraries in the *usbfs* folder use the USBFS peripheral (PB6/PB7), which is only available on the CH32V203F8, G8, C8 and RB.

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
- The system clock must be 48MHz, 96MHz or 144MHz (`F_CPU`).

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
Disconnect the board from the USB port and all voltage sources. Pull BOOT0 high (e.g. press and hold the BOOT0 button) while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, BOOT0 can be released and new firmware can be uploaded via USB. Alternatively, leave the board connected, press and hold the BOOT0 button, press and release the RESET button and then release the BOOT0 button.

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
- The template uses `F_CPU` = 144MHz by default (HSI with PLL).
- The CH32V203 operates at 2.4V - 3.6V. It must not be supplied with 5V directly, but most I/O pins are 5V-tolerant.
- The flash consists of a zero-wait-state area (32KB/64KB) and a slower non-zero-wait area. The linker scripts only use the zero-wait area.
- The ADC of the CH32V203 has an offset that should be compensated by calibration (`ADC_calibrate()`, see notes in *gpio.h*).

## Development Boards and Projects
- [CH32V203 F6P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH32V203F6P6_DevBoard)
- [Development Board Adapter](https://github.com/wagiminator/Development-Boards/tree/main/DevBoard_Adapter)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [chprog on PyPI](https://pypi.org/project/chprog/)
- [rvprog on PyPI](https://pypi.org/project/rvprog/)
- [WCH USB Programmer Web App](https://wagiminator.github.io/apps/chprog/)
- [WCH RISC-V Programmer Web App](https://wagiminator.github.io/apps/rvprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [WCH: CH32V203 Product Page](https://www.wch-ic.com/products/CH32V203.html)
- [WCH: CH32V203 Datasheet](https://www.wch-ic.com/downloads/CH32V203DS0_PDF.html)
- [WCH: CH32V20x/30x Reference Manual](https://www.wch-ic.com/downloads/CH32FV2x_V3xRM_PDF.html)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
