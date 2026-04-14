/********************************************************************
	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’u
*-------------------------------------------------------------------*
	’ÊM§Œäƒ‚ƒWƒ…[ƒ‹
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/09		K.Honda
			a01. 2000/09/07		K.Honda

*********************************************************************/

#include    <machine.h>
#include	"iodefine.h"

#include	"timer.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

int (*HDLC_ReceiveEnd)( char *recvdata, short recvcnt, unsigned char status );
int (*HDLC_ReceiveAbort)( void );
int (*HDLC_SendEnd)( void );
int (*HDLC_SendAbort)( void );
int (*HDLC_SendStart)( void );

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

#define	HDLCCmdWrite( cmd ) { \
	ctrldata.HDLCBIT.WBIT.CONT = 1; \
	HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; \
	HDLCCMD = cmd;	\
}

#define	HDLCCmdRead( sts ) { \
	ctrldata.HDLCBIT.WBIT.CONT = 1; \
	HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; \
	sts = HDLCCMD;	\
}

#define	HDLCDataWrite( data ) { \
	ctrldata.HDLCBIT.WBIT.CONT = 0; \
	HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; \
	HDLCCMD = data;	\
}

#define	HDLCDataRead( data ) { \
	ctrldata.HDLCBIT.WBIT.CONT = 0; \
	HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; \
	data = HDLCCMD;	\
}

#define	CH_CR_SET_A( sel, data ) { \
	ctrldata.HDLCBIT.WBIT.CONT = 1; \
	HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; \
	HDLCCMD = sel;	\
	HDLCCMD = data;	\
}

#define	CH_SR_SET_A( sel, data ) { \
	ctrldata.HDLCBIT.WBIT.CONT = 1; \
	HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; \
	HDLCCMD = sel;	\
	data = HDLCCMD;	\
}

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
extern	struct	TIMER_CONTROL	timedata;		// ƒ^ƒCƒ}[î•ñ

extern	void	(*IMIA1Jump)(void);
extern	void	(*IRQ5Jump)(void);


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

static	struct st_hdlcCTRL	ctrldata;

static	char	first_receive;
static	char	*sendbuff;
static	short	sending_len;

static	char	status_flag;
static	char	bef_esstatus;
static	char	sending;

static	char			recvdata[256];
static	short			recvcnt;		// ŽóMƒoƒCƒg”‚ÌƒJƒEƒ“ƒg

/*@@@E*/
/************************************************************************

	Function name	<< HDLC_Int >>
					‚g‚c‚k‚bŠ„‚èž‚Ýˆ—
	Syntax			HDLC_Int( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
				a01 2000/09/07  ŽóMI—¹‚ÅŒÄ‚Ño‚·‚æ‚¤•ÏX

*************************************************************************/
/*. --------------------------y‹@”\Ú×z----------------------------[@@@S]
@E‘—ŽóMƒf[ƒ^‚PƒoƒCƒg‚²‚Æ‚ÉŠ„‚èž‚Ý‚ð”­¶‚³‚¹‚é
@EŽóMƒf[ƒ^‚Íƒoƒbƒtƒ@[‚ÖŠi”[‚µAI—¹Š„‚èž‚Ý‚É‚æ‚Á‚ÄŽóMˆ—‚Ö“n‚·
	------------------------------------------------------------------[@@@E]*/
void    HDLC_Int(void)
{
		unsigned char	status0,status1;
		unsigned char	data;
		unsigned char	change;

		ctrldata.HDLCBIT.WBIT.PRI = 0; 
		HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; 
		ctrldata.HDLCBIT.WBIT.INTACK = 0; 
		HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; 
		ctrldata.HDLCBIT.WBIT.INTACK = 1; 
		HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; 
		ctrldata.HDLCBIT.WBIT.PRI = 1; 
		HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; 
		ctrldata.HDLCBIT.WBIT.INTACK = 0; 
		HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; 
		ctrldata.HDLCBIT.WBIT.INTACK = 1; 
		HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; 

		CH_SR_SET_A( 0, status0 );		/* Status Read */
		CH_SR_SET_A( 1, status1 );		/* Status Read */

// Tx/Rx Interrupt Check
		if( status0 & 0x01 ){		/* Rx Data ON? */
			HDLCDataRead( data );
			if( recvcnt<sizeof(recvdata) ){
				recvdata[recvcnt++] = data;
			}
//			first_receive = 0;
//			ITU.TSTR.BIT.STR1 = 0;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—’âŽ~ */
//			ITU.TSTR.BIT.STR1 = 1;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—ŠJŽn */
			CH_CR_SET_A( 0, 0x20 );	/* Next Rx Char */
		}
		if( status0 & 0x04 ){		/* Tx Data ON? */
			if( sending ){
				if( sending_len==0 ){
					CH_CR_SET_A( 0, 0x28 );	/* Reset INT/DMA Pending */
				}
				else {
					data = *sendbuff++;
					HDLCDataWrite( data );		/* Å‰‚Ì‚PƒoƒCƒg‚ð‘—M */
					sending_len--;
				}
			}
		}

		if( status0 & 0x02 ){		/* Tx Abort */
			if( sending ){
//				first_receive = 1;
				CH_CR_SET_A( 5, 0x61 );		/* Send Disable */
				CH_CR_SET_A( 3, 0xDC );		/* Receive Disable */
//				HDLC_SendAbort();
//				ITU.TSTR.BIT.STR1 = 0;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—’âŽ~ */
				sending = 0;
				recvcnt = 0;
				CH_CR_SET_A( 3, 0xDD );		/* Receive Enable, Adr Search, Enter Hunt */
			}
		}

// Special Condition Check

//		if( status0 & 0x20 ){		// Over Run
//			HDLCDataRead( data );
//			HDLCDataRead( data );
//			HDLCDataRead( data );
//			status_flag |= 0x20;
//			ITU.TSTR.BIT.STR1 = 0;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—’âŽ~ */
//		}
		if( sending==0 ){
			if( status0 & 0x80 ){		// End of Frame
				if( status0 & 0x40 ){		// CRC Error
					status_flag |= 0x40;
				}
//				ITU.TSTR.BIT.STR1 = 0;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—’âŽ~ */
				CH_CR_SET_A( 3, 0xDC );		/* Rx Disable */
				HDLC_ReceiveEnd( recvdata, recvcnt, status_flag );
				recvcnt = 0;
				CH_CR_SET_A( 3, 0xDD );		/* Receive Enable, Adr Search, Enter Hunt */
			}
		}

// External Status Change Check

		change = status1 ^ bef_esstatus;
		if( sending==0 ){
			if( change & 0x80 ){	// Abort Detect change?
				if( status1 & 0x80 ){		// Abort Detect 0->1 
//					ITU.TSTR.BIT.STR1 = 0;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—’âŽ~ */
//					CH_CR_SET_A( 3, 0xDC );		/* Rx Disable */
					HDLC_ReceiveAbort();
					recvcnt = 0;
//					CH_CR_SET_A( 3, 0xDD );		/* Receive Enable, Adr Search, Enter Hunt */
				}
			}
			if( change & 0x10 ){	// Hunt Phase Change?
				if( status1 & 0x10 ){		// Hunt Phase 0->1
					;
				}
				else {						// Hunt Phase 1->0
//					CH_CR_SET_A( 5, 0x61 );		/* Send Disable */
//					first_receive = 1;
//					ITU.TSTR.BIT.STR1 = 0;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—’âŽ~ */
					status_flag = 0;
				}
			}
		}

		if( change & 0x04 ){	// ALL Send Change?
			if( status1 & 0x04 ){		// ALL Sent 0->1
//				first_receive = 1;
//				ITU.TSTR.BIT.STR1 = 0;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—’âŽ~ */
//				HDLC_SendEnd();
				CH_CR_SET_A( 5, 0x61 );		/* Send Disable */
				sending = 0;
				recvcnt = 0;
				CH_CR_SET_A( 3, 0xDD );		/* Receive Enable, Adr Search, Enter Hunt */
			}
		}
		bef_esstatus = status1;

// Reset Interrupt
		if( status0 & 0xA0 ){		// Over Run or End Frame 
			CH_CR_SET_A( 0, 0x30 );		/* Error Reset */
		}

//		if( change || status0 & 0x02 ){				// E/S Change
			CH_CR_SET_A( 0, 0x10 );		/* Reset E/S Latch Bit */
//		}
		CH_CR_SET_A( 0, 0x38 );		/* EOI */
}


/************************************************************************
*																		*
*	Function name	<< HDLC_SendEnable >>	
*					‘—M—v‹
*	Syntax			HDLC_SendEnable( buffer, length );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/08/07
				a01 2000/09/07
*																		*
*************************************************************************/
/*. --------------------------y‹@”\Ú×z----------------------------[@@@S]
@E‚s‚l‚r‚Ö‰ž“š‚·‚éƒf[ƒ^‚ÍAˆê“x‚±‚ÌŠÖ”‚É‚æ‚Á‚Ä‘—MM†‚ð
@‘—M‚Å‚«‚éó‘Ô‚É‚·‚éB
@EOpening PAD ‚ð‘—M‚·‚é‚½‚ßAƒ^ƒCƒ}[Š„‚èž‚Ý‚É‚æ‚Á‚ÄŽžŠÔ‘Ò‚¿‚·‚é
	------------------------------------------------------------------[@@@E]*/
void HDLC_SendEnable( char *buff, int send_length )
{
		sendbuff = buff;

		sending = 1;

		CH_CR_SET_A( 3, 0xDC );		/* Rx Disable */

		sending_len = send_length;

		INTC.IER.BIT.IRQ5E = 0;		/* IRQ5 Use */	// 00/09/13
		CH_CR_SET_A( 0, 0x28 );		/* Reset Tx INT/DMA Pending */
		CH_CR_SET_A( 5, 0x61 );		/* Send Disable */
		CH_CR_SET_A( 8, send_length & 0x0FF );		/* Tx Data Length (Low)*/
		CH_CR_SET_A( 9, (send_length>>8) & 0x0FF );	/* Tx Data length (High) */
		CH_CR_SET_A( 0, 0x28 );		/* Reset Tx INT/DMA Pending */
		CH_CR_SET_A( 13, 0x02 );	/* TxDCL Enable */
//		CH_CR_SET_A( 0x80 | 5, 0x6B );		/* Init TxCRC: 8bit, Tx Enable, RTS ON */
		CH_CR_SET_A( 5, 0x6B );		/* Tx Enable, RTS ON */

		ITU1.TCNT = 0;				/* ƒJƒEƒ“ƒ^[ƒNƒŠƒA */	// 00/09/13
		ITU1.GRA = 7373;
		ITU.TSTR.BIT.STR1 = 1;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—ŠJŽn */
}


/************************************************************************
*																		*
*	Function name	<< timer_1_Intr >>	
*					ƒ^ƒCƒ}[Š„‚èž‚Ý‚É‚æ‚é‚c‚l‚`“]‘—
*	Syntax			timer_1_Intr( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/08/07
*																		*
*************************************************************************/
/*. --------------------------y‹@”\Ú×z----------------------------[@@@S]
@EOpening PAD ‚ÌI—¹‚ðŽžŠÔ‚É‚æ‚Á‚Ä§Œä‚·‚é
@E‘—M‚ðŠJŽn‚·‚é
	------------------------------------------------------------------[@@@E]*/
void    timer_1_Intr(void)
{
		ITU.TSTR.BIT.STR1 = 0;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—ŠJŽn */

		if( sending==0 ){
			HDLC_SendStart();
		}
		else {
			INTC.IER.BIT.IRQ5E = 1;		/* IRQ5 Use */
		}
}

/************************************************************************
*																		*
*	Function name	<< HDLC_Initialize >>	
*					‰ŠúÝ’è
*	Syntax			HDLC_Initialize( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/08/07
*																		*
*************************************************************************/
void HDLC_Initialize( int baud, char lowadr, int send_length )
{
		int		clk,dpllclk;
		unsigned char	data;

		ctrldata.HDLCBIT.BYTE = 0;
		ctrldata.HDLCBIT.WBIT.PRI = 1; 
		ctrldata.HDLCBIT.WBIT.INTACK = 1; 
		HDLCCTRL.HDLCBIT.BYTE = ctrldata.HDLCBIT.BYTE; 

		clk =2400L*768L/(long)baud-2;		/* 14.7456MHz/4 = 3.6864MHz
										2400bps = 766
										4800bps = 382
										9600bps = 190
										19200bps = 84
									*/
		dpllclk =2400L*24L/(long)baud-2;		/* 14.7456MHz/4 = 3.6864MHz
										2400bps = 24
										4800bps = 12
										9600bps = 6
										19200bps = 3
									*/

		CH_CR_SET_A( 0, 0x18 );		/* CH. Reset */
		CH_CR_SET_A( 1, 0x28 );		/* First Rx Int Mask, First Rx Int */
//		CH_CR_SET_A( 2, 0x92 );		/* Vector Use, Type A-3, DMA Use */
//		CH_CR_SET_A( 2, 0x90 );		/* Vector Use, Type A-3, INT Use */
		CH_CR_SET_A( 2, 0xA0 );		/* Vector Use, Type B-2, INT Use */
		CH_CR_SET_A( 4, 0x20 );		/* BOP(hdlc) mode */

		CH_CR_SET_A( 12, 0x81 );	/* RxBRG Set */
		CH_CR_SET_A( dpllclk & 0x0FF, (dpllclk>>8) & 0x0FF );	/* BRG Set */
		CH_SR_SET_A( 0, data );		/* dummy read */

		CH_CR_SET_A( 12, 0x82 );	/* TxBRG Set */
		CH_CR_SET_A( clk & 0x0FF, (clk>>8) & 0x0FF );	/* BRG Set */
		CH_SR_SET_A( 0, data );		/* dummy read */

		CH_CR_SET_A( 15, 0x75 );	/* no XTal, RxClk = DPLL, TxCLK=TxBRG, TRxC=Out, TRxC=TxCLK */
		CH_CR_SET_A( 14, 0x07 );	/* DPLL: BRG=System, RxBRG,TxBRG=Enable */

		CH_CR_SET_A( 10, 0xA0 );	/* CRC Initial 1, NRZI */
		CH_CR_SET_A( 6, lowadr );	/* Low Address Set */
		CH_CR_SET_A( 7, 0x7E );		/* Flag bit Set */
		CH_CR_SET_A( 8, (send_length-1) & 0x0FF );		/* Tx Data Length (Low)*/
		CH_CR_SET_A( 9, ((send_length-1)>>8) & 0x0FF );	/* Tx Data length (High) */

		CH_CR_SET_A( 0x40 | 3, 0xC8 );	/* Init Rx CRC: 8bit, Rx Disable */
		CH_CR_SET_A( 0x80 | 5, 0x61 );	/* Init Tx CRC:8bit, Tx Disable, RTS Off */

		CH_CR_SET_A( 11, 0x54 );	/* Abort/TxUnder/SyncHunt/AllSent Use */

		CH_CR_SET_A( 0x10 | 1, 0x0F );	/* Reset E/S : Overrun Error, E/S Int Enable, First Rx Int, DMA Use  */

		CH_CR_SET_A( 14, 0x87 );	/* DPLL Source: RxBRG */
		CH_CR_SET_A( 14, 0xE7 );	/* NRZI receive */
		CH_CR_SET_A( 14, 0x27 );	/* Enter Search */

		ITU1.TCR.BYTE = 0;
		ITU1.TCR.BIT.CCLR = 1;		/* ‚f‚q‚`ƒJƒEƒ“ƒg‚ÅƒNƒŠƒA */
		ITU1.TCR.BIT.CKEG = 0;		/* ƒNƒƒbƒN‚ÌÙ‚¿ã‚ª‚è‚ÅƒJƒEƒ“ƒg */
		ITU1.TCR.BIT.TPSC = 2;		/* ƒ³^‚SƒNƒƒbƒN */
		ITU1.TIOR.BYTE = 0;			/* ƒRƒ“ƒyƒAƒ}ƒbƒ`‚É‚æ‚éo—Í‹ÖŽ~ */
		ITU1.TIER.BYTE = 0;
		ITU1.TIER.BIT.IMIEA = 1;	/* ‚h‚l‚e‚`ƒtƒ‰ƒO‚É‚æ‚éŠ„‚èž‚Ý */
		ITU1.TSR.BYTE = 0;			/* ƒ^ƒCƒ}[ƒXƒe[ƒ^ƒXƒNƒŠƒA */

		ITU1.TCNT = 0;				/* ƒJƒEƒ“ƒ^[ƒNƒŠƒA */
										/* 14.7456MHz -> ƒ³
										ƒ³^‚S = 3.6864MHz
										0.27126uS
										10mS / 0.27126uS = 36864(0x9000)
										 3mS / 0.27126uS = 11060
										 2mS / 0.27126uS = 7373
										ƒI[ƒvƒjƒ“ƒO‚o‚`‚c‚Ì‘—MŽžŠÔ
									*/
		ITU1.GRA = 7373;

		IMIA1Jump = timer_1_Intr;

		first_receive = 1;

		sending = 0;

		IRQ5Jump = HDLC_Int;
		INTC.IER.BIT.IRQ5E = 1;		/* IRQ5 Use */

}

/************************************************************************
*																		*
*	Function name	<< HDLC_RecvEnable >>	
*					ŽóM—v‹
*	Syntax			HDLC_RecvEnable( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/08/07
*																		*
*************************************************************************/
void HDLC_RecvEnable( void )
{
		char	data;

		first_receive = 1;

		status_flag = 0;

		sending=0;

		CH_CR_SET_A( 5, 0x61 );		/* Send Disable */
		CH_CR_SET_A( 3, 0xDC );		/* Rx Disable */

		CH_CR_SET_A( 3, 0xDD );		/* Receive Enable, Adr Search, Enter Hunt */
}



/************************************************************************
*																		*
*	Function name	<< HDLC_RecvSendChange >>	
*					‘—M—v‹
*	Syntax			HDLC_RecvSendChange( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/08/07
*																		*
*************************************************************************/
void HDLC_RecvSendChange( void )
{
		sending_len = 0;
		sending = 0;

		INTC.IER.BIT.IRQ5E = 0;		/* IRQ5 Use */	// 00/09/13
		ITU1.GRA = 1843;
		ITU1.TCNT = 0;				/* ƒJƒEƒ“ƒ^[ƒNƒŠƒA */
		ITU.TSTR.BIT.STR1 = 1;		/* ƒ^ƒCƒ}[‚É‚æ‚é‚c‚l‚`“]‘—ŠJŽn */
}

