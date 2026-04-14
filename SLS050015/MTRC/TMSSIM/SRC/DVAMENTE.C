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

#include <stdio.h>
#include <string.h>

#include "iodefine.h"
#include "table.h"

#include "dosaccs.h"
#include "fcntl.h"

#include	"timer.h"


/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
#define	CRLF		0x0D
#define	ESC			0x1B

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
extern	void asci_1_Initialize( int baud, char data, char parity, char stop );
extern	void asci_1_tx_enable( void );
extern	void asci_1_tx_disable( char tend );
extern	void asci_1_transmit( unsigned char data );

extern	char	tms_start;
extern	union SDFORM	tms_sd_data;		// ‚s‚l‚r‚Ö‚ÌŽóMƒf[ƒ^
extern	union SDRFORM	tms_sdr_data;	// ‚s‚l‚r‚©‚ç‚Ì‘—Mƒf[ƒ^
extern	union SDFORM	tms_ld_data;		// ‚s‚l‚r‚Ö‚ÌŽóMƒf[ƒ^
extern	union SDRFORM	tms_ldr_data;	// ‚s‚l‚r‚©‚ç‚Ì‘—Mƒf[ƒ^
extern	struct	TIMER_CONTROL	timedata;		// ƒ^ƒCƒ}[î•ñ

extern	int	send_abort;
extern	int	recv_abort;
extern	int	send_ldr;
extern	int	recv_ldr;

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
static	char	recvbuff[512];		/*ŽóMƒoƒbƒtƒ@[*/
static	short	recvlen;			/*ŽóMƒoƒCƒg”*/
static	char	sendbuff[2048];		/*‘—Mƒoƒbƒtƒ@[*/
static	char	sendbuff2[2048];	/*‘—Mƒoƒbƒtƒ@[*/
static	short	sendlen;			/*‘—MƒoƒCƒg”*/
static	short	sendcnt;			/*‘—MƒJƒEƒ“ƒ^[*/
static	char	recvon;

static	char	recvmode;			// ŽóMƒ‚[ƒh
static	short	recvlen2;
static	char	recvbcc1;
static	char	recvbcc2;
static	char	recvbcc3;

static	char	sendmode;

static	char	resend;				// Ä‘—Mƒtƒ‰ƒO


							/* 5432 1098 7654 3210 */
							/* 0001 0000 0010 0001 */
#define	CRCCCITT	0x8408		/** X^16 + X^12 + X^5 + X^0 */

unsigned short	crcCCITT_table[256];
unsigned short	crcCode;

void GenerateCRC( void )
{
		int		i,j;
		unsigned short	crc;

		for( i=0; i<256; i++ ){
			crc = (unsigned short)i;
			for( j=0; j<8; j++ ){
				if( crc & 0x0001 ){
					crc >>=1;
					crc ^= CRCCCITT;
				}
				else {
					crc >>=1;
				}
			}
			crcCCITT_table[i] = crc;
		}
}

void InitializeCRC( void )
{
		crcCode = 0;
}

void CalculateCRCCCITT( unsigned char data )
{
		crcCode = crcCCITT_table[ ( crcCode ^  (unsigned short)data ) & 0xFF ] ^ ( crcCode >> 8 );
}

/*@@@E*/
/************************************************************************

	Function name	<< asci_1_send_ir >>
					‘—MŠ„‚èž‚Ýˆ—
	Syntax			asci_1_send_ir( tend );
	Argument		char tend : i: 	1=I—¹Š„‚èž‚Ý
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void asci_1_send_ir( char tend )
{
		if( sendcnt>=sendlen ){	/*‚·‚×‚Ä‘—M‚µ‚½‚Æ‚«*/
			asci_1_tx_disable( tend );		/*ŽóM‚ÖØ‚è‘Ö‚¦*/
			return;
		}

		asci_1_transmit( sendbuff2[sendcnt++] );	/*‚PƒoƒCƒg‚Ì‘—M*/
}

/************************************************************************

	Function name	<< dvauMenteSend >>
					‘—M€”õˆ—
	Syntax			dvauMenteSend( len );
	Argument		short len : i : ‘—Mƒf[ƒ^ƒoƒCƒg”
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void dvauMenteSend( short len )
{
		char	data;
		char	bcc;

		if( recvmode ){		/* ƒƒ“ƒeƒiƒ“ƒXƒ^[ƒ~ƒiƒ‹ƒRƒ}ƒ“ƒh */
			bcc = 0;
			sendbuff2[0] = 0x02;		/* STX */
			for( sendlen=0; sendlen<len; sendlen++ ){	/* Data Copy & BCC Calc */
				bcc = bcc+sendbuff[sendlen];
				sendbuff2[sendlen+1] = sendbuff[sendlen];
			}
			sendlen++;
			sprintf( sendbuff, "%02X", bcc & 0x00FF );	/* BCC Set */
			sendbuff2[sendlen++] = sendbuff[0];
			sendbuff2[sendlen++] = sendbuff[1];
			sendbuff2[sendlen++] = 0x03;	/* ETX */
		}
		else {			/* Debug Terminal */
			for( sendlen=0; sendlen<len; sendlen++ ){
				sendbuff2[sendlen] = sendbuff[sendlen];
			}
			sendbuff2[sendlen++] = '\r';	/* CR/LF */
			sendbuff2[sendlen++] = '\n';
			timedata.menteSendTimer = 0;	// ƒ^[ƒ~ƒiƒ‹ƒ‚[ƒh‚Ì‚Æ‚«‚ÍŽŸ‚Ìs‚ð‚·‚®‚É‘—M‚·‚é
		}

		sendcnt = 0;

		asci_1_tx_enable();		/*‘—MŠJŽn  RTS=ON*/

		data = sendbuff2[sendcnt++];
		asci_1_transmit( data );	/*‚PƒoƒCƒg‚Ì‘—M*/
}


/****** W <ADDRESS> <DATA> ******/
int W_command(char *p)
{
	unsigned long	address;
	int 	write_data;
	char	byte_data,*byte_address;

	sendmode = 0;

	if ( strlen(p)>=31 ){
		strcpy( sendbuff, "\r\nWRITE ADDRESS & DATA ERROR" ) ;
		dvauMenteSend( strlen(sendbuff) );
		return( -1 );		/*•¶Žš—ñ‚ª‘½‚·‚¬‚é*/
	}

	sscanf( p, " %lX %X", &address, &write_data );

	byte_address = (char *)address;
	byte_data = (char)write_data ;
	*byte_address = byte_data;
	write_data = (int)*byte_address;
	sprintf( sendbuff, "\r\nWRITE BYTE ADDRESS %08lX WRITE DATA %02X READ DATA %02X"
				,byte_address, byte_data & 0x00FF, write_data & 0x00FF	) ;

	return( 0 );
}

/****** R <ADDRESS> <DATA> ******/
int R_command(char *p)
{
	unsigned long	address;
	char	byte_data,*byte_address;

	sendmode = 0;

	sscanf( p, " %lX", &address );
	byte_address = (char *)address;
	byte_data = (int)*byte_address;
	sprintf( sendbuff,"\r\nREAD BYTE ADDRESS %08lX READ DATA %02X" ,byte_address, byte_data & 0xFF ) ;

	return( 0 );
}

/****** D <ADDRESS> ******/
int D_command(char *p)
{
static	unsigned long	address;
	unsigned char	byte_data,*byte_address;
	unsigned char	dump_data[16];
	int 	i,j;

	sendmode = 0;

	sscanf( p, " %lX", &address );
	address &= 0xFFFFFFF0L;
	printf( "\r\n                            B I N A R Y                        ASCII      " ) ;
	printf( "\r\n ADDRESS  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF\r\n\r\n" ) ;
	for ( i=0; i<16; i++,address+=16L ){
		memset( dump_data, '\0', sizeof ( dump_data )) ;
		for ( j=0; j<16; j++ ) {
			byte_address = (unsigned char *)(address+(unsigned long)j);
			byte_data = *byte_address;
			dump_data[j] = byte_data;
		}
		printf( "%08lX  " , address ) ;
		for ( j=0; j<16; j++ ) {
			printf( "%02X " , dump_data[j] ) ;
		}
		for ( j=0; j<16; j++ ) {
			if ( dump_data[j] < ' ' || dump_data[j] > 'z' ) dump_data[j] = ' ' ;
			printf( "%c" , dump_data[j] );
		}
		printf( "\r\n" );
	}
	sendbuff[0]='\0';
	return( 1 );
}




/****** TMSSDR ******/
const char	*TalkBackTable[][5] = {
{		"V1  ", "V2  ", "V3  ", "V4  ", "V5  " },
{		"Z1  ", "Z2  ", "Z3  ", "Z4  ", "Z5  " },
{		"X1  ", "X2  ", "X3  ", "X4  ", "X5  " },
{		"W1  ", "W2  ", "W3  ", "W4  ", "W5  " },
{		"Y1  ", "Y2  ", "Y3  ", "Y4  ", "Y5  " },
{		"X1  ", "X2  ", "X3  ", "X4  ", "X5  " },
{		"Z1  ", "Z2  ", "Z3  ", "Z4  ", "Z5  " },
{		"V1  ", "V2  ", "V3  ", "V4  ", "V5  " }
};

int TMSSDR_command(char *p)
{
		int		i,j;

		p = sendbuff;

		if( recv_ldr ){
			sprintf( p, "LDR %02X", recv_ldr );
			recv_ldr = 0;
			return( 0 );
		}

		switch( sendmode ){
		default:
			sendmode = 1;
		case 1:
				// ƒeƒXƒgó‹µ
			p += sprintf( p, "SDR1" );
			if( tms_sdr_data.SDR.testRequest.testExecRequest ){
				p += sprintf( p, "EXEC" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.testRequest.testStartRequest1 ){
				p += sprintf( p, "CAT1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.testRequest.testStartRequest2 ){
				p += sprintf( p, "CAT2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			p += sprintf( p, "%02X", tms_sdr_data.SDR.itemNo );

			sendmode++;
			break;

		case 2:
				// ƒeƒLƒXƒgƒAƒNƒeƒBƒuƒtƒ‰ƒOó‹µ
			p += sprintf( p, "SDR2" );
			if( tms_sdr_data.SDR.activeText.timeDateValid ){
				p += sprintf( p, "TIME" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.activeText.timeDateAdjustment ){
				p += sprintf( p, "ADJ " );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.activeText.carNoValid ){
				p += sprintf( p, "CARNO" );
			}
			else {
				p += sprintf( p, "     " );
			}
			if( tms_sdr_data.SDR.activeText.carTypeValid ){
				p += sprintf( p, "CARTYPE" );
			}
			else {
				p += sprintf( p, "       " );
			}

			sendmode++;
			break;

		case 3:
				// “ú•tŽž
			p += sprintf( p, "SDR3" );
			p += sprintf( p, "%01X%01X/%01X%01X/%01X%01X %01X%01X:%01X%01X:%01X%01X",
				tms_sdr_data.SDR.timeDate.dd10,
				tms_sdr_data.SDR.timeDate.dd1,
				tms_sdr_data.SDR.timeDate.MM10,
				tms_sdr_data.SDR.timeDate.MM1,
				tms_sdr_data.SDR.timeDate.yy10,
				tms_sdr_data.SDR.timeDate.yy1,
				tms_sdr_data.SDR.timeDate.hh10,
				tms_sdr_data.SDR.timeDate.hh1,
				tms_sdr_data.SDR.timeDate.mm10,
				tms_sdr_data.SDR.timeDate.mm1,
				tms_sdr_data.SDR.timeDate.ss10,
				tms_sdr_data.SDR.timeDate.ss1 );

			sendmode++;
			break;

		case 4:
				// ŽÔ—¼”Ô†
			p += sprintf( p, "SDR4" );
			p += sprintf( p, "%03X", tms_sdr_data.SDR.carInfo.carNo );
			p += sprintf( p, "%1X", tms_sdr_data.SDR.carInfo.carType );

			sendmode++;
			break;

		case 5:
				// ˜AŒ‹ƒR[ƒh
			p += sprintf( p, "SDR5" );
			p += sprintf( p, "%01X", tms_sdr_data.SDR.couplingUnitCode );

			sendmode++;
			break;

		case 6:
				// ƒXƒe[ƒVƒ‡ƒ“”Ô†
			p += sprintf( p, "SDR6" );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.stationNumber & 0x1F );
			p += sprintf( p, "%01u", (tms_sdr_data.SDR.stationNumber & 0x0E0)>>5 );

			sendmode++;
			break;

		case 7:
				// NextStation
			p += sprintf( p, "SDR7" );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.nextStationName );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.destination );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.routeCode );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.specialCode );

			sendmode++;
			break;

		case 8:
				// ”ñí’Ê•ñŠí
			p += sprintf( p, "SDR8" );
			for( i=0; i<8; i++ ){
				for( j=0; j<5; j++ ){
					if( tms_sdr_data.SDR.talkBack[i] & (0x01<<j) ){
						p+= sprintf( p, "%s", TalkBackTable[i][j] );
					}
					else {
						p+= sprintf( p, "    " );
					}
				}
			}

			sendmode++;
			break;

		case 9:
				// •ú‘—ƒtƒ‰ƒO
			p += sprintf( p, "SDR9" );
			if( tms_sdr_data.SDR.ATCpaActive.atcActive ){
				p+= sprintf( p, "PA" );
			}
			else {
				p+= sprintf( p, "  " );
			}
			if( tms_sdr_data.SDR.ATCpaActive.dbEffective){
				p+= sprintf( p, "DB" );
			}
			else {
				p+= sprintf( p, "  " );
			}

			sendmode++;
			break;

		case 10:
				// ƒhƒAƒtƒ‰ƒO
			p += sprintf( p, "SDRA" );
			if( tms_sdr_data.SDR.door.doorOpen==0 ){
				p+= sprintf( p, "  " );
			}
			else if( tms_sdr_data.SDR.door.doorOpen==1 ){
				p+= sprintf( p, " R" );
			}
			else if( tms_sdr_data.SDR.door.doorOpen==2 ){
				p+= sprintf( p, "L " );
			}
			else {
				p+= sprintf( p, "LR" );
			}
			if( tms_sdr_data.SDR.door.doorOpenEffective ){
				p+= sprintf( p, "DOOR" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.door.broadCastOff ){
				p+= sprintf( p, "OFF" );
			}
			else {
				p+= sprintf( p, "   " );
			}
			if( tms_sdr_data.SDR.doorStatus.doorClosed ){	// 2001/07/09 Add
				p+= sprintf( p, "DOOR" );
			}
			else {
				p+= sprintf( p, "    " );
			}

			sendmode++;
			break;

		case 11:
				// ŽÔ—¼”Ô†
			p += sprintf( p, "SDRB" );
			for( i=0; i<8; i++ ){
				p += sprintf( p, "%03X", tms_sdr_data.SDR.carInfo2[i].carNo );
				p += sprintf( p, "%1X", tms_sdr_data.SDR.carInfo2[i].carType );
			}

			sendmode=0;
			break;
		}

		return( 0 );
}

/****** TMSSD  ******/
int TMSSD__command(char *p)
{
		int		i,j;

			p = sendbuff;

		switch( sendmode ){
		default:
			sendmode = 1;

		case 1:
				// ƒeƒXƒgó‹µ
			p += sprintf( p, "SD1" );
			if( tms_sd_data.SD.testResponce.testExecutionStatus==0 ){
				p += sprintf( p, "INIT" );
			}
			if( tms_sd_data.SD.testResponce.testExecutionStatus==1 ){
				p += sprintf( p, "EXEC" );
			}
			if( tms_sd_data.SD.testResponce.testExecutionStatus==2 ){
				p += sprintf( p, "END " );
			}
			if( tms_sd_data.SD.testResponce.testStartResponce2 ){
				p += sprintf( p, "CAT2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.testResponce.testStartResponce1 ){
				p += sprintf( p, "CAT1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			p += sprintf( p, "%04X", tms_sd_data.SD.itemNo );
			if( tms_sd_data.SD.itemResult.OK ){
				p += sprintf( p, "OK" );
			}
			else if( tms_sd_data.SD.itemResult.NG ){
				p += sprintf( p, "NG" );
			}
			else {
				p += sprintf( p, "  " );
			}

			for( i=0; i<8; i++ ){
				if( tms_sd_data.SD.subItemResult & (0x01<<i) ){
					p += sprintf( p, "NG%u", i+1 );
				}
				else {
					p += sprintf( p, "   " );
				}
			}

			sendmode++;
			break;

		case 2:
			// ƒo[ƒWƒ‡ƒ“
			p += sprintf( p, "SD2" );
			p += sprintf( p, "%04X", tms_sd_data.SD.version );

			p += sprintf( p, "%02X%02X", tms_sd_data.SD.crcL, tms_sd_data.SD.crcH );

			sendmode++;
			break;

		case 3:
			// ”ñí’Ê•ñŠí
			p += sprintf( p, "SD3" );
			for( i=0; i<8; i++ ){
				for( j=0; j<5; j++ ){
					if( tms_sd_data.SD.passengerAlarm[i] & (0x01<<j) ){
						p+= sprintf( p, "%s", TalkBackTable[i][j] );
					}
					else {
						p+= sprintf( p, "    " );
					}
				}
			}

			sendmode++;
			break;

		case 4:
			// ’ÊMƒGƒ‰[
			p += sprintf( p, "SD4" );
			if( tms_sd_data.SD.dvaCommError.BIT.dvauCommError1 ){
				p += sprintf( p, "DVA1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.dvauCommError2 ){
				p += sprintf( p, "DVA2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.mcpCommError1 ){
				p += sprintf( p, "MCP1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.mcpCommError2 ){
				p += sprintf( p, "MCP2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.paaCommError1 ){
				p += sprintf( p, "PAA1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.paaCommError2 ){
				p += sprintf( p, "PAA2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.paaCommError3 ){
				p += sprintf( p, "PAA3" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.paaCommError4 ){
				p += sprintf( p, "PAA4" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.paaCommError5 ){
				p += sprintf( p, "PAA5" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.paaCommError6 ){
				p += sprintf( p, "PAA6" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.paaCommError7 ){
				p += sprintf( p, "PAA7" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.BIT.paaCommError8 ){
				p += sprintf( p, "PAA8" );
			}
			else {
				p += sprintf( p, "    " );
			}

			sendmode++;
			break;

		case 5:
			// ƒ‚[ƒh
			p += sprintf( p, "SD5" );
			if( tms_sd_data.SD.dvaStatus.BIT.manualMode ){
				p += sprintf( p, "MANUAL" );
			}
			else {
				p += sprintf( p, "      " );
			}
			if( tms_sd_data.SD.dvaStatus.BIT.cabMode ){
				p += sprintf( p, "CAB/CAB" );
			}
			else {
				p += sprintf( p, "       " );
			}
			if( tms_sd_data.SD.dvaStatus.BIT.emgMode ){
				p += sprintf( p, "EMERGENCY" );
			}
			else {
				p += sprintf( p, "         " );
			}
			if( tms_sd_data.SD.dvaStatus.BIT.specialMode ){
				p += sprintf( p, "SPECIAL" );
			}
			else {
				p += sprintf( p, "       " );
			}
			if( tms_sd_data.SD.dvaStatus.BIT.autoMode ){
				p += sprintf( p, "AUTO" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaStatus.BIT.doorMode ){
				p += sprintf( p, "DOOR" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaStatus.BIT.doorChimeMode ){
				p += sprintf( p, "CHIME" );
			}
			else {
				p += sprintf( p, "     " );
			}
			sendmode = 0;
			break;
		}

		return( 0 );
}

/****** START ******/
int START_command(char *p)
{
		sendmode = 0;

			tms_start = 1;
			
			strcpy( sendbuff, "OK" );

		return( 0 );
}

/****** STOP ******/
int STOP_command(char *p)
{
		sendmode = 0;

			tms_start = 0;
			
			strcpy( sendbuff, "OK" );

		return( 0 );
}


/****** SDR1 ******/
int SDR1_command(char *p)
{
		char	work[16];
		int		text1;

		sendmode = 0;

			if( memcmp( p, "EXEC", 4 )==0 ){
				tms_sdr_data.SDR.testRequest.testExecRequest=1;
			}
			else {
				tms_sdr_data.SDR.testRequest.testExecRequest=0;
			}
			p += 4;
			if( memcmp( p, "CAT1", 4 )==0 ){
				tms_sdr_data.SDR.testRequest.testStartRequest1=1;
			}
			else {
				tms_sdr_data.SDR.testRequest.testStartRequest1=0;
			}
			p += 4;
			if( memcmp( p, "CAT2", 4 )==0 ){
				tms_sdr_data.SDR.testRequest.testStartRequest2=1;
			}
			else {
				tms_sdr_data.SDR.testRequest.testStartRequest2=0;
			}
			p += 4;
			memcpy( work, p, 2 );
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.itemNo = text1;

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR2 ******/
int SDR2_command(char *p)
{
		sendmode = 0;

				// ƒeƒLƒXƒgƒAƒNƒeƒBƒuƒtƒ‰ƒOó‹µ
			if( memcmp( p, "TIME", 4 )==0 ){
				tms_sdr_data.SDR.activeText.timeDateValid =1;
			}
			else {
				tms_sdr_data.SDR.activeText.timeDateValid =0;
			}
			p += 4;
			if( memcmp( p, "ADJ ", 4 )==0 ){
				tms_sdr_data.SDR.activeText.timeDateAdjustment=1;
			}
			else {
				tms_sdr_data.SDR.activeText.timeDateAdjustment=0;
			}
			p += 4;
			if( memcmp( p, "CARNO", 5 )==0 ){
				tms_sdr_data.SDR.activeText.carNoValid=1;
			}
			else {
				tms_sdr_data.SDR.activeText.carNoValid=0;
			}
			p += 5;
			if( memcmp( p, "CARTYPE", 7 )==0 ){
				tms_sdr_data.SDR.activeText.carTypeValid=1;
			}
			else {
				tms_sdr_data.SDR.activeText.carTypeValid=0;
			}

			timedata.testTimeTimer = 100;		// ƒ^ƒCƒ€ƒtƒ‰ƒO‚ð‚n‚e‚e‚·‚éƒ^ƒCƒ}[

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR3 ******/
int SDR3_command(char *p)
{
		sendmode = 0;

				// “ú•tŽž
			tms_sdr_data.SDR.timeDate.dd10 = *p & 0x0F;
			p++;
			tms_sdr_data.SDR.timeDate.dd1 = *p & 0x0F;
			p++;
			p++;
			tms_sdr_data.SDR.timeDate.MM10 = *p & 0x0F;
			p++;
			tms_sdr_data.SDR.timeDate.MM1 = *p & 0x0F;
			p++;
			p++;
			tms_sdr_data.SDR.timeDate.yy10 = *p & 0x0F;
			p++;
			tms_sdr_data.SDR.timeDate.yy1 = *p & 0x0F;
			p++;
			p++;
			tms_sdr_data.SDR.timeDate.hh10 = *p & 0x0F;
			p++;
			tms_sdr_data.SDR.timeDate.hh1 = *p & 0x0F;
			p++;
			p++;
			tms_sdr_data.SDR.timeDate.mm10 = *p & 0x0F;
			p++;
			tms_sdr_data.SDR.timeDate.mm1 = *p & 0x0F;
			p++;
			p++;
			tms_sdr_data.SDR.timeDate.ss10 = *p & 0x0F;
			p++;
			tms_sdr_data.SDR.timeDate.ss1 = *p & 0x0F;

			timedata.tm_year = tms_sdr_data.SDR.timeDate.yy10 * 10
								+ tms_sdr_data.SDR.timeDate.yy1;
			timedata.tm_mon = tms_sdr_data.SDR.timeDate.MM10 * 10
								+ tms_sdr_data.SDR.timeDate.MM1;
			timedata.tm_day = tms_sdr_data.SDR.timeDate.dd10 * 10
								+ tms_sdr_data.SDR.timeDate.dd1;
			timedata.tm_hour = tms_sdr_data.SDR.timeDate.hh10 * 10
								+ tms_sdr_data.SDR.timeDate.hh1;
			timedata.tm_min = tms_sdr_data.SDR.timeDate.mm10 * 10
								+ tms_sdr_data.SDR.timeDate.mm1;
			timedata.tm_sec = tms_sdr_data.SDR.timeDate.ss10 * 10
								+ tms_sdr_data.SDR.timeDate.ss1;

			sprintf( sendbuff, "OK" );


		return( 0 );
}

/****** SDR4 ******/
int SDR4_command(char *p)
{
		char	work[16];
		int		text1;

		sendmode = 0;

				// ŽÔ—¼”Ô†
			memcpy( work, p, 3 );
			p+=3;
			work[3]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.carInfo.carNo = text1;

			memcpy( work, p, 1 );
			p+=1;
			work[1]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.carInfo.carType = text1;

			sprintf( sendbuff, "OK" );


		return( 0 );
}

/****** SDR5 ******/
int SDR5_command(char *p)
{
		char	work[16];
		int		text1;

		sendmode = 0;

				// ˜AŒ‹ƒR[ƒh
			memcpy( work, p, 1 );
			p+=1;
			work[1]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.couplingUnitCode = text1;

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR6 ******/
int SDR6_command(char *p)
{
		char	work[16];
		int		text1,text2;

		sendmode = 0;

				// ƒXƒe[ƒVƒ‡ƒ“”Ô†
			memcpy( work, p, 2 );
			p+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			memcpy( work, p, 1 );
			p+=1;
			work[1]='\0';
			sscanf( work, "%x", &text2 );
			tms_sdr_data.SDR.stationNumber = text1 | (text2<<5);

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR7 ******/
int SDR7_command(char *p)
{
		char	work[16];
		int		text1;

		sendmode = 0;

				// NextStation
			memcpy( work, p, 2 );
			p+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.nextStationName = text1;
			memcpy( work, p, 2 );
			p+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.destination = text1;
			memcpy( work, p, 2 );
			p+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.routeCode = text1;
			memcpy( work, p, 2 );
			p+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.specialCode = text1;

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR8 ******/
int SDR8_command(char *p)
{
		int		i,j;

		sendmode = 0;

				// ”ñí’Ê•ñŠí
		for( i=0; i<8; i++ ){
			for( j=0; j<5; j++ ){
				if( memcmp( p, TalkBackTable[i][j], 4 )==0 ){
					tms_sdr_data.SDR.talkBack[i] |= (0x0001<<j);
				}
				else {
					tms_sdr_data.SDR.talkBack[i] &= ~(0x0001<<j);
				}
				p+=4;
			}
		}

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR9 ******/
int SDR9_command(char *p)
{
		sendmode = 0;

				// •ú‘—ƒtƒ‰ƒO
			if( memcmp( p, "PA", 2 )==0 ){
				tms_sdr_data.SDR.ATCpaActive.atcActive=1;
			}
			else {
				tms_sdr_data.SDR.ATCpaActive.atcActive=0;
			}
			p += 2;
			if( memcmp( p, "DB", 2 )==0 ){
				tms_sdr_data.SDR.ATCpaActive.dbEffective=1;
			}
			else {
				tms_sdr_data.SDR.ATCpaActive.dbEffective=0;
			}
			p += 2;

			timedata.testBroadcastTimer = 100;		// •ú‘—ƒtƒ‰ƒO‚ð‚n‚e‚e‚·‚éƒ^ƒCƒ}[

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDRA ******/
int SDRA_command(char *p)
{
		sendmode = 0;


				// ƒhƒAƒtƒ‰ƒO
			if( memcmp( p, "  ", 2 )==0 ){
				tms_sdr_data.SDR.door.doorOpen=0;
			}
			else if( memcmp( p, " R", 2 )==0 ){
				tms_sdr_data.SDR.door.doorOpen=1;
			}
			else if( memcmp( p, "L ", 2 )==0 ){
				tms_sdr_data.SDR.door.doorOpen=2;
			}
			else {
				tms_sdr_data.SDR.door.doorOpen=3;
			}
			p += 2;

			if( memcmp( p, "DOOR", 4 )==0 ){
				tms_sdr_data.SDR.door.doorOpenEffective =1;
			}
			else {
				tms_sdr_data.SDR.door.doorOpenEffective =0;
			}
			p += 4;

			if( memcmp( p, "OFF", 3 )==0 ){
				tms_sdr_data.SDR.door.broadCastOff=1;
			}
			else {
				tms_sdr_data.SDR.door.broadCastOff=0;
			}
			p += 3;

			if( memcmp( p, "DOOR", 4 )==0 ){					// 2001/07/09 Add
				tms_sdr_data.SDR.doorStatus.doorClosed=1;
			}
			else {
				tms_sdr_data.SDR.doorStatus.doorClosed=0;
			}
			p += 4;

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDRB ******/
int SDRB_command(char *p)
{
		char	work[16];
		int		text1;
		int		i;

		sendmode = 0;

				// ŽÔ—¼”Ô†
			for( i=0; i<8; i++ ){
				memcpy( work, p, 3 );
				p+=3;
				work[3]='\0';
				sscanf( work, "%x", &text1 );
				tms_sdr_data.SDR.carInfo2[i].carNo = text1;
				memcpy( work, p, 1 );
				p+=1;
				work[1]='\0';
				sscanf( work, "%x", &text1 );
				tms_sdr_data.SDR.carInfo2[i].carType = text1;
			}

			sprintf( sendbuff, "OK" );

		return( 0 );
}


/****** LDR ******/
int LDR_command(char *p)
{
		char	work[16];
		int		text1;
		int		i;

		sendmode = 0;

		tms_ldr_data.LDR.blockNo = 0x80;

		InitializeCRC();
		for( i=0; i<16; i++ ){
			memcpy( work, p, 2 );
			p+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );

			tms_ldr_data.LDR.data[i] = text1;

			CalculateCRCCCITT( (unsigned char)text1 );
		}
		tms_ldr_data.LDR.data[i++] = (unsigned char)((crcCode) & 0x0FF);
		tms_ldr_data.LDR.data[i++] = (unsigned char)(((crcCode)>>8) & 0x0FF);

		send_ldr = 1;

		sprintf( sendbuff, "OK" );

		return( 0 );
}

struct JUMP_STR {
		char	*command;
		int		(*jump)( char *p );
};
static	struct JUMP_STR cmdtable[] = {
	"TMSSDR",	TMSSDR_command,
	"TMSSD ",	TMSSD__command,

	"START",	START_command,
	"STOP",	STOP_command,

	"SDR1",	SDR1_command,
	"SDR2",	SDR2_command,
	"SDR3",	SDR3_command,
	"SDR4",	SDR4_command,
	"SDR5",	SDR5_command,
	"SDR6",	SDR6_command,
	"SDR7",	SDR7_command,
	"SDR8",	SDR8_command,
	"SDR9",	SDR9_command,
	"SDRA",	SDRA_command,
	"SDRB",	SDRB_command,

	"LDR",	LDR_command,

	"D ",		D_command,
	"W ",		W_command,
	"R ",		R_command,
	NULL,	NULL
};

static	int		(*ContinueCommand)( char *p );

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
void dvaMenteReceive( void )
{
		int		i;
		char	*datan;
		char	buff[16];

		if( recvmode ){
			sprintf( buff, "%02X", recvbcc3 & 0x00FF );
			if( recvbuff[recvlen-2]!=buff[0] )
				return;
			if( recvbuff[recvlen-1]!=buff[1] )
				return;
			recvlen -= 2;
			recvbuff[recvlen] = '\0';
		}
		datan = recvbuff;

		for( i=0; cmdtable[i].command!=NULL; i++ ){
			if( memcmp( datan, cmdtable[i].command, strlen(cmdtable[i].command) )==0 ){
				datan += strlen(cmdtable[i].command);
				ContinueCommand = cmdtable[i].jump;
				if( cmdtable[i].jump( datan )==0 ){
					timedata.menteSendTimer = 500;		// 5 sec.
					dvauMenteSend( strlen(sendbuff) );
				}
				return;
			}
		}

		if( memcmp( datan, "RESET", 5 )==0 ){
			RESET();
		}

		strcpy( sendbuff, "NG" );
		dvauMenteSend( strlen(sendbuff) );
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

		if( data==0x02 ){
			recvmode = 1;
			recvlen = 0;
			recvbcc1 = 0;
			recvbcc2 = 0;
			recvbcc3 = 0;
			return;
		}
		if( data==0x03 ){
			recvon = 1;
			recvlen2 = recvlen;
			recvmode = 1;
			resend = 0;
			sendmode = 0;
			return;
		}

		if( data==0x06 ){		/* ACK */
			timedata.menteSendTimer = 0;		// ŽŸ‚Ìs‚ð‘—M‚·‚é
			resend = 0;
			return;
		}
		if( data==0x15 ){		/* NAK */
			timedata.menteSendTimer = 500;		// 5 sec.
			resend = 0;
			return;
		}

		if( data==CRLF ){
			recvbuff[recvlen]='\0';
			recvlen = 0;
			recvon = 1;
			recvmode = 0;
			resend = 0;
			sendmode = 0;
			return;
		}
		if( data==ESC ){
			recvmode = 0;
			recvon = 0;
			recvlen = 0;
			recvbuff[recvlen]='\0';

			resend = 0;
			sendmode = 0;

			sendcnt = 0;
			sendlen = 0;

			sendbuff[0]='\0';
			dvauMenteSend( strlen(sendbuff) );
			return;
		}

		recvbcc3 = recvbcc2;
		recvbcc2 = recvbcc1;
		recvbcc1 = recvbcc1 + data;
		if( recvlen<sizeof(recvbuff) ){
			recvbuff[recvlen++] = data;
			recvbuff[recvlen]='\0';
		}
}

/************************************************************************
*																		*
*	Function name	<< dvauMENTEInitialize >>						*
*					ƒƒ“ƒeƒiƒ“ƒX’ÊM‰ŠúÝ’è									*
*	Syntax			dvauMENTEInitialize( void );							*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauMENTEInitialize( void )
{
		asci_1_Initialize( 9600, 8, 'N', 0 );

		recvon = 0;
		recvlen = 0;
		sendcnt = 0;
		sendlen = 0;
		sendmode = 0;
		resend = 0;

		GenerateCRC();
}

void dvauMENTEMain( void )
{
		char	data;

		if( resend ){		// Ä‘—M—v‹‚ ‚è
			resend = 0;
			sendcnt = 0;

			asci_1_tx_enable();		/*‘—MŠJŽn  RTS=ON*/

			data = sendbuff2[sendcnt++];
			asci_1_transmit( data );	/*‚PƒoƒCƒg‚Ì‘—M*/
		}
		else if( sendcnt==sendlen ){		/* ‚·‚×‚Ä‘—M‚µ‚½‚Æ‚« */
			if( sendmode ){		/* ˜A‘±‘—M’† */
				if( timedata.menteSendTimer==0 ){		// ‘—MƒfƒBƒŒƒC
					if( ContinueCommand( "" )==0 ){	/* Ä“xŽŸ‚Ì‘—M‚·‚é */
						timedata.menteSendTimer = 500;		// 5 sec.
						dvauMenteSend( strlen(sendbuff) );
					}
				}
			}
		}

		if( recvon ){		/* ƒRƒ}ƒ“ƒhŽóMŠm”F */
			recvon = 0;
			dvaMenteReceive();	/* ƒRƒ}ƒ“ƒhˆ— */
		}

}

