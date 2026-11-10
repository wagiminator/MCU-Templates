# CH573F Example: Self Test (self_test)
## Overview
Automatic test of clock, timer, RTC, PWM, UART, DataFlash, chip ID, ADC (results via USB). This example is part of a collection of simple examples for CH573F boards (e.g. the WeAct CH573F core board). It uses the bare-metal CH573 template of the [MCU-Templates](https://github.com/wagiminator/MCU-Templates) repository and its libraries. The folder is self-contained: it includes all source files, libraries and the linker script needed for this example. No additional hardware has to be connected, the example only uses the components on the board (LED, BOOT key, KEY, USB, internal temperature sensor and supply voltage measurement).

## Description
```
Automatic test of the microcontroller and the template functions without any
additional hardware. The results are shown in the terminal (USB serial port). The
tests start when the terminal is opened and can be repeated with Enter or a key:

   1. System clock       measures the CPU clock (SysTick) against the RTC (32kHz
                         clock), i.e. checks PLL, SysTick and LSI calibration
   2. Timer interrupt    periodic timer interrupt (TMR0), 100 x 1ms
   3. RTC trigger        RTC trigger interrupt after 0.5s while the CPU idles
   4. Timer PWM          TMR1 PWM on PA10 (25%), duty cycle is measured by reading
                         the pin
   5. PWMX               PWM4 on PA12 (75%), measured the same way
   6. UART loopback      UART0 with internal loopback, 64 bytes at 1 Mbaud
   7. DataFlash          erase, write and read back the last 256-byte page
   8. Chip ID            MAC address and unique ID (flash ROM commands)
   9. Temperature        internal temperature sensor (ADC), plausibility check
  10. Supply voltage     VDD (ADC), plausibility check

Afterwards, the LED is on if all tests have passed and blinks if a test has
failed. Type "w" in the terminal to test the watchdog: the watchdog is started
and not fed, so the board restarts after about 0.5 seconds; after the restart,
the self test shows "watchdog" as the reason of the last reset.

Note: PA10 (also crystal input X32KI) and PA12 must not be connected to anything
during the PWM tests. If the 32.768kHz crystal is used (SYS_CLK32K = 1), test 4
is skipped.

Tested template functions: see above, plus USB device (CDC), keys, reset reasons
and the watchdog.

Settings in config.h: F_CPU, PIN_LED, TEST_FLASH (0: skip the flash write test).
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
| src/term.c/.h | terminal output via USB serial port |
| src/usb.h | USB definitions |
| src/usb_cdc.c/.h | USB CDC (virtual serial port) |
| src/usb_descr.c/.h | USB descriptors |
| src/usb_handler.c/.h | USB handler (MCU-specific) |
| src/timer.c/.h | timers TMR0..3 (interrupt, PWM) |
| src/pwm.h | PWMX (8-bit PWM) |
| src/uart.c/.h | UART0..3 |
| src/flash.c/.h | DataFlash/CodeFlash |
| ld/ch573.ld | linker script (448KB flash, 18KB RAM) |
| bin/self_test.bin | ready-to-flash firmware, compiled with the default config.h |

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
make all        compile only (bin/self_test.bin/.hex/.asm)
make clean      remove all build files
```

### Upload without Toolchain
The folder *bin* contains the firmware of this example, compiled with the default *config.h*. Hold the BOOT key while connecting the board, then run `chprog bin/self_test.bin` in the folder of the example. Note that `make clean` also removes this file (it is created again by `make flash` or `make bin`).

### Serial Terminal
The board appears as a USB serial port (Linux: */dev/ttyACM0*, Windows: COMx, macOS: */dev/cu.usbmodem...*). Any terminal program can be used, the baud rate doesn't matter (e.g. `make monitor` with pyserial installed, PuTTY, the Arduino serial monitor, or a web serial terminal in Chrome). The example starts its output when the terminal opens the port.

## Hardware Test
All 10 tests show OK (PA10 and PA12 must be unconnected). Type `w`: the board restarts after about 0.5 s and shows "watchdog" as last reset.

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
