; ===================================================================================
; PIOC Program: UART Transmitter                                             * v1.0 *
; ===================================================================================
;
; Software UART transmitter (8N1, LSB first) on IO0 or IO1. The bit time is set by
; the host as a delay loop count, each byte written by the host is sent. Since the
; host can write the next byte as soon as the PIOC has read the previous one, the
; transfer runs without gaps.
;
; Host interface:
; ---------------
; DATA_REG0/1   bit delay count (16-bit): (F_CPU / BAUD - 8) / 8
; DATA_REG2     busy flag, set by the PIOC while a byte is being sent
; CTRL_WR       first byte after start: configuration (bit 0: 0 = TX on IO0,
;               1 = TX on IO1); following bytes: data bytes to be sent
;
; Assemble with: python3 pioc_asm.py uart_pioc.asm
;
; 2026 by Stefan Wagner:   https://github.com/wagiminator

INCLUDE         PIOC_INC.ASM

; Parameters (written by host)
BAUD_L          EQU   SFR_DATA_REG0       ; bit delay count, low byte
BAUD_H          EQU   SFR_DATA_REG1       ; bit delay count, high byte
BUSY            EQU   SFR_DATA_REG2       ; busy flag (read by host)

; Internal variables
DLY_L           EQU   SFR_DATA_REG30      ; delay loop counter, low byte
DLY_H           EQU   SFR_DATA_REG31      ; delay loop counter, high byte

; Reset vector
                ORG   0x0000
                DW    0x0000              ; word 0 is never executed
                JMP   START               ; program starts at address 1

; -----------------------------------------------------------------------------------
; Main program
; -----------------------------------------------------------------------------------
START:          WAITB WB_DATA_MW_SR_1             ; wait for configuration byte
                MOV   SFR_CTRL_WR,A               ; read configuration
                ANDL  0x01
                JNZ   START_IO1                   ; TX on IO1

; Transmitter on IO0
START_IO0:      BS    SFR_PORT_IO,SB_PORT_OUT0    ; IO0 high (idle)
                BS    SFR_PORT_DIR,SB_PORT_DIR0   ; IO0 output
WAIT_IO0:       CLR   BUSY                        ; not busy
                WAITB WB_DATA_MW_SR_1             ; wait for data byte
                MOVL  0x01
                MOVA  BUSY                        ; busy
                MOV   SFR_CTRL_WR,A               ; read data byte (clears flag)
                MOVA  SFR_DATA_EXCH
                BC    SFR_PORT_IO,SB_PORT_OUT0    ; start bit
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT0,0              ; data bit 0
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT0,1              ; data bit 1
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT0,2              ; data bit 2
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT0,3              ; data bit 3
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT0,4              ; data bit 4
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT0,5              ; data bit 5
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT0,6              ; data bit 6
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT0,7              ; data bit 7
                CALL  BIT_DELAY
                BS    SFR_PORT_IO,SB_PORT_OUT0    ; stop bit
                CALL  BIT_DELAY
                JMP   WAIT_IO0

; Transmitter on IO1
START_IO1:      BS    SFR_PORT_IO,SB_PORT_OUT1    ; IO1 high (idle)
                BS    SFR_PORT_DIR,SB_PORT_DIR1   ; IO1 output
WAIT_IO1:       CLR   BUSY                        ; not busy
                WAITB WB_DATA_MW_SR_1             ; wait for data byte
                MOVL  0x01
                MOVA  BUSY                        ; busy
                MOV   SFR_CTRL_WR,A               ; read data byte (clears flag)
                MOVA  SFR_DATA_EXCH
                BC    SFR_PORT_IO,SB_PORT_OUT1    ; start bit
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT1,0              ; data bit 0
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT1,1              ; data bit 1
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT1,2              ; data bit 2
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT1,3              ; data bit 3
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT1,4              ; data bit 4
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT1,5              ; data bit 5
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT1,6              ; data bit 6
                CALL  BIT_DELAY
                BP2F  BO_PORT_OUT1,7              ; data bit 7
                CALL  BIT_DELAY
                BS    SFR_PORT_IO,SB_PORT_OUT1    ; stop bit
                CALL  BIT_DELAY
                JMP   WAIT_IO1

; -----------------------------------------------------------------------------------
; Bit delay: 8 * BAUD + 7 clock cycles (incl. CALL/RET), i.e. one bit including
; the output instruction takes 8 * BAUD + 8 clock cycles
; -----------------------------------------------------------------------------------
BIT_DELAY:      MOV   BAUD_L,A                    ; 1 load delay count
                MOVA  DLY_L                       ; 1
                MOV   BAUD_H,A                    ; 1
                MOVA  DLY_H                       ; 1
DELAY:          DEC   DLY_L                       ; 1 decrement 16-bit counter
                INC   DLY_L,A                     ; 1 Z=1 if borrow
                BTSC  SFR_STATUS_REG,SB_FLAG_Z    ; 2 (no borrow) or 1
                DEC   DLY_H                       ; 1 (borrow)
                MOV   DLY_L,A                     ; 1
                IOR   DLY_H,A                     ; 1
                JNZ   DELAY                       ; 2 (taken) or 1
                RET                               ; 2

                END
