# MCU Template for CH558 and CH559 Microcontrollers
## Overview
The CH559 from WCH is an 8-bit microcontroller with an enhanced E8051 core that is compatible with the MCS51 instruction set and runs at up to 56MHz. It has the following features:
- a USB 2.0 full-speed controller with integrated transceiver, supporting device mode and host mode with a root hub for two USB ports
- a 10/11-bit ADC with 8 channels
- two UARTs and two SPI interfaces
- 8-bit and 16-bit PWM
- multiplication and division instructions
- 64KB code flash and 6KB xRAM

The CH558 is a smaller variant with 32KB code flash, 5KB data flash and 4KB xRAM, without USB host. Both chips have a factory built-in bootloader for uploading firmware via USB (and UART).

This template is a lightweight starting point for bare-metal firmware development in C with the SDCC compiler. It contains:
- a makefile and a wrapper file for the Arduino IDE
- the system, GPIO and delay functions
- an example program
- a simple upload tool

Optional libraries for the internal peripherals, a USB CDC device library and USB host libraries for keyboards, mice and gamepads are also included.

## Supported Microcontrollers

| Part | Package | Code Flash | Data Flash | xRAM | GPIO | ADC | UART | SPI | USB |
|:-|:-|-:|-:|-:|-:|-:|-:|-:|:-|
| CH559L | LQFP48 | 60KB | 1KB | 6KB | up to 45 | 8 | 2 | 2 | Host (root hub)/Device |
| CH559T | SSOP20 | 60KB | 1KB | 6KB | 16 | 5 | 2 | 1 | Host/Device |
| CH558L | LQFP48 | 32KB | 5KB | 4KB | see datasheet | 8 | 2 | 1 | Device |
| CH558T | SSOP20 | 32KB | 5KB | 4KB | 16 | 5 | 2 | 1 | Device |

GPIO counts include the USB pins. Check the datasheet for the exact functions available on each package.

Common features of all variants:
- Enhanced E8051 core (MCS51 compatible) with 8x8 multiplication and 16/8 division, up to 56MHz from the internal 12MHz oscillator and PLL (no crystal required for USB)
- 3KB bootloader, 256 bytes iRAM, unique chip ID
- 4 timers, PWM (2x 8-bit, 1x 16-bit), watchdog
- Integrated 3.3V regulator, supply voltage 3.3V or 5V

## Pinout and Peripheral Mapping
### CH559T (SSOP20)

| Pin | Port | ADC | UART | SPI | PWM | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|
| 1 | P1.2 | AIN2 |  |  | PWM3 |  |
| 2 | P1.4 | AIN4 |  | SCS |  |  |
| 3 | P1.5 | AIN5 |  | MOSI |  |  |
| 4 | P1.6 | AIN6 |  | MISO |  |  |
| 5 | P1.7 | AIN7 |  | SCK |  |  |
| 6 | P5.7/RST |  |  |  |  | Reset input (active high) |
| 7 | P3.2 |  |  |  |  | INT0, LED0 |
| 8 | P3.4 |  |  |  |  | T0 |
| 9 | P4.6 |  |  |  |  | XI (crystal) |
| 10 | P4.7 |  |  |  |  | XO (crystal) |
| 11 | P2.5 |  | TNOW (UART1) |  | PWM2 | LED (template example) |
| 12 | P2.6 |  | RXD1 |  |  |  |
| 13 | P2.7 |  | TXD1 |  |  |  |
| 14 | P5.1 |  |  |  |  | USB D+ (DP) |
| 15 | P5.0 |  |  |  |  | USB D- (DM) |
| 16 | P0.3 |  | TXD_ |  |  |  |
| 17 | P0.2 |  | RXD_ |  |  |  |
| 18 | GND |  |  |  |  | Ground |
| 19 | VIN5 |  |  |  |  | 5V supply input |
| 20 | VDD33 |  |  |  |  | 3.3V supply / internal regulator output (100nF to GND) |

Notes:
- Signals ending in "_" are alternate pins (e.g. RXD_/TXD_ of UART0 on P0.2/P0.3).
- The CH558T has the same pinout, but no USB host.
- Only P1 pins can be used as ADC inputs (`PIN_input_AN()`, `ADC_input()`). PWM1/PWM2 are available on P2.4/P2.5 or (mapped together) on P4.3/P4.5 with `PWM_start()`.
- The open-drain/bidirectional mode (`bPn_OC`) can only be set for a whole port. `PIN_output()` sets the whole port to push-pull, `PIN_output_OD()`/`PIN_output_BI()` the whole port to open-drain.
- USB host port 0 uses DP/DM (P5.1/P5.0), port 1 HP/HM (P5.5/P5.4, LQFP48 only).

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (SDCC toolchain) |
| template/template.ino | Wrapper file for compiling with the Arduino IDE ([ch55xduino](https://github.com/DeqingSun/ch55xduino)) |
| template/src/main.c | Example program (blinking LED on P2.5) |
| template/src/system.h | Clock, sleep, watchdog, reset, interrupt and other system functions |
| template/src/gpio.h | GPIO, wake-up pin, PWM and ADC functions |
| template/src/delay.c/h | Delay functions (µs and ms) |
| template/src/ch559.h | Register definitions |
| template/tools/chprog.py | Simple Python tool for uploading via the USB bootloader (see *tools/readme.md*) |
| libraries/ | Optional libraries for internal peripherals and USB (see below) |
| datasheet/ | Datasheet(s) and the CH55x instruction set |

There is no linker script: memory sizes are passed to SDCC by the makefile (`XRAM_LOC`, `XRAM_SIZE`, `CODE_SIZE`).

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals, for USB device and for USB host. Parameters are set at the beginning of the respective header file. Some libraries need definitions in a *config.h* file (e.g. `PIN_SDA`, `PIN_SCL`, `PIN_NEO`, `NEO_COUNT`).

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) via UART0 or UART1 with lightweight print functions or printf |
| flash | Read/erase/write the 1KB data flash |
| i2c | Software I2C master (bit-banging, up to 400kHz) on any P0-P3 pins (`PIN_SDA`, `PIN_SCL` in *config.h*) |
| millis | Millisecond counter using TIMER2 (auto-reload, no drift) |
| neo | NeoPixel (WS2812) control on any P0-P3 pin (`PIN_NEO`, `NEO_GRB`, `NEO_COUNT` in *config.h*), timing calculated for all F_CPU >= 6MHz |
| print | Lightweight print functions (strings, decimal and hex numbers) for any output function, much smaller than printf |
| spi | Hardware SPI0 master functions |
| uart | Basic UART0 (TIMER1 BAUD rate generator) and UART1 (16550 compatible with FIFO) functions |
| uart_int | UART0 or UART1 with RX interrupt and ring buffer |
| usb/usb_cdc | USB device: CDC (virtual serial port) |
| usb_host/host_keyboard | USB host: keyboard (boot protocol, ASCII conversion with US layout, LEDs) |
| usb_host/host_mouse | USB host: mouse (boot protocol, buttons, movement, wheel) |
| usb_host/host_gamepad | USB host: HID gamepad/joystick (report descriptor parser for axes, hat switch and buttons) |

Note: The *makefile* compiles all *.c* files in the template folder and in *src*, but not in an *include* folder. Therefore, libraries must be copied into *src*.

### Using the USB Host Libraries
Each USB host library consists of the class driver (e.g. *host_keyboard.c/h*), the USB host core (*usb_host.c/h*), the USB definitions (*usb.h*) and the configuration (*config.h*). Copy all files into the *src* folder. The host is polled, so the `xxx_update()` function of the class driver must be called frequently in the main loop. Only devices that are directly connected to the root hub ports are supported (no external USB hubs). The board must provide 5V (VBUS) for the USB device. `F_CPU` should be 48MHz or 56MHz (at least 12MHz).

### Using the USB Device Libraries
Each USB device library consists of the following files:
- the class driver (e.g. *usb_cdc.c/h*)
- the USB descriptors (*usb_descr.c/h*)
- the device configuration (*config.h*)
- the USB handler (*usb_handler.c/h*)
- the USB definitions (*usb.h*)

To use a library, copy all of its files into the *src* folder (and replace the *config.h* there, if present). Adjust the *config.h* (pins, vendor and product strings, VID/PID) according to your needs. The default VID/PID pairs are shared IDs that are only suitable for private projects. The USB endpoint buffers are located at the beginning of the xRAM, so `XRAM_LOC` in the makefile must not be changed.

## Compiling Firmware
### Using the Makefile
To use the *makefile*, some additional software components must be installed to set up the toolchain.

#### Install SDCC Toolchain for Compiling
```
sudo apt install build-essential sdcc
```

#### Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install chprog
```

The template also contains the simple programming tool *tools/chprog.py*, which only needs PyUSB (`pip install pyusb`).

#### Give Access Permission to the Factory built-in USB Bootloader
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-ch55x.rules
sudo udevadm control --reload-rules
```

#### Compiling/Uploading
Edit the *makefile* according to the needs of the project. The system clock is set with `FREQ_SYS` (default 56MHz), the memory layout with `XRAM_LOC` (0x0100) and `CODE_SIZE` (0xF000 = 60KB; use 0x8000 for the CH558). By default, `make flash` uses *tools/chprog.py*. To use the chprog package from PyPI instead, change `ISPTOOL` to `chprog $(BIN)/$(TARGET).bin`. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile, build and keep all files
make hex       compile and build <firmware>.hex
make bin       compile and build <firmware>.bin
make flash     compile, build and upload to MCU
make clean     remove all build files
```

### Using the Arduino IDE
The template can also be compiled with the Arduino IDE and [ch55xduino](https://github.com/DeqingSun/ch55xduino). Follow the instructions in *template.ino*; note the recommended ch55xduino version mentioned there. The *.ino* file must have the same name as the folder in which it is located, and the *src* folder must be present.

## Uploading Firmware
### Installing Drivers for the Bootloader
On Linux you do not need to install a driver, but you need to grant access permissions (see above). On Windows, you need the [CH372 driver](http://www.wch-ic.com/downloads/CH372DRV_EXE.html) for WCHISPTool. Alternatively, use the [Zadig](https://zadig.akeo.ie/) tool: click "Options" -> "List All Devices", select the USB module and install the libusb-win32 driver (for chprog) or the WinUSB driver (for the web app). To do this, the board must be connected and the microcontroller must be in bootloader mode.

### Entering Bootloader Mode
A brand-new chip starts automatically in bootloader mode as soon as it is connected to the PC via USB. Once firmware has been uploaded, the bootloader must be started manually for new uploads. To do this, disconnect the board from the USB port and all voltage sources. Press and hold the BOOT button of the development board while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, the BOOT button can be released and new firmware can be uploaded within the next couple of seconds.

### Uploading Firmware Binary
- **chprog**: If [Python3](https://www.pythontutorial.net/getting-started/install-python/) is installed, use the platform-independent open-source command-line tool [chprog](https://pypi.org/project/chprog/) (`pip install chprog`) or the included *tools/chprog.py*:
  ```
  chprog <firmware>.bin
  ```
- **Web app**: Use the [WCH USB Programmer web app](https://wagiminator.github.io/apps/chprog/). It only works in Chrome, Edge or Opera on a desktop computer and needs the WinUSB driver on Windows.
- **WCHISPTool**: WCH offers the free but closed-source software [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) for Windows. Open the *<firmware>.hex* file and upload it to the microcontroller.

## Notes and Tips
- The template uses `FREQ_SYS` = 56MHz by default. For the CH558, also adjust `CODE_SIZE` and `XRAM_SIZE` in the makefile (32KB code, 4KB xRAM).
- Libraries with interrupt service routines (*millis*) must be included in the file containing `main()`, otherwise SDCC does not link the interrupt vector.
- The USB host libraries and the *flash* library are for the CH559 only (the CH558 has no USB host and a different data flash).
- `printf` from the SDCC library needs about 2KB of flash. The *print* library or the `DEBUG_print` functions need only a few hundred bytes.
- The 8051 has only 256 bytes of iRAM. Large arrays should be placed in xRAM (`__xdata`) and constants in code flash (`__code`).
- The CH559 can also be programmed with the Arduino IDE using [ch55xduino](https://github.com/DeqingSun/ch55xduino), see *template.ino*.

## Development Boards and Projects
- [CH55xT Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH55xT_DevBoard) (CH552T, CH554T, CH558T, CH559T)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [chprog on PyPI](https://pypi.org/project/chprog/)
- [WCH USB Programmer Web App](https://wagiminator.github.io/apps/chprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [ch55xduino](https://github.com/DeqingSun/ch55xduino)
- [WCH: CH559 Datasheet](http://www.wch-ic.com/downloads/CH559DS1_PDF.html)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
