// ===================================================================================
// Basic CAN Functions (CAN1, bxCAN) for CH32V203                             * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "can.h"

// Bit timing: bit time = (1 + TS1 + TS2) time quanta, sample point ~ 75 .. 87.5%
#if   (F_CPU % (CAN_BAUD * 16)) == 0
  #define CAN_TQ          16
  #define CAN_TS1         13
  #define CAN_TS2         2
#elif (F_CPU % (CAN_BAUD * 12)) == 0
  #define CAN_TQ          12
  #define CAN_TS1         9
  #define CAN_TS2         2
#elif (F_CPU % (CAN_BAUD * 10)) == 0
  #define CAN_TQ          10
  #define CAN_TS1         7
  #define CAN_TS2         2
#elif (F_CPU % (CAN_BAUD * 8)) == 0
  #define CAN_TQ          8
  #define CAN_TS1         5
  #define CAN_TS2         2
#else
  #error F_CPU must be a multiple of 8 * CAN_BAUD
#endif
#define CAN_BRP           (F_CPU / CAN_BAUD / CAN_TQ)
#if CAN_BRP < 1 || CAN_BRP > 1024
  #error CAN_BAUD not possible with this F_CPU
#endif

// Init CAN
void CAN_init(void) {
  // Setup GPIO pins (RX: input with pullup, TX: alternate function push-pull)
  RCC->APB2PCENR |= RCC_AFIOEN;
  RCC->APB1PCENR |= RCC_CAN1EN;
  #if CAN_MAP == 0
    RCC->APB2PCENR |= RCC_IOPAEN;
    AFIO->PCFR1    &= ~((uint32_t)0b11 << 13);   // CAN1 pins PA11/PA12
    GPIOA->CFGHR    = (GPIOA->CFGHR & ~(((uint32_t)0b1111<<(3<<2)) | ((uint32_t)0b1111<<(4<<2))))
                                    |  (((uint32_t)0b1000<<(3<<2)) | ((uint32_t)0b1011<<(4<<2)));
    GPIOA->BSHR     = (uint32_t)1<<11;
  #elif CAN_MAP == 1
    RCC->APB2PCENR |= RCC_IOPBEN;
    AFIO->PCFR1     = (AFIO->PCFR1 & ~((uint32_t)0b11 << 13)) | ((uint32_t)0b10 << 13); // PB8/PB9
    GPIOB->CFGHR    = (GPIOB->CFGHR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                    |  (((uint32_t)0b1000<<(0<<2)) | ((uint32_t)0b1011<<(1<<2)));
    GPIOB->BSHR     = (uint32_t)1<<8;
  #else
    #warning No automatic pin mapping for CAN
  #endif

  // Enter initialization mode
  CAN1->CTLR &= ~CAN_CTLR_SLEEP;                // exit sleep mode
  CAN1->CTLR |=  CAN_CTLR_INRQ;                 // request initialization
  while(!(CAN1->STATR & CAN_STATR_INAK));       // wait for initialization mode

  // Setup CAN (automatic bus-off management, automatic wake-up, TX FIFO priority)
  CAN1->CTLR  = CAN_CTLR_INRQ | CAN_CTLR_ABOM | CAN_CTLR_AWUM | CAN_CTLR_TXFP;
  CAN1->BTIMR = ((uint32_t)(CAN_TS2 - 1) << 20)   // time segment 2
              | ((uint32_t)(CAN_TS1 - 1) << 16)   // time segment 1
              | ((uint32_t)0 << 24)               // resynchronization jump width: 1
              | (CAN_BRP - 1)                     // baud rate prescaler
              #if CAN_LOOPBACK > 0
              | ((uint32_t)1 << 30)               // loopback mode
              | ((uint32_t)1 << 31)               // silent mode
              #endif
              ;

  // Setup filter 0: accept all frames into FIFO 0 (32-bit mask mode, mask = 0)
  CAN1->FCTLR  |=  CAN_FCTLR_FINIT;             // filter initialization mode
  CAN1->FWR    &= ~(uint32_t)1;                 // deactivate filter 0
  CAN1->FSCFGR |=  (uint32_t)1;                 // 32-bit scale
  CAN1->FMCFGR &= ~(uint32_t)1;                 // mask mode
  CAN1->FAFIFOR &= ~(uint32_t)1;                // assign to FIFO 0
  CAN1->sFilterRegister[0].FR1 = 0;             // identifier
  CAN1->sFilterRegister[0].FR2 = 0;             // mask: don't care
  CAN1->FWR    |=  (uint32_t)1;                 // activate filter 0
  CAN1->FCTLR  &= ~CAN_FCTLR_FINIT;             // leave filter initialization mode

  // Leave initialization mode
  CAN1->CTLR &= ~CAN_CTLR_INRQ;
  while(CAN1->STATR & CAN_STATR_INAK);          // wait for normal mode
}

// Send frame (waits for free mailbox), returns 0 on success
uint8_t CAN_write(uint32_t id, const uint8_t* buf, uint8_t len) {
  uint8_t mb;
  uint32_t tstatr;
  if(len > 8) len = 8;
  while(!CAN_ready());                          // wait for free mailbox
  tstatr = CAN1->TSTATR;
  mb = (tstatr >> 24) & 0x03;                   // code of free mailbox
  if(mb > 2) return 1;
  if(id & CAN_EXT) CAN1->sTxMailBox[mb].TXMIR = ((id & 0x1FFFFFFF) << 3) | (1 << 2); // IDE
  else             CAN1->sTxMailBox[mb].TXMIR = (id & 0x7FF) << 21;
  if(id & CAN_RTR) CAN1->sTxMailBox[mb].TXMIR |= (1 << 1);                         // RTR
  CAN1->sTxMailBox[mb].TXMDTR = len;            // data length code
  CAN1->sTxMailBox[mb].TXMDLR = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8)
                              | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
  CAN1->sTxMailBox[mb].TXMDHR = (uint32_t)buf[4] | ((uint32_t)buf[5] << 8)
                              | ((uint32_t)buf[6] << 16) | ((uint32_t)buf[7] << 24);
  CAN1->sTxMailBox[mb].TXMIR |= 1;              // request transmission
  return 0;
}

// Read received frame (waits), returns data length
uint8_t CAN_read(uint32_t* id, uint8_t* buf) {
  uint32_t mir, dl, dh;
  uint8_t  len;
  while(!CAN_available());                      // wait for frame
  mir = CAN1->sFIFOMailBox[0].RXMIR;
  if(mir & (1 << 2)) *id = ((mir >> 3) & 0x1FFFFFFF) | CAN_EXT;  // extended identifier
  else               *id = (mir >> 21) & 0x7FF;                  // standard identifier
  if(mir & (1 << 1)) *id |= CAN_RTR;                             // remote frame
  len = CAN1->sFIFOMailBox[0].RXMDTR & 0x0F;
  if(len > 8) len = 8;
  dl  = CAN1->sFIFOMailBox[0].RXMDLR;
  dh  = CAN1->sFIFOMailBox[0].RXMDHR;
  buf[0] = dl; buf[1] = dl >> 8; buf[2] = dl >> 16; buf[3] = dl >> 24;
  buf[4] = dh; buf[5] = dh >> 8; buf[6] = dh >> 16; buf[7] = dh >> 24;
  CAN1->RFIFO0 |= CAN_RFIFO0_RFOM0;             // release FIFO output mailbox
  return len;
}
