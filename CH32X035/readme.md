# MCU Template for CH32X035, CH32X034 and CH32X033 Microcontrollers
## Overview
The CH32X035 is a low-cost 32-bit RISC-V microcontroller from WCH with integrated USB and USB Power Delivery. It is based on the QingKe RISC-V4C core (RV32IMAC instruction set plus WCH extended instructions) and runs at up to 48MHz. It has 62KB flash, 20KB SRAM and a supply voltage range of 2.0V to 5.5V. The chip has the following peripherals:
- USB 2.0 full-speed device controller (and host on some packages) with integrated PHY
- USB Power Delivery and Type-C controller (sink, source and DRP)
- programmable protocol I/O controller (PIOC)
- 2 operational amplifiers with programmable gain and 3 comparators
- 12-bit ADC and up to 14 touch key channels
- 4 USARTs, I2C, SPI and multiple timers

It has a factory built-in USB bootloader. The CH32X034 and CH32X033 are variants with fewer features, e.g. the CH32X033 has no USB PD.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or closed-source IDE. It contains:
- a makefile and a PlatformIO configuration
- a linker script
- a startup and system configuration
- basic GPIO and ADC functions
- an example program

Optional libraries for the internal peripherals including USB and USB PD are also included.

## Supported Microcontrollers

| Part | Package | Flash | SRAM | GPIO | ADC Channels | USART | OPA/CMP | Touch Key | USB Host/Device | USB PD |
|:-|:-|-:|-:|-:|-:|-:|:-:|-:|:-:|:-:|
| CH32X035R8T6 | LQFP64M | 62KB | 20KB | 60 | 14+1 | 4 | 2/3 | 14 | ✓/✓ | ✓ |
| CH32X035C8T6 | LQFP48 | 62KB | 20KB | 46 | 10+1 | 4 | 2/3 | 10 | ✓/✓ | ✓ |
| CH32X035G8U6 | QFN28 | 62KB | 20KB | 27 | 12+1 | 4 | 2/1 | 12 | ✓/✓ | ✓ |
| CH32X035G8R6 | QSOP28 | 62KB | 20KB | 26 | 11+1 | 4 | 2/3 | 11 | ✓/✓ | ✓ |
| CH32X035F8U6 | QFN20 | 62KB | 20KB | 19 | 10+1 | 3 | 2/- | 10 | -/✓ | ✓ |
| CH32X035F7P6 | TSSOP20 | 62KB | 20KB | 18 | 11+1 | 3 | 1/1 | 11 | -/✓ | ✓ |
| CH32X035D8U6 | QFN12 | 62KB | 20KB | 11 | 5+1 | 3 | 1/- | 5 | -/✓ | ✓ |
| CH32X033F8P6 | TSSOP20 | 62KB | 20KB | 18 | 10+1 | 4 | 2/2 | 10 | -/✓ | - |
| CH32X034 | see datasheet | 62KB | 20KB | | | | | | | |

Common features of all variants:
- QingKe RISC-V4C core, RV32IMAC instruction set, hardware prologue/epilogue, up to 48MHz
- 62KB flash, 20KB SRAM, 3328 bytes bootloader
- Internal 48MHz RC oscillator (HSI), internal low-speed RC oscillator (LSI); no external crystal required for USB
- 12-bit ADC, touch key, advanced-control and general-purpose timers, watchdogs, DMA controller
- 2-wire serial debug interface (SWDIO on PC18, SWCLK on PC19)
- Supply voltage 2.0V - 5.5V

## Pinout and Peripheral Mapping
### CH32X035F7P6 (TSSOP20)

| Pin | Port | ADC | USART (UARTx_REMAP) | SPI | USB / PD | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | PB12 |  |  |  |  |  |
| 2 | PC14 |  |  |  | CC1 |  |
| 3 | PC15 |  |  |  | CC2 |  |
| 4 | PC3 | IN13 |  |  |  | RST (if enabled) |
| 5 | PC1 | IN11 | U2 RX (3) |  |  |  |
| 6 | PA0 | IN0 |  |  |  |  |
| 7 | PA1 | IN1 |  |  |  |  |
| 8 | PA2 | IN2 | U2 TX (0) |  |  | debug_serial TX |
| 9 | PA3 | IN3 | U2 RX (0) |  |  |  |
| 10 | PA4 | IN4 |  | NSS |  |  |
| 11 | PA5 | IN5 | U4 TX (1) | SCK |  |  |
| 12 | PA6 | IN6 |  | MISO |  |  |
| 13 | PA7 | IN7 | U1 TX (3) | MOSI |  | NeoPixel data (neo_spi) |
| 14 | PB1 | IN9 | U4 RX (0) |  |  |  |
| 15 | GND |  |  |  |  | Ground |
| 16 | VDD |  |  |  |  | Supply voltage 2.0V - 5.5V |
| 17 | PC16 |  | U4 TX (2), U4 RX (5) |  | D- (UDM) |  |
| 18 | PC17 |  | U4 TX (5), U4 RX (2) |  | D+ (UDP) | BOOT detection (high at power-up = bootloader) |
| 19 | PC18 |  | U3 TX (1) |  |  | SWDIO |
| 20 | PC19 |  | U3 RX (1), U4 RX (4) |  |  | SWCLK |

Notes:
- The numbers in brackets are the mapping options (`UART1_REMAP` ... `UART4_REMAP`) that are selected in the parameter section of the *uart.h* library. U1 ... U4 stand for USART1 ... USART4.
- The *debug_serial* and *uart2_dma* libraries use USART2 (default TX on PA2). The *uart2_dma* library supports further mappings (see header).
- PC17 (USB D+) is the boot detection pin: if it is high at power-up (e.g. BOOT button connecting D+ to 3V3), the chip starts the bootloader.
- The RST function on PC3 is only available on the CH32X035 in the QSOP28 and TSSOP20 packages and must be enabled in the option bytes.
- There is no library for the hardware I2C yet, use the *i2c_soft* library instead.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC RISC-V toolchain, chprog/rvprog) |
| template/platformio.ini | Configuration file for PlatformIO |
| template/ld/ | Linker scripts (ch32x035.ld) |
| template/src/main.c | Example program (blinking LED on PB1) |
| template/src/system.c/h | Startup code, clock, power, watchdog, sleep and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt, ADC, OPA, comparator and touch key functions |
| template/src/ch32x035.h | Register definitions |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheet, reference manual and QingKe V4 core manual |

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. The parameters (e.g. pin mapping, baud rate, clock rate) can be adjusted in the parameter section at the beginning of the respective header file.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) via USART2 (PA2) with simple printf |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| millis | Millisecond counter using the SysTick timer |
| neo_spi | NeoPixel (WS2812) control using hardware SPI (MOSI pin PA7, PA9, PA10 or PC7) |
| neo_sw | NeoPixel (WS2812) control using software bit-banging on any pin |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| spi | SPI master functions |
| spi_soft | Software SPI master (bit-banging) on any pins |
| spi_tx | SPI master functions (TX only) |
| uart | Basic USART1/2/3/4 functions (no buffer, no interrupt, no DMA) |
| uart2_dma | USART2 with DMA-driven RX ring buffer |
| usb/usb_cdc | USB CDC (virtual serial port) |
| usb/hid_keyboard | USB HID keyboard |
| usb/hid_mouse | USB HID mouse |
| usb/hid_consumer | USB HID consumer control (multimedia keys) |
| usb/hid_composite | USB HID composite device (keyboard, consumer and mouse) |
| usb_pd/usbpd_3_0_sink | USB Power Delivery sink (PD 3.0, fixed and PPS supplies) |
| usb_pd/usbpd_3_1_sink | USB Power Delivery sink (PD 3.1 incl. EPR and AVS) |
| usb_host/host_keyboard | USB host for a keyboard (boot protocol, ASCII conversion) |
| usb_host/host_mouse | USB host for a mouse (boot protocol) |
| usb_host/host_gamepad | USB host for a HID gamepad/joystick (report descriptor parser) |

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
- The USB PD libraries use the CC1/CC2 pins PC14/PC15 and are not available on the CH32X033.

### Using the USB Host Libraries
Each USB host library consists of the following files:
- the class driver (e.g. *host_keyboard.c/h*)
- the configuration (*config.h*)
- the USB host handler (*usb_host.c/h*), which contains all MCU-specific parts incl. the register accesses
- the USB definitions (*usb.h*)

To use a library, copy all of its files into the *src* folder. The library is started with its init function (e.g. `KBD_init()`); the update function (e.g. `KBD_update()`) must be called frequently, it handles connecting/disconnecting and enumerating the device and polls its reports. A single HID device can be connected directly to the USB port (PC16/PC17); hubs are not supported, and for composite devices only the first matching interface is used. The host works without interrupts.
Additionally:
- VBUS (5V) for the connected device must be provided by the board.
- Set `USB_VDD` in *config.h* according to the supply voltage of the MCU (0: 3.3V, 1: 5V).
- The USB device libraries and the USB host libraries cannot be used at the same time.

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
Disconnect the board from the USB port and all voltage sources. Press and hold the BOOT button (which pulls PC17/D+ high) while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, the BOOT button can be released and new firmware can be uploaded via USB within the next couple of seconds.

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
- The template uses `F_CPU` = 48MHz by default (internal HSI, no external crystal required).
- The USB PD controller can also be used for other Type-C functions (e.g. as source or for detecting the connection orientation), see the reference manual.
- The CH32X033 has no USB PD controller, but the same flash/SRAM and a similar pinout in TSSOP20.

## Development Boards and Projects
### Development Boards
- [CH32X035 F7P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH32X035F7P6_DevBoard)
- [CH32X033 F8P6 Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH32X033F8P6_DevBoard)
- [Development Board Adapter](https://github.com/wagiminator/Development-Boards/tree/main/DevBoard_Adapter)

### Projects
- [USB PD Tester](https://github.com/wagiminator/CH32X035-USB-PD-Tester)
- [USB PD Adapter](https://github.com/wagiminator/CH32X035-USB-PD-Adapter)
- [USB Knob - Rotary Encoder with USB Interface](https://github.com/wagiminator/CH32X033-USB-Knob)
- [USB2NRF - USB Controlled 2.4GHz Transceiver](https://github.com/wagiminator/CH32X033-USB-NRF)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [chprog on PyPI](https://pypi.org/project/chprog/)
- [rvprog on PyPI](https://pypi.org/project/rvprog/)
- [WCH USB Programmer Web App](https://wagiminator.github.io/apps/chprog/)
- [WCH RISC-V Programmer Web App](https://wagiminator.github.io/apps/rvprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [WCH: CH32X035 Product Page](https://www.wch-ic.com/products/CH32X035.html)
- [WCH: CH32X035 Datasheet](https://www.wch-ic.com/downloads/CH32X035DS0_PDF.html)
- [WCH: CH32X035 Reference Manual](https://www.wch-ic.com/downloads/CH32X035RM_PDF.html)
- [openwch: CH32X035 SDK and examples](https://github.com/openwch/ch32x035)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
