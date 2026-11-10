// ===================================================================================
// Basic UART2 Functions (SERCOM USART) for SAMD21 (no buffer, no interrupt)  * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART2_init()             Init UART with 8N1 and default BAUD rate (115200)
// UART2_setBAUD(n)         Set BAUD rate
// UART2_setDataBits(n)     Set number of data bits (n = 5, 6, 7, 8, 9)
// UART2_setStopBits(n)     Set number of stop bits (n = 1, 2)
// UART2_setNoParity()      Set no parity bit
// UART2_setOddParity()     Set parity bit, odd
// UART2_setEvenParity()    Set parity bit, even
//
// UART2_ready()            Check if UART is ready to write
// UART2_available()        Check if there is something to read
// UART2_completed()        Check if transmission is completed
//
// UART2_read()             Read character via UART
// UART2_write(c)           Send character via UART
// UART2_writeBuffer(buf,len) Send buffer (*buf) with length (len) via UART
// UART2_readBuffer(buf,len) Read buffer (*buf) with length (len) via UART (waits)
//
// UART2_enable()           Enable USART
// UART2_disable()          Disable USART
// UART2_TX_enable()        Enable transmitter
// UART2_TX_disable()       Disable transmitter
// UART2_RX_enable()        Enable receiver
// UART2_RX_disable()       Disable receiver
// UART2_LP_enable()        Enable operation in STANDBY (clock must run in standby)
// UART2_LP_disable()       Disable operation in STANDBY
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UART2_printf(f, ...)      printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// UART2_printD(n)          Print decimal value
// UART2_printW(n)          Print 32-bit hex word value
// UART2_printH(n)          Print 16-bit hex half-word value
// UART2_printB(n)          Print  8-bit hex byte value
// UART2_printS(s)          Print string
// UART2_print(s)           Print string (alias)
// UART2_println(s)         Print string with newline
// UART2_newline()          Send newline
//
// UART pin mapping (set below in UART parameters):
// ------------------------------------------------
// UART2_MAP 0     1     2     3     4     5     6     7     8     9     10    
// SERCOM    0     0     1     2     2     3     4     4     5     5     1     
// TX-pin    PA10  PA4   PA16  PA8   PA14  PA22  PB8   PB10  PB22  PB2   PA0   No mapping
// RX-pin    PA11  PA5   PA17  PA9   PA15  PA23  PB9   PB11  PB23  PB3   PA1   No mapping
// Device    EGJ   EGJ   EGJ   EGJ   EGJ   EGJ   GJ    GJ    GJ    GJ    EGJ   
//
// Notes:
// ------
// - Device row: mapping available on SAMD21E (E), SAMD21G (G) and SAMD21J (J).
// - PA0/PA1 are the pins for the 32.768kHz crystal (XOSC32K).
// - The SERCOM is clocked by GCLK0 (F_CPU), max BAUD rate is F_CPU / 16.
// - The fractional BAUD rate generator is used (BAUD error < 0.5% for most rates).
// - This is a copy of the uart library with the prefix UART2_ instead of UART_, so
//   that two UARTs (on different SERCOMs) can be used at the same time.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART2_BAUD             115200      // default UART baud rate
#define UART2_MAP              2           // UART pin mapping (see above)
#define UART2_PRINT            0           // 1: include print functions (needs print.h)

// UART2 pin mapping (SERCOM, TX/RX pin, peripheral function, TX/RX pad)
#if   UART2_MAP == 0
  #define UART2_SERCOM        0
  #define UART2_TX_PIN        10          // PA10
  #define UART2_RX_PIN        11          // PA11
  #define UART2_TX_PMUX       2           // C
  #define UART2_RX_PMUX       2           // C
  #define UART2_TXPO          1           // TX on PAD2
  #define UART2_RXPO          3           // RX on PAD3
#elif UART2_MAP == 1
  #define UART2_SERCOM        0
  #define UART2_TX_PIN        4           // PA4
  #define UART2_RX_PIN        5           // PA5
  #define UART2_TX_PMUX       3           // D
  #define UART2_RX_PMUX       3           // D
  #define UART2_TXPO          0           // TX on PAD0
  #define UART2_RXPO          1           // RX on PAD1
#elif UART2_MAP == 2
  #define UART2_SERCOM        1
  #define UART2_TX_PIN        16          // PA16
  #define UART2_RX_PIN        17          // PA17
  #define UART2_TX_PMUX       2           // C
  #define UART2_RX_PMUX       2           // C
  #define UART2_TXPO          0           // TX on PAD0
  #define UART2_RXPO          1           // RX on PAD1
#elif UART2_MAP == 3
  #define UART2_SERCOM        2
  #define UART2_TX_PIN        8           // PA8
  #define UART2_RX_PIN        9           // PA9
  #define UART2_TX_PMUX       3           // D
  #define UART2_RX_PMUX       3           // D
  #define UART2_TXPO          0           // TX on PAD0
  #define UART2_RXPO          1           // RX on PAD1
#elif UART2_MAP == 4
  #define UART2_SERCOM        2
  #define UART2_TX_PIN        14          // PA14
  #define UART2_RX_PIN        15          // PA15
  #define UART2_TX_PMUX       2           // C
  #define UART2_RX_PMUX       2           // C
  #define UART2_TXPO          1           // TX on PAD2
  #define UART2_RXPO          3           // RX on PAD3
#elif UART2_MAP == 5
  #define UART2_SERCOM        3
  #define UART2_TX_PIN        22          // PA22
  #define UART2_RX_PIN        23          // PA23
  #define UART2_TX_PMUX       2           // C
  #define UART2_RX_PMUX       2           // C
  #define UART2_TXPO          0           // TX on PAD0
  #define UART2_RXPO          1           // RX on PAD1
#elif UART2_MAP == 6
  #define UART2_SERCOM        4
  #define UART2_TX_PIN        40          // PB8
  #define UART2_RX_PIN        41          // PB9
  #define UART2_TX_PMUX       3           // D
  #define UART2_RX_PMUX       3           // D
  #define UART2_TXPO          0           // TX on PAD0
  #define UART2_RXPO          1           // RX on PAD1
#elif UART2_MAP == 7
  #define UART2_SERCOM        4
  #define UART2_TX_PIN        42          // PB10
  #define UART2_RX_PIN        43          // PB11
  #define UART2_TX_PMUX       3           // D
  #define UART2_RX_PMUX       3           // D
  #define UART2_TXPO          1           // TX on PAD2
  #define UART2_RXPO          3           // RX on PAD3
#elif UART2_MAP == 8
  #define UART2_SERCOM        5
  #define UART2_TX_PIN        54          // PB22
  #define UART2_RX_PIN        55          // PB23
  #define UART2_TX_PMUX       3           // D
  #define UART2_RX_PMUX       3           // D
  #define UART2_TXPO          1           // TX on PAD2
  #define UART2_RXPO          3           // RX on PAD3
#elif UART2_MAP == 9
  #define UART2_SERCOM        5
  #define UART2_TX_PIN        34          // PB2
  #define UART2_RX_PIN        35          // PB3
  #define UART2_TX_PMUX       3           // D
  #define UART2_RX_PMUX       3           // D
  #define UART2_TXPO          0           // TX on PAD0
  #define UART2_RXPO          1           // RX on PAD1
#elif UART2_MAP == 10
  #define UART2_SERCOM        1
  #define UART2_TX_PIN        0           // PA0
  #define UART2_RX_PIN        1           // PA1
  #define UART2_TX_PMUX       3           // D
  #define UART2_RX_PMUX       3           // D
  #define UART2_TXPO          0           // TX on PAD0
  #define UART2_RXPO          1           // RX on PAD1
#else
  #define UART2_SERCOM        0           // pins must be set by the application
  #define UART2_TXPO          1
  #define UART2_RXPO          3
#endif

#if (UART2_MAP == 6) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (UART2_MAP == 7) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (UART2_MAP == 8) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (UART2_MAP == 9) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif

#if (UART2_SERCOM > 3) && defined(SAMD21E)
  #error SERCOM4 and SERCOM5 are not available on SAMD21E
#endif

#if   UART2_SERCOM == 0
  #define UART2_SER          SERCOM0
  #define UART2_GCLK_ID      SERCOM0_GCLK_ID_CORE
  #define UART2_APBMASK      PM_APBCMASK_SERCOM0
  #define UART2_IRQn         SERCOM0_IRQn
  #define UART2_IRQHandler   SERCOM0_IRQHandler
  #define UART2_DMAC_ID_TX   SERCOM0_DMAC_ID_TX
#elif UART2_SERCOM == 1
  #define UART2_SER          SERCOM1
  #define UART2_GCLK_ID      SERCOM1_GCLK_ID_CORE
  #define UART2_APBMASK      PM_APBCMASK_SERCOM1
  #define UART2_IRQn         SERCOM1_IRQn
  #define UART2_IRQHandler   SERCOM1_IRQHandler
  #define UART2_DMAC_ID_TX   SERCOM1_DMAC_ID_TX
#elif UART2_SERCOM == 2
  #define UART2_SER          SERCOM2
  #define UART2_GCLK_ID      SERCOM2_GCLK_ID_CORE
  #define UART2_APBMASK      PM_APBCMASK_SERCOM2
  #define UART2_IRQn         SERCOM2_IRQn
  #define UART2_IRQHandler   SERCOM2_IRQHandler
  #define UART2_DMAC_ID_TX   SERCOM2_DMAC_ID_TX
#elif UART2_SERCOM == 3
  #define UART2_SER          SERCOM3
  #define UART2_GCLK_ID      SERCOM3_GCLK_ID_CORE
  #define UART2_APBMASK      PM_APBCMASK_SERCOM3
  #define UART2_IRQn         SERCOM3_IRQn
  #define UART2_IRQHandler   SERCOM3_IRQHandler
  #define UART2_DMAC_ID_TX   SERCOM3_DMAC_ID_TX
#elif UART2_SERCOM == 4
  #define UART2_SER          SERCOM4
  #define UART2_GCLK_ID      SERCOM4_GCLK_ID_CORE
  #define UART2_APBMASK      PM_APBCMASK_SERCOM4
  #define UART2_IRQn         SERCOM4_IRQn
  #define UART2_IRQHandler   SERCOM4_IRQHandler
  #define UART2_DMAC_ID_TX   SERCOM4_DMAC_ID_TX
#elif UART2_SERCOM == 5
  #define UART2_SER          SERCOM5
  #define UART2_GCLK_ID      SERCOM5_GCLK_ID_CORE
  #define UART2_APBMASK      PM_APBCMASK_SERCOM5
  #define UART2_IRQn         SERCOM5_IRQn
  #define UART2_IRQHandler   SERCOM5_IRQHandler
  #define UART2_DMAC_ID_TX   SERCOM5_DMAC_ID_TX
#endif

// UART macros
#define UART2_ready()          (UART2_SER->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)
#define UART2_available()      (UART2_SER->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC)
#define UART2_completed()      (UART2_SER->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC)

#define UART2_sync()           while(UART2_SER->USART.SYNCBUSY.reg)
#define UART2_enable()         (UART2_SER->USART.CTRLA.reg |=  SERCOM_USART_CTRLA_ENABLE, UART2_wait())
#define UART2_disable()        (UART2_SER->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE, UART2_wait())
#define UART2_TX_enable()      (UART2_SER->USART.CTRLB.reg |=  SERCOM_USART_CTRLB_TXEN,   UART2_wait())
#define UART2_TX_disable()     (UART2_SER->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_TXEN,   UART2_wait())
#define UART2_RX_enable()      (UART2_SER->USART.CTRLB.reg |=  SERCOM_USART_CTRLB_RXEN,   UART2_wait())
#define UART2_RX_disable()     (UART2_SER->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_RXEN,   UART2_wait())
static inline void UART2_wait(void) { UART2_sync(); }
#define UART2_LP_enable()      UART2_setCTRLA(SERCOM_USART_CTRLA_RUNSTDBY, SERCOM_USART_CTRLA_RUNSTDBY)
#define UART2_LP_disable()     UART2_setCTRLA(SERCOM_USART_CTRLA_RUNSTDBY, 0)

#define UART2_setDataBits(n)   UART2_setCTRLB(SERCOM_USART_CTRLB_CHSIZE_Msk, \
                              SERCOM_USART_CTRLB_CHSIZE(((n) == 9) ? 1 : ((n) & 7)))
#define UART2_setStopBits(n)   UART2_setCTRLB(SERCOM_USART_CTRLB_SBMODE, \
                              ((n) == 2) ? SERCOM_USART_CTRLB_SBMODE : 0)
#define UART2_setNoParity()    UART2_setCTRLA(SERCOM_USART_CTRLA_FORM_Msk, 0)
#define UART2_setEvenParity()  (UART2_setCTRLB(SERCOM_USART_CTRLB_PMODE, 0), \
                               UART2_setCTRLA(SERCOM_USART_CTRLA_FORM_Msk, SERCOM_USART_CTRLA_FORM(1)))
#define UART2_setOddParity()   (UART2_setCTRLB(SERCOM_USART_CTRLB_PMODE, SERCOM_USART_CTRLB_PMODE), \
                               UART2_setCTRLA(SERCOM_USART_CTRLA_FORM_Msk, SERCOM_USART_CTRLA_FORM(1)))

// UART functions
void UART2_init(void);                     // init UART with default BAUD rate
char UART2_read(void);                     // read character via UART
void UART2_write(const char c);            // send character via UART
void UART2_writeBuffer(const uint8_t* buf, uint16_t len); // send buffer
void UART2_readBuffer(uint8_t* buf, uint16_t len); // receive buffer
void UART2_setBAUD(uint32_t baud);         // set BAUD rate
void UART2_setCTRLA(uint32_t mask, uint32_t val);  // change CTRLA (USART disabled)
void UART2_setCTRLB(uint32_t mask, uint32_t val);  // change CTRLB (USART disabled)

// Additional print functions (if activated, see above)
#if UART2_PRINT == 1
#include "print.h"
#define UART2_printD(n)        printD(UART2_write, n)   // print decimal as string
#define UART2_printW(n)        printW(UART2_write, n)   // print word as string
#define UART2_printH(n)        printH(UART2_write, n)   // print half-word as string
#define UART2_printB(n)        printB(UART2_write, n)   // print byte as string
#define UART2_printS(s)        printS(UART2_write, s)   // print string
#define UART2_println(s)       println(UART2_write, s)  // print string with newline
#define UART2_print            UART2_printS             // alias
#define UART2_newline()        UART2_write('\n')        // send newline
#define UART2_printf(f, ...)   printF(UART2_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
