; ===================================================================================
; PIOC Program: NeoPixel (WS2812) Driver                                     * v1.0 *
; ===================================================================================
;
; Sends a data stream to addressable LEDs (WS2812, NeoPixels) on IO0. The bits are
; encoded by the hardware bit encoder of the PIOC (PWM duty cycle modulation:
; 0-bit 25% high, 1-bit 75% high). The data is read from the code RAM of the PIOC,
; starting at word address 0x200 (byte offset 0x400 from PIOC_SRAM_BASE).
;
; Host interface:
; ---------------
; RAM 0x400...  pixel data (written by host while the PIOC is paused)
; DATA_REG0/1   number of bytes to send (16-bit, 0..3072)
; DATA_REG2/3   delay loop count for the latch time (16-bit): cycles / 8
; CTRL_WR       command: bit period in clock cycles - 1 (4..127), starts transfer
; CTRL_RD       0 after the transfer (incl. latch time) is finished
;
; Assemble with: python3 pioc_asm.py neo_pioc.asm
;
; 2026 by Stefan Wagner:   https://github.com/wagiminator

INCLUDE         PIOC_INC.ASM

; Parameters (written by host)
LEN_L           EQU   SFR_DATA_REG0       ; number of bytes, low byte
LEN_H           EQU   SFR_DATA_REG1       ; number of bytes, high byte
LAT_L           EQU   SFR_DATA_REG2       ; latch delay count, low byte
LAT_H           EQU   SFR_DATA_REG3       ; latch delay count, high byte

; Internal variables
CYCLE           EQU   SFR_DATA_REG27      ; bit period
ADR_L           EQU   SFR_DATA_REG28      ; word address in code RAM, low byte
ADR_H           EQU   SFR_DATA_REG29      ; word address in code RAM, high byte
DLY_L           EQU   SFR_DATA_REG30      ; delay loop counter, low byte
DLY_H           EQU   SFR_DATA_REG31      ; delay loop counter, high byte

; Data buffer in code RAM (word address)
NEO_BUF         EQU   0x200

; Reset vector
                ORG   0x0000
                DW    0x0000              ; word 0 is never executed
                JMP   START               ; program starts at address 1

; -----------------------------------------------------------------------------------
; Main program
; -----------------------------------------------------------------------------------
START:          CLR   SFR_BIT_CYCLE               ; bit encoder off
                BC    SFR_PORT_IO,SB_PORT_OUT0    ; IO0 low
                BS    SFR_PORT_DIR,SB_PORT_DIR0   ; IO0 output
                MOVL  (1<<SB_BIT_TX_EN)           ; PWM modulation, transmit
                MOVA  SFR_BIT_CONFIG

WAIT_CMD:       WAITB WB_DATA_MW_SR_1             ; wait for command
                MOV   SFR_CTRL_WR,A               ; read command (clears flag)
                ANDL  0x7F
                MOVA  CYCLE                       ; bit period
                MOV   LEN_L,A
                IOR   LEN_H,A
                JZ    FINISH                      ; nothing to send
                MOVL  (NEO_BUF&0xFF)              ; start address of buffer
                MOVA  ADR_L
                MOVL  (NEO_BUF>>8)
                MOVA  ADR_H
                MOV   ADR_L,A                     ; read first word
                MOVA  SFR_INDIR_ADDR
                MOV   ADR_H,A
                RDCODE                            ; A = low byte, INDIR_ADDR = high byte
                MOVA  SFR_DATA_EXCH               ; first byte
                ANDL  0x80                        ; first bit
                IOR   CYCLE,A
                MOVA  SFR_BIT_CYCLE               ; start bit encoder with first bit

; Low byte of word (bit 7 is already loaded)
WORD_NEXT:      WAITB WB_PORT_I0_RISE             ; wait for start of bit 7
                BP2F  BO_BIT_TX_O0,6              ; load bit 6
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,5
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,4
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,3
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,2
                DEC   LEN_L                       ; decrement byte counter
                INC   LEN_L,A
                BTSC  SFR_STATUS_REG,SB_FLAG_Z
                DEC   LEN_H
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,1
                MOV   LEN_L,A                     ; check if it was the last byte
                IOR   LEN_H,A
                WAITB WB_PORT_I0_RISE             ; wait for start of bit 1
                JZ    LAST_BIT                    ; last byte -> finish
                BP2F  BO_BIT_TX_O0,0

; High byte of word
                WAITB WB_PORT_I0_RISE             ; wait for start of bit 0
                MOV   SFR_INDIR_ADDR,A            ; get high byte
                MOVA  SFR_DATA_EXCH
                BP2F  BO_BIT_TX_O0,7              ; load bit 7
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,6
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,5
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,4
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,3
                INC   ADR_L                       ; next word address
                BTSC  SFR_STATUS_REG,SB_FLAG_Z
                INC   ADR_H
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,2
                DEC   LEN_L                       ; decrement byte counter
                INC   LEN_L,A
                BTSC  SFR_STATUS_REG,SB_FLAG_Z
                DEC   LEN_H
                WAITB WB_PORT_I0_RISE
                BP2F  BO_BIT_TX_O0,1
                MOV   LEN_L,A                     ; check if it was the last byte
                IOR   LEN_H,A
                WAITB WB_PORT_I0_RISE             ; wait for start of bit 1
                JZ    LAST_BIT                    ; last byte -> finish
                BP2F  BO_BIT_TX_O0,0
                MOV   ADR_L,A                     ; read next word
                MOVA  SFR_INDIR_ADDR
                MOV   ADR_H,A
                RDCODE
                WAITB WB_PORT_I0_RISE             ; wait for start of bit 0
                MOVA  SFR_DATA_EXCH               ; next low byte
                BP2F  BO_BIT_TX_O0,7              ; load bit 7
                JMP   WORD_NEXT

; Last bit of transfer
LAST_BIT:       BP2F  BO_BIT_TX_O0,0              ; load last bit
                WAITB WB_PORT_I0_RISE             ; wait for start of last bit
                BC    SFR_BIT_CYCLE,SB_BIT_TX_O0  ; clear edge records
                WAITB WB_PORT_I0_FALL             ; wait for end of high phase
                CLR   SFR_BIT_CYCLE               ; stop bit encoder, IO0 low
                MOV   LAT_L,A                     ; latch time (IO0 low)
                MOVA  DLY_L
                MOV   LAT_H,A
                MOVA  DLY_H
                CALL  DELAY

FINISH:         CLRA
                MOVA  SFR_CTRL_RD                 ; report end of transfer
                JMP   WAIT_CMD

; -----------------------------------------------------------------------------------
; Delay subroutine: 8 * DLY - 1 clock cycles (+4 for CALL/RET), DLY = 0 -> 65536
; -----------------------------------------------------------------------------------
DELAY:          DEC   DLY_L                       ; 1 decrement 16-bit counter
                INC   DLY_L,A                     ; 1 Z=1 if borrow
                BTSC  SFR_STATUS_REG,SB_FLAG_Z    ; 2 (no borrow) or 1
                DEC   DLY_H                       ; 1 (borrow)
                MOV   DLY_L,A                     ; 1
                IOR   DLY_H,A                     ; 1
                JNZ   DELAY                       ; 2 (taken) or 1
                RET                               ; 2

                END
