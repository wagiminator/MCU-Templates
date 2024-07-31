# MCU Template for CH32X035/X034/X033 Microcontrollers
CH32X035 is a cost-effective microcontroller based on the QingKe 32-bit RISC-V4C core supporting the RV32IMAC instruction set and self-extending instructions. CH32X035 has built-in USB and PD PHY, supports USB Host and USB Device functions, USB PD and type C fast charging functions, built-in programmable protocol I/O controller, provides 2 groups of OPA, 3 groups of CMP, 4 groups of USART, I2C, SPI, multiple timers, 12-bit ADC, 14-channel Touchkey, and other rich peripheral resources. It can operate at clock frequencies of up to 48MHz and at a supply voltage range of 2.0V - 5.5V. CH32V035 is equipped with 62KB flash, 20KB SRAM, and an embedded USB bootloader.

The *template* directory includes a makefile, a PlatformIO.ini file, an illustrative program, a linker script, fundamental libraries, and a programming tool. These elements are intended to serve as a foundation for beginning the development of bare-metal firmware in the C programming language for this particular microcontroller.

Furthermore, inside the *libraries* directory, there are optional libraries available for certain peripherals. If you intend to utilize these libraries, you need to copy them into the *src* folder within the *template* directory.

# Using the Makefile
To make use of the Makefile, it's necessary to install some extra software components to fully set up the toolchain:

## Install GCC toolchain for compiling
```
sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
```

## Install Python3 and PyUSB for the programming tool
```
sudo apt install python3 python3-pip
python3 -m pip install pyusb
```

## Give access permission to the factory built-in bootloader
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-ch55x.rules
sudo udevadm control --reload-rules
```

## Give access permission to the WCH-LinkE programmer
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8010", MODE="666"' | sudo tee /etc/udev/rules.d/99-WCH-LinkE.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8012", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-WCH-LinkE.rules
sudo udevadm control --reload-rules
```

## Compiling/Uploading
Edit the *makefile* according to the needs of the project. Use one of the following commands inside the *template* folder to compile/upload:

```
make all       compile and build <firmware>.elf/.bin/.hex/.asm
make hex       compile and build <firmware>.hex
make asm       compile and disassemble to <firmware>.asm
make bin       compile and build <firmware>.bin
make flash     compile and upload to MCU
make clean     remove all build files
```

# Using PlatformIO
- Install [PlatformIO](https://platformio.org) and [platform-ch32v](https://github.com/Community-PIO-CH32V/platform-ch32v). Follow [these instructions](https://pio-ch32v.readthedocs.io/en/latest/installation.html) to do so. Linux/Mac users may also need to install [pyenv](https://realpython.com/intro-to-pyenv).
- Click on "Open Project" and select the *template* folder with the *platformio.ini* file.
- Edit the *platformio.ini* according to the needs of the project and your programmmer.
- Connect the WCH-LinkE to the target board, then click "Upload".

# Links
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
