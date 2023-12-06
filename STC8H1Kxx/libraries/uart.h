// ===================================================================================
// Basic UART Functions for STC8H Microcontrollers                            * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART1_init()             Init UART1 with 8N1 and default BAUD rate (115200)
// UART1_setBaud(n)         Set BAUD rate
// UART1_ready()            Check if UART is ready to write
// UART1_available()        Check if there is something to read
// UART1_read()             Read character via UART
// UART1_write(c)           Send character via UART
//
// UART2_init()             Init UART2 with 8N1 and default BAUD rate (115200)
// UART2_setBaud(n)         Set BAUD rate
// UART2_ready()            Check if UART is ready to write
// UART2_available()        Check if there is something to read
// UART2_read()             Read character via UART
// UART2_write(c)           Send character via UART
//
// UART1 pin mapping (set below in UART parameters):
// -------------------------------------------------
// UART1    MAP0   MAP1   MAP2   MAP3
// RX-pin   P3.0   P3.6   P1.6   P4.3
// TX-pin   P3.1   P3.7   P1.7   P4.4
//
// UART2 pin mapping (set below in UART parameters):
// -------------------------------------------------
// UART2    MAP0   MAP1
// RX-pin   P1.0   P4.6
// TX-pin   P1.1   P4.7
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

// UART parameters
#define UART1_MAP           0               // UART1 port pin map (0 - 3)
#define UART1_TIMER         1               // UART1 timer (1 or 2)
#define UART1_BAUD          115200          // UART1 baud rate

#define UART2_MAP           0               // UART1 port pin map (0 or 1)
#define UART2_BAUD          115200          // UART2 baud rate

// UART state macros
#define UART1_ready()       (TI)            // UART1 ready to write?
#define UART1_available()   (RI)            // UART1 received data byte?
#define UART2_ready()       (S2CON & 0x02)  // UART2 ready to write?
#define UART2_available()   (S2CON & 0x01)  // UART2 received data byte?

// UART calculate BAUD rate setting
#define UART1_BAUD_SET  (65536 - (((F_CPU / UART1_BAUD) + 2) / 4))
#define UART2_BAUD_SET  (65536 - (((F_CPU / UART2_BAUD) + 2) / 4))

#if UART1_TIMER == 1
  #define UART1_setBAUD(baud) {                        \
    TR1 = 0;                                           \
    TL1 = (65536 - (((F_CPU / baud) + 2) / 4)) & 0xff; \
    TH1 = (65536 - (((F_CPU / baud) + 2) / 4)) >> 8;   \
    TR1 = 1;                                           \
  }
#elif UART1_TIMER == 2
  #define UART1_setBAUD(baud) {                        \
    T2L = (65536 - (((F_CPU / baud) + 2) / 4)) & 0xff; \
    T2H = (65536 - (((F_CPU / baud) + 2) / 4)) >> 8;   \
  }
#endif

#define UART2_setBAUD(baud) {                          \
  T2L = (65536 - (((F_CPU / baud) + 2) / 4)) & 0xff;   \
  T2H = (65536 - (((F_CPU / baud) + 2) / 4)) >> 8;     \
}

// UART1 setup
inline void UART1_init(void) {
  #if UART1_MAP > 0
    P_SW1 |= DEBUG_MAP << 6;            // UART1 set alternate pins
  #endif
  SCON = 0x52;                          // RX/TX, 8 bits, variable baudrate, TI flag
  #if UART1_TIMER == 1
    AUXR = 0x40;                        // timer1 in 1T mode as baudrate generator
    TL1  = UART1_BAUD_SET & 0xff;       // initial timer value according to BAUD
    TH1  = UART1_BAUD_SET >> 8;         // initial timer value according to BAUD
    TR1  = 1;                           // timer1 start run
  #elif UART1_TIMER == 2
    AUXR = 0x15;                        // timer2 in 1T mode as baudrate generator
    T2L  = UART1_BAUD_SET & 0xff;       // initial timer value according to BAUD
    T2H  = UART1_BAUD_SET >> 8;         // initial timer value according to BAUD
  #endif
}

// UART1 read single byte
inline uint8_t UART1_read(void) {
  while(!RI);                           // wait for incoming data byte
  RI = 0;                               // clear receive complete flag
  return SBUF;                          // return the received byte
}

// UART1 write single byte
inline void UART1_write(uint8_t data) {
  while(!TI);                           // wait for last data byte to be transmitted
  TI = 0;                               // clear transmit complete flag
  SBUF = data;                          // start transmitting data byte
}

// UART2 setup
inline void UART2_init(void) {
  #if UART2_MAP > 0
    P_SW2 |= 0x01;                      // UART2 set alternate pins
  #endif
  S2CON = 0x12;                         // RX/TX, 8 bits, set transmit complete flag
  AUXR |= 0x14;                         // timer2 in 1T mode as baudrate generator
  T2L   = UART2_BAUD_SET & 0xff;        // initial timer value according to BAUD
  T2H   = UART2_BAUD_SET >> 8;          // initial timer value according to BAUD
}

// UART2 read single byte
inline uint8_t UART2_read(void) {
  while(!(S2CON & 0x01));               // wait for incoming data byte
  S2CON &= ~0x01;                       // clear receive complete flag
  return S2BUF;                         // return the received byte
}

// UART2 write single byte
inline void UART2_write(uint8_t data) {
  while(!(S2CON & 0x02));               // wait for last data byte to be transmitted
  S2CON &= ~0x02;                       // clear transmit complete flag
  S2BUF = data;                         // start transmitting data byte
}
