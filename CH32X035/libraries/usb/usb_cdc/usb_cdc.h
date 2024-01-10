// ===================================================================================
// Basic USB CDC Functions for CH32X035/X034/X033                             * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "usb_descr.h"
#include "usb_handler.h"

// ===================================================================================
// CDC Functions
// ===================================================================================
void CDC_init(void);              // setup USB-CDC
void CDC_flush(void);             // flush OUT buffer
char CDC_read(void);              // read single character from IN buffer
void CDC_write(char c);           // write single character to OUT buffer
void CDC_print(char* str);        // write string to OUT buffer
void CDC_println(char* str);      // write string with newline to OUT buffer and flush
uint8_t CDC_available(void);      // check number of bytes in the IN buffer
uint8_t CDC_ready(void);          // check if OUT buffer is ready to be written
#define CDC_writeflush(c)         {CDC_write(c);CDC_flush();}   // write & flush char

// ===================================================================================
// CDC Control Line State
// ===================================================================================
extern volatile uint8_t CDC_controlLineState;               // control line state
#define CDC_DTR_flag    (CDC_controlLineState & 1)          // DTR flag
#define CDC_RTS_flag    ((CDC_controlLineState >> 1) & 1)   // RTS flag
#define CDC_getDTR()    (CDC_DTR_flag)                      // get DTR flag
#define CDC_getRTS()    (CDC_RTS_flag)                      // get RTS flag

// ===================================================================================
// CDC Line Coding
// ===================================================================================
typedef struct {
  uint32_t baudrate;              // baud rate
  uint8_t  stopbits;              // number of stopbits (0:1bit,1:1.5bits,2:2bits)
  uint8_t  parity;                // parity (0:none,1:odd,2:even,3:mark,4:space)
  uint8_t  databits;              // number of data bits (5,6,7,8 or 16)
} CDC_LINE_CODING_TYPE, *PCDC_LINE_CODING_TYPE;

extern CDC_LINE_CODING_TYPE CDC_lineCoding;
#define CDC_getBAUD()   (CDC_lineCoding->baudrate)

#ifdef __cplusplus
}
#endif
