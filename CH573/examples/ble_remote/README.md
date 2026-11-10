# CH573F Example: Bluetooth LE Remote (ble_remote)
## Overview
Own GATT server: switch LED, read keys (notify), temperature (Environmental Sensing). This example is part of a collection of simple examples for CH573F boards (e.g. the WeAct CH573F core board). It uses the bare-metal CH573 template of the [MCU-Templates](https://github.com/wagiminator/MCU-Templates) repository and its libraries. The folder is self-contained: it includes all source files, libraries and the linker script needed for this example. No additional hardware has to be connected, the example only uses the components on the board (LED, BOOT key, KEY, USB, internal temperature sensor and supply voltage measurement, Bluetooth LE).

## Description
```
Shows how to build an own GATT server with the BLE host API. The board advertises
as BLE_NAME and offers two services, which can be used with a generic BLE app
like "nRF Connect" (Android/iOS) or "LightBlue" (iOS/Android):

  LED & Keys service (custom 128-bit UUID, see below):
    LED          (read, write)   0x00: LED off, 0x01: LED on
    Keys         (read, notify)  bit 0: BOOT key, bit 1: KEY (1 = pressed),
                                 a notification is sent with each change

  Environmental Sensing service (0x181A, Bluetooth standard):
    Temperature  (read, notify)  0x2A6E, sint16 in 0.01 degrees Celsius,
                                 updated every TEMP_INTERVAL milliseconds

Connection events and LED changes are reported via the USB serial port. While no
device is connected, the LED flashes briefly every 2 seconds.

Tested template functions: BLE handler, BLE host API (16- and 128-bit services
and characteristics, read/write callbacks, notifications, events), USB device
(CDC), keys, temperature sensor.

Settings in config.h: BLE_NAME, TEMP_INTERVAL, BLE_TX_POWER, BLE library settings.
```

## Files

| Path | Description |
|:-|:-|
| config.h | **Configuration**: board pins, example settings, Bluetooth settings, USB descriptors |
| makefile | Compiles and uploads the example |
| src/main.c | the example application |
| src/system.c/.h | template core: clock, delay, RTC, sleep, reset, watchdog, flash ROM access, startup |
| src/gpio.h | template core: GPIO, pin interrupts, ADC, TouchKey |
| src/ch573.h | register definitions (WCH) |
| src/print.c/.h | printf and number output |
| src/millis.c/.h | milliseconds via SysTick |
| src/board.c/.h | board support of the examples: LED, keys, info, temperature, VDD |
| src/term.c/.h | terminal output via USB serial port |
| src/usb.h | USB definitions |
| src/usb_cdc.c/.h | USB CDC (virtual serial port) |
| src/usb_descr.c/.h | USB descriptors |
| src/usb_handler.c/.h | USB handler (MCU-specific) |
| src/ble_handler.c/.h | BLE handler (MCU-specific, WCH BLE library) |
| src/ble_host.h | unified BLE host API |
| ld/ch573.ld | linker script (448KB flash, 18KB RAM) |
| blobs/ | precompiled WCH Bluetooth LE library (see blobs/README.md) |
| bin/ble_remote.bin | ready-to-flash firmware, compiled with the default config.h |

## Board
The examples are made for CH573F boards with an LED on PA8 (lights up when the pin is low) and two keys to GND on PB22 (BOOT) and PB23 (KEY/RST), like the WeAct CH573F core board. Other boards can be used by changing the pins in *config.h*. If KEY (PB23) resets the chip, the RST# function has been enabled in the configuration bits (WCH ISP tool); then only the BOOT key can be used.

## Compiling and Uploading
### Install Toolchain (Linux)
```
sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf python3 python3-pip
pip install chprog rvprog
```
Alternatively, the [xPack RISC-V GCC](https://xpack-dev-tools.github.io/riscv-none-elf-gcc-xpack/) can be used (the makefile detects it). To use the USB bootloader without root privileges, grant access:
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-ch55x.rules
sudo udevadm control --reload-rules
```

### Upload via Makefile
Hold the BOOT key (PB22) while connecting the board to the PC via USB, then release it (the USB bootloader is active for a few seconds). Run one of these commands in the folder of the makefile:
```
make flash      compile and upload the example via USB bootloader (chprog)
make flash-link compile and upload via WCH-LinkE (rvprog, PB14 = SWDIO, PB15 = SWCLK)
make monitor   open serial monitor (exit with CTRL-])
make all        compile only (bin/ble_remote.bin/.hex/.asm)
make clean      remove all build files
```

### Upload without Toolchain
The folder *bin* contains the firmware of this example, compiled with the default *config.h*. Hold the BOOT key while connecting the board, then run `chprog bin/ble_remote.bin` in the folder of the example. Note that `make clean` also removes this file (it is created again by `make flash` or `make bin`).

### Serial Terminal
The board appears as a USB serial port (Linux: */dev/ttyACM0*, Windows: COMx, macOS: */dev/cu.usbmodem...*). Any terminal program can be used, the baud rate doesn't matter (e.g. `make monitor` with pyserial installed, PuTTY, the Arduino serial monitor, or a web serial terminal in Chrome). The example starts its output when the terminal opens the port.

## Hardware Test
Connect with "nRF Connect" to "CH573 Remote": write 01/00 to the LED characteristic, enable notifications for Keys and Temperature and press the keys.

## Notes
- This example compiles without warnings, but could not yet be tested on real hardware when it was written. Please report any problems.
- Bluetooth LE uses the precompiled WCH BLE library (*blobs/LIBCH57xBLE.a*, V2.20) and the 32kHz clock for timing. By default the internal RC oscillator (LSI) is used and calibrated; if the board has a 32.768kHz crystal (WeAct board), `SYS_CLK32K` can be set to 1 in *config.h*.
- Library details are described in the README of the CH573 template.

## Links
- [CH573 MCU Template](https://github.com/wagiminator/MCU-Templates)
- [WCH: CH573 Datasheet](https://www.wch-ic.com/products/CH573.html)
- [WeAct Studio: CH573F Core Board](https://github.com/WeActStudio/WeActStudio.WCH-BLE-Core)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)). The register definitions in *ch573.h* and the Bluetooth LE library in *blobs* are provided by WCH for use with their microcontrollers.
