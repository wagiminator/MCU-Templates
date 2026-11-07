// ===================================================================================
// Basic MAX31855/MAX6675 Thermocouple Interface Functions                    * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "max31855.h"

// Init CS pin (and SPI)
void THERMO_init(void) {
  PIN_high(THERMO_PIN_CS);
  PIN_output(THERMO_PIN_CS);
  #if THERMO_INIT_SPI > 0
  SPI_init();
  #endif
}

// Read temperature in 0.25°C
int16_t THERMO_read(void) {
  uint16_t data;
  PIN_low(THERMO_PIN_CS);
  data  = (uint16_t)SPI_transfer(0) << 8;
  data |= SPI_transfer(0);
  #if THERMO_TYPE == 6675
  PIN_high(THERMO_PIN_CS);                        // MAX6675: 16 bits
  if(data & 0x0004) return THERMO_ERROR;          // open thermocouple
  return (int16_t)(data >> 3);                    // 12 bits, 0.25°C
  #else
  {
    uint8_t fault;
    SPI_transfer(0);                              // MAX31855: 32 bits
    fault = SPI_transfer(0) & 0x07;               // fault bits (OC, SCG, SCV)
    PIN_high(THERMO_PIN_CS);
    if((data & 0x0001) || fault) return THERMO_ERROR;
    return (int16_t)data >> 2;                    // signed 14 bits, 0.25°C
  }
  #endif
}
