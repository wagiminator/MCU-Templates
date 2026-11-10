# MCU Template for CH573 and CH571 Microcontrollers
## Overview
The CH573 is a low-cost 32-bit RISC-V microcontroller from WCH with integrated Bluetooth Low Energy (BLE 4.2) and USB. It is based on the QingKe RISC-V3A core (RV32IMAC instruction set) and runs at up to 60MHz using the PLL (480MHz) of the 32MHz crystal oscillator. It has 448KB flash for code, 32KB DataFlash, 18KB SRAM and a supply voltage range of 1.7V (QFN32) or 2.3V to 3.6V. The chip has the following peripherals:
- 2.4GHz Bluetooth Low Energy transceiver (BLE 4.2, -96dBm RX sensitivity, up to +5dBm TX power)
- USB 2.0 full-speed device and host controller with integrated PHY
- 4 UARTs, SPI, 4 26-bit timers with capture and PWM, 8 8-bit PWM channels (PWMX)
- 12-bit ADC with temperature sensor, capacitive touch key detection
- RTC with 32kHz clock (internal RC oscillator or external crystal), watchdog
- DC-DC converter, several sleep modes down to 0.3uA, battery voltage monitor

It has a factory built-in USB bootloader. The CH571 is a simplified version of the CH573 with less flash (192KB), without USB host, touch key, PWMX and UART2/3.

This template is a lightweight starting point for bare-metal firmware development in C without any HAL or closed-source IDE. It contains:
- a makefile and a PlatformIO configuration
- linker scripts
- a startup and system configuration
- basic GPIO, ADC and touch key functions
- an example program

Optional libraries for the internal peripherals including USB and Bluetooth LE are also included.

## Supported Microcontrollers

| Part | Package | Flash | DataFlash | SRAM | GPIO | ADC Channels | UART | Timer | PWMX | Touch Key | USB Host/Device | BLE |
|:-|:-|-:|-:|-:|-:|-:|-:|-:|-:|-:|:-:|:-:|
| CH573X | QFN32 | 448KB | 32KB | 18KB | 22 | 10+1 | 4 | 4 | 8 | 10 | ✓/✓ | ✓ |
| CH573Q | LQFP32 | 448KB | 32KB | 18KB | 22 | 10+1 | 4 | 4 | 8 | 10 | ✓/✓ | ✓ |
| CH573F | QFN28 | 448KB | 32KB | 18KB | 20 | 8+1 | 4 | 4 | 6 | 8 | ✓/✓ | ✓ |
| CH571F | QFN28 | 192KB | 32KB | 18KB | 20 | 6+1 | 2 | 4 | - | - | -/✓ | ✓ |
| CH571D, CH571K | QFN20, ESSOP10 | 192KB | 32KB | 18KB | see datasheet | | | | | | | |

Common features of all variants:
- QingKe RISC-V3A core, RV32IMAC instruction set, hardware interrupt stack, up to 60MHz
- 18KB SRAM (16KB + 2KB, retained in sleep mode), 32KB DataFlash, 24KB bootloader
- External 32MHz crystal (HSE) required for clock and BLE, PLL 480MHz; internal 32kHz RC oscillator (LSI) or optional 32.768kHz crystal (LSE) for RTC and BLE timing
- 12-bit ADC with internal temperature sensor and supply voltage measurement
- 2-wire serial debug interface (SWDIO on PB14, SWCLK on PB15, must be enabled with the WCH ISP tool)
- Supply voltage 2.3V - 3.6V (CH573X: 1.7V - 3.6V), 5V tolerant pins PB12 - PB15

The CH582/CH583 (QingKe V4A core, 32KB SRAM, 2 USB ports, I2C, different BLE library) are not covered by this template.

## Pinout and Peripheral Mapping
### CH573F/CH571F (QFN28)

| Pin | Port | ADC | UART (UARTx_REMAP) | SPI (SPI_REMAP) | Timer (TMRx_REMAP) | PWMX | Other Functions |
|:-:|:-|:-|:-|:-|:-|:-|:-|
| 1 | PA11 |  |  |  | TMR2 (0) |  | X32KO (32.768kHz crystal) |
| 2 | PA10 |  |  |  | TMR1 (0) |  | X32KI (32.768kHz crystal) |
| 3 | VDCID |  |  |  |  |  | Digital LDO input |
| 4 | VSW |  |  |  |  |  | DC-DC switch output |
| 5 | VDD33 |  |  |  |  |  | Supply voltage 2.3V - 3.6V |
| 6 | PA8 | AIN12 | U1 RX (0) |  |  |  |  |
| 7 | PA9 | AIN13 | U1 TX (0) |  | TMR0 (0) |  | debug_serial TX |
| 8 | PB15 |  |  | MISO (1) |  |  | SWCLK (if enabled) |
| 9 | PB14 |  |  | MOSI (1) |  | PWM10 | SWDIO (if enabled), NeoPixel (neo_spi, 1) |
| 10 | PB13 |  | U1 TX (1) | SCK (1) |  |  |  |
| 11 | PB12 |  | U1 RX (1) | SCS (1) |  |  |  |
| 12 | PB11 |  |  |  | TMR2 (1) |  | USB D+ |
| 13 | PB10 |  |  |  | TMR1 (1) |  | USB D- |
| 14 | PB7 |  | U0 TX (0) |  |  | PWM9 |  |
| 15 | PB4 |  | U0 RX (0) |  |  | PWM7 |  |
| 16 | PB23 |  | U2 TX |  | TMR0 (1) | PWM11 | RST# (if enabled) |
| 17 | PB22 |  | U2 RX |  | TMR3 |  | BOOT (low at power-up = bootloader) |
| 18 | X32MO |  |  |  |  |  | 32MHz crystal |
| 19 | X32MI |  |  |  |  |  | 32MHz crystal |
| 20 | VINTA |  |  |  |  |  | Analog power node |
| 21 | ANT |  |  |  |  |  | Antenna |
| 22 | VDCIA |  |  |  |  |  | Analog LDO input |
| 23 | PA4 | AIN0 | U3 RX |  |  |  |  |
| 24 | PA5 | AIN1 | U3 TX |  |  |  |  |
| 25 | PA15 | AIN5 | U0 RX (1) | MISO (0) |  |  |  |
| 26 | PA14 | AIN4 | U0 TX (1) | MOSI (0) |  |  | NeoPixel (neo_spi, 0) |
| 27 | PA13 | AIN3 |  | SCK (0) |  | PWM5 | i2c_soft SDA, spi_soft SCK |
| 28 | PA12 | AIN2 |  | SCS (0) |  | PWM4 | i2c_soft SCL |
| 0 | GND |  |  |  |  |  | Ground (exposed pad) |

Notes:
- The numbers in brackets are the mapping options that are selected in the parameter section of the respective library header (*uart.h*, *spi.h*, *timer.h*, *neo_spi.h*). U0 ... U3 stand for UART0 ... UART3.
- The CH573X/CH573Q (32 pins) additionally have PB0 (AIN8, PWM6) and PB6 (AIN9, PWM8).
- PB22 is the BOOT detection pin: if it is pulled low at power-up (e.g. BOOT key to GND), the chip starts the USB bootloader. A software reset always starts the application.
- PB23 can be configured as reset pin (RST#), PB14/PB15 as 2-wire debug interface with the WCH ISP tool. If enabled, these pins cannot be used as GPIO.
- If a 32.768kHz crystal is used (`SYS_CLK32K` = 1), PA10 and PA11 cannot be used as GPIO.
- There is no I2C hardware in the CH573, use the *i2c_soft* library instead (no DMA/interrupt variant).

## Template Structure

| Path | Description |
|:-|:-|
| template/makefile | Makefile for compiling and uploading (GCC RISC-V toolchain, chprog/rvprog) |
| template/platformio.ini | Configuration file for PlatformIO |
| template/ld/ | Linker scripts (ch573.ld, ch571.ld) |
| template/src/main.c | Example program (blinking LED on PA8) |
| template/src/system.c/h | Startup code, clock, DC-DC, 32kHz clock, delay, RTC, reset, watchdog, sleep, battery monitor, flash ROM and interrupt functions |
| template/src/gpio.h | GPIO, pin interrupt, ADC (incl. temperature and supply voltage) and touch key functions |
| template/src/ch573.h | Register definitions (WCH) plus SysTick and interrupt controller (PFIC) |
| libraries/ | Optional libraries for internal peripherals (see below) |
| datasheet/ | Datasheet and WCH BLE software development reference manual |

## System Functions
All settings of the system functions are in the parameter section of *system.h*. They can also be overridden in an optional *config.h* in the *src* folder (included automatically if present).

| Function group | Description |
|:-|:-|
| `CLK_*` | System clock (`F_CPU` = 60/48/40/32/30/24/20/16/15MHz via PLL or 8/6.4/4/3.2/2MHz from HSE), set via makefile |
| `DCDC_*` | Enable/disable internal DC-DC converter (`SYS_DCDC_EN`, only if inductor is fitted) |
| `CLK32K_*`, `LSI_*` | 32kHz clock for RTC and BLE: internal RC oscillator incl. calibration or 32.768kHz crystal (`SYS_CLK32K`) |
| `DLY_*`, `STK_*` | Delay functions and system ticks (64-bit SysTick, runs at F_CPU) |
| `RTC_*` | Real-time clock (day counter + 2-second ticks), trigger (alarm) and periodic timer, interrupts |
| `IDLE_now()`, `HALT_now()`, `SLEEP_now()`, `SHUTDOWN_now()`, `SLEEP_ms()` | Sleep modes, wake-up by RTC, GPIO or USB (`WAKE_*`) |
| `RST_*` | Software reset and reset cause |
| `WDT_*` | Watchdog timer (max. about 2 seconds at 60MHz) |
| `BAT_*` | Battery (supply voltage) low monitor |
| `ESIG_*` | Chip ID, unique ID, factory MAC address, temperature sensor calibration |
| `FLASH_ROM_*` | Low-level flash access (used by the *flash* library) |
| `INT_*`, `NVIC_*` | Global interrupt enable/disable, atomic block, interrupt controller |

Functions that must run from RAM (e.g. while the flash is busy) are marked with `RAM_FUNC`; the startup code copies them into SRAM.

## Libraries
The *libraries* folder contains optional libraries for the internal peripherals. To use a library, copy the corresponding *.c* and *.h* files into the *src* folder of the template and include the header file in your program. The parameters (e.g. pin mapping, baud rate, clock rate) can be adjusted in the parameter section at the beginning of the respective header file.

| Library | Description |
|:-|:-|
| debug_serial | Serial debug output (TX only) via UART1 (PA9) with simple printf |
| flash | Read, erase and write the 32KB DataFlash and the CodeFlash |
| i2c_soft | Software I2C master (bit-banging) on any pins |
| millis | Millisecond and microsecond counter using the SysTick timer |
| neo_spi | NeoPixel (WS2812) control using hardware SPI (MOSI pin PA14 or PB14) |
| print | Print functions (printf, numbers, strings) for the serial and other libraries |
| pwm | 8-bit PWM (PWMX, channels 4 - 11) |
| spi | SPI0 master functions |
| spi_dma | SPI0 master functions with DMA (background transfers of buffers) |
| spi_soft | Software SPI master (bit-banging) on any pins |
| timer | Timer TMR0 ... TMR3 (26-bit) with interrupt and PWM output |
| uart | Basic UART0/1/2/3 functions (no buffer, no interrupt, no DMA) |
| uart_int | UART0..3 (selectable) with RX interrupt and ring buffer |
| usb/usb_cdc | USB CDC (virtual serial port) |
| usb/hid_keyboard | USB HID keyboard |
| usb/hid_mouse | USB HID mouse |
| usb/hid_consumer | USB HID consumer control (multimedia keys) |
| usb/hid_composite | USB HID composite device (keyboard, consumer and mouse) |
| ble/ble_uart | Bluetooth LE UART (Nordic UART Service, virtual serial port) |
| ble/ble_beacon | Bluetooth LE beacon (iBeacon, Eddystone-URL, Eddystone-UID, raw data) |
| blobs | Precompiled WCH Bluetooth LE library (required by the BLE libraries) |
| usb_host/host_keyboard | USB host for a keyboard (boot protocol, ASCII conversion) |
| usb_host/host_mouse | USB host for a mouse (boot protocol) |
| usb_host/host_gamepad | USB host for a HID gamepad/joystick (report descriptor parser) |

All I2C, SPI and UART libraries provide functions for buffers with the same API (e.g. `I2C_sendBuffer(addr,buf,len)`, `I2C_getBuffer(addr,buf,len)`, `I2C_writeBuffer(buf,len)`, `I2C_readBuffer(buf,len)`, `SPI_writeBuffer(buf,len)`, `SPI_readBuffer(buf,len)`, `UART_writeBuffer(buf,len)`, `UART_readBuffer(buf,len)`). The *_dma* and *_int* variants transfer write buffers in the background; in this case the buffer must not be changed until the respective `..._busy()` function returns 0.

### Using the USB Libraries
Each USB library consists of the following files:
- the class driver (e.g. *usb_cdc.c/h*)
- the USB descriptors (*usb_descr.c/h*)
- the device configuration (*config.h*)
- the USB handler (*usb_handler.c/h*), which contains all MCU-specific parts incl. the register accesses
- the USB definitions (*usb.h*)

To use a library, copy all of its files into the *src* folder. Adjust the *config.h* (e.g. vendor and product strings, VID/PID) according to your needs. The library is started with its init function (e.g. `CDC_init()`). The default VID/PID pairs are shared IDs that are only suitable for private projects.
Additionally:
- The interrupt vector table must be enabled (`SYS_USE_VECTORS` = 1 in *system.h*, default).
- USB requires a system clock generated by the PLL (`F_CPU` = 60MHz, 48MHz, ...).
- The class drivers are the same as for the other templates of this repository, only the USB handler is MCU-specific.

### Using the Bluetooth LE Libraries
The Bluetooth LE libraries follow the same concept as the USB libraries: an MCU-specific handler with a unified API and application-oriented, MCU-independent libraries on top of it. Each BLE library consists of the following files:
- the application library (e.g. *ble_uart.c/h* or *ble_beacon.c/h*), MCU-independent
- the unified BLE host API (*ble_host.h*): GAP peripheral (advertising, one connection) and GATT server (services, characteristics, notifications)
- the BLE handler (*ble_handler.c/h*), which contains all MCU-specific parts: it starts the precompiled WCH BLE library (controller and host stack, TMOS task scheduler) and implements the unified API on top of it
- the configuration (*config.h*): device name, TX power, number of attributes, ATT MTU, memory

To use a library, copy all of its files into the *src* folder and copy the *blobs* folder into the project folder (the makefile also finds *../libraries/blobs*, so the template can be built inside this repository without copying). The makefile links the WCH BLE library automatically if *ble_handler.c* is in the *src* folder. The library is started with its init function (e.g. `BUART_init()`), `BLE_poll()` (or the poll function of the library) must be called frequently. The API is identical to the one of the ESP32-C3 template, so applications based on *ble_host.h* can be used on both microcontrollers. Own GATT services can be created directly with the functions of *ble_host.h* (e.g. `BLE_addService16()`, `BLE_addChar128()`, `BLE_notify()`).
Additionally:
- The interrupt vector table must be enabled (`SYS_USE_VECTORS` = 1, default). The BLE library uses the BB and LLE interrupts.
- `F_CPU` must be at least 24MHz (default 60MHz). SysTick must be running (`SYS_TICK_INIT` = 1, default).
- The RTC is used as time base of the BLE stack: RTC triggers, `RTC_init()` and the sleep functions must not be used while BLE is active.
- The internal 32kHz RC oscillator (LSI) is calibrated regularly; for better accuracy and lower power consumption, a 32.768kHz crystal on PA10/PA11 can be used (`SYS_CLK32K` = 1).
- The WCH BLE library needs about 100KB flash and 6 - 8KB RAM (`BLE_MEMHEAP_SIZE`).
- The BLE library is closed source and licensed by WCH for use with their microcontrollers only (see *libraries/blobs/README.md*).

### Using the USB Host Libraries
Each USB host library consists of the following files:
- the class driver (e.g. *host_keyboard.c/h*)
- the configuration (*config.h*)
- the USB host handler (*usb_host.c/h*), which contains all MCU-specific parts incl. the register accesses
- the USB definitions (*usb.h*)

To use a library, copy all of its files into the *src* folder. The library is started with its init function (e.g. `KBD_init()`); the update function (e.g. `KBD_update()`) must be called frequently, it handles connecting/disconnecting and enumerating the device and polls its reports. A single HID device can be connected directly to the USB port (PB10/PB11); hubs are not supported, and for composite devices only the first matching interface is used. The host works without interrupts. The class drivers are the same as for the other templates with USB host libraries.
Additionally:
- VBUS (5V) for the connected device must be provided by the board.
- The system clock must be generated by the PLL (e.g. `F_CPU` = 60MHz). The CH571 has no USB host function.
- The USB device libraries and the USB host libraries cannot be used at the same time.

## Compiling Firmware
### Using the Makefile
To use the *makefile*, some additional software components must be installed to set up the toolchain.

#### Install GCC Toolchain for Compiling
```
sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
```

Alternatively, the [xPack RISC-V GCC](https://xpack-dev-tools.github.io/riscv-none-elf-gcc-xpack/) toolchain (riscv-none-elf) can be used, the makefile detects it automatically.

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
Edit the *makefile* according to the needs of the project (e.g. `F_CPU`, `LDSCRIPT`; use *ld/ch571.ld* for the CH571). Use one of the following commands inside the *template* folder to compile/upload:

```
make all        compile and build <firmware>.elf/.bin/.hex/.asm
make hex        compile and build <firmware>.hex
make asm        compile and disassemble to <firmware>.asm
make bin        compile and build <firmware>.bin
make flash      compile and upload to MCU via USB bootloader (chprog)
make flash-link compile and upload to MCU via WCH-LinkE (rvprog)
make clean      remove all build files
```

### Using PlatformIO
- Install [PlatformIO](https://platformio.org) and [platform-ch32v](https://github.com/Community-PIO-CH32V/platform-ch32v). Follow [these instructions](https://pio-ch32v.readthedocs.io/en/latest/installation.html) to do so. Linux/Mac users may also need to install [pyenv](https://realpython.com/intro-to-pyenv).
- Click on "Open Project" and select the *template* folder with the *platformio.ini* file.
- Edit the *platformio.ini* according to the needs of the project and your programmer. For Bluetooth LE, the WCH BLE library must be linked (see comment in *platformio.ini*).
- Connect the WCH-LinkE to the target board, then click "Upload".

## Uploading Firmware
### Via Factory built-in USB Bootloader
#### Installing Drivers for the Bootloader
On Linux you do not need to install a driver, but you need to grant access permissions (see above). On Windows, you need the [CH372 driver](http://www.wch-ic.com/downloads/CH372DRV_EXE.html). Alternatively, you can use the [Zadig](https://zadig.akeo.ie/) tool: click "Options" -> "List All Devices", select the USB module and install the libusb-win32 driver. To do this, the board must be connected and the microcontroller must be in bootloader mode.

#### Entering Bootloader Mode
Disconnect the board from the USB port and all voltage sources. Press and hold the BOOT button (which pulls PB22 low) while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, the BOOT button can be released and new firmware can be uploaded via USB within the next couple of seconds. Unlike other WCH microcontrollers, the CH573 cannot jump into the bootloader by software.

#### Uploading Firmware Binary
- **chprog**: If [Python3](https://www.pythontutorial.net/getting-started/install-python/) is installed, use the platform-independent open-source command-line tool [chprog](https://pypi.org/project/chprog/) (`pip install chprog`):
  ```
  chprog <firmware>.bin
  ```
- **Web app**: Use the [WCH USB Programmer web app](https://wagiminator.github.io/apps/chprog/). It only works in Chrome, Edge or Opera on a desktop computer. On Linux, the udev rules mentioned above are required. On Windows, the web app needs the WinUSB driver (install it with [Zadig](https://zadig.akeo.ie/)); the libusb-win32 and CH372 drivers do not work in browsers.
- **WCHISPTool**: WCH offers the free but closed-source software [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) for Windows. Open the *<firmware>.hex* file and upload it to the microcontroller. This tool is also used to change the configuration bits (e.g. enable RST# on PB23 or the 2-wire debug interface).

### Via 2-Wire Serial Debug Interface
#### WCH-LinkE Programming and Debugging Device
To program the microcontroller via its serial debug interface (SWD), you need the [WCH-LinkE](http://www.wch-ic.com/products/WCH-Link.html) (pay attention to the "E" in the name), which can be purchased commercially. On the CH573, the debug interface is disabled by default and must first be enabled with the WCHISPTool via the USB bootloader. On Linux, grant access permissions beforehand (see above). On Windows, if necessary, you can install the WinUSB driver for WCH interface 1 using the [Zadig](https://zadig.akeo.ie/) tool.

Make the following connections between the WCH-LinkE and the MCU board:

```
WCH-LinkE      MCU Board
+-------+      +-------+
|  SWCLK| ---> |PB15   |
|  SWDIO| <--> |PB14   |
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

## Notes and Tips
- The template uses `F_CPU` = 60MHz by default (PLL from the 32MHz crystal). The CH573 has no internal high-speed RC oscillator, so the 32MHz crystal is mandatory.
- Some registers (clock, sleep, watchdog, ...) are protected and can only be written in safe access mode; the template functions take care of this (`SAFE_ACCESS` macro in *system.h*).
- SysTick is a 64-bit down counter on the CH573; `STK_getTicks()` returns the elapsed ticks since startup.
- The register definitions in *ch573.h* are taken from WCH (CH573SFR.h) and extended by the definitions for SysTick and the interrupt controller.
- The factory MAC address used for Bluetooth can be read with `ESIG_getMAC()`, a unique ID with `ESIG_getUID()`.
- The *CH573F-Examples* collection (USB shell, self test, RTC clock, USB keyboard/mouse/media keys, BLE shell, BLE beacon, BLE remote) shows how to use the template and its libraries on a CH573F core board without additional hardware.

## Development Boards
- [WeAct Studio: CH573F Core Board](https://github.com/WeActStudio/WeActStudio.WCH-BLE-Core) (LED on PA8, BOOT key on PB22, second key on PB23, 32.768kHz crystal)

## Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [chprog on PyPI](https://pypi.org/project/chprog/)
- [rvprog on PyPI](https://pypi.org/project/rvprog/)
- [WCH USB Programmer Web App](https://wagiminator.github.io/apps/chprog/)
- [WCH RISC-V Programmer Web App](https://wagiminator.github.io/apps/rvprog/)
- [WCH: CH573 Product Page](https://www.wch-ic.com/products/CH573.html)
- [WCH: CH573 Datasheet](https://www.wch-ic.com/downloads/CH573DS1_PDF.html)
- [WCH: CH573 EVT (SDK and examples)](https://www.wch-ic.com/downloads/CH573EVT_ZIP.html)
- [ch32fun (CH5xx support)](https://github.com/cnlohr/ch32fun)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)). The register definitions (*ch573.h*) and the Bluetooth LE library (*libraries/blobs*) are provided by WCH for use with their microcontrollers.
