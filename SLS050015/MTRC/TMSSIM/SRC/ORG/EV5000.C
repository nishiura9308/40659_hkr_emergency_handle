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

#include <string.h>
#include	"iodefine.h"
#include	"table.h"

#include	"timer.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
#define	SEND_INITIAL		1
#define	SEND_INITIAL_RECV	2
#define	SEND_SETUP			3
#define	SEND_WAIT			4
#define	SEND_WAITUP			5
#define	SEND_VOICE			6
#define	SEND_VOICE_RECV		7
#define	SEND_BUSY			8
#define	SEND_BUSY_RECV		9
#define	SEND_ABORT			10
#define	SEND_ABORT_RECV		11

#define	MAX_EV5000_TIMEOUT	200
#define	MAX_EV5000_RETRY	3

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

extern	struct	CONFIG_INFO		config_data;		// Ý’èî•ñ

extern	struct	TIMER_CONTROL	timedata;		// ƒ^ƒCƒ}[î•ñ


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
static	char	phase;				// ˆ—ƒtƒF[ƒY
static	char	requestNo;			// —v‹•ú‘—”Ô†
static	char	timeout;			// ƒ^ƒCƒ€ƒAƒEƒgƒJƒEƒ“ƒ^[
static	char	errcnt;				// ƒGƒ‰[ƒJƒEƒ“ƒ^[
static	char	recvbuff[4];		// ŽóMƒoƒbƒtƒ@[
static	char	recvlen;			// ŽóMƒoƒCƒg”
static	char	recv_complete;		// ŽóMŠ®—¹ƒtƒ‰ƒO
static	char	sendbuff[16];		// ‘—Mƒoƒbƒtƒ@[
static	char	*sendp;				// ‘—Mƒ|ƒCƒ“ƒ^[

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< dvauEV5000Initialize >>									*
*					‚d‚u‚T‚O‚O‚O‰ŠúÝ’è										*
*	Syntax			dvauEV5000Initialize( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauEV5000Initialize( void )
{
		asci_0_Initialize( 19200, 8, 'E', 0 );
		sendp = "";
		phase = SEND_SETUP;
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
void asci_0_send_ir( int tend )
{
		if( *sendp=='\0' ){	/*‚·‚×‚Ä‘—M‚µ‚½‚Æ‚«*/
			if( tend ){
				asci_0_tx_disable();		/*ŽóM‚ÖØ‚è‘Ö‚¦*/
			}
			return;
		}

		asci_0_transmit( *sendp++ );	/*‚PƒoƒCƒg‚Ì‘—M*/
}

/************************************************************************
*																		*
*	Function name	<< asci_0_recv_ir >>								*
*					ŽóMŠ„‚èž‚Ýˆ—									*
*	Syntax			asci_0_recv_ir( char data, char status );			*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_0_recv_ir( char data, char status )
{
/*.	---------------------@ˆ—Ú×à–¾  ----------------------------	*/
/*  ƒtƒF[ƒY–ˆ‚ÉŽóMƒf[ƒ^‚ðŽæ‚èž‚Þ									*/
/*. ------------------------------------------------------------------  */
status=0;
		if( recvlen<sizeof(recvbuff)-1 ){
			recvbuff[recvlen++] = data;
			recvbuff[recvlen]='\0';
		}
		if( data=='\r' ){
			recv_complete = 1;
		}
}


/************************************************************************
*																		*
*	Function name	<< dvauEV5000Cyclic >>						*
*					‚d‚u‚T‚O‚O‚O‚Ö‚Ì‘—M‚Æ‰ž“šŠm”F							*
*	Syntax			dvauEV5000Cyclic( void );						*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauEV5000Cyclic( void )
{
		int		goOnFlag;

		switch( phase ){
		case SEND_SETUP:		/* ‘—Mƒf[ƒ^€”õˆ— */
			if( DUALPORT.ev5000_request.Initial ){
				DUALPORT.ev5000_request.Initial=0;

				phase = SEND_INITIAL;
			}
			if( DUALPORT.ev5000_request.Abort ){
				DUALPORT.ev5000_request.Abort=0;
			}
			if( DUALPORT.ev5000_request.Request ){
				DUALPORT.ev5000_request.Request=0;

				requestNo = 0;

				phase = SEND_WAIT;
			}
			timeout=0;
			errcnt=0;
			break;

		case SEND_WAIT:
			goOnFlag = 0;
			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.en ){
				if( config_data.lang.en ){
					goOnFlag = 1;
				}
			}
			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.cn ){
				if( config_data.lang.cn ){
					goOnFlag = 1;
				}
			}
			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.ma ){
				if( config_data.lang.ma ){
					goOnFlag = 1;
				}
			}

			if( goOnFlag ){
				timedata.ev5000timer = DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.timewait;
				phase = SEND_WAITUP;
				break;
			}

			requestNo++;
			if( requestNo>=MAX_SEQUENCE ){
				DUALPORT.ev5000_request.Complete=1;
				phase = SEND_SETUP;
				break;
			}
			break;

		case SEND_WAITUP:
			if( timedata.ev5000timer==0 ){
				phase = SEND_BUSY;
			}
			break;

		case SEND_VOICE:
			if( DUALPORT.ev5000_request.Abort ){
				DUALPORT.ev5000_request.Complete=1;
				phase = SEND_SETUP;
				break;
			}
			sprintf( sendbuff, "M000,0%03X\r", DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.sendCode );

			recvlen = 0;
			recv_complete = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_1_transmit( *sendp++ );
			phase = SEND_VOICE_RECV;
			break;

		case SEND_VOICE_RECV:
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
					timeout++;
					if( timeout>2 ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						DUALPORT.ev5000_request.Complete=1;
						phase = SEND_SETUP;
					}
					else {
						phase = SEND_VOICE;
					}
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='A' ){		/* EV5000 ‚ª ‚`‚b‚j‚ð‚©‚¦‚µ‚½ */
				// ³íˆ—
				timedata.ev5000timer=10;
				phase = SEND_BUSY;
				break;
			}

			// ˆÙíŽóMˆ—
			errcnt++;
			if( errcnt<3 ){
				phase = SEND_VOICE;
				break;
			}

			DUALPORT.ev5000_request.ev5000_error = 2;
			DUALPORT.ev5000_request.Complete=1;
			phase = SEND_SETUP;
			break;

		case SEND_BUSY:
			if( DUALPORT.ev5000_request.Abort ){
				phase = SEND_ABORT;
				break;
			}
			if( timedata.ev5000timer!=0 )
				break;

			strcpy( sendbuff, "B\r" );
			recvlen = 0;
			recv_complete = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_1_transmit( *sendp++ );
			phase = SEND_BUSY_RECV;
			break;

		case SEND_BUSY_RECV:
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
					timeout++;
					if( timeout>2 ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						DUALPORT.ev5000_request.Complete=1;
						phase = SEND_SETUP;
					}
					else {
						phase = SEND_BUSY;
					}
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='Y' ){		/* EV5000 ‚ª@‚a‚t‚r‚x‚Ì‚Æ‚« */
				// ƒrƒW[ˆ—
				timedata.ev5000timer = 10;
				phase = SEND_BUSY;
				break;
			}

			if( recvbuff[0]=='A' ){		/* EV5000 ‚ª ‚`‚b‚j‚ð‚©‚¦‚µ‚½ */
				// ³íˆ—
				requestNo++;
				if( requestNo>=MAX_SEQUENCE ){
					DUALPORT.ev5000_request.Complete=1;
					phase = SEND_SETUP;
					break;
				}
				phase = SEND_WAIT;
				break;
			}

			// ˆÙíŽóMˆ—
			errcnt++;

			if( errcnt<3 ){
				phase = SEND_BUSY;
				break;
			}

			DUALPORT.ev5000_request.ev5000_error = 2;
			DUALPORT.ev5000_request.Complete=1;
			phase = SEND_SETUP;
			break;

		case SEND_ABORT:
			strcpy( sendbuff, "A\r" );
			recvlen = 0;
			recv_complete = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_1_transmit( *sendp++ );
			phase = SEND_ABORT_RECV;
			break;

		case SEND_ABORT_RECV:
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
					timeout++;
					if( timeout>2 ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						DUALPORT.ev5000_request.Complete=1;
						phase = SEND_SETUP;
					}
					else {
						phase = SEND_ABORT;
					}
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='A' ){		/* EV5000 ‚ª ‚`‚b‚j‚ð‚©‚¦‚µ‚½ */
				// ³íˆ—
				DUALPORT.ev5000_request.Complete=1;
				phase = SEND_SETUP;
				break;
			}

			// ˆÙíŽóMˆ—
			errcnt++;

			if( errcnt<3 ){
				phase = SEND_ABORT;
				break;
			}

			DUALPORT.ev5000_request.ev5000_error = 2;
			DUALPORT.ev5000_request.Complete=1;
			phase = SEND_SETUP;
			break;

		case SEND_INITIAL:
			if( DUALPORT.ev5000_request.Abort ){
				phase = SEND_ABORT;
				break;
			}
			if( timedata.ev5000timer!=0 )
				break;
			strcpy( sendbuff, "B\r" );
			recvlen = 0;
			recv_complete = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_1_transmit( *sendp++ );
			phase = SEND_BUSY_RECV;
			break;

		case SEND_INITIAL_RECV:
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
					timeout++;
					if( timeout>2 ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						DUALPORT.ev5000_request.Complete=1;
						phase = SEND_SETUP;
					}
					else {
						phase = SEND_INITIAL;
					}
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='Y' ){		/* EV5000 ‚ª@‚a‚t‚r‚x‚Ì‚Æ‚« */
				// ƒrƒW[ˆ—
				timedata.ev5000timer = 10;
				phase = SEND_INITIAL;
				break;
			}

			if( recvbuff[0]=='A' ){		/* EV5000 ‚ª ‚`‚b‚j‚ð‚©‚¦‚µ‚½ */
				// ³íˆ—
				DUALPORT.ev5000_request.Complete=1;
				phase = SEND_SETUP;
				break;
			}

			// ˆÙíŽóMˆ—
			errcnt++;

			if( errcnt<3 ){
				phase = SEND_INITIAL;
				break;
			}

			DUALPORT.ev5000_request.ev5000_error = 2;
			DUALPORT.ev5000_request.Complete=1;
			phase = SEND_SETUP;
			break;
		}
}

