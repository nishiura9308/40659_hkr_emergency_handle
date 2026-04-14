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
static	unsigned char	recvbuff[256];		/*ŽóMƒoƒbƒtƒ@[*/
static	unsigned char	recvlen;			/*ŽóMƒoƒCƒg”*/
static	unsigned char	sendbuff[128];		/*‘—Mƒoƒbƒtƒ@[*/
static	unsigned char	sendlen;			/*‘—MƒoƒCƒg”*/
static	unsigned char	sendcnt;			/*‘—MƒJƒEƒ“ƒ^[*/

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< dvauMenteSend >>									*
*					‘—M€”õˆ—										*
*	Syntax			dvauMenteSend( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauMenteSend( void )
{
		strcpy( sendbuff, menteMessage );
		strcat( sendbuff, "\r\n" );
		sendlen = strlen(sendbuff);

		sendcnt = 0;

		asci_0_tx_enable();		/*‘—MŠJŽn  RTS=ON*/

		asci_1_send_ir();		/*Å‰‚Ì‚PƒoƒCƒg‚ð‘—M*/
}

/************************************************************************
*																		*
*	Function name	<< dvauMenteReceive >>									*
*					ŽóMˆ—											*
*	Syntax			dvauMenteReceive( unsigned char *data );		*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int dvauMenteReceive( unsigned char *datan )
{
		return 0;
}


/************************************************************************
*																		*
*	Function name	<< asci_1_recv_ir >>								*
*					ŽóMŠ„‚èž‚Ýˆ—									*
*	Syntax			asci_1_recv_ir( char data, char status );			*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_recv_ir( char data, char status )
{
/*.	---------------------@ˆ—Ú×à–¾  ----------------------------	*/
/*  ƒtƒF[ƒY–ˆ‚ÉŽóMƒf[ƒ^‚ðŽæ‚èž‚Þ									*/
/*. ------------------------------------------------------------------  */
status=0;

		if( data==CR ){
			recvbuff[recvlen]='\0';
			dvauReceive( recvbuff );
			return;
		}
		if( data==ESC ){
			recvlen = 0;
			recvbuff[recvlen]='\0';
			return;
		}

		if( recvlen<sizeof(recvbuff) ){
			recvbuff[recvlen++] = data;
			recvbuff[recvlen]='\0';
		}
}

/************************************************************************
*																		*
*	Function name	<< asci_1_send_ir >>								*
*					‘—MŠ„‚èž‚Ýˆ—									*
*	Syntax			asci_1_send_ir( void );								*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_send_ir( void )
{
		if( sendcnt==sendlen ){	/*‚·‚×‚Ä‘—M‚µ‚½‚Æ‚«*/
			asci_1_tx_disable();		/*ŽóM‚ÖØ‚è‘Ö‚¦*/
			return;
		}

		asci_1_transmit( sendbuff[sendcnt++] );	/*‚PƒoƒCƒg‚Ì‘—M*/
}

