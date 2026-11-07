# Libraries for External Peripheral Devices
## Overview
This folder contains lightweight libraries for common external devices such as sensors, actuators, RF modules, displays and memories. They do not access any MCU registers directly. Instead they use the standard interface functions of the [MCU Templates](https://github.com/wagiminator/MCU-Templates) (I2C, SPI, GPIO and delay). This means they can be used with all microcontrollers of the collection.

All libraries follow the same principles:
- Simple API with only the most important functions
- Small memory footprint, no dynamic memory, no floating point
- Parameters (I2C address, pins, ranges, etc.) in the parameter section at the beginning of the header file
- Integer results in fixed-point units, e.g. temperature in 0.01°C ("2391" means 23.91°C)
- Blocking functions with timeouts, no interrupts used

## Folder Structure
| Folder | Compiler | Microcontrollers |
|:-|:-|:-|
| [gcc](https://github.com/wagiminator/MCU-Templates/tree/main/Libraries/gcc) | GCC (avr-gcc, arm-none-eabi-gcc, riscv-none-elf-gcc) | ATtiny, ATSAMD, CH32V, CH32X, PY32F, STM32 |
| [sdcc](https://github.com/wagiminator/MCU-Templates/tree/main/Libraries/sdcc) | SDCC | 8051-based: CH55x, STC8H |

Most libraries are written in portable C and exist in both folders. The SDCC versions differ only in the default pin names (e.g. P14 instead of PA4) and in the I2C library include. *oled_term* and *nrf24l01* have their own SDCC versions that use XRAM (`__xdata`) for their buffers.

## Libraries
### Sensors
| Library | Interface | gcc | sdcc | Description |
|:-|:-|:-:|:-:|:-|
| aht20 | I2C | ✓ | ✓ | AHT10/AHT20/AHT21/AHT25 temperature and humidity sensor |
| ads1115 | I2C | ✓ | ✓ | ADS1115/ADS1015 16/12-bit 4-channel ADC with programmable gain |
| bh1750 | I2C | ✓ | ✓ | BH1750 ambient light sensor (lux) |
| bme280 | I2C | ✓ | ✓ | BME280/BMP280 temperature, pressure and humidity sensor |
| dht | GPIO | ✓ | ✓ | DHT11/DHT22/AM2302 single-wire temperature and humidity sensor |
| ds18b20 | 1-Wire | ✓ | ✓ | DS18B20/DS18S20 temperature sensor (needs *onewire*) |
| hcsr04 | GPIO | ✓ | ✓ | HC-SR04 ultrasonic distance sensor |
| hx711 | GPIO | ✓ | ✓ | HX711 24-bit ADC for load cells (scales) |
| ina219 | I2C | ✓ | ✓ | INA219 bidirectional current, voltage and power monitor |
| max31855 | SPI | ✓ | ✓ | MAX31855/MAX6675 K-type thermocouple interface |
| mpu6050 | I2C | ✓ | ✓ | MPU-6050 3-axis accelerometer and 3-axis gyroscope |
| rotary | GPIO | ✓ | ✓ | Rotary encoder with switch (polling, state table) |
| sht3x | I2C | ✓ | ✓ | SHT30/SHT31/SHT35 and SHT40/SHT41/SHT45 temperature and humidity sensor |

### Actuators
| Library | Interface | gcc | sdcc | Description |
|:-|:-|:-:|:-:|:-|
| buzzer | GPIO | ✓ | ✓ | Square wave tones for passive buzzers and speakers |
| mcp4725 | I2C | ✓ | ✓ | MCP4725 12-bit DAC with EEPROM |
| pca9685 | I2C | ✓ | ✓ | PCA9685 16-channel 12-bit PWM driver for servos and LEDs |
| pcf8574 | I2C | ✓ | ✓ | PCF8574/PCF8574A 8-bit I/O expander |
| stepper | GPIO | ✓ | ✓ | Stepper motors with STEP/DIR driver (A4988, DRV8825, ...) or ULN2003 (28BYJ-48) |

### RF Modules
| Library | Interface | gcc | sdcc | Description |
|:-|:-|:-:|:-:|:-|
| kt0803 | I2C | ✓ | ✓ | KT0803K/L FM transmitter |
| nrf24l01 | SPI | ✓ | ✓ | nRF24L01+ 2.4GHz transceiver with auto-acknowledge and dynamic payload |
| rc433 | GPIO | ✓ | ✓ | 433MHz ASK/OOK transmitter for radio sockets (EV1527/PT2262 codes) |
| rda5807 | I2C | ✓ | ✓ | RDA5807 FM stereo receiver with RDS station name |
| sx1276 | SPI | ✓ | ✓ | SX1276/SX1278 (RFM95/96/98) LoRa transceiver, point-to-point |

### Displays
| Library | Interface | gcc | sdcc | Description |
|:-|:-|:-:|:-:|:-|
| lcd_i2c | I2C | ✓ | ✓ | HD44780 character LCD (16x2, 20x4, ...) with PCF8574 I2C backpack |
| max7219 | SPI | ✓ | ✓ | MAX7219/MAX7221 8-digit 7-segment display and 8x8 LED matrix |
| oled_term | I2C | ✓ | ✓ | SSD1306 128x64 OLED as scrolling text terminal |
| ssd1306_gfx | I2C | ✓ | | SSD1306/SH1106 OLED graphics with screen buffer (lines, circles, bitmaps, fonts) |
| ssd1306_txt | I2C | ✓ | ✓ | SSD1306/SH1106/SH1107 OLED text and bitmaps without screen buffer |
| st7302_gfx_soft | GPIO | ✓ | | ST7302 250x122 monochrome low-power LCD graphics (software SPI) |
| st7735_gfx | SPI | ✓ | | ST7735/ST7789/ILI9341 color TFT graphics without screen buffer |
| st7735_gfx_soft | GPIO | ✓ | | ST7735/ST7789/ILI9341 color TFT graphics (software SPI) |
| tm1637 | GPIO | ✓ | ✓ | TM1637 4-digit 7-segment display (same API as tm1650) |
| tm1650 | I2C | ✓ | ✓ | TM1650 4-digit 7-segment display |

### Memory
| Library | Interface | gcc | sdcc | Description |
|:-|:-|:-:|:-:|:-|
| eeprom_24c | I2C | ✓ | ✓ | 24C01 ... 24C512 EEPROMs and FM24C/MB85RC FRAMs |
| w25q | SPI | ✓ | ✓ | W25Qxx and compatible SPI NOR flash (up to 16MB) |

### Miscellaneous
| Library | Interface | gcc | sdcc | Description |
|:-|:-|:-:|:-:|:-|
| ds3231 | I2C | ✓ | ✓ | DS3231/DS1307 real-time clock |
| onewire | GPIO | ✓ | ✓ | 1-Wire bus master (bit-banging) with CRC8 |
| utils | - | ✓ | | Useful macros and functions (min, max, constrain, swap, map, random) |

## Usage
### Adding a Library
1. Copy the *.c* and *.h* file of the library into the *src* folder of the MCU template. Libraries that build on others (e.g. *ds18b20* needs *onewire*) need these as well.
2. Copy the required interface library (e.g. *i2c.c/h* or *spi.c/h*) from the *libraries* folder of the MCU template into the *src* folder (see below).
3. Adjust the parameters at the beginning of the header file (I2C address, pins, ranges, etc.).
4. Include the header file in your program and call the init function of the library.

```c
#include "system.h"
#include "bme280.h"

int main(void) {
  BME_init();                       // also initializes I2C (BME_INIT_I2C = 1)
  while(1) {
    int32_t  temp  = BME_getTemp();       // 2391 = 23.91°C
    uint32_t press = BME_getPressure();   // 101945 = 1019.45hPa
    uint32_t hum   = BME_getHumidity();   // 4697 = 46.97%RH
    DLY_ms(1000);
  }
}
```

### Interface Libraries
The libraries only use the following functions, which all MCU templates provide in the same way:

| Interface | Header | Functions used |
|:-|:-|:-|
| I2C | *i2c.h* (default) or *i2c_soft.h* | `I2C_init()`, `I2C_start(addr)`, `I2C_write(b)`, `I2C_read(ack)`, `I2C_stop()` |
| SPI | *spi.h* (default) or *spi_soft.h* | `SPI_init()`, `SPI_transfer(b)` |
| GPIO | *gpio.h* | `PIN_input()`, `PIN_output()`, `PIN_high()`, `PIN_low()`, `PIN_read()` |
| Delay | *system.h* (GCC) / *delay.h* (SDCC) | `DLY_us(n)`, `DLY_ms(n)` with compile-time constants |

The default include can be changed in the header of each library (line with the comment "choose your I2C/SPI library"). Notes for some microcontrollers:
- **CH32X035**: there is no hardware I2C library, use *i2c_soft.h*.
- **CH55x, STC8H**: *i2c.h* is a software I2C, the SPI library is header-only.
- *ssd1306_gfx* uses *i2c_dma.h*, *ssd1306_txt* uses *i2c_tx.h* and *st7735_gfx* uses *spi_tx.h* by default (fast write-only interfaces). These are not available for all MCUs; *i2c.h* or *spi.h* work as well.
- Set the SPI mode and clock in the SPI library according to the device (most devices use mode 0, the ST77xx displays mode 3).

The I2C or SPI interface is initialized by the init function of the device library if the parameter `xxx_INIT_I2C` or `xxx_INIT_SPI` is set to 1. If several devices share the same bus, set it to 0 for all but one of them, or initialize the bus yourself.

### Notes
- Pin names in the headers are examples (e.g. PA4), change them according to your board.
- The I2C bus needs pull-up resistors on SDA and SCL (most breakout boards have them). The 1-Wire bus (DS18B20) and the DHT data line need a 4.7k pull-up resistor.
- Pay attention to the voltage levels: 5V modules (e.g. HC-SR04, some LCDs) may need level shifters when used with 3.3V MCUs.
- The timing-critical bit-banging libraries (*onewire*, *dht*) can be disturbed by interrupts. They have an option to disable interrupts during the critical parts (`OW_INT_LOCK`, `DHT_INT_LOCK`).
- Software-generated timing (*buzzer*, *hcsr04*, *rc433*, *stepper*) depends on the accuracy of `DLY_us()`. For precise timing use the timer peripherals of the MCU.
- Some libraries use the same function names (e.g. *tm1637*/*tm1650*, *lcd_i2c*/*st7302_gfx_soft*) and can therefore not be used together in one project.
- For SDCC, buffers that are passed to the libraries can be located in any memory (generic pointers). Large buffers should be placed in XRAM (`__xdata`).

## Links
- [MCU Templates](https://github.com/wagiminator/MCU-Templates)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MCU Development Boards](https://github.com/wagiminator/Development-Boards)
- [Bosch Sensortec: BME280](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)
- [Analog Devices: DS18B20](https://www.analog.com/en/products/ds18b20.html)
- [Analog Devices: DS3231](https://www.analog.com/en/products/ds3231.html)
- [Analog Devices: MAX7219](https://www.analog.com/en/products/max7219.html)
- [Analog Devices: MAX31855](https://www.analog.com/en/products/max31855.html)
- [Texas Instruments: INA219](https://www.ti.com/product/INA219)
- [Texas Instruments: ADS1115](https://www.ti.com/product/ADS1115)
- [Texas Instruments: PCF8574](https://www.ti.com/product/PCF8574)
- [Microchip: MCP4725](https://www.microchip.com/en-us/product/MCP4725)
- [NXP: PCA9685](https://www.nxp.com/part/PCA9685PW)
- [Semtech: SX1276](https://semtech.com/products/wireless-rf/lora-core/sx1276)
- [Adafruit: BME280 Library](https://github.com/adafruit/Adafruit_BME280_Library)
- [David Johnson-Davies: Tiny BME280](https://github.com/technoblogy/tiny-bme280)
- [Stephen Denne: Tiny4kOLED](https://github.com/datacute/Tiny4kOLED)
- [Neven Boyanov: ssd1306xled](https://github.com/tinusaur/ssd1306xled)
- [rc-switch](https://github.com/sui77/rc-switch)
- [ATtiny412 PocketRadio (RDA5807)](https://github.com/wagiminator/ATtiny412-PocketRadio)

## License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License ([CC BY-SA 3.0](http://creativecommons.org/licenses/by-sa/3.0/)).
