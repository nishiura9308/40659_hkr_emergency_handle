/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	’ÊM§Œäƒ‚ƒWƒ…[ƒ‹												*
*-------------------------------------------------------------------*
*	Author		TOA Corporation									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

#include <string.h>

#include    <machine.h>
#include    "vect.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< asci_0_DTR_ON >>	
*					‚c‚s‚q@‚n‚m									*
*	Syntax			asci_0_DTR_ON( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_0_DTR_ON( void )
{
		PA.DR.BIT.B3 = 1;		/* DTR ON */
}

/************************************************************************
*																		*
*	Function name	<< asci_0_DTR_OFF >>	
*					‚c‚s‚q@‚n‚e‚e									*
*	Syntax			asci_0_DTR_OFF( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_0_DTR_OFF( void )
{
		PA.DR.BIT.B3 = 0;		/* DTR OFF */
}

/************************************************************************
*																		*
*	Function name	<< asci_0_tx_enable >>	
*					‘—M‹–‰Âˆ—										*
*	Syntax			asci_0_tx_enable( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_0_tx_enable( void )
{
		PA.DR.BIT.B2 = 0;		/* RTS ON */
}

/************************************************************************
*																		*
*	Function name	<< asci_0_tx_disable >>	
*					‘—M‹ÖŽ~ˆ—										*
*	Syntax			asci_0_tx_disable( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_0_tx_disable( void )
{
		SCI0.SCR.BIT.TIE = 0;			/* TIE off */
		PA.DR.BIT.B2 = 1;					/* RTS ON */
}

/************************************************************************
*																		*
*	Function name	<< asci_0_transmit >>	
*					‚PƒoƒCƒg‘—Mˆ—					
*	Syntax			asci_0_transmit( unsigned char data );	
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_0_transmit( unsigned char data )
{
		SCI0.TDR = data; 				/*‘—Mƒf[ƒ^ƒZƒbƒg*/
		SCI0.SSR.BIT.TDRE = 0;			/*‘—Mƒf[ƒ^—L‚èƒZƒbƒg*/
		SCI0.SCR.BIT.TIE = 1;			/* TIE on */
}

/************************************************************************
*																		*
*	Function name	<< asci_0_Signal >>	
*					DCD/RI/CTS/DSRM†ó‘Ô								*
*	Syntax			asci_0_Signal( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
/*
		b3: DCD
		b2: RI
		b1: CTS
		b0: DSR
*/
int asci_0_Signal( void )
{
		return ((P7.DR.BYTE>>4) & 0x0F);		/* input Signal */
}


/************************************************************************
*																		*
*	Function name	<< asci_1_DTR_ON >>	
*					‚c‚s‚q@‚n‚m									*
*	Syntax			asci_1_DTR_ON( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_DTR_ON( void )
{
		PA.DR.BIT.B1 = 1;		/* DTR ON */
}

/************************************************************************
*																		*
*	Function name	<< asci_1_DTR_OFF >>	
*					‚c‚s‚q@‚n‚e‚e									*
*	Syntax			asci_1_DTR_OFF( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_DTR_OFF( void )
{
		PA.DR.BIT.B1 = 0;		/* DTR OFF */
}


/************************************************************************
*																		*
*	Function name	<< asci_1_tx_enable >>	
*					‘—M‹–‰Âˆ—										*
*	Syntax			asci_1_tx_enable( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_tx_enable( void )
{
		PA.DR.BIT.B2 = 0;		/* RTS ON */
}

/************************************************************************
*																		*
*	Function name	<< asci_1_tx_disable >>	
*					‘—M‹ÖŽ~ˆ—										*
*	Syntax			asci_1_tx_disable( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_tx_disable( void )
{
		SCI1.SCR.BIT.TIE = 0;			/* TIE off */
		PA.DR.BIT.B2 = 1;					/* RTS ON */
}

/************************************************************************
*																		*
*	Function name	<< asci_1_transmit >>	
*					‚PƒoƒCƒg‘—Mˆ—					
*	Syntax			asci_1_transmit( unsigned char data );	
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_transmit( unsigned char data )
{
		SCI1.TDR = data; 				/*‘—Mƒf[ƒ^ƒZƒbƒg*/
		SCI1.SSR.BIT.TDRE = 0;			/*‘—Mƒf[ƒ^—L‚èƒZƒbƒg*/
		SCI1.SCR.BIT.TIE = 1;			/* TIE on */
}

/************************************************************************
*																		*
*	Function name	<< asci_1_Signal >>	
*					DCD/RI/CTS/DSRM†ó‘Ô								*
*	Syntax			asci_1_Signal( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
/*
		b3: DCD
		b2: RI
		b1: CTS
		b0: DSR
*/
int asci_1_Signal( void )
{
		return ((P7.DR.BYTE) & 0x0F);		/* input Signal */
}



#pragma section IntPRG
#pragma interrupt(RXI0,ERI0,TXI0)
//  vector 53 RXI0
/***************************************************
	ŠÖ”–¼  FRXI0
	ˆ—“à—eF³íŽóMŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void RXI0( void )
{
		unsigned char	status;
		unsigned char	data;

		status = SCI0.SSR.BYTE;			/*Š„‚èž‚ÝƒXƒe[ƒ^ƒX‚Ì“Çž‚Ý*/
		if( status & 0x40 ){			/*ŽóMƒf[ƒ^—L‚èH*/
			data = SCI0.RDR;
			SCI0.SSR.BYTE = (unsigned char)(status & ~0x78); 	/*ŽóMƒGƒ‰[‚ðŠÜ‚ÞƒXƒe[ƒ^ƒXƒNƒŠƒA*/
			asci_0_recv_ir( data, status );
		}
}

//  vector 52 ERI0
/***************************************************
	ŠÖ”–¼  FERI0
	ˆ—“à—eFˆÙíŽóMŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void ERI0( void )
{
		unsigned char	status;
		unsigned char	data;

		status = SCI0.SSR.BYTE;	/*Š„‚èž‚ÝƒXƒe[ƒ^ƒX‚Ì“Çž‚Ý*/
		if( status & 0x40 ){	/*ŽóMƒf[ƒ^—L‚èH*/
			data = SCI0.RDR;
			asci_0_recv_ir( data, status );
		}
		SCI0.SSR.BYTE = (unsigned char)(status & ~0x78); 	/*ŽóMƒGƒ‰[‚ðŠÜ‚ÞƒXƒe[ƒ^ƒXƒNƒŠƒA*/
}

//  vector 54 TXI0
/***************************************************
	ŠÖ”–¼  FTXI0
	ˆ—“à—eF‘—MŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void TXI0( void )
{
		if( SCI0.SSR.BIT.TDRE ){ /*‘—MƒŒƒWƒXƒ^[‹ó‚«*/
			asci_0_send_ir();
		}
}

#pragma interrupt(RXI1,ERI1,TXI1)
//  vector 57 RXI1
/***************************************************
	ŠÖ”–¼  FRXI1
	ˆ—“à—eF³íŽóMŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void RXI1( void )
{
		unsigned char	status;
		unsigned char	data;

		status = SCI1.SSR.BYTE;			/*Š„‚èž‚ÝƒXƒe[ƒ^ƒX‚Ì“Çž‚Ý*/
		if( status & 1x41 ){			/*ŽóMƒf[ƒ^—L‚èH*/
			data = SCI1.RDR;
			SCI1.SSR.BYTE = (unsigned char)(status & ~1x78); 	/*ŽóMƒGƒ‰[‚ðŠÜ‚ÞƒXƒe[ƒ^ƒXƒNƒŠƒA*/
			asci_1_recv_ir( data, status );
		}
}

//  vector 56 ERI1
/***************************************************
	ŠÖ”–¼  FERI1
	ˆ—“à—eFˆÙíŽóMŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void ERI1( void )
{
		unsigned char	status;
		unsigned char	data;

		status = SCI1.SSR.BYTE;	/*Š„‚èž‚ÝƒXƒe[ƒ^ƒX‚Ì“Çž‚Ý*/
		if( status & 1x41 ){	/*ŽóMƒf[ƒ^—L‚èH*/
			data = SCI1.RDR;
			asci_1_recv_ir( data, status );
		}
		SCI1.SSR.BYTE = (unsigned char)(status & ~1x78); 	/*ŽóMƒGƒ‰[‚ðŠÜ‚ÞƒXƒe[ƒ^ƒXƒNƒŠƒA*/
}

//  vector 58 TXI1
/***************************************************
	ŠÖ”–¼  FTXI1
	ˆ—“à—eF‘—MŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void TXI1( void )
{
		if( SCI1.SSR.BIT.TDRE ){ /*‘—MƒŒƒWƒXƒ^[‹ó‚«*/
			asci_0_send_ir();
		}
}

