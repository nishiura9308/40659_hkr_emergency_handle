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
#include	"iodefine.h"
#include	"table.h"

#include	"timer.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
union SDFORM	tms_sd_data;		// ‚s‚l‚r‚Ö‚ÌŽóMƒf[ƒ^
union SDRFORM	tms_sdr_data;	// ‚s‚l‚r‚©‚ç‚Ì‘—Mƒf[ƒ^

int	send_abort;
int	recv_abort;

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
extern	unsigned short	VERSION;				// ƒo[ƒWƒ‡ƒ“î•ñ
extern	struct	BACKUP_DATA		backup_data;	// ƒoƒbƒNƒAƒbƒvƒf[ƒ^

extern	struct st_ReadSIGNAL nowSignal;			// ÅV“Ç‚Ý‚¾‚µ’l
extern	struct	TIMER_CONTROL	timedata;		// ƒ^ƒCƒ}[î•ñ

extern	void HDLC_Initialize( int baud, char lowadr, int send_length );
extern	void HDLC_RecvEnable( void );
extern	void HDLC_SendEnable( char *buff, int send_length );

extern	int (*HDLC_ReceiveActive)( unsigned char data, unsigned char first_receive );
extern	int (*HDLC_ReceiveEnd)( char *recvdata, short recvcnt, unsigned char status );
extern	int (*HDLC_ReceiveAbort)( void );
extern	int (*HDLC_SendEnd)( void );
extern	int (*HDLC_SendAbort)( void );
extern	int (*HDLC_SendStart)( void );

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

struct st_dualport dual;

static	char			recvdata[256];
static	int				recvcnt;		// ŽóMƒoƒCƒg”‚ÌƒJƒEƒ“ƒg

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< tms_receive >>	
*					ŽóMˆ—
*	Syntax			tms_receive( data, status, first_receive );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int tms_receive( unsigned char data, unsigned char first_receive )
{
		if( first_receive ){
			recvcnt = 0;			// Å‰‚ÌŽóMƒoƒCƒg‚ÌŠm”F
		}
		if( recvcnt<sizeof(recvdata) ){
			recvdata[recvcnt++] = data;		// ŽóMƒf[ƒ^‚ÌÝ’è
		}

		return 0;
}

/************************************************************************
*																		*
*	Function name	<< tms_receive_end >>	
*					ŽóMˆ—
*	Syntax			tms_receive_end( status );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int tms_receive_end( char *recvdata, short recvcnt, unsigned char status )
{
		union SDFORM	*recvp;
		int		i;
		unsigned char	onoff,on;

		// ’ÊMƒGƒ‰[‚Ì‚Æ‚«‚Í@‰½‚à‚µ‚È‚¢
		if( status & 0x60 ){	/* CRC ERROR? or Over RUN Error? */
			recvcnt = 0;
			return 1;
		}

		recvp = (union SDFORM *)recvdata;

		// ŽóMƒAƒhƒŒƒXƒ`ƒFƒbƒN
		// Ž©ƒAƒhƒŒƒXˆÈŠO‚Í@‰½‚à‚µ‚È‚¢
		if( recvp->SD.lowadr!=0x60 ){
			recvcnt = 0;
			return 1;
		}
		if( recvp->SD.highadr!=0x01 ){
			recvcnt = 0;
			return 1;
		}
		if( recvp->SD.control!=0x13 ){
			recvcnt = 0;
			return 1;
		}

		if( recvcnt < sizeof(*recvp) ){
			return 1;
		}

		timedata.tmstimeout = 0;

		memcpy( &tms_sd_data, recvp, sizeof(tms_sd_data) );
}


/************************************************************************
*																		*
*	Function name	<< tms_send >>	
*					‘—Mˆ—
*	Syntax			tms_send( void );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int tms_send( void )
{
		timedata.tms_timer = 10;		// Cyclic Polling Time Set
		timedata.tmstimeout = 5;		// Timeout Set
		HDLC_RecvSendChange();
}

/************************************************************************
*																		*
*	Function name	<< tms_send_end >>	
*					‘—MI—¹ˆ—
*	Syntax			tms_send_end( void );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int tms_send_end( void )
{
		timedata.tmstimeout = 5;		// Timeout Set
		HDLC_RecvEnable();
}


/************************************************************************
*																		*
*	Function name	<< tms_send_abort >>	
*					‘—M’†’fˆ—
*	Syntax			tms_send_abort( void );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int tms_send_abort( void )
{
	send_abort++;
		timedata.tmstimeout = 2;		// Timeout Set
		HDLC_RecvEnable();
}

/************************************************************************
*																		*
*	Function name	<< tms_recv_abort >>	
*					ŽóM’†’fˆ—
*	Syntax			tms_recv_abort( void );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int tms_recv_abort( void )
{
	recv_abort++;
		HDLC_RecvEnable();
}


int tms_send_start( void )
{
		HDLC_SendEnable( (char *)&tms_sdr_data, sizeof(tms_sdr_data) );
}

/************************************************************************
*																		*
*	Function name	<< tms_initialize >>	
*					‰ŠúÝ’è
*	Syntax			tms_initialize( void );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void tms_initialize( void )
{
		/** ‚g‚c‚k‚b’ÊM•”‚ð‰ŠúÝ’è **/
		HDLC_Initialize( 9600, 0x60, sizeof(tms_sdr_data) );

		// ‘—Mî•ñ‚Ì‰ŠúÝ’è
		memset( &tms_sdr_data, 0, sizeof(tms_sdr_data) );	// ‰Šú‚ÍƒŠƒZƒbƒgó‘Ô
		tms_sdr_data.SDR.lowadr = 0x60;					// ƒAƒhƒŒƒXÝ’è
		tms_sdr_data.SDR.highadr = 0x01;
		tms_sdr_data.SDR.control = 0x13;					// ‚t‚hƒR[ƒhÝ’è

		tms_sdr_data.SDR.code = 0x20;						// ‚r‚c‚qƒtƒH[ƒ}ƒbƒg‚ÌƒR[ƒh

		HDLC_ReceiveActive = tms_receive;
		HDLC_ReceiveEnd = tms_receive_end;
		HDLC_ReceiveAbort = tms_recv_abort;
		HDLC_SendEnd = tms_send_end;
		HDLC_SendAbort = tms_send_abort;
		HDLC_SendStart = tms_send_start;
}


