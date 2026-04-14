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
#include    "iodefine.h"

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

extern	void	(*ERI0Jump)(void);
extern	void	(*RXI0Jump)(void);
extern	void	(*TXI0Jump)(void);
extern	void	(*TEI0Jump)(void);

extern	void	(*ERI1Jump)(void);
extern	void	(*RXI1Jump)(void);
extern	void	(*TXI1Jump)(void);
extern	void	(*TEI1Jump)(void);

extern	void asci_0_recv_ir( char data, char status );
extern	void asci_0_send_ir( char tend );

extern	void asci_1_recv_ir( char data, char status );
extern	void asci_1_send_ir( char tend );

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*@@@E*/

/***************************************************
	ŠÖ”–¼  FRX
	ˆ—“à—eF³íŽóMŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void asci_0_RX( void )
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

/***************************************************
	ŠÖ”–¼  Fasci_0_ER
	ˆ—“à—eFˆÙíŽóMŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void asci_0_ER( void )
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

/***************************************************
	ŠÖ”–¼  Fasci_0_TX
	ˆ—“à—eF‘—MŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void asci_0_TX( void )
{
		if( SCI0.SSR.BIT.TEND ){
			asci_0_send_ir( 1 );
		}
		else if( SCI0.SSR.BIT.TDRE ){
			asci_0_send_ir( 0 );
		}
}

/***************************************************
	ŠÖ”–¼  Fasci_0_TE
	ˆ—“à—eF‘—MI—¹Š„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void asci_0_TE( void )
{
		if( SCI0.SSR.BIT.TEND ){
			asci_0_send_ir( 1 );
		}
		else if( SCI0.SSR.BIT.TDRE ){
			asci_0_send_ir( 0 );
		}
}

/***************************************************
	ŠÖ”–¼  Fasci_1_RX
	ˆ—“à—eF³íŽóMŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void asci_1_RX( void )
{
		unsigned char	status;
		unsigned char	data;

		status = SCI1.SSR.BYTE;			/*Š„‚èž‚ÝƒXƒe[ƒ^ƒX‚Ì“Çž‚Ý*/
		if( status & 0x40 ){			/*ŽóMƒf[ƒ^—L‚èH*/
			data = SCI1.RDR;
			SCI1.SSR.BYTE = (unsigned char)(status & ~0x78); 	/*ŽóMƒGƒ‰[‚ðŠÜ‚ÞƒXƒe[ƒ^ƒXƒNƒŠƒA*/
			asci_1_recv_ir( data, status );
		}
}

/***************************************************
	ŠÖ”–¼  Fasci_1_ER
	ˆ—“à—eFˆÙíŽóMŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void asci_1_ER( void )
{
		unsigned char	status;
		unsigned char	data;

		status = SCI1.SSR.BYTE;	/*Š„‚èž‚ÝƒXƒe[ƒ^ƒX‚Ì“Çž‚Ý*/
		if( status & 0x40 ){	/*ŽóMƒf[ƒ^—L‚èH*/
			data = SCI1.RDR;
			asci_1_recv_ir( data, status );
		}
		SCI1.SSR.BYTE = (unsigned char)(status & ~0x78); 	/*ŽóMƒGƒ‰[‚ðŠÜ‚ÞƒXƒe[ƒ^ƒXƒNƒŠƒA*/
}

/***************************************************
	ŠÖ”–¼  Fasci_1_TX
	ˆ—“à—eF‘—MŠ„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void asci_1_TX( void )
{
		if( SCI1.SSR.BIT.TEND ){
			asci_1_send_ir( 1 );
		}
		else if( SCI1.SSR.BIT.TDRE ){
			asci_1_send_ir( 0 );
		}
}

/***************************************************
	ŠÖ”–¼  Fasci_1_TE
	ˆ—“à—eF‘—MI—¹Š„‚èž‚Ýˆ—

			name		i/o		explanation
parameter:	-
return	 :	-
***************************************************/
void asci_1_TE( void )
{
		if( SCI1.SSR.BIT.TEND ){
			asci_1_send_ir( 1 );
		}
		else if( SCI1.SSR.BIT.TDRE ){
			asci_1_send_ir( 0 );
		}
}

/************************************************************************
*																		*
*	Function name	<< asci_0_Initialize >>	
*					‰ŠúÝ’è
*	Syntax			asci_0_Initialize( int baud, int data, int parity, int stop );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_0_Initialize( int baud, char data, char parity, char stop )
{
		SCI0.SMR.BYTE = 0;
		SCI0.SMR.BIT.CA   = 0;	/* ’²•à“¯Šú */
		if( data==8 ){
			SCI0.SMR.BIT.CHR  = 0;	
		}
		else {
			SCI0.SMR.BIT.CHR  = 1;	
		}
		if( parity=='E' ){
			SCI0.SMR.BIT.PE   = 1;
			SCI0.SMR.BIT.OE   = 0;
		}
		else if( parity=='O' ){
			SCI0.SMR.BIT.PE   = 1;
			SCI0.SMR.BIT.OE   = 1;
		}
		else {
			SCI0.SMR.BIT.PE   = 0;
		}
		if( stop==0 ){
			SCI0.SMR.BIT.STOP = 0;
		}
		else {
			SCI0.SMR.BIT.STOP = 1;
		}
		SCI0.SMR.BIT.MP   = 0;
		if( baud>=2400 ){
			SCI0.SMR.BIT.CKS  = 0;					/* 1/1 Clock */
			SCI0.BRR = 191/( baud / 2400 );		/* 14.7456MHz */
		}
		else {
			SCI0.SMR.BIT.CKS  = 1;					/* 1/4 Clock */
			SCI0.BRR = 191/( baud / 600 );		/* 14.7456MHz */
		}

		PA.DR.BIT.B1 = 1;					/* RTS OFF */

		ERI0Jump = asci_0_ER;
		RXI0Jump = asci_0_RX;
		TXI0Jump = asci_0_TX;
		TEI0Jump = asci_0_TE;

		SCI0.SCR.BIT.RIE  = 1;
		SCI0.SCR.BIT.TE   = 1;
		SCI0.SCR.BIT.RE   = 1;
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
		SCI0.SCR.BIT.RE  = 0;
		SCI0.SCR.BIT.RIE  = 0;
		PA.DR.BIT.B1 = 0;		/* RTS ON */
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
void asci_0_tx_disable( char tend )
{
		SCI0.SCR.BIT.TIE = 0;			/* TIE off */
		if( tend ){
			SCI0.SCR.BIT.TEIE = 0;		/* TEIE off */
			PA.DR.BIT.B1 = 1;			/* RTS OFF */
			SCI0.SCR.BIT.RE  = 1;
			SCI0.SCR.BIT.RIE  = 1;
		}
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
		SCI0.SCR.BIT.TEIE = 1;			/* TEIE on */
}



/************************************************************************
*																		*
*	Function name	<< asci_1_Initialize >>	
*					‰ŠúÝ’è
*	Syntax			asci_1_Initialize( int baud, int data, int parity, int stop );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_Initialize( int baud, char data, char parity, char stop )
{
		SCI1.SMR.BYTE = 0;
		SCI1.SMR.BIT.CA   = 0;	/* ’²•à“¯Šú */
		if( data==8 ){
			SCI1.SMR.BIT.CHR  = 0;	
		}
		else {
			SCI1.SMR.BIT.CHR  = 1;	
		}
		if( parity=='E' ){
			SCI1.SMR.BIT.PE   = 1;
			SCI1.SMR.BIT.OE   = 0;
		}
		else if( parity=='O' ){
			SCI1.SMR.BIT.PE   = 1;
			SCI1.SMR.BIT.OE   = 1;
		}
		else {
			SCI1.SMR.BIT.PE   = 0;
		}
		if( stop==0 ){
			SCI1.SMR.BIT.STOP = 0;
		}
		else {
			SCI1.SMR.BIT.STOP = 1;
		}
		SCI1.SMR.BIT.MP   = 0;
		if( baud>=2400 ){
			SCI1.SMR.BIT.CKS  = 0;					/* 1/1 Clock */
			SCI1.BRR = 191/( baud / 2400 );		/* 14.7456MHz */
		}
		else {
			SCI1.SMR.BIT.CKS  = 1;					/* 1/4 Clock */
			SCI1.BRR = 191/( baud / 600 );		/* 14.7456MHz */
		}

		PA.DR.BIT.B0 = 1;		/* RTS OFF */

		ERI1Jump = asci_1_ER;
		RXI1Jump = asci_1_RX;
		TXI1Jump = asci_1_TX;
		TEI1Jump = asci_1_TE;

		SCI1.SCR.BIT.RIE  = 1;
		SCI1.SCR.BIT.TE   = 1;
		SCI1.SCR.BIT.RE   = 1;
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
		SCI1.SCR.BIT.RE  = 0;
		SCI1.SCR.BIT.RIE  = 0;
		PA.DR.BIT.B0 = 0;		/* RTS ON */
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
void asci_1_tx_disable( char tend )
{
		SCI1.SCR.BIT.TIE = 0;			/* TIE off */
		if( tend ){
			SCI1.SCR.BIT.TEIE = 0;			/* TEIE off */
			SCI1.SCR.BIT.RE  = 1;
			SCI1.SCR.BIT.RIE  = 1;
			PA.DR.BIT.B0 = 1;		/* RTS OFF */
		}
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
		SCI1.SCR.BIT.TEIE = 1;			/* TEIE on */
}

