// ===================================================================================
// Bluetooth LE UART (Nordic UART Service)                                    * v1.0 *
// ===================================================================================
//
// A virtual serial port via Bluetooth LE. It uses the widely supported Nordic UART
// Service (NUS), so it works with apps like "Serial Bluetooth Terminal" (Android),
// "Bluefruit Connect" or "nRF Toolbox" and with web pages using Web Bluetooth.
//
// Functions available:
// --------------------
// BUART_init()             init BLE and start advertising (name: BLE_NAME)
// BUART_read()             read single character from receive buffer
// BUART_write(c)           write single character to transmit buffer
// BUART_flush()            flush transmit buffer (send notification)
// BUART_writeflush(c)      write & flush character
// BUART_newline()          newline and flush
//
// BUART_available()        check number of bytes in the receive buffer
// BUART_connected()        check if a device is connected and listening
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// BUART_printf(f, ...)     printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// BUART_printD(n)          print decimal value
// BUART_printW(n)          print 32-bit hex word value
// BUART_printH(n)          print 16-bit hex half-word value
// BUART_printB(n)          print  8-bit hex byte value
// BUART_printS(s)          print string
// BUART_print(s)           print string (alias)
// BUART_println(s)         print string with newline and flush
//
// Notes:
// ------
// - The functions call BLE_poll() automatically. If none of them is called for a
//   while, BLE_poll() should be called regularly by the application.
// - Characters written while no device is connected are dropped.
//
// 2025 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ble_host.h"

// ===================================================================================
// BLE UART Parameters
// ===================================================================================
#define BUART_PRINT       0           // 1: include print functions (needs print.h)
#ifndef BUART_RXBUF
  #define BUART_RXBUF     256         // size of receive ring buffer
#endif

// ===================================================================================
// BLE UART Functions
// ===================================================================================
uint8_t  BUART_init(void);            // init BLE and start advertising
char     BUART_read(void);            // read single character from receive buffer
void     BUART_write(char c);         // write single character to transmit buffer
void     BUART_flush(void);           // flush transmit buffer
uint16_t BUART_available(void);       // check number of bytes in receive buffer
uint8_t  BUART_connected(void);       // check if a device is connected and listening

#define BUART_writeflush(c) (BUART_write(c), BUART_flush())     // write & flush
#define BUART_newline()     (BUART_write('\n'), BUART_flush())  // newline and flush

// ===================================================================================
// Additional Print Functions (if activated, see above)
// ===================================================================================
#if BUART_PRINT == 1
#include "print.h"
#define BUART_printD(n)       printD(BUART_write, n)    // print decimal as string
#define BUART_printW(n)       printW(BUART_write, n)    // print word as string
#define BUART_printH(n)       printH(BUART_write, n)    // print half-word as string
#define BUART_printB(n)       printB(BUART_write, n)    // print byte as string
#define BUART_printS(s)       printS(BUART_write, s)    // print string
#define BUART_println(s)      (println(BUART_write, s), BUART_flush())
#define BUART_print           BUART_printS              // alias
#define BUART_printf(f, ...)  (printF(BUART_write, f, ##__VA_ARGS__), BUART_flush())
#endif

#ifdef __cplusplus
}
#endif
