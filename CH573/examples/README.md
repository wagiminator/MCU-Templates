# CH573F Examples
## Overview
This is a collection of simple, practical example applications for CH573F boards (e.g. the WeAct CH573F core board). The examples use the bare-metal CH573 template of the [MCU-Templates](https://github.com/wagiminator/MCU-Templates) repository and its libraries, without the WCH SDK or an IDE. Their main purpose is to test the template and its libraries on real hardware, but they are also useful starting points for your own projects.

All examples work with the bare board - no additional hardware has to be connected. They only use the on-board components:

| Component | Pin | Notes |
|:-|:-|:-|
| LED | PA8 | lights up when the pin is low |
| BOOT key | PB22 | to GND, held at power-on starts the USB bootloader |
| KEY | PB23 | to GND, works as RST# if enabled in the configuration bits |
| USB | PB10/PB11 | upload via USB bootloader, used by the examples as serial port, keyboard, mouse or media keys |
| Bluetooth LE | ANT | antenna on the board |
| Temperature, VDD | - | internal sensor and supply voltage measurement (ADC) |

| Example | Interface | Description |
|:-|:-|:-|
| [usb_shell](usb_shell/) | USB serial | Command line via USB serial port (board info, LED, keys, ADC, TouchKey, temperature, VDD) |
| [self_test](self_test/) | USB serial | Automatic test of clock, timer, RTC, PWM, UART, DataFlash, chip ID, ADC (results via USB) |
| [rtc_clock](rtc_clock/) | USB serial | Clock with daily alarm using the RTC, set via USB terminal, CPU sleeps in between |
| [usb_keyboard](usb_keyboard/) | USB HID | USB keyboard: keys type text, temperature and voltage, LED shows Caps Lock |
| [usb_mouse](usb_mouse/) | USB HID | USB mouse: draws a circle, mouse jiggler, KEY as left mouse button |
| [usb_media](usb_media/) | USB HID | USB media keys: play/pause, next track, mute, volume up/down |
| [ble_shell](ble_shell/) | Bluetooth LE + USB serial | Command line via Bluetooth LE (Nordic UART Service), USB <-> BLE message bridge |
| [ble_beacon](ble_beacon/) | Bluetooth LE + USB serial | Bluetooth LE beacon: Eddystone-URL, iBeacon, Eddystone-UID, sensor beacon |
| [ble_remote](ble_remote/) | Bluetooth LE + USB serial | Own GATT server: switch LED, read keys (notify), temperature (Environmental Sensing) |

Each example is **self-contained** in its own folder: it has its own *README.md*, its own *config.h* with all settings of the example, its own *makefile*, the source files of the template core and of the libraries it needs (*src*), the linker script (*ld*), the WCH Bluetooth library (*blobs*, only Bluetooth examples) and the ready-to-flash firmware (*bin*). A folder can therefore be copied and used as a starting point for your own project without the rest of this collection.

## Quick Start
### Install Upload Tool
```
pip install chprog
```

### Upload without Toolchain
Hold the BOOT key (PB22) while connecting the board via USB, then release it. Upload the ready-to-flash firmware of an example:
```
chprog usb_shell/bin/usb_shell.bin
```

### Install Toolchain (Linux)
```
sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
```

### Compile and Upload
Hold the BOOT key while connecting the board via USB, then:
```
cd usb_shell                     change into the folder of an example
make flash                       compile and upload the example
make clean                       remove all build files
```
See the README of the respective example for details. A good order for testing a new board is self_test, usb_shell, rtc_clock, usb_keyboard, usb_mouse, usb_media, ble_shell, ble_beacon and ble_remote.

## How the Software is Built Up
```
  src/main.c                     application (your code)
        |
  board, term, shell             helper libraries of the examples
        |
  usb_cdc / usb_keyboard / ...   USB class drivers (same as for other templates)
  ble_uart / ble_beacon          BLE application libraries (MCU-independent)
        |
  usb_handler                    MCU-specific USB handler
  ble_host.h -> ble_handler      unified BLE host API, MCU-specific BLE handler
                                 (WCH BLE library in blobs)
  timer, pwm, uart, flash, ...   peripheral libraries of the template
        |
  system, gpio, millis, print    template core
```

## Hardware Test Checklist
| Example | What to check |
|:-|:-|
| usb_shell | Open the terminal: welcome message with chip ID 0x73, MAC and unique ID. Try `led on`, `blink 5`, `temp`, `vdd`, `keys`, `adc a4`, `touch a12` (touch the pin), `uptime`, `reset`. Keys toggle the LED. |
| self_test | All 10 tests show OK (PA10 and PA12 must be unconnected). Type `w`: the board restarts after about 0.5 s and shows "watchdog" as last reset. |
| rtc_clock | The time runs and is shown every second, the LED flashes every second. Set an alarm one minute ahead: the LED blinks and an alarm message appears; a key stops it. |
| usb_keyboard | Short press on BOOT types the text with temperature and uptime into a text editor, a long press toggles Caps Lock (LED follows), KEY types the voltage. |
| usb_mouse | Short press on BOOT moves the pointer along a circle, long press switches the jiggler (LED on), KEY works as left mouse button. |
| usb_media | BOOT: play/pause, next track (double press), mute (long press). KEY: volume up/down (short/long press). |
| ble_shell | Connect with a BLE terminal app to "CH573 Shell", type `help`, `info`, `temp`, `led on`. Lines typed in the USB terminal appear on the smartphone, `msg hello` appears in the USB terminal. |
| ble_beacon | The beacon is visible in "nRF Connect" or a beacon scanner app; each press on BOOT switches the type (LED flashes 1..4 times). The sensor beacon updates temperature and counter every 5 s. |
| ble_remote | Connect with "nRF Connect" to "CH573 Remote": write 01/00 to the LED characteristic, enable notifications for Keys and Temperature and press the keys. |

## Notes
- The firmware runs at 60 MHz (PLL from the 32 MHz crystal), which is required for USB and recommended for Bluetooth LE.

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
