/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	メンテナンスターミナル制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/07		K.Honda
				a01. 2001/3/22
				a02. 2001/07/09

*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "iodefine.h"

#include "table.h"
#include "trace.h"
#include	"timer.h"

#include "dosaccs.h"
#include "fcntl.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#define	CRLF		0x0D
#define	ESC			0x1B

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	void asci_1_Initialize( int baud, char data, char parity, char stop );
extern	void asci_1_tx_enable( void );
extern	void asci_1_tx_disable( char tend );
extern	void asci_1_transmit( unsigned char data );

extern	struct TRACE *TraceGet( int first );
extern	void TraceText( struct TRACE *getp, char *buffer );

extern	union SDRFORM	tms_sdr_data;		// ＴＭＳからの受信データ
extern	union SDFORM	tms_sd_data;	// ＴＭＳへの送信データ

extern	struct	BACKUP_DATA		backup_data;		// バックアップデータ

extern	struct	TIMER_CONTROL	timedata;		// タイマー情報

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	char	recvbuff[512];		/*受信バッファー*/
static	short	recvlen;			/*受信バイト数*/
static	char	sendbuff[512];		/*送信バッファー*/
static	char	sendbuff2[512];		/*送信バッファー*/
static	short	sendlen;			/*送信バイト数*/
static	short	sendcnt;			/*送信カウンター*/
static	char	recvon;

static	char	recvmode;			// 受信モード
static	short	recvlen2;
static	char	recvbcc1;
static	char	recvbcc2;
static	char	recvbcc3;

static	char	sendmode;

static	char	down_fname[32];
static	FILE	*downfd;			// ダウンロードファイルＦＤ
static	char	befSeqcount;		// 前回受信したシーケンス番号

static	int		downno;

static	struct TRACE *tracep;		// トレース取り出しポインター

static	char	resend;				// 再送信フラグ

/*@@@E*/
/************************************************************************

	Function name	<< asci_1_send_ir >>
					送信割り込み処理
	Syntax			asci_1_send_ir( tend );
	Argument		char tend : i: 	1=終了割り込み
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void asci_1_send_ir( char tend )
{
		if( sendcnt>=sendlen ){	/*すべて送信したとき*/
			asci_1_tx_disable( tend );		/*受信へ切り替え*/
			return;
		}

		asci_1_transmit( sendbuff2[sendcnt++] );	/*１バイトの送信*/
}

/************************************************************************

	Function name	<< dvauMenteSend >>
					送信準備処理
	Syntax			dvauMenteSend( len );
	Argument		short len : i : 送信データバイト数
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/10/10	送受信にＡＣＫ／ＮＡＫを追加

*************************************************************************/
void dvauMenteSend( short len )
{
		char	data;
		char	bcc;

		if( recvmode ){		/* メンテナンスターミナルコマンド */
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
			timedata.menteSendTimer = 0;	// ターミナルモードのときは次の行をすぐに送信する
		}

		sendcnt = 0;

		asci_1_tx_enable();		/*送信開始  RTS=ON*/

		data = sendbuff2[sendcnt++];
		asci_1_transmit( data );	/*１バイトの送信*/
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
		return( -1 );		/*文字列が多すぎる*/
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




/************************************************************************

	Function name	<< TMSSDR_command >>
					ＴＭＳ通信のＳＤＲデータ送信
	Syntax			ret = TMSSDR_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
					a01 2001/07/09  ドア情報の追加

*************************************************************************/
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

/****** TMSSDR ******/
int TMSSDR_command(char *p)
{
		int		i,j;

		p = sendbuff;

		switch( sendmode ){
		default:
			sendmode = 1;
		case 1:
				// テスト状況
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
				// テキストアクティブフラグ状況
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
				// 日付時刻
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
				// 車両番号
			p += sprintf( p, "SDR4" );
			p += sprintf( p, "%03X", tms_sdr_data.SDR.carInfo.carNo );
			p += sprintf( p, "%1X", tms_sdr_data.SDR.carInfo.carType );

			sendmode++;
			break;

		case 5:
				// 連結コード
			p += sprintf( p, "SDR5" );
			p += sprintf( p, "%01X", tms_sdr_data.SDR.couplingUnitCode );

			sendmode++;
			break;

		case 6:
				// ステーション番号
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
				// 非常通報器
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
				// 放送フラグ
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
				// ドアフラグ
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
			if( tms_sdr_data.SDR.doorStatus.doorClosed ){
				p+= sprintf( p, "DOOR" );
			}
			else {
				p+= sprintf( p, "    " );
			}

			sendmode++;
			break;

		case 11:
				// 車両番号
			p += sprintf( p, "SDRB" );
			for( i=0; i<8; i++ ){
				p += sprintf( p, "%03X", tms_sdr_data.SDR.carInfo2[i].carNo );
				p += sprintf( p, "%1X", tms_sdr_data.SDR.carInfo2[i].carType );
			}

			sendmode++;
			break;

		case 12:
				// 非常通報器リセット
			p += sprintf( p, "SDRC" );
			for( i=0; i<8; i++ ){
				for( j=0; j<5; j++ ){
					if( tms_sdr_data.SDR.PadReset[i] & (0x01<<j) ){
						p+= sprintf( p, "%s", TalkBackTable[i][j] );
					}
					else {
						p+= sprintf( p, "    " );
					}
				}
			}
			sendmode=0;
			break;
		}

		return( 0 );
}

/************************************************************************

	Function name	<< TMSSD__command >>
					ＴＭＳ通信のＳＤデータ送信
	Syntax			ret = TMSSD__command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/03/22	ＣＲＣデータの追加

*************************************************************************/
/****** TMSSD  ******/
int TMSSD__command(char *p)
{
		int		i,j;

			p = sendbuff;

		switch( sendmode ){
		default:
			sendmode = 1;

		case 1:
				// テスト状況
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
			// バージョン
			p += sprintf( p, "SD2" );
			p += sprintf( p, "%04X", tms_sd_data.SD.version );

			p += sprintf( p, "%02X%02X", tms_sd_data.SD.crcL, tms_sd_data.SD.crcH );

			sendmode++;
			break;

		case 3:
			// 非常通報器
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
			// 通信エラー
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
			// モード
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


/************************************************************************

	Function name	<< TROUBLE__command >>
					ＤＶＡＵトラブルデータ送信
	Syntax			ret = TROUBLE_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
/****** TROUBLE  ******/
int TROUBLE_command(char *p)
{
		int		i,j;

		p = sendbuff;

		switch( sendmode ){
		default:
			sendmode = 1;

		case 1:
			p += sprintf( p, "TRB1" );

			if( DUALPORT.nowTrouble & 0x01 ){
				p += sprintf( p, "24VSIG" );
			}
			else {
				p += sprintf( p, "      " );
			}
			if( DUALPORT.nowTrouble & 0x02 ){
				p += sprintf( p, "SIGNAL" );
			}
			else {
				p += sprintf( p, "      " );
			}
			if( DUALPORT.nowTrouble & 0x04 ){
				p += sprintf( p, "EV5000" );
			}
			else {
				p += sprintf( p, "      " );
			}
			sendmode++;
			break;

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			i = sendmode-2;
			p += sprintf( p, "TRB%u", sendmode );

			if( DUALPORT.answerData.trouble[i].BYTE ){
				p += sprintf( p, "PAA%u : ", i+1 );
				for( j=0; j<8; j++ ){
					if( DUALPORT.answerData.trouble[i].BYTE & (0x001<<j) ){
						if( j<2 ){
							p += sprintf( p, "AMP%u", j+1 );
						}
						else {
							p += sprintf( p, "SP%u ", j-2+1 );
						}
					}
					else {
						p += sprintf( p, "    " );
					}
				}
			}
			else {
				p += sprintf( p, "       " );
			}

			if( sendmode<9 )
				sendmode++;
			else
				sendmode = 0;
			break;
		}


		return( 0 );
}

/************************************************************************

	Function name	<< DOWN_command >>
					ファイルデータ受信処理
	Syntax			ret = DOWN_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
int DOWN_command(char *p)
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
	・メンテナンスパソコンからＡＴＡカードのファイルを受信する
	     012345678901234567890
	DOWN CONFIG.DAT  B0hex data
	DOWN ANNOUNCE.DATB0hex data
	DOWN ROUTExx.DAT B0hex data
	DOWN DVAU1.MOT   T0asci data
	DOWN DVAU2.MOT   T0asci data
	DOWN xxxxxxxxx   E0
	------------------------------------------------------------------[@@@E]*/
		int		i,j;
		char	fname[32];
		int		text1;
		char	text2;
		FILE	*fd1,*fd2;
		int		year,mon,day,hour,min,sec;
extern	unsigned short	now_date;		/*現在日付　ＤＯＳフォーマット*/
extern	unsigned short	now_time;		/*現在時刻　ＤＯＳフォーマット*/
extern	char	dosaccs_workbuff[512];		/*ストリームバッファー*/

		sendmode = 0;

		INTC.IER.BIT.IRQ5E = 0;		/* IRQ5 unUse */

		fname[0]='A';
		fname[1]=':';
		fname[2]='\\';
		for( i=0; i<12; i++ ){
			if( *(p+i)==' ' ){
				break;
			}
			fname[i+3]= *(p+i);
			fname[i+4]='\0';
		}
		if( strcmp( down_fname, fname )!=0 || *(p+13)=='0' ){
			if( downfd!=NULL ){
				fclose( downfd );
				downfd = NULL;
			}
			strcpy( down_fname, fname );
		}

		if( downfd==NULL ){
			if( *(p+12)=='E' ){
				sprintf( sendbuff, "OK" );		// すでに受信している
				return( 0 );
			}

			unlink( "A:\\TEMP.TMP" );
			downfd = fopen( "A:\\TEMP.TMP", "w" );
			if( downfd==NULL ){
				sprintf( sendbuff, "NG1" );
				return( 0 );
			}
			setvbuf( downfd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );
		}
		else {
			if( befSeqcount == *(p+13) ){
				sprintf( sendbuff, "OK" );		// すでに受信している
				return( 0 );
			}
		}

		befSeqcount = *(p+13);

		if( *(p+12)=='T' ){
			p += 14;
			fprintf( downfd, "%s\r\n", p );		// テキストデータ
			sprintf( sendbuff, "OK" );
		}
		else if( *(p+12)=='B' ){
			p += 14;
			while( *p ){
				sscanf( p, "%02X", &text1 );
				p += 2;
				text2 = (char)text1;
				fwrite( &text2, 1, 1, downfd );
			}
			sprintf( sendbuff, "OK" );
		}
		else if( *(p+12)=='E' ){
			sscanf( p+14, "%04u%02u%02u%02u%02u%02u",
					&year, &mon, &day,
					&hour, &min, &sec );
			now_date = (((unsigned short)(year-1980))<<9)		/* 2000-1980 */
						| (((unsigned short)mon)<<5)
						| (((unsigned short)day));
			now_time = (((unsigned short)hour)<<11)
						| (((unsigned short)min)<<5)
						| (((unsigned short)sec)>>1);

			if( downfd==NULL ){
				sprintf( sendbuff, "NG2" );
				return( 0 );
			}
			fclose( downfd );
			downfd = NULL;

			unlink( fname );
			if( rename( "A:\\TEMP.TMP", fname ) ){
				sprintf( sendbuff, "NG3" );
			}
			else {
				sprintf( sendbuff, "OK" );
				if( fname[3]=='A' ){
					TraceAdd( EVENT_DOWNLOAD_CONFIG );
				}
				if( fname[3]=='R' ){
					TraceAdd( EVENT_DOWNLOAD_ROUTE );
				}
				if( fname[3]=='D' ){
					TraceAdd( EVENT_DOWNLOAD_PROG );
				}
			}
		}
		else {
			sprintf( sendbuff, "NG4" );
		}

		return(0);
}

/************************************************************************

	Function name	<< REBOOT_command >>
					再起動処理
	Syntax			ret = REBOOT_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
/****** REBOOT ******/
/*相手ＤＶＡＵへファイルを送信し、リセットする*/
static const	char *downfiles[] = {
	"A:\\DVAU1.MOT",
	"A:\\DVAU2.MOT",
	NULL
};

int REBOOT_command(char *p)
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
	・相手のＤＶＡＵへプログラムを送信し、互いにリセットする
	------------------------------------------------------------------[@@@E]*/
		int		i;
		char	filename[32];
		FCB		*fcb;
		int		len;
extern	unsigned short	now_date;		/*現在日付　ＤＯＳフォーマット*/
extern	unsigned short	now_time;		/*現在時刻　ＤＯＳフォーマット*/
extern	char	dosaccs_workbuff[512];		/*ストリームバッファー*/

		INTC.IER.BIT.IRQ5E = 0;		/* IRQ5 unUse */

		sprintf( sendbuff, "WAIT" );
		switch( sendmode ){
		default:
			sendmode = 1;
			downno = 0;
		case 1:				// Fix files Download Start
			if( downfd!=NULL ){
				fclose( downfd );
				downfd = NULL;
			}

			strcpy( filename, downfiles[downno] );

			downfd = fopen(filename, "r");
			if( downfd==NULL ){
				sendmode = 0;
				sprintf( sendbuff, "NG1" );
				return( 0 );
			}

			setvbuf( downfd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );

			for( i=0; filename[i]; i++ ){
				DUALPORT.download_filename[i] = filename[i];
			}
			DUALPORT.download_filename[i] = '\0';

			DUALPORT.download_seq=0;
			DUALPORT.download_size = 0;
			DUALPORT.download_active = 1;

			sendmode++;

		case 2:			// fix files check & send
			if( DUALPORT.download_active==3 ){
				fclose( downfd );
				sendmode = 4;
				sprintf( sendbuff, "NG2" );
				return( 0 );
			}
			if( DUALPORT.download_active ){
				if( timedata.checkTimer==0 ){
					timedata.checkTimer=100;
					dvauMenteSend( strlen(sendbuff) );
				}
				return( 1 );
			}

			len=fread( sendbuff2, 1, 64, downfd );

			fcb = get_fcb( downfd->_iofd );

			DUALPORT.download_date = fcb->w_date;
			DUALPORT.download_time = fcb->w_time;
			DUALPORT.download_size = len;

			for( i=0; i<len; i++ ){
				DUALPORT.download_data[i] = sendbuff2[i];
			}
			if( len<64 ){
				DUALPORT.download_seq++;
				if( DUALPORT.download_seq>9 )
					DUALPORT.download_seq=1;
				DUALPORT.download_active = 2;
				fclose( downfd );
				downfd = NULL;
				sendmode++;
			}
			else {
				DUALPORT.download_seq++;
				if( DUALPORT.download_seq>9 )
					DUALPORT.download_seq=1;
				DUALPORT.download_active = 1;
			}
			break;

		case 3:				// next fix files check
			if( DUALPORT.download_active==3 ){
				sendmode = 4;
				sprintf( sendbuff, "NG3" );
				return( 0 );
			}
			if( DUALPORT.download_active ){
				if( timedata.checkTimer==0 ){
					timedata.checkTimer=100;
					dvauMenteSend( strlen(sendbuff) );
				}
				return( 1 );
			}

			downno++;
			if( downfiles[downno]!=NULL ){
				sendmode = 1;
				break;
			}
			sendmode++;
			downno = 0;

		case 4:
			sendmode++;

			sprintf( sendbuff, "OK" );
			dvauMenteSend( strlen(sendbuff) );

			TraceAdd( EVENT_DOWNLOAD_REBOOT );

			for( timedata.checkTimer = 200; timedata.checkTimer!=0; ){
				DUALPORT.activeDVAU = 1;
			}

		case 5:			// Reset & Reboot
			for( timedata.checkTimer = 500; timedata.checkTimer!=0; ){
				DUALPORT.activeDVAU = 1;
				DUALPORT.mainActive = 2;
			}
			RESET();
			break;
		}

		return(1);
}

/************************************************************************

	Function name	<< RECONFIG_command >>
					再起動処理
	Syntax			ret = RECONFIG_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
/****** RECONFIG ******/
/*相手ＤＶＡＵへファイルを送信し、リセットする*/
static const	char *downfiles2[] = {
	"A:\\CONFIG.DAT",
	"A:\\ANNOUNCE.DAT",
	"A:\\CONFIG.BEF",
	"A:\\ANNOUNCE.BEF",
	"A:\\SWITCH.DAT",
	NULL
};

int RECONFIG_command(char *p)
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
	・相手のＤＶＡＵへパラメータを送信し、互いにリセットする
	------------------------------------------------------------------[@@@E]*/
		int		i;
		char	filename[32];
		FCB		*fcb;
		int		len;
extern	unsigned short	now_date;		/*現在日付　ＤＯＳフォーマット*/
extern	unsigned short	now_time;		/*現在時刻　ＤＯＳフォーマット*/
extern	char	dosaccs_workbuff[512];		/*ストリームバッファー*/

		INTC.IER.BIT.IRQ5E = 0;		/* IRQ5 unUse */

		sprintf( sendbuff, "WAIT");
		switch( sendmode ){
		default:
			sendmode = 1;
			downno = 0;
		case 1:				// Fix files Download Start
			if( downfd!=NULL ){
				fclose( downfd );
				downfd = NULL;
			}

			strcpy( filename, downfiles2[downno] );

			downfd = fopen(filename, "r");
			if( downfd==NULL ){
				if( downno>=2 ){
					sendmode = 3;		// SWITCH.DAT がなければ次へ
					return( 1 );
				}
				sendmode = 0;
				sprintf( sendbuff, "NG1" );
				return( 0 );
			}

			setvbuf( downfd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );

			for( i=0; filename[i]; i++ ){
				DUALPORT.download_filename[i] = filename[i];
			}
			DUALPORT.download_filename[i] = '\0';

			DUALPORT.download_seq=0;
			DUALPORT.download_size = 0;
			DUALPORT.download_active = 1;

			sendmode++;

		case 2:			// fix files check & send
			if( DUALPORT.download_active==3 ){
				fclose( downfd );
				sendmode = 7;
				sprintf( sendbuff, "NG2" );
				return( 0 );
			}
			if( DUALPORT.download_active ){
				if( timedata.checkTimer==0 ){
					timedata.checkTimer=100;
					dvauMenteSend( strlen(sendbuff) );
				}
				return( 1 );
			}

			len=fread( sendbuff2, 1, 64, downfd );

			fcb = get_fcb( downfd->_iofd );

			DUALPORT.download_date = fcb->w_date;
			DUALPORT.download_time = fcb->w_time;
			DUALPORT.download_size = len;

			for( i=0; i<len; i++ ){
				DUALPORT.download_data[i] = sendbuff2[i];
			}
			if( len<64 ){
				DUALPORT.download_seq++;
				if( DUALPORT.download_seq>9 )
					DUALPORT.download_seq=1;
				DUALPORT.download_active = 2;
				fclose( downfd );
				downfd = NULL;
				sendmode++;
			}
			else {
				DUALPORT.download_seq++;
				if( DUALPORT.download_seq>9 )
					DUALPORT.download_seq=1;
				DUALPORT.download_active = 1;
			}
			break;

		case 3:				// next fix files check
			if( DUALPORT.download_active==3 ){
				sendmode = 7;
				sprintf( sendbuff, "NG3" );
				return( 0 );
			}
			if( DUALPORT.download_active ){
				if( timedata.checkTimer==0 ){
					timedata.checkTimer=100;
					dvauMenteSend( strlen(sendbuff) );
				}
				return( 1 );
			}

			downno++;
			if( downfiles2[downno]!=NULL ){
				sendmode = 1;
				break;
			}
			sendmode++;
			downno = 0;

		case 4:				// Route File Search
			sprintf( filename, "A:\\ROUTE%02X.DAT", downno );
			downfd = fopen( filename, "rb" );
			if( downfd==NULL ){
				sendmode = 6;
				break;
			}

			setvbuf( downfd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );

			for( i=0; filename[i]; i++ ){
				DUALPORT.download_filename[i] = filename[i];
			}
			DUALPORT.download_filename[i] = '\0';

			DUALPORT.download_seq=0;
			DUALPORT.download_size = 0;
			DUALPORT.download_active = 1;

			sendmode++;

		case 5:			// route files check & send
			if( DUALPORT.download_active==3 ){
				fclose( downfd );
				sendmode = 4;
				sprintf( sendbuff, "NG4" );
				return( 0 );
			}
			if( DUALPORT.download_active ){
				if( timedata.checkTimer==0 ){
					timedata.checkTimer=100;
					dvauMenteSend( strlen(sendbuff) );
				}
				return( 1 );
			}

			len=fread( sendbuff2, 1, 64, downfd );

			fcb = get_fcb( downfd->_iofd );

			DUALPORT.download_date = fcb->w_date;
			DUALPORT.download_time = fcb->w_time;
			DUALPORT.download_size = len;

			for( i=0; i<len; i++ ){
				DUALPORT.download_data[i] = sendbuff2[i];
			}
			if( len<64 ){
				DUALPORT.download_seq++;
				if( DUALPORT.download_seq>9 )
					DUALPORT.download_seq=1;
				DUALPORT.download_active = 2;
				fclose( downfd );
				downfd = NULL;
				sendmode++;
			}
			else {
				DUALPORT.download_seq++;
				if( DUALPORT.download_seq>9 )
					DUALPORT.download_seq=1;
				DUALPORT.download_active = 1;
			}
			break;

		case 6:			// Next Route Check
			if( DUALPORT.download_active==3 ){
				sendmode = 7;
				sprintf( sendbuff, "NG5" );
				return( 0 );
			}
			if( DUALPORT.download_active ){
				if( timedata.checkTimer==0 ){
					timedata.checkTimer=100;
					dvauMenteSend( strlen(sendbuff) );
				}
				return( 1 );
			}

			downno++;
			if( downno<MAX_ROUTE ){
				sendmode = 4;
				break;
			}
			downno = 0;
			sendmode++;

		case 7:
			sprintf( filename, "A:\\ROUTE%02X.BEF", downno );
			downfd = fopen( filename, "rb" );
			if( downfd==NULL ){
				sendmode = 9;
				break;
			}

			setvbuf( downfd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );

			for( i=0; filename[i]; i++ ){
				DUALPORT.download_filename[i] = filename[i];
			}
			DUALPORT.download_filename[i] = '\0';

			DUALPORT.download_seq=0;
			DUALPORT.download_size = 0;
			DUALPORT.download_active = 1;

			sendmode++;

		case 8:			// route files check & send
			if( DUALPORT.download_active==3 ){
				fclose( downfd );
				sendmode = 7;
				sprintf( sendbuff, "NG6" );
				return( 0 );
			}
			if( DUALPORT.download_active ){
				if( timedata.checkTimer==0 ){
					timedata.checkTimer=100;
					dvauMenteSend( strlen(sendbuff) );
				}
				return( 1 );
			}

			len=fread( sendbuff2, 1, 64, downfd );

			fcb = get_fcb( downfd->_iofd );

			DUALPORT.download_date = fcb->w_date;
			DUALPORT.download_time = fcb->w_time;
			DUALPORT.download_size = len;

			for( i=0; i<len; i++ ){
				DUALPORT.download_data[i] = sendbuff2[i];
			}
			if( len<64 ){
				DUALPORT.download_seq++;
				if( DUALPORT.download_seq>9 )
					DUALPORT.download_seq=1;
				DUALPORT.download_active = 2;
				fclose( downfd );
				downfd = NULL;
				sendmode++;
			}
			else {
				DUALPORT.download_seq++;
				if( DUALPORT.download_seq>9 )
					DUALPORT.download_seq=1;
				DUALPORT.download_active = 1;
			}
			break;

		case 9:			// Next Route Check
			if( DUALPORT.download_active==3 ){
				sendmode = 10;
				sprintf( sendbuff, "NG7" );
				return( 0 );
			}
			if( DUALPORT.download_active ){
				if( timedata.checkTimer==0 ){
					timedata.checkTimer=100;
					dvauMenteSend( strlen(sendbuff) );
				}
				return( 1 );
			}

			downno++;
			if( downno<MAX_ROUTE ){
				sendmode = 7;
				break;
			}

			sendmode++;


		case 10:
			sendmode++;

			sprintf( sendbuff, "OK" );
			dvauMenteSend( strlen(sendbuff) );

			TraceAdd( EVENT_DOWNLOAD_RECONF );

			for( timedata.checkTimer = 200; timedata.checkTimer!=0; ){
				DUALPORT.activeDVAU = 1;
			}

		case 11:			// Reset & Reboot
			for( timedata.checkTimer = 500; timedata.checkTimer!=0; ){
				DUALPORT.activeDVAU = 1;
				DUALPORT.mainActive = 2;
			}
			RESET();
			break;
		}

		return(1);
}

/************************************************************************

	Function name	<< UPLOAD_command >>
					ファイルデータ受信処理
	Syntax			ret = UPLOAD_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
/****** UPLOAD from SUB CPU  ******/
int UPLOAD_command( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
	・相手ＤＶＡＵからのデータを送信し、ＡＴＡカードへ格納する
	------------------------------------------------------------------[@@@E]*/
		int		i;
		char	fname[32];
		int		len;
extern	unsigned short	now_date;		/*現在日付　ＤＯＳフォーマット*/
extern	unsigned short	now_time;		/*現在時刻　ＤＯＳフォーマット*/
extern	char	dosaccs_workbuff[512];		/*ストリームバッファー*/

		INTC.IER.BIT.IRQ5E = 0;		/* IRQ5 unUse */

			// アップロードデータ処理開始
		for( i=0; i<16; i++ ){
			fname[i] = DUALPORT.upload_filename[i];
		}
		fname[i]='\0';
		if( strcmp( down_fname, fname )!=0 || DUALPORT.upload_seq==0 ){
			if( downfd!=NULL ){
				fclose( downfd );
				downfd = NULL;
			}
			strcpy( down_fname, fname );
		}

		if( downfd==NULL ){
			if( DUALPORT.upload_active==2 ){
				DUALPORT.upload_active = 0;		// すでに受信している
				return( 0 );
			}

			unlink( "A:\\TEMP.TMP" );
			downfd = fopen( "A:\\TEMP.TMP", "w" );
			if( downfd==NULL ){
				DUALPORT.upload_active = 3;
				return( 0 );
			}
			setvbuf( downfd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );
		}
		else {
			if( befSeqcount == DUALPORT.upload_seq ){
				DUALPORT.upload_active = 0;		// すでに受信している
				return( 0 );
			}
		}

		befSeqcount = DUALPORT.upload_seq;

		len = (int)DUALPORT.upload_size & 0x0FFFF;
		if( len ){
			fwrite( DUALPORT.upload_data, 1, len, downfd );
		}
		if( DUALPORT.upload_active==1 ){
			DUALPORT.upload_active = 0;
		}
		if( DUALPORT.upload_active==2 ){
			now_date = DUALPORT.upload_date;
			now_time = DUALPORT.upload_time;

			if( downfd==NULL ){
				DUALPORT.upload_active = 3;
				return( 0 );
			}
			fclose( downfd );
			downfd = NULL;

			unlink( fname );
			if( rename( "A:\\TEMP.TMP", fname ) ){
				DUALPORT.upload_active = 3;
			}
			else {
				DUALPORT.upload_active = 0;
			}
		}

		return(0);
}

/************************************************************************

	Function name	<< TRACE_command >>
					トレースデータ送信処理
	Syntax			ret = TRACE_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
static	int		traceNo;	// トレース確認用番号
/****** TRACE  ******/
int TRACE_command(char *p)
{
		char	work[32];

		sendmode = 1;
		if( tracep==NULL ){
			tracep = TraceGet( 1 );
			if( tracep==NULL ){
				sprintf( sendbuff, "TRACEEND" );
				sendmode = 0;
				return( 0 );
			}
			traceNo=1;
			sprintf( work, "%04d", traceNo );
			strcpy( sendbuff, "TRACE" );
			strcat( sendbuff, work );
			strcat( sendbuff, " " );
			TraceText( tracep, &sendbuff[10] );
			traceNo++;
		}
		else {
			tracep = TraceGet( 0 );
			if( tracep==NULL ){
				strcpy( sendbuff, "TRACEEND" );
				sendmode = 0;
				return( 0 );
			}
			sprintf( work, "%04d", traceNo );
			strcpy( sendbuff, "TRACE" );
			strcat( sendbuff, work );
			strcat( sendbuff, " " );
			TraceText( tracep, &sendbuff[10] );
			traceNo++;
		}

		return(0);
}

/************************************************************************

	Function name	<< OPEMODE_command >>
					操作モード送信
	Syntax			ret = OPEMODE_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
/****** OPEMODE  ******/
int OPEMODE_command(char *p)
{
		int		i,j;
		char	fname[32];

		sendmode = 0;
			switch( backup_data.opeMode ){
			case	OPEMODE_INITIAL_TEST:
				strcpy( sendbuff, "INITTEST" );
				return(0);

			case	OPEMODE_STANDBY:
				strcpy( sendbuff, "STANDBY" );
				return(0);

			case	OPEMODE_IDLE:
				strcpy( sendbuff, "IDLE" );
				return(0);

			case	OPEMODE_TEST:
				strcpy( sendbuff, "TEST" );
				return(0);

			case	OPEMODE_TRS:
				strcpy( sendbuff, "TRS" );
				return(0);

			case	OPEMODE_AUTO:
				strcpy( sendbuff, "AUTO" );
				return(0);

			case	OPEMODE_EMGHL:
				strcpy( sendbuff, "EMGHL" );
				return(0);

			case	OPEMODE_EMGMA:
				strcpy( sendbuff, "EMGMA" );
				return(0);

			case	OPEMODE_SPCMA:
				strcpy( sendbuff, "SPCMA" );
				return(0);

			case	OPEMODE_SELF_TEST:
				strcpy( sendbuff, "SELFTEST" );
				return(0);
			}

			sprintf( sendbuff, "NG" );
		return(0);
}

/************************************************************************

	Function name	<< VOICE_command >>
					音声テストコマンド
	Syntax			ret = VOICE_command( p );
	Argument		char *p : i : コマンドパラメータ
				int ret : o : 0=送信データあり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
static	union EV5000_SEQUENCE Sequence[MAX_SEQUENCE];	// 放送シーケンスコード
int VOICE_command(char *p)
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
	・ＥＶ５０００へ放送文章番号を送信する
	------------------------------------------------------------------[@@@E]*/
		int		sentence;
		int		code;

		sendmode = 0;

		sscanf( p, "%x %x", &sentence, &code );
		memset( Sequence, 0, sizeof(Sequence) );
		Sequence[0].sendRequest.timewait = 100;
		Sequence[0].sendRequest.en = 1;
		Sequence[0].sendRequest.sendCode = code;

		DUALPORT.ev5000_request.SentenceNo = sentence;
		DUALPORT.ev5000_request.Sequence = Sequence;

		DUALPORT.ev5000_request.ev5000_error = 0;
		DUALPORT.ev5000_request.Complete = 0;
		DUALPORT.ev5000_request.Request = 1;

		sprintf( sendbuff, "OK" );

		return( 0 );
}



/// シミュレータ処理
#if 1

extern	struct st_ReadSIGNAL nowSignal;			// 最新読みだし値
extern	char	dataVersionCRCL;
extern	char	dataVersionCRCH;
static	char			befAdjust;		// 時間設定の前回値
static	char			befTalkBack[8];	// 前回の非常通報器要求の状態

void tms_simulate( void )
{
		int		i;
		unsigned char	onoff,on;

			// テスト開始の状態設定
			DUALPORT.tms_request.testExecRequest = tms_sdr_data.SDR.testRequest.testExecRequest;
			// テスト要求の状態設定
			DUALPORT.tms_request.testStartRequest1 = tms_sdr_data.SDR.testRequest.testStartRequest1;
			DUALPORT.tms_request.testStartRequest2 = tms_sdr_data.SDR.testRequest.testStartRequest2;
			// テスト項目の設定
			DUALPORT.tms_request.itemNo = tms_sdr_data.SDR.itemNo;

			// 日時情報が設定されていて、時刻設定のとき　内部時計を設定する
			if( tms_sdr_data.SDR.activeText.timeDateValid ){
				if( tms_sdr_data.SDR.activeText.timeDateAdjustment ){
					if( befAdjust==0 ){
						befAdjust = 1;
						timedata.tm_year = (tms_sdr_data.SDR.timeDate.yy10 * 10 + tms_sdr_data.SDR.timeDate.yy1 );
						timedata.tm_mon = tms_sdr_data.SDR.timeDate.MM10 * 10 + tms_sdr_data.SDR.timeDate.MM1;
						timedata.tm_day = tms_sdr_data.SDR.timeDate.dd10 * 10 + tms_sdr_data.SDR.timeDate.dd1;
						timedata.tm_hour = tms_sdr_data.SDR.timeDate.hh10 * 10 + tms_sdr_data.SDR.timeDate.hh1;
						timedata.tm_min = tms_sdr_data.SDR.timeDate.mm10 * 10 + tms_sdr_data.SDR.timeDate.mm1;
						timedata.tm_sec = tms_sdr_data.SDR.timeDate.ss10 * 10 + tms_sdr_data.SDR.timeDate.ss1;
						timedata.tm_msec = 0;
					}
				}
				else {
					befAdjust = 0;
				}
			}
			else {
				befAdjust = 0;
			}
			// 車両タイプの設定
			if( tms_sdr_data.SDR.activeText.carTypeValid ){
				for( i=0; i<8; i++ ){
					backup_data.carInfo[i].carType = tms_sdr_data.SDR.carInfo2[i].carType;
				}
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					DUALPORT.tms_request.carType = backup_data.carInfo[0].carType;
				}
				else {
					DUALPORT.tms_request.carType = backup_data.carInfo[7].carType;
				}
			}
			// 車両番号の設定
			if( tms_sdr_data.SDR.activeText.carNoValid ){
				for( i=0; i<8; i++ ){
					backup_data.carInfo[i].carNo = tms_sdr_data.SDR.carInfo2[i].carNo;
					DUALPORT.tms_request.carInfo[i].carNo = tms_sdr_data.SDR.carInfo2[i].carNo;
				}
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					DUALPORT.tms_request.carNo = backup_data.carInfo[0].carNo;
				}
				else {
					DUALPORT.tms_request.carNo = backup_data.carInfo[0].carNo;
				}
				DUALPORT.pollingData.carNo = backup_data.carInfo[0].carNo;
			}
			// 連結状態の設定
			DUALPORT.tms_request.couplingUnitCode = tms_sdr_data.SDR.couplingUnitCode;

			// ステーションコードの設定
			DUALPORT.tms_request.stationNumber = tms_sdr_data.SDR.stationNumber;
			// 次駅コードの設定
			if( DUALPORT.tms_request.nextStationName != tms_sdr_data.SDR.nextStationName ){
				DUALPORT.tms_request.nextStationName = tms_sdr_data.SDR.nextStationName;
				DUALPORT.pollingData.nextStation = tms_sdr_data.SDR.nextStationName;
				DUALPORT.tms_request.change_nextStationName = 1;
			}
			// 到着駅コードの設定
			DUALPORT.tms_request.destination = tms_sdr_data.SDR.destination;
			DUALPORT.pollingData.destination = tms_sdr_data.SDR.destination;
			// ルートコードの設定
			if( DUALPORT.tms_request.routeCode != tms_sdr_data.SDR.routeCode ){
				DUALPORT.tms_request.routeCode = tms_sdr_data.SDR.routeCode;
				DUALPORT.pollingData.routeCode = tms_sdr_data.SDR.routeCode;
				DUALPORT.tms_request.change_routeCode = 1;
			}

			// スペシャル放送コードの設定
			DUALPORT.tms_request.specialCode = tms_sdr_data.SDR.specialCode;

			// 非常通報器の接続要求  ( OFF->ON 要求 )
			for( i=0; i<8; i++ ){
				onoff = befTalkBack[i] ^ tms_sdr_data.SDR.talkBack[i];
				on = onoff & tms_sdr_data.SDR.talkBack[i];
				DUALPORT.tms_request.talkBack[i] |= on;
				befTalkBack[i] = tms_sdr_data.SDR.talkBack[i];
			}

			// 自動放送の要求
			if( DUALPORT.tms_request.dbEffective != tms_sdr_data.SDR.ATCpaActive.dbEffective ){
				DUALPORT.tms_request.change_dbEffective = 1;
				DUALPORT.tms_request.dbEffective = tms_sdr_data.SDR.ATCpaActive.dbEffective;
			}
			// 自動放送の要求
			if( DUALPORT.tms_request.atcActive != tms_sdr_data.SDR.ATCpaActive.atcActive ){
				DUALPORT.tms_request.change_atcActive = 1;
				DUALPORT.tms_request.atcActive = tms_sdr_data.SDR.ATCpaActive.atcActive;
			}
			// 自動放送の禁止
			if( DUALPORT.tms_request.broadCastOff != tms_sdr_data.SDR.door.broadCastOff ){
				DUALPORT.tms_request.change_broadCastOff = 1;
				DUALPORT.tms_request.broadCastOff = tms_sdr_data.SDR.door.broadCastOff;
			}
			// ドア状態の設定
			if( tms_sdr_data.SDR.door.doorOpen==0 ){
				DUALPORT.eids.Door = 1;
			}
			else {
				DUALPORT.eids.Door = 0;
			}

			if( DUALPORT.tms_request.doorOpenEffective ){
				if( DUALPORT.tms_request.doorOpen != tms_sdr_data.SDR.door.doorOpen ){
					DUALPORT.tms_request.change_doorOpen = 1;
					DUALPORT.tms_request.doorOpen = tms_sdr_data.SDR.door.doorOpen;
				}
			}

			// ＴＭＳアクティブフラグ
			DUALPORT.tms_request.Active = 1;
			// ＴＭＳ有効タイマー
			timedata.tmsActiveTimer = 10000;


		if( tms_sdr_data.SDR.activeText.timeDateValid ){
			tms_sdr_data.SDR.timeDate.yy10 = timedata.tm_year/10;
			tms_sdr_data.SDR.timeDate.yy1 = timedata.tm_year%10;
			tms_sdr_data.SDR.timeDate.MM10 = timedata.tm_mon/10;
			tms_sdr_data.SDR.timeDate.MM1 = timedata.tm_mon%10;
			tms_sdr_data.SDR.timeDate.dd10 = timedata.tm_day/10;
			tms_sdr_data.SDR.timeDate.dd1 = timedata.tm_day%10;
			tms_sdr_data.SDR.timeDate.hh10 = timedata.tm_hour/10;
			tms_sdr_data.SDR.timeDate.hh1 = timedata.tm_hour%10;
			tms_sdr_data.SDR.timeDate.mm10 = timedata.tm_min/10;
			tms_sdr_data.SDR.timeDate.mm1 = timedata.tm_min%10;
			tms_sdr_data.SDR.timeDate.ss10 = timedata.tm_sec/10;
			tms_sdr_data.SDR.timeDate.ss1 = timedata.tm_sec%10;
		}
		tms_sdr_data.SDR.activeText.timeDateAdjustment = 0;

		// テスト状況の設定
		tms_sd_data.SD.testResponce = DUALPORT.testResponce;
		tms_sd_data.SD.itemNo = DUALPORT.testItemNo;
		tms_sd_data.SD.itemResult = DUALPORT.testItemResult;
		tms_sd_data.SD.subItemResult = DUALPORT.testSubItemResult;
		tms_sd_data.SD.crcL = dataVersionCRCL;
		tms_sd_data.SD.crcH = dataVersionCRCH;

		// 非常通報器の状態応答
		for( i=0; i<MAX_PAA; i++ ){
			tms_sd_data.SD.passengerAlarm[i] = DUALPORT.tms_answer.passengerAlarm[i];
		}
		// 通信エラーの状態応答
		tms_sd_data.SD.dvaCommError = DUALPORT.tms_answer.dvaCommError;
		// モード状態の応答
		tms_sd_data.SD.dvaStatus = DUALPORT.tms_answer.dvaStatus;

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

			tms_simulate();

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR2 ******/
int SDR2_command(char *p)
{
		sendmode = 0;

				// テキストアクティブフラグ状況
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

			tms_simulate();

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR3 ******/
int SDR3_command(char *p)
{
		sendmode = 0;

				// 日付時刻
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

			tms_simulate();

			sprintf( sendbuff, "OK" );


		return( 0 );
}

/****** SDR4 ******/
int SDR4_command(char *p)
{
		char	work[16];
		int		text1;

		sendmode = 0;

				// 車両番号
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

			tms_simulate();

			sprintf( sendbuff, "OK" );


		return( 0 );
}

/****** SDR5 ******/
int SDR5_command(char *p)
{
		char	work[16];
		int		text1;

		sendmode = 0;

				// 連結コード
			memcpy( work, p, 1 );
			p+=1;
			work[1]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.couplingUnitCode = text1;

			tms_simulate();

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR6 ******/
int SDR6_command(char *p)
{
		char	work[16];
		int		text1,text2;

		sendmode = 0;

				// ステーション番号
			memcpy( work, p, 2 );
			p+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			memcpy( work, p, 1 );
			p+=1;
			work[1]='\0';
			sscanf( work, "%x", &text2 );
			tms_sdr_data.SDR.stationNumber = text1 | (text2<<5);

			tms_simulate();

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

			tms_simulate();

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR8 ******/
int SDR8_command(char *p)
{
		int		i,j;

		sendmode = 0;

				// 非常通報器
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

			tms_simulate();

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDR9 ******/
int SDR9_command(char *p)
{
		sendmode = 0;

				// 放送フラグ
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

			tms_simulate();

			sprintf( sendbuff, "OK" );

		return( 0 );
}

/****** SDRA ******/
int SDRA_command(char *p)
{
		sendmode = 0;


				// ドアフラグ
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

			if( memcmp( p, "DOOR", 4 )==0 ){
				tms_sdr_data.SDR.doorStatus.doorClosed =1;
			}
			else {
				tms_sdr_data.SDR.doorStatus.doorClosed =0;
			}
			p += 4;

			tms_simulate();

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

				// 車両番号
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

/****** SDRC ******/
int SDRC_command(char *p)
{
		int		i,j;

		sendmode = 0;

				// 非常通報器リセット
		for( i=0; i<8; i++ ){
			for( j=0; j<5; j++ ){
				if( memcmp( p, TalkBackTable[i][j], 4 )==0 ){
					tms_sdr_data.SDR.PadReset[i] |= (0x0001<<j);
				}
				else {
					tms_sdr_data.SDR.PadReset[i] &= ~(0x0001<<j);
				}
				p+=4;
			}
		}

			tms_simulate();

			sprintf( sendbuff, "OK" );

		return( 0 );
}

#endif

struct JUMP_STR {
		char	*command;
		int		(*jump)( char *p );
};
static	struct JUMP_STR cmdtable[] = {
	"TMSSDR",	TMSSDR_command,
	"TMSSD ",	TMSSD__command,
	"DOWN ",	DOWN_command,
	"REBOOT",	REBOOT_command,
	"RECONF",	RECONFIG_command,
	"TRACE",	TRACE_command,
	"TROUBLE",	TROUBLE_command,
	"OPEMODE",	OPEMODE_command,
	"VOICE",	VOICE_command,

#if 1
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
	"SDRC",	SDRC_command,
#endif

	"D ",		D_command,
	"W ",		W_command,
	"R ",		R_command,
	NULL,	NULL
};

static	int		(*ContinueCommand)( char *p );

/************************************************************************

	Function name	<< dvauMenteReceive >>
					メンテナンスコマンド受信処理
	Syntax			 dvauMenteReceive( data );
	Argument		char *data : i : 受信コマンド文字列
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/10/10	送受信にＡＣＫ／ＮＡＫを追加

*************************************************************************/
void dvaMenteReceive( void )
{
		int		i;
		char	*datan;
		char	buff[16];

		if( recvmode ){		/* メンテナンスターミナルコマンド */
			sprintf( buff, "%02X", recvbcc3 & 0x00FF );
			if( recvbuff[recvlen-2]!=buff[0] )	/* BCC Check */
				return;
			if( recvbuff[recvlen-1]!=buff[1] )
				return;
			recvlen -= 2;
			recvbuff[recvlen] = '\0';
		}
		datan = recvbuff;

		for( i=0; cmdtable[i].command!=NULL; i++ ){	/* コマンド別関数へのジャンプ */
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

		if( memcmp( datan, "RESET", 5 )==0 ){	/* for Debug */
			for( timedata.checkTimer = 500; timedata.checkTimer!=0; ){
				DUALPORT.activeDVAU = 1;
				DUALPORT.mainActive = 2;
			}
			RESET();
		}
		if( memcmp( datan, "HALT", 4 )==0 ){	/* For Debug */
			DUALPORT.activeDVAU = 0;
			DUALPORT.mainActive = 0;
			Halt();
		}
		if( memcmp( datan, "DOWN", 4 )==0 ){	/* For Debug */
			PB.DR.BIT.B7 = 1;
			strcpy( sendbuff, "SUB CPU DOWN\r\n" );
			dvauMenteSend( strlen(sendbuff) );
			return;
		}

		strcpy( sendbuff, "NG" );		/* 該当コマンドなし */
		dvauMenteSend( strlen(sendbuff) );
}


/************************************************************************

	Function name	<< asci_1_recv_ir >>
					受信割り込み処理
	Syntax			asci_1_recv_ir( data, status );
	Argument		char data   : i : 受信データ
				char status : i : 受信ステータス
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	送受信にＡＣＫ／ＮＡＫを追加

*************************************************************************/
void asci_1_recv_ir( char data, char status )
{
/*.	---------------------　処理詳細説明  ----------------------------	*/
/*  フェーズ毎に受信データを取り込む									*/
/*. ------------------------------------------------------------------  */
status=0;

		if( data==0x02 ){		/* STX */
			recvmode = 1;
			recvlen = 0;
			recvbcc1 = 0;
			recvbcc2 = 0;
			recvbcc3 = 0;
			return;
		}
		if( data==0x03 ){		/* ETX */
			recvon = 1;
			recvlen2 = recvlen;
			recvmode = 1;		/* メンテナンスターミナルコマンドと認識 */
			resend = 0;
			sendmode = 0;
			return;
		}

		if( data==0x06 ){		/* ACK */
			timedata.menteSendTimer = 0;		// 次の行を送信する
			resend = 0;
			return;
		}
		if( data==0x15 ){		/* NAK */
			timedata.menteSendTimer = 500;		// 5 sec.
			resend = 0;
			return;
		}

		if( data==CRLF ){		/* CR */
			recvbuff[recvlen]='\0';
			recvlen = 0;
			recvon = 1;
			recvmode = 0;		/* Debug Terminal */
			resend = 0;
			sendmode = 0;
			return;
		}
		if( data==ESC ){		/* コマンドキャンセル */
			recvmode = 0;
			recvon = 0;
			recvlen = 0;
			recvbuff[recvlen]='\0';

			resend = 0;
			sendmode = 0;

			sendcnt = 0;
			sendlen = 0;

			tracep=NULL;

			sendbuff[0]='\0';
			dvauMenteSend( strlen(sendbuff) );
			return;
		}

		recvbcc3 = recvbcc2;			/* BCC Calc */
		recvbcc2 = recvbcc1;
		recvbcc1 = recvbcc1 + data;
		if( recvlen<sizeof(recvbuff) ){
			recvbuff[recvlen++] = data;	/* 受信バッファーへ */
			recvbuff[recvlen]='\0';
		}
}

/************************************************************************

	Function name	<< dvauMENTEInitialize >>
					メンテナンス処理初期設定
	Syntax			dvauMENTEInitialize();
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/10/10	送受信にＡＣＫ／ＮＡＫを追加

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

		tracep=NULL;
}

/************************************************************************

	Function name	<< dvauMENTEMain >>
					メンテナンス処理メイン関数
	Syntax			dvauMENTEMain();
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void dvauMENTEMain( void )
{
		char	data;

		if( resend ){		// 再送信要求あり
			resend = 0;
			sendcnt = 0;

			asci_1_tx_enable();		/*送信開始  RTS=ON*/

			data = sendbuff2[sendcnt++];
			asci_1_transmit( data );	/*１バイトの送信*/
		}
		else if( sendcnt==sendlen ){		/* すべて送信したとき */
			if( sendmode ){		/* 連続送信中 */
				if( timedata.menteSendTimer==0 ){		// 送信ディレイ
					if( ContinueCommand( "" )==0 ){	/* 再度次の送信する */
						timedata.menteSendTimer = 500;		// 5 sec.
						dvauMenteSend( strlen(sendbuff) );
					}
				}
			}
		}

		if( recvon ){		/* コマンド受信確認 */
			recvon = 0;
			dvaMenteReceive();	/* コマンド処理 */
		}

		if( DUALPORT.upload_active==1 || DUALPORT.upload_active==2 ){	/* 相手ＤＶＡＵからのファイルデータ要求のとき */
			UPLOAD_command();	/* データ受信処理を実行 */
		}
}

