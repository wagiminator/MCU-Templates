# CH573F Example: USB Mouse (usb_mouse)
## Overview
USB mouse: draws a circle, mouse jiggler, KEY as left mouse button. This example is part of a collection of simple examples for CH573F boards (e.g. the WeAct CH573F core board). It uses the bare-metal CH573 template of the [MCU-Templates](https://github.com/wagiminator/MCU-Templates) repository and its libraries. The folder is self-contained: it includes all source files, libraries and the linker script needed for this example. No additional hardware has to be connected, the example only uses the components on the board (LED, BOOT key, KEY, USB, internal temperature sensor and supply voltage measurement).

## Description
```
The board acts as a USB mouse (HID). A short press on the BOOT key moves the mouse
pointer along a circle (optionally drawing with the left button pressed), a long
press switches the mouse jiggler on/off (moves the pointer one pixel every few
seconds, e.g. to keep the screen awake), the LED shows the jiggler state. The
KEY works as the left mouse button.

Tested template functions: USB device (HID mouse), keys (short and long press),
SysTick (millis, delays).

Settings in config.h: CIRCLE_RADIUS, MOUSE_DRAW, JIGGLE_INTERVAL, PIN_LED, USB
descriptors.
```

## Files

| Path | Description |
|:-|:-|
| config.h | **Configuration**: board pins, example settings, USB descriptors |
| makefile | Compiles and uploads the example |
| src/main.c | the example application |
| src/system.c/.h | template core: clock, delay, RTC, sleep, reset, watchdog, flash ROM access, startup |
| src/gpio.h | template core: GPIO, pin interrupts, ADC, TouchKey |
| src/ch573.h | register definitions (WCH) |
| src/print.c/.h | printf and number output |
| src/millis.c/.h | milliseconds via SysTick |
| src/board.c/.h | board support of the examples: LED, keys, info, temperature, VDD |
| src/usb.h | USB definitions |
| src/usb_descr.c/.h | USB descriptors |
| src/usb_handler.c/.h | USB handler (MCU-specific) |
| src/usb_hid.c/.h | USB HID class |
| src/usb_mouse.c/.h | USB HID mouse |
| ld/ch573.ld | linker script (448KB flash, 18KB RAM) |
| bin/usb_mouse.bin | ready-to-flash firmware, compiled with the default config.h |

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
make all        compile only (bin/usb_mouse.bin/.hex/.asm)
make clean      remove all build files
```

### Upload without Toolchain
The folder *bin* contains the firmware of this example, compiled with the default *config.h*. Hold the BOOT key while connecting the board, then run `chprog bin/usb_mouse.bin` in the folder of the example. Note that `make clean` also removes this file (it is created again by `make flash` or `make bin`).

## Hardware Test
Short press on BOOT moves the pointer along a circle, long press switches the jiggler (LED on), KEY works as left mouse button.

## Notes
- This example compiles without warnings, but could not yet be tested on real hardware when it was written. Please report any problems.
- Library details are described in the README of the CH573 template.

## Links
- [CH573 MCU Template](https://github.com/wagiminator/MCU-Templates)
- [WCH: CH573 Datasheet](https://www.wch-ic.com/products/CH573.html)
- [WeAct Studio: CH573F Core Board](https://github.com/WeActStudio/WeActStudio.WCH-BLE-Core)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)). The register definitions in *ch573.h* and the Bluetooth LE library in *blobs* are provided by WCH for use with their microcontrollers.
