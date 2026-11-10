# Raspberry Pi Pico 2 Examples
## Overview
This is a collection of simple, practical example applications for the Raspberry Pi Pico 2 (RP2350). The examples use the bare-metal RP2350 template of the [MCU-Templates](https://github.com/wagiminator/MCU-Templates) repository and its libraries, without the Pico SDK, any HAL or IDE. Their main purpose is to test the template on real hardware, but they are also useful starting points for your own projects.

All examples work with the bare board - no additional hardware has to be connected. They only use the on-board components:

| Component | Pin | Notes |
|:-|:-|:-|
| LED | GP25 | lights up when the pin is HIGH (not on the Raspberry Pi Pico 2 W) |
| BOOTSEL button | QSPI_SS | connected to the chip select of the flash, read via software (see *board.c*) |
| USB | USB_DP/USB_DM | upload via USB bootloader (rpprog), used by the examples as serial port, keyboard, mouse or media keys |
| VSYS measurement | GP29 | VSYS/3 (supply voltage) |
| Temperature sensor | - | internal sensor (chip temperature) |

| Example | Interface | Description |
|:-|:-|:-|
| [usb_shell](usb_shell/) | USB serial | Command line via USB serial port: LED, temperature, VSYS, button, ADC, info, ... |
| [self_test](self_test/) | USB serial | Automatic test of clock, timer, RTC, DMA, flash, both cores, PIO, ADC and PWM |
| [rtc_clock](rtc_clock/) | USB serial | Clock with alarm (real-time clock, sleep, PWM), set via USB serial port |
| [usb_keyboard](usb_keyboard/) | USB HID | USB keyboard: BOOTSEL button types a text, LED shows Caps Lock |
| [usb_mouse](usb_mouse/) | USB HID | USB mouse: BOOTSEL button draws a circle or switches a mouse jiggler on/off |
| [usb_media](usb_media/) | USB HID | USB media keys: play/pause, next track and mute with the BOOTSEL button |

Each example is **self-contained** in its own folder: it has its own *README.md*, its own *config.h* with all settings of the example, its own *makefile*, the source files of the template core and of the libraries it needs (*src*), the linker script (*ld*) and the ready-to-flash firmware (*bin*). A folder can therefore be copied and used as a starting point for your own project without the rest of this collection.

## Folder Structure of an Example

| Path | Description |
|:-|:-|
| README.md | Description, configuration, compiling/uploading, hardware test, troubleshooting |
| config.h | Configuration of the example: clock, LED pin, example settings, USB descriptors |
| makefile | Compiles and uploads the example (`make flash`) |
| src/ | main.c of the example and all libraries it needs (template core, board support, USB) |
| ld/ | Linker script |
| bin/ | Ready-to-flash firmware (BIN), compiled with the default config.h; also the output folder of the makefile |

## Quick Start
### Install Upload Tool
The examples are uploaded with [rpprog](https://github.com/wagiminator/MCU-Flash-Tools) via the USB bootloader (BOOTSEL mode):
```
pip install rpprog
```
On Linux, grant access to the USB bootloader to use rpprog without root privileges:
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="2e8a", ATTR{idProduct}=="000f", MODE="666"' | sudo tee /etc/udev/rules.d/99-rp2350.rules
sudo udevadm control --reload-rules
```

### Upload without Toolchain
Hold the BOOTSEL button while connecting the board via USB, then release it (the board appears as a USB drive named RP2350). Upload the ready-to-flash firmware of an example, the board restarts and runs the example:
```
rpprog -f usb_shell/bin/usb_shell.bin
```

### Install Toolchain (Linux)
```
sudo apt install build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi
```

### Compile and Upload
Hold the BOOTSEL button while connecting the board via USB (examples with USB serial port are restarted into the bootloader automatically), then:
```
cd usb_shell                     change into the folder of an example
make flash                       compile and upload the example
make clean                       remove all build files
```
See the README of the respective example for details. A good order for testing a new board is self_test, usb_shell, rtc_clock, usb_keyboard, usb_mouse and usb_media.

## How the Software is Built Up
```
  src/main.c                     application (your code)
        |
  board, term, shell             helper libraries of the examples
        |
  usb_cdc / usb_keyboard / ...   USB class drivers (identical for all templates)
  usb_handler                    MCU-specific USB handler
  timer, flash, multicore, ...   peripheral libraries of the template
        |
  system, gpio, millis, print    template core
```

## Hardware Test Checklist
As the examples are primarily meant to test the template, here is what to check on a real board:

| Example | What to check | Tests |
|:-|:-|:-|
| usb_shell | Serial port appears, `info` shows plausible chip revision/unique ID, `led` works, `temp` shows ~20..45 °C, `vsys` ~4.8 V on USB, `button` follows BOOTSEL, `adc` changes when touching GP26..GP28, `bootloader` opens the USB drive | USB CDC, GPIO, BOOTSEL, ADC, temperature sensor, ring oscillator, reset reasons, watchdog reset, boot ROM |
| self_test | All 10 tests pass, `w` resets the board and the reason 'watchdog' is shown | clock/PLL, tick generator, SysTick, timer, RTC, sleep, DMA/CRC, flash, multicore, spinlocks, PIO, ADC, PWM, watchdog |
| rtc_clock | Time runs correctly (compare with PC clock for some minutes), alarm triggers at the set time, BOOTSEL stops it | RTC (time, date, alarm interrupt), sleep, PWM |
| usb_keyboard | Text is typed into an editor, long press switches Caps Lock, LED follows Caps Lock of the PC | USB HID (keyboard, output report), BOOTSEL |
| usb_mouse | Pointer moves along a circle, jiggler moves the pointer every 30 s | USB HID (mouse), BOOTSEL, delay |
| usb_media | Media player pauses/plays, skips and mutes | USB HID (consumer control), BOOTSEL (short, long, double) |

## Notes
- The Raspberry Pi Pico 2 W is not supported: its LED is connected to the CYW43439 wireless chip and GP29 (VSYS measurement) is shared with it. Most other functions work on it as well.

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
