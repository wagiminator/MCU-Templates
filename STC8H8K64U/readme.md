# MCU Template for the STC8H8K64U Series Microcontrollers
The STC8H8K64U is a cost-effective 8-bit ultra-high speed 8051 core USB2.0 Microcontroller compatible with the MCS51 instruction set. The average instruction speed is about 12 times faster than that of the traditional 8051. It allows a clock frequency of up to 40Mhz, operates in the voltage range from 1.9V to 5.5V, and incorporates all standard peripherals such as 12-bit ADC, analog comparator, multiple timers, PWM, DMA, UART, SPI, and I2C. It has a factory built-in USB bootloader so firmware can be uploaded directly via USB without the need for an additional programming device.

The 'template' directory includes a makefile, an illustrative program, a linker script, fundamental libraries, and a programming tool. These elements are intended to serve as a foundation for beginning the development of bare-metal firmware in the C programming language for this particular microcontroller.

Furthermore, inside the 'libraries' directory, there are optional libraries available for certain peripherals. If you intend to utilize these libraries, you need to copy them into the 'include' folder within the 'template' directory.

# Libraries
|Library|Description|
|:-|:-|
|debug_serial|Basic serial debug output via UART1 or UART2 with a simple printf|
|eeprom|Read, write and erase the data flash (EEPROM/IAP)|
|i2c|Basic I2C master functions using the hardware I2C module|
|neo|Control of addressable LEDs (NeoPixel/WS2812), timing is calculated from F_CPU|
|spi|Basic SPI master functions using the hardware SPI module|
|uart|Basic functions for UART1 - UART4 (polling)|
|uart1_int|UART1 with RX interrupt and ring buffer|
|uart_tx_soft|Software UART (TX only) on any pin|
|usb/usb_cdc|USB CDC (virtual serial port)|
|usb/hid_keyboard|USB HID keyboard|
|usb/hid_mouse|USB HID mouse|
|usb/hid_composite|USB HID composite device (keyboard, consumer keys and mouse)|
|usb/hid_multitouch|USB HID multitouch digitizer|
|usb/hid_data|USB HID raw data transfer (64-byte reports)|

## USB Libraries
Each USB library consists of the class driver, the USB descriptors (*usb_descr.c/h*), the device configuration (*config.h*) and the USB handler (*usb_handler.c/h*), which is identical for all libraries. All accesses to the USB peripheral (the SIE registers are accessed indirectly via USBADR/USBDAT) take place exclusively within the USB handler; the class drivers only use the endpoint functions *USB_EP_init()*, *USB_EP_send()* and *USB_EP_receive()*. Copy all files of the selected library into the 'include' folder and adjust the *config.h* according to your needs.

Since SDCC requires the interrupt service routine to be visible in the file containing *main()*, the USB interrupt service routine must be defined in *main.c*:

```c
#include "usb_cdc.h"                      // or any other USB library

void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

void main(void) {
  P_SW2 |= 0x80;                          // enable access to extended registers
  CDC_init();                             // init USB (also starts IRC48M)
  ...
}
```

The USB peripheral is clocked by the internal 48MHz oscillator (IRC48M), which is enabled by the USB init function. The system clock (F_CPU) can be selected independently. Since the shared VID/PID pairs used by default are not suitable for commercial products, you need to use your own VID/PID for these.

# Using the Makefile
To make use of the *makefile*, it's necessary to install some extra software components to fully set up the toolchain:

## Install SDCC Toolchain for Compiling
```
sudo apt install build-essential sdcc
```

## Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install stc8usb
```

## Give Access Permission to the Factory built-in Bootloader
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="34bf", ATTR{idProduct}=="1001", MODE="666"' | sudo tee /etc/udev/rules.d/99-STC-ISP.rules
sudo udevadm control --reload-rules
```

## Compiling/Uploading
Edit the *makefile* according to the needs of the project. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile, build and keep all files
make hex       compile and build <firmware>.hex
make bin       compile and build <firmware>.bin
make flash     compile, build and upload to MCU
make clean     remove all build files
```

# Factory built-in USB Bootloader
The MCU has an embedded bootloader with USB interface, which can be used to upload firmware. The open-source platform-independent Python tool [stc8usb](https://pypi.org/project/stc8usb/) can be used for this purpose.

To initiate new uploads, the bootloader needs to be manually started. Begin by unplugging the board from the USB port and disconnecting all power sources. Then, short pin P3.2 to ground while reconnecting the board to your PC's USB port. This action triggers the chip to enter bootloader mode. Once in this mode, you can release the pin P3.2 and proceed to upload new firmware via USB by running the following command (example):

```
stc8usb -t 24000000 -f firmware.bin
```

# Development Board and Example Software
Take a look [here](https://github.com/wagiminator/Development-Boards/tree/main/STC8H8K64U_DevBoard).

# Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
