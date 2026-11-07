// ===================================================================================
// Basic HX711 24-Bit ADC for Load Cells Functions                            * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "hx711.h"

// Number of extra clock pulses for gain/channel selection
#if   HX_GAIN == 64
  #define HX_PULSES           3
#elif HX_GAIN == 32
  #define HX_PULSES           2
#else
  #define HX_PULSES           1
#endif

int32_t HX_offset = 0;                // zero offset

// Init pins
void HX_init(void) {
  PIN_input(HX_PIN_DOUT);
  PIN_low(HX_PIN_SCK);
  PIN_output(HX_PIN_SCK);
}

// Set sleep state (0: wake up, 1: power down)
void HX_sleep(uint8_t slp) {
  if(slp) {
    PIN_high(HX_PIN_SCK);                         // SCK high > 60us -> power down
    DLY_us(80);
  }
  else PIN_low(HX_PIN_SCK);
}

// Wait for and read raw value (signed 24-bit)
int32_t HX_read(void) {
  uint8_t  i;
  uint16_t timeout = 500;
  uint32_t value = 0;
  while(!HX_ready()) {                            // wait for conversion (max. 500ms)
    if(!--timeout) return HX_ERROR;
    DLY_ms(1);
  }
  for(i = 24 + HX_PULSES; i; i--) {               // 24 data bits + gain pulses
    PIN_high(HX_PIN_SCK);
    DLY_us(1);
    PIN_low(HX_PIN_SCK);
    if(i > HX_PULSES) {
      value <<= 1;
      if(PIN_read(HX_PIN_DOUT)) value |= 1;
    }
    DLY_us(1);
  }
  if(value & 0x800000) value |= 0xFF000000;       // sign extension
  return (int32_t)value;
}

// Average n readings and use the result as zero offset
void HX_tare(uint8_t n) {
  int32_t sum = 0;
  uint8_t i;
  if(!n) n = 1;
  for(i = n; i; i--) sum += HX_read();
  HX_offset = sum / n;
}
