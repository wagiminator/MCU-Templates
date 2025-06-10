# MCU Template for the tinyAVR 0-series and 1-series Microcontrollers
The tinyAVR 0-series and 1-series microcontrollers belong to Atmel/Microchip's AVR family of 8-bit microcontrollers, designed for low-power and cost-effective applications. The 0-series offers basic features and is suitable for power-constrained projects, while the 1-series provides expanded capabilities, larger memory options, and improved performance for more complex applications. Both series include features such as timers, USART, SPI, and I2C. The following microcontrollers are supported: ATtiny202, 204, 212, 214, 402, 404, 406, 412, 414, 416, 804, 806, 814, 816, 1604, 1606, 1614, 1616, and 3216. 

The *template* directory includes a makefile, a PlatformIO.ini file, an Arduino IDE wrapper file, an illustrative program, a linker script, and fundamental libraries. These elements are intended to serve as a foundation for beginning the development of bare-metal firmware in the C programming language for this particular microcontroller.

Furthermore, inside the *libraries* directory, there are optional libraries available for certain peripherals. If you intend to utilize these libraries, you need to copy them into the *src* folder within the *template* directory.

# Compiling Firmware
## Using the Makefile
To make use of the *makefile*, it's necessary to install some extra software components to fully set up the toolchain:

### Install GCC Toolchain for Compiling
```
sudo apt install build-essential gcc-avr avr-libc
```

### Install Python3 and Programming Tool
```
sudo apt install python3 python3-pip
pip install tinyupdi
```

### Compiling/Uploading
Edit the *makefile* according to the needs of the project. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make fuses     burn fuses of MCU
make clean     remove all build files
```

## Using PlatformIO
- Install [PlatformIO](https://platformio.org). Linux/Mac users may also need to install [pyenv](https://realpython.com/intro-to-pyenv).
- Click on "Open Project" and select the *template* folder with the *platformio.ini* file.
- Edit the *platformio.ini* according to the needs of the project and your programmmer.
- Connect your programmer to the target board, then click "Upload".

## Using Arduino IDE
- Install [Arduino IDE](https://www.arduino.cc/en/software).
- Install [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore) for the Arduino IDE.
- Open the *template.ino* file in the Arduino IDE.
- Select the appropriate MCU, the desired clock frequency, the programmer "SerialUPDI - 230400 baud", and the correct port.
- Then click on "Upload."

The *template.ino* sketch intentionally contains no code and serves only as an anchor for the Arduino IDE. Ensure that this .ino file has the same name as the folder in which it is located. Additionally, the subfolder "src" must be present, as this is where the code and libraries are located.

# Development Board and Example Software
Take a look [here](https://github.com/wagiminator/AVR-Development-Boards/tree/master/ATtiny814_DevBoard_v2).

# Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [AVR Programmers](https://github.com/wagiminator/AVR-Programmer)
- [AVR Development Boards](https://github.com/wagiminator/AVR-Development-Boards)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
