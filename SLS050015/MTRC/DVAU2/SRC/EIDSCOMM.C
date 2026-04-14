/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	’ÊM§Œäƒ‚ƒWƒ…[ƒ‹												*
*-------------------------------------------------------------------*
*	Author		Nissin system lab.									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

#include    <machine.h>
#include	<string.h>

#include	"timer.h"
#include	"table.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
#define		STX			(0x02)				/* ƒeƒLƒXƒgŠJŽn			*/
#define		ETX			(0x03)				/* ƒeƒLƒXƒgI—¹			*/
#define		DLE			(0x10)				/* ‚c‚k‚dŠg’£			*/
#define		ACK			(0x06)				/* ³í‘—M‰ž“š			*/
#define		NAK			(0x15)				/* ˆÙí‘—M‰ž“š			*/
#define		SPC			(0x20)				/* ƒf[ƒ^‚Ì‹æØ‚è		*/


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
extern	struct	TIMER_CONTROL	timedata;

extern	void asci_1_Initialize( int baud, char data, char parity, char stop );
extern	void asci_1_tx_enable( void );
extern	void asci_1_tx_disable( char tend );
extern	void asci_1_transmit( unsigned char data );


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
/** ‘—MƒGƒŠƒA **/
static	struct	EIDS_FORMAT		eidsarea;

static	unsigned char	statusCount;

static	unsigned char	befMessage;		// ‘O‰ñ‘—MƒƒbƒZ[ƒWƒR[ƒh
static	unsigned char	*sendp;				/*‘—Mƒoƒbƒtƒ@[*/
static	unsigned char	sendlen;			/*‘—MƒoƒCƒg”*/
static	unsigned char	sendcnt;			/*‘—MƒJƒEƒ“ƒ^[*/

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< asci_1_recv_ir >>								*
*					ŽóMŠ„‚èž‚Ýˆ—									*
*	Syntax			asci_1_recv_ir( char data, char status );			*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_recv_ir( char data, char status )
{
status=0;
data =0;
}

/************************************************************************
*																		*
*	Function name	<< asci_1_send_ir >>								*
*					‘—MŠ„‚èž‚Ýˆ—									*
*	Syntax			asci_1_send_ir( int tend );								*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_send_ir( char tend )
{
		if( sendcnt==sendlen ){	/*‚·‚×‚Ä‘—M‚µ‚½‚Æ‚«*/
			asci_1_tx_disable( tend );		/*ŽóM‚ÖØ‚è‘Ö‚¦*/
			return;
		}

		sendcnt++;
		asci_1_transmit( *sendp++ );	/*‚PƒoƒCƒg‚Ì‘—M*/
}

/************************************************************************
*																		*
*	Function name	<< dvauEIDSInitialize >>						*
*					‚d‚h‚c‚r’ÊM‰ŠúÝ’è									*
*	Syntax			dvauEIDSInitialize( void );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauEIDSInitialize( void )
{

		asci_1_Initialize( 1200, 7, 'E', 0 );

		memset( &eidsarea, 0, sizeof(eidsarea) );

		eidsarea.Messages = 0x20;
		eidsarea.status.BIT.All1 = 1;
		eidsarea.status.BIT.reset = 1;

		statusCount = 5*5;
		timedata.eidstimer = 20;
}

/************************************************************************
*																		*
*	Function name	<< dvauEIDSSend >>										*
*					‘—M€”õˆ—										*
*	Syntax			dvauEIDSSend( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauEIDSSend( void )
{
		asci_1_tx_enable();		/*‘—MŠJŽn  RTS=ON*/

		sendcnt = 0;					/* ‘—Mƒ|ƒCƒ“ƒ^[‚OƒNƒŠƒA */

		eidsarea.stx = STX;
		eidsarea.etx = ETX;

		sendp = (unsigned char *)&eidsarea;
		sendlen = sizeof(eidsarea);

		sendcnt++;
		asci_1_transmit( *sendp++ );	/*Å‰‚Ì‚PƒoƒCƒg‚ð‘—M*/
}

/************************************************************************
*																		*
*	Function name	<< dvauEIDSSendCheck >>						*
*					‚Q‚O‚O‚‚r–ˆ‚Ì‘—M‹N“®									*
*	Syntax			dvauEIDSSendCheck( void );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauEIDSSendCheck( void )
{
		if( DUALPORT2.activeDVAU && DUALPORT2.activeDVAUrequest==0 ){	// Ø‚è‘Ö‚¦’†‚Å‚È‚¢‚±‚Æ
			if( timedata.eidstimer==0 ){
				timedata.eidstimer = 20;		// 200mS Wait

				if( statusCount ){			// ƒƒbƒZ[ƒW‚ð‚T•bŠÔ‘—M‚·‚é‚½‚ß‚Ìƒ`ƒFƒbƒN
					statusCount--;
					if( statusCount==0 ){
						eidsarea.status.BIT.reset = 0;
						eidsarea.Messages = 0x20;
					}
				}

				if( befMessage != DUALPORT2.eids.Message ){	// ƒƒbƒZ[ƒWƒR[ƒh‚ª•Ï‰»‚µ‚½‚ç@‘—M
					eidsarea.Messages = DUALPORT2.eids.Message;
					befMessage = DUALPORT2.eids.Message;
					statusCount = 5*5;		/* ‚T•bŠÔ‘—M‚·‚é */
				}
				eidsarea.status.BIT.doorstatus = DUALPORT2.eids.Door;	// ƒhƒAî•ñ‚ð‘—M
				eidsarea.Station = DUALPORT2.eids.Station;				// ‰w–¼î•ñ‚ð‘—M
				eidsarea.Route = DUALPORT2.eids.Route;					// ƒ‹[ƒgî•ñ‚ð‘—M

				dvauEIDSSend();
			}
		}
}

