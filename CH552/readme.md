# MCU Template for CH551, CH552 and CH554 Microcontrollers
## Overview
The CH55x from WCH is a family of low-cost 8-bit microcontrollers with an enhanced E8051 core that is compatible with the MCS51 instruction set. 79% of its instructions are single-byte single-cycle instructions, and the average instruction speed is 8 - 15 times faster than that of the standard MCS51. The chips have the following features:
- a USB 2.0 full-speed/low-speed controller with integrated transceiver and DMA, supporting device mode (all) and host mode (CH554 only)
- USB Type-C master/slave detection
- an 8-bit ADC and capacitive touch key detection
- UARTs, SPI and PWM

A factory built-in USB bootloader allows uploading firmware directly via USB without an additional programming device.

This template is a lightweight starting point for bare-metal firmware development in C with the SDCC compiler. It contains:
- a makefile and a wrapper file for the Arduino IDE
- the system, GPIO and delay functions
- an example program
- a simple upload tool

Optional libraries for the internal peripherals and for USB devices are also included.

## Supported Microcontrollers

| Part | Package | Code Flash | Data Flash | xRAM | GPIO | ADC | Touch | UART | USB | Type-C |
|:-|:-|-:|-:|-:|-:|-:|-:|-:|:-|:-:|
| CH551G | SOP16 | 10KB | 128B | 512B | 12 | - | 4 | 1 | Device | - |
| CH552T | TSSOP20 | 14KB | 128B | 1KB | 16 | 4 | 6 | 2 | Device | ✓ |
| CH552G | SOP16 | 14KB | 128B | 1KB | 12 | 4 | 5 | 2 | Device | ✓ |
| CH552P | QFN16 | 14KB | 128B | 1KB | 12 | 4 | 5 | 2 | Device | ✓ |
| CH552E | MSOP10 | 14KB | 128B | 1KB | 6 | 2 | 4 | 1 | Device | ✓ |
| CH554T | TSSOP20 | 14KB | 128B | 1KB | 16 | 4 | 6 | 2 | Host/Device | ✓ |
| CH554G | SOP16 | 14KB | 128B | 1KB | 12 | 4 | 5 | 2 | Host/Device | ✓ |
| CH554E | MSOP10 | 14KB | 128B | 1KB | 6 | 2 | 4 | 1 | Host/Device | ✓ |

GPIO counts include the USB pins (P3.6/P3.7). The values for the CH552P are assumed to be equal to the CH552G. Check the datasheet for the exact functions available on each package.

Common features of all variants:
- Enhanced E8051 core (MCS51 compatible), up to 24MHz from the internal oscillator (no crystal required for USB)
- 16KB code flash including 2KB bootloader (CH551: 10KB code flash), 128 bytes data flash ("EEPROM"), 256 bytes iRAM
- 3 timers (T0, T1, T2 with capture), 2 PWM channels (8-bit), watchdog
- Integrated 3.3V regulator, supply voltage 3.3V or 5V
- Unique chip ID

## Pinout and Peripheral Mapping
### CH552T/CH554T (TSSOP20)

| Pin | Port | ADC | Touch | UART | SPI | PWM | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|:-|
| 1 | P3.2 | AIN3 |  | TXD1 (1) |  |  | INT0 |
| 2 | P1.4 | AIN1 | TIN2 |  | SCS |  | UCC1 (Type-C CC1) |
| 3 | P1.5 | AIN2 | TIN3 |  | MOSI | PWM1 | UCC2 (Type-C CC2) |
| 4 | P1.6 |  | TIN4 | RXD1 (0) | MISO |  |  |
| 5 | P1.7 |  | TIN5 | TXD1 (0) | SCK |  |  |
| 6 | RST |  |  |  |  |  | Reset input (active high) |
| 7 | P1.0 |  | TIN0 |  |  |  | T2, CAP1 |
| 8 | P1.1 | AIN0 | TIN1 |  |  |  | T2EX, CAP2 |
| 9 | P3.1 |  |  | TXD (0) |  | PWM2_ |  |
| 10 | P3.0 |  |  | RXD (0) |  | PWM1_ |  |
| 11 | P3.3 |  |  |  |  |  | INT1, LED (template example) |
| 12 | P3.4 |  |  | RXD1 (1) |  | PWM2 | T0 |
| 13 | P3.5 |  |  |  |  |  | T1 |
| 14 | P3.6 |  |  |  |  |  | USB D+ (UDP) |
| 15 | P3.7 |  |  |  |  |  | USB D- (UDM) |
| 16 | P1.3 |  |  | TXD (1) |  |  | XO (crystal) |
| 17 | P1.2 |  |  | RXD (1) |  |  | XI (crystal) |
| 18 | GND |  |  |  |  |  | Ground |
| 19 | VCC |  |  |  |  |  | Supply voltage (3.3V or 5V) |
| 20 | V33 |  |  |  |  |  | Internal 3.3V regulator output, connect 100nF to GND (connect to VCC when supplied with 3.3V) |

Notes:
- The numbers in brackets for UART select the standard (0) or alternate (1) pins, set with `UART0_ALTER`/`UART1_ALTER` in *uart.h* or `DEBUG_ALTER` in *debug_serial.h*. Signals ending in "_" are alternate pins (e.g. PWM1_).
- *debug_serial* uses UART1 (TXD1 = P1.7) by default with 9600 baud (`DEBUG_PORT`, `DEBUG_BAUD`).
- Pins for the software I2C (*i2c*), NeoPixels (*neo*) and touch keys (*touch*) can be freely chosen and are defined in *config.h*.
- CH552G/CH554G (SOP16): P3.2, P1.4, P1.5, P1.6, P1.7, RST, P3.1, P3.0, P1.1, P3.3, P3.4, P3.6 (D+), P3.7 (D-), GND, VCC, V33.
- CH552E/CH554E (MSOP10): P1.4, P1.5, P1.6, P1.7, RST, P3.6 (D+), P3.7 (D-), GND, VCC, V33.
- On boards with a BOOT button, the button usually connects P3.6 (D+) to 3.3V via a resistor.

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (SDCC toolchain) |
| template/template.ino | Wrapper file for compiling with the Arduino IDE ([ch55xduino](https://github.com/DeqingSun/ch55xduino)) |
| template/src/main.c | Example program (blinking LED on P3.3) |
| template/src/system.h | Clock, sleep, watchdog, reset, interrupt and other system functions |
| template/src/gpio.h | GPIO, wake-up pin, PWM and ADC functions |
| template/src/delay.c/h | Delay functions (µs and ms) |
| template/src/ch554.h | Register definitions |
| template/tools/chprog.py | Simple Python tool for uploading via the USB bootloader (see *tools/readme.md*) |
| libraries/ | Optional libraries for internal peripherals and USB (see below) |
| datasheet/ | Datasheet(s) and the CH55x instruction set |

There is no linker script: memory sizes are passed to SDCC by the makefile (`XRAM_LOC`, `XRAM_SIZE`, `CODE_SIZE`).

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals and USB. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. Parameters are set at the beginning of the respective header file. Some libraries need definitions in a *config.h* file (e.g. `PIN_SDA`, `PIN_SCL`, `PIN_NEO`, `NEO_COUNT`, touch thresholds), which must be created in the *src* folder if it does not exist yet.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) via UART0 or UART1 with lightweight print functions or printf |
| eeprom | Read/write the 128-byte data flash ("EEPROM") |
| i2c | Software I2C master (bit-banging, up to 400kHz) on any pins (`PIN_SDA`, `PIN_SCL` in *config.h*) |
| millis | Millisecond counter using TIMER2 (auto-reload, no drift) |
| neo | NeoPixel (WS2812) control on any pin (`PIN_NEO`, `NEO_GRB`, `NEO_COUNT` in *config.h*, F_CPU at least 6MHz) |
| print | Lightweight print functions (strings, decimal and hex numbers) for any output function, much smaller than printf |
| spi | Hardware SPI master functions |
| tim2 | TIMER2 capture (period/pulse measurement on T2/T2EX), counter and clock output, header only |
| touch | Capacitive touch key functions (thresholds in *config.h*) |
| typec | USB Type-C CC pull-up/pull-down control and detection of the source current (CH552/CH554), header only |
| uart | Basic UART0/UART1 functions (polling, no buffer) |
| uart_int | UART0 or UART1 with RX interrupt and ring buffer |
| usb/usb_cdc | USB CDC (virtual serial port) |
| usb/hid_keyboard | USB HID keyboard |
| usb/hid_mouse | USB HID mouse |
| usb/hid_composite | USB HID composite device (keyboard, consumer control and mouse) |
| usb/hid_multitouch | USB HID multitouch (digitizer) |
| usb/hid_data | USB HID raw data transfer |
| usb_host/host_keyboard | USB host for a keyboard (boot protocol, ASCII conversion), CH554 only |
| usb_host/host_mouse | USB host for a mouse (boot protocol), CH554 only |
| usb_host/host_gamepad | USB host for a HID gamepad/joystick (report descriptor parser), CH554 only |

Note: The *makefile* compiles all *.c* files in the template folder and in *src*, but not in an *include* folder. Therefore, libraries must be copied into *src*.

### Using the USB Libraries
Each USB library consists of the following files:
- the class driver (e.g. *usb_cdc.c/h*)
- the USB descriptors (*usb_descr.c/h*)
- the device configuration (*config.h*)
- the USB handler (*usb_handler.c/h*)
- the USB definitions (*usb.h*)

To use a library, copy all of its files into the *src* folder (and replace the *config.h* there, if present). Adjust the *config.h* (pins, vendor and product strings, VID/PID) according to your needs. The default VID/PID pairs are shared IDs that are only suitable for private projects. The USB endpoint buffers are located at the beginning of the xRAM, so `XRAM_LOC` in the makefile must not be changed. USB requires a system clock (`FREQ_SYS`) of at least 6MHz (recommended: 16MHz or 24MHz).

### Using the USB Host Libraries
Each USB host library consists of the following files:
- the class driver (e.g. *host_keyboard.c/h*)
- the configuration (*config.h*)
- the USB host handler (*usb_host.c/h*), which contains all MCU-specific parts incl. the register accesses
- the USB definitions (*usb.h*)

To use a library, copy all of its files into the *src* folder. The library is started with its init function (e.g. `KBD_init()`); the update function (e.g. `KBD_update()`) must be called frequently, it handles connecting/disconnecting and enumerating the device and polls its reports. A single HID device can be connected directly to the USB port (UDP/UDM, P3.6/P3.7); hubs are not supported, and for composite devices only the first matching interface is used. The host works without interrupts. The class drivers are the same as for the other templates with USB host libraries.
Additionally:
- Only the CH554 supports the USB host mode (not CH551/CH552).
- VBUS (5V) for the connected device must be provided by the board.
- The USB DMA buffers are at the beginning of the xRAM, so `XRAM_LOC` in the makefile must not be changed.
- The USB device libraries and the USB host libraries cannot be used at the same time.

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
Edit the *makefile* according to the needs of the project. The system clock is set with `FREQ_SYS` (default 16MHz), the memory layout with `XRAM_LOC` (0x0100) and `CODE_SIZE` (0x3800 = 14KB). By default, `make flash` uses *tools/chprog.py*. To use the chprog package from PyPI instead, change `ISPTOOL` to `chprog $(BIN)/$(TARGET).bin`. Use one of the following commands inside the *template* folder to compile/upload:

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
A brand-new chip starts automatically in bootloader mode as soon as it is connected to the PC via USB. Once firmware has been uploaded, the bootloader must be started manually for new uploads. To do this, disconnect the board from the USB port and all voltage sources. Press and hold the BOOT button (which pulls P3.6/D+ high) while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, the BOOT button can be released and new firmware can be uploaded within the next couple of seconds. The firmware itself can also jump to the bootloader (see `BOOT_now()` in *system.h*).

### Uploading Firmware Binary
- **chprog**: If [Python3](https://www.pythontutorial.net/getting-started/install-python/) is installed, use the platform-independent open-source command-line tool [chprog](https://pypi.org/project/chprog/) (`pip install chprog`) or the included *tools/chprog.py*:
  ```
  chprog <firmware>.bin
  ```
- **Web app**: Use the [WCH USB Programmer web app](https://wagiminator.github.io/apps/chprog/). It only works in Chrome, Edge or Opera on a desktop computer and needs the WinUSB driver on Windows.
- **WCHISPTool**: WCH offers the free but closed-source software [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) for Windows. Open the *<firmware>.hex* file and upload it to the microcontroller.

## Notes and Tips
- The template uses `FREQ_SYS` = 16MHz by default. The maximum system clock is 24MHz (VCC >= 4.5V), 16MHz needs VCC >= 3.8V. The setting 32MHz is reserved by WCH and therefore not supported. According to the datasheet, 12MHz is the maximum at a supply voltage of 3.3V (ch55xduino also offers 16MHz at 3.3V, which usually works).
- When the chip is supplied with 3.3V, connect V33 with VCC. When supplied with 5V, connect a 100nF capacitor between V33 and GND.
- `printf` from the SDCC library needs about 2KB of flash. The *print* library or the `DEBUG_print` functions need only a few hundred bytes.
- Libraries with interrupt service routines (*millis*, *uart_int*) must be included in the file containing `main()`, otherwise SDCC does not link the interrupt vector.
- The 8051 has only 256 bytes of iRAM. Large arrays should be placed in xRAM (`__xdata`) and constants in code flash (`__code`).
- The CH552 can also be programmed with the Arduino IDE using [ch55xduino](https://github.com/DeqingSun/ch55xduino).

## Development Boards and Projects
### Development Boards
- [CH55xT Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH55xT_DevBoard) (CH552T, CH554T, CH558T, CH559T)
- [CH55xG Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH55xG_DevBoard) (CH551G, CH552G, CH554G)
- [CH55xE Development Stick](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick) (CH552E, CH554E)
- [CH554E USB Host Development Board](https://github.com/wagiminator/Development-Boards/tree/main/CH554E_HostDevBoard)

### Projects
- [CMSIS-DAP Compliant SWD and JTAG Programmer (CH552-DAPLink)](https://github.com/wagiminator/CH552-DAPLink)
- [picoDAP - CMSIS-DAP Compliant SWD Programmer (CH552E)](https://github.com/wagiminator/CH552-picoDAP)
- [USB2NRF - USB Controlled 2.4GHz Transceiver](https://github.com/wagiminator/CH552-USB-NRF)
- [USB OLED Display (CH552E/CH554E)](https://github.com/wagiminator/CH552-USB-OLED)
- [USB Knob - Rotary Encoder (CH552E/CH554E)](https://github.com/wagiminator/CH552-USB-Knob)
- [MacroPad plus (CH552G)](https://github.com/wagiminator/CH552-MacroPad-plus)
- [MacroPad mini (CH552E)](https://github.com/wagiminator/CH552-Macropad-mini)
- [Mouse Wiggler (CH552E)](https://github.com/wagiminator/CH552-MouseWiggler)
- [picoAVR - USB AVR Programmer (CH552G)](https://github.com/wagiminator/AVR-Programmer/tree/master/picoAVR_Programmer)
- [picoISP - USB ISP Programmer (CH552E)](https://github.com/wagiminator/AVR-Programmer/tree/master/picoISP_Programmer)
- [C64 DiskMaster64 (CH552E)](https://github.com/wagiminator/C64-Collection/tree/master/C64_DiskMaster64)
- [C64 Joystick Adapter (CH552G)](https://github.com/wagiminator/C64-Collection/tree/master/C64_Joystick_Adapter)
- [C64 1351 Mouse Adapter (CH552E)](https://github.com/wagiminator/C64-Collection/tree/master/C64_1351_Mouse_Adapter)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [chprog on PyPI](https://pypi.org/project/chprog/)
- [WCH USB Programmer Web App](https://wagiminator.github.io/apps/chprog/)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [ch55xduino](https://github.com/DeqingSun/ch55xduino)
- [CH55x SDK for SDCC (Blinkinlabs)](https://github.com/Blinkinlabs/ch554_sdcc)
- [WCH: CH552 Datasheet](http://www.wch-ic.com/downloads/CH552DS1_PDF.html)
- [WCH: CH554 Datasheet](http://www.wch-ic.com/downloads/CH554DS1_PDF.html)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
