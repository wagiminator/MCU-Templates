// ===================================================================================
// Basic Serial Debug Functions for tinyAVR 0-Series and 1-Series             * v1.0 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "debug_serial.h"

#if DEBUG_ENABLE > 0

// Init debug interface (USART0 TX)
void DEBUG_init(void) {
  // Setup TX pin
  #if DEBUG_TX == 0
    VPORTB.DIR |= (1 << 2);
  #elif DEBUG_TX == 1
    VPORTA.DIR |= (1 << 1);
    PORTMUX.CTRLB |= PORTMUX_USART0_bm;
  #else
    #warning Wrong UART REMAP
  #endif
	
  // Setup and start UART (8N1, TX, default BAUD rate)
  USART0.BAUD  = (((F_CPU * 8) / DEBUG_BAUD) + 1) / 2;
  USART0.CTRLB = USART_TXEN_bm;
}

// Send byte via UART
void DEBUG_write(const char c) {
  while(!(USART0.STATUS & USART_DREIF_bm));
  USART0.TXDATAL = c;
}

// Send string via UART
void DEBUG_print(const char* str) {
  while(*str) DEBUG_write(*str++);
}

// Send string via UART with newline
void DEBUG_println(const char* str) {
  DEBUG_print(str);
  DEBUG_write('\n');
}

// For BCD conversion
const uint16_t DIVIDER[] = {1, 10, 100, 1000, 10000};

// Print decimal value (BCD conversion by substraction method)
void DEBUG_printD(uint16_t value) {
  uint8_t digits   = 5;                           // print 5 digits
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digits--) {                               // for all digits digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint32_t divider = DIVIDER[digits];           // read current divider
    while(value >= divider) {                     // if current divider fits into the value
      leadflag = 1;                               // end of leading spaces
      digitval++;                                 // increase digit value
      value -= divider;                           // decrease value by divider
    }
    if(!digits)  leadflag++;                      // least digit has to be printed
    if(leadflag) DEBUG_write(digitval + '0');     // print the digit
  }
}

// Convert 4-bit byte nibble into hex character and print it via UART
void DEBUG_printN(uint8_t nibble) {
  DEBUG_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert 8-bit byte into hex characters and print it via UART
void DEBUG_printB(uint8_t value) {
  DEBUG_printN(value >> 4);
  DEBUG_printN(value & 0x0f);
}

// Convert 16-bit half-word into hex characters and print it via UART
void DEBUG_printH(uint16_t value) {
  DEBUG_printB(value >> 8);
  DEBUG_printB(value);
}

// Convert 32-bit word into hex characters and print it via UART
void DEBUG_printW(uint32_t value) {
  DEBUG_printH(value >> 16);
  DEBUG_printH(value);
}

// printf, supports %s, %c, %d, %u, %x, %b, %02d, %%
void DEBUG_itoa(int16_t, int8_t, int8_t);
static void DEBUG_vfprintf(const char *format, va_list arg);

void DEBUG_printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  DEBUG_vfprintf(format, arg);
  va_end(arg);
}

static void DEBUG_vfprintf(const char* str,  va_list arp) {
  int16_t d, r, w, s;
  char *c;

  while((d = *str++) != 0) {
    if(d != '%') {
      DEBUG_write(d);
      continue;
    }
    d = *str++;
    w = r = s = 0;
    if(d == '%') {
      DEBUG_write(d);
      d = *str++;
    }
    if(d == '0') {
      d = *str++;
      s = 1;
    }
    while((d >= '0') && (d <= '9')) {
      w += w * 10 + (d - '0');
      d = *str++;
    }
    if(s) w = -w;
    if(d == 's') {
      c = va_arg(arp, char*);
      while(*c) DEBUG_write(*(c++));
      continue;
    }
    if(d == 'c') {
      DEBUG_write((char)va_arg(arp, int));
      continue;
    }
    if(d =='\0') break;
    else if(d == 'u') r = 10;
    else if(d == 'd') r = -10;
    else if(d == 'x') r = 16;
    else if(d == 'b') r = 2;
    else str--;
    if(r == 0) continue;
    if(r > 0) DEBUG_itoa((uint16_t)va_arg(arp, int16_t), r, w);
    else DEBUG_itoa((int16_t)va_arg(arp, int16_t), r, w);
  }
}

void DEBUG_itoa(int16_t val, int8_t rad, int8_t len) {
  char c, sgn = 0, pad = ' ';
  char s[10];
  uint8_t i = 0;

  if(rad < 0) {
    rad = -rad;
    if(val < 0) {
      val = -val;
      sgn = '-';
    }
  }
  if(len < 0) {
    len = -len;
    pad = '0';
  }
  if(len > 10) return;
  do {
    c = (char)((uint16_t)val % rad);
    if (c >= 10) c += ('A' - 10);
    else c += '0';
    s[i++] = c;
    val = (uint16_t)val / rad;
  } while(val);
  if((sgn != 0) && (pad != '0')) s[i++] = sgn;
  while(i < len) s[i++] = pad;
  if((sgn != 0) && (pad == '0')) s[i++] = sgn;
  do DEBUG_write(s[--i]);
  while(i);
}

#endif // DEBUG_ENABLE > 0
