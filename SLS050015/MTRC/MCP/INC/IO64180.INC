;			-  64180.inc -
;
;	This files defines the internal register addresses
;	for HD64180
;
;	Version: 1.0   7/Apr/94 IERH



; ========================================
;
;	ASCI Channel Control Registers
;
; ========================================

sfrtype CNTLA0	byte,read,write		= 00h	; ASCI Ctrl Reg A, Ch 0
sfrtype CNTLA1	byte,read,write		= 01h	; ASCI Ctrl Reg A, Ch 1
sfrtype CNTLB0	byte,read,write		= 02h	; ASCI Ctrl Reg B, Ch 0
sfrtype CNTLB1	byte,read,write		= 03h	; ASCI Ctrl Reg B, Ch 1
;---------------------------------------------------------------------------
sfrtype STAT0	byte,read,write		= 04h	; ASCI Status Reg, Ch 0
sfrtype STAT1	byte,read,write		= 05h	; ASCI Status Reg, Ch 1
;---------------------------------------------------------------------------
sfrtype TDR0	byte,write		= 06h	; ASCI Transmit Data Reg, Ch 0
sfrtype TDR1	byte,write		= 07h	; ASCI Transmit Data Reg, Ch 1
sfrtype RDR0	byte,read		= 08h	; ASCI Receive Data Reg, Ch 0
sfrtype RDR1	byte,read		= 09h	; ASCI Receive Data Reg, Ch 1

; ========================================
;
;	CSI/O Registers
;
; ========================================

sfrtype CNTR	byte,read,write		= 0Ah	; CSI/O Ctrl Reg
sfrtype TRDR	byte,read,write		= 0Bh	; CSI/O Trans/Rec Data Reg

; ========================================
;
;	Timer Registers
;
; ========================================

sfrtype TMDR0L	byte,read,write		= 0Ch	; Timer 0 Data Reg L
sfrtype TMDR0H	byte,read,write		= 0Dh	; Timer 0 Data Reg H
sfrtype TMDR1L	byte,read,write		= 014h	; Timer 1 Data Reg L
sfrtype TMDR1H	byte,read,write		= 015h	; Timer 1 Data Reg H
;---------------------------------------------------------------------------
sfrtype RLDR0L	byte,read,write		= 0Eh	; Timer 0 Reload Reg L
sfrtype RLDR0H	byte,read,write		= 0Fh	; Timer 0 Reload Reg H
sfrtype RLDR1L	byte,read,write		= 016h	; Timer 1 Reload Reg L
sfrtype RLDR1H	byte,read,write		= 017h	; Timer 1 Reload Reg H
;---------------------------------------------------------------------------
sfrtype TCR	byte,read,write		= 010h	; Timer Ctrl Reg

; ========================================
;
;	Free Running Counter
;
; ========================================

sfrtype FRC	byte,read		= 018h	; Free Running Counter

; ========================================
;
;	DMA Registers
;
; ========================================

sfrtype SAR0L	byte,read,write		= 020h	; DMA 0 Source Addr Reg
sfrtype SAR0H	byte,read,write		= 021h	; DMA 0 Source Addr Reg
sfrtype SAR0B	byte,read,write		= 022h	; DMA 0 Source Addr Reg
;---------------------------------------------------------------------------
sfrtype DAR0L	byte,read,write		= 023h	; DMA 0 Destination Addr Reg
sfrtype DAR0H	byte,read,write		= 024h	; DMA 0 Destination Addr Reg
sfrtype DAR0B	byte,read,write		= 025h	; DMA 0 Destination Addr Reg
;---------------------------------------------------------------------------
sfrtype BCR0L	byte,read,write		= 026h	; DMA 0 Counter Reg
sfrtype BCR0H	byte,read,write		= 027h	; DMA 0 Counter Reg
;---------------------------------------------------------------------------
sfrtype MAR1L	byte,read,write		= 028h	; DMA 1 Memory Addr Reg
sfrtype MAR1H	byte,read,write		= 029h	; DMA 1 Memory Addr Reg
sfrtype MAR1B	byte,read,write		= 02Ah	; DMA 1 Memory Addr Reg
;---------------------------------------------------------------------------
sfrtype IAR1L	byte,read,write		= 02Bh	; DMA I/O Addr Reg
sfrtype IAR1H	byte,read,write		= 02Ch	; DMA I/O Addr Reg
;---------------------------------------------------------------------------
sfrtype BCR1L	byte,read,write		= 02Eh	; DMA 1 Byte Count Reg
sfrtype BCR1H	byte,read,write		= 02Fh	; DMA 1 Byte Count Reg
;---------------------------------------------------------------------------
sfrtype DSTAT	byte,read,write		= 030h	; DMA Status Reg
sfrtype DMODE	byte,read,write		= 031h	; DMA Mode Reg
sfrtype DCNTL	byte,read,write		= 032h	; DMA/WAIT Ctrl Reg

; ========================================
;
;	MMU Registers
;
; ========================================

sfrtype CBR	byte,read,write		= 038h	; MMU Common Base Reg
sfrtype BBR	byte,read,write		= 039h	; MMU Bank Base Reg
sfrtype CBAR	byte,read,write		= 03Ah	; MMU Bank/Common Area Reg

; ========================================
;
;	Interrupt Registers
;
; ========================================

sfrtype IL	byte,read,write		= 033h	; Int Vect Low Reg
sfrtype ITC	byte,read,write		= 034h	; Int/Trap Ctrl Reg

; ========================================
;
;	Refresh Registers
;
; ========================================

sfrtype RCR	byte,read,write		= 036h	; Refresh Ctrl Reg

; ========================================
;
;	I/O Registers
;
; ========================================
sfrtype OMCR    byte,read,write         = 03Eh  ; Operation Mode Control Register

sfrtype ICR	byte,read,write		= 03Fh	; I/O Ctrl Reg

; ========================================
;
;	Timer2 Control Registes
;
; ========================================
; ========================================
;
;	Timer2 Free Running Counter
;
; ========================================

sfrtype T2FRCL	byte,read		= 040h	; Free Running Counter
sfrtype T2FRCH	byte,read		= 041h	; Free Running Counter

sfrtype T2OCR1L	byte,read,write		= 042h	; Output Compare Register
sfrtype T2OCR1H	byte,read,write		= 043h	; Output Compare Register
sfrtype T2OCR2L	byte,read,write		= 044h	; Output Compare Register
sfrtype T2OCR2H	byte,read,write		= 045h	; Output Compare Register

sfrtype T2ICRL	byte,read,write		= 046h	; Input Capture Register
sfrtype T2ICRH	byte,read,write		= 047h	; Input Capture Register

sfrtype T2CSR1	byte,read,write		= 048h	; Timer2 Control/Status
sfrtype T2CSR2	byte,read,write		= 049h	; Timer2 Control/Status

sfrtype CCSR	byte,read,write		= 050h	; Comparator Control/Status

sfrtype RMCR	byte,read,write		= 051h	; RAM Control

sfrtype DERA	byte,read,write		= 053h	; Port A Disable Regsiter
sfrtype IDRA	byte,read			= 060h	; Port A Input Data
sfrtype ODRA	byte,read,write		= 060h	; Port A Ouput Data
sfrtype IDRB	byte,read			= 061h	; Port B Input Data
sfrtype ODRB	byte,read,write		= 061h	; Port B Ouput Data
sfrtype IDRC	byte,read			= 062h	; Port C Input Data
sfrtype ODRC	byte,read,write		= 062h	; Port C Ouput Data
sfrtype IDRD	byte,read			= 063h	; Port D Input Data
sfrtype ODRD	byte,read,write		= 063h	; Port D Ouput Data
sfrtype IDRE	byte,read			= 064h	; Port E Input Data
sfrtype ODRE	byte,read,write		= 064h	; Port E Ouput Data
sfrtype IDRF	byte,read			= 065h	; Port F Input Data
sfrtype ODRF	byte,read,write		= 065h	; Port F Ouput Data
sfrtype IDRG	byte,read			= 066h	; Port G Input Data

sfrtype DDRA	byte,read,write		= 070h	; Port A Direction
sfrtype DDRB	byte,read,write		= 071h	; Port B Direction
sfrtype DDRC	byte,read,write		= 072h	; Port C Direction
sfrtype DDRD	byte,read,write		= 073h	; Port D Direction
sfrtype DDRE	byte,read,write		= 074h	; Port E Direction
sfrtype DDRF	byte,read,write		= 075h	; Port F Direction

