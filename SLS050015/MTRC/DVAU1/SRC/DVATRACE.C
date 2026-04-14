/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	トレース制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation
																	*
	Revision	a00. 2000/08/09		K.Honda
	Revision	a01. 2000/12/07		K.Honda

*********************************************************************/

#include	<stdio.h>
#include    <machine.h>

#include    "iodefine.h"
#include	"table.h"

#include    "timer.h"
#include    "trace.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

#define	MAX_TRACE	3001		// トレース情報を３０００件表示するため　３００１とした

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	struct	TIMER_CONTROL	timedata;

extern	struct	BACKUP_DATA		backup_data;		// バックアップデータ

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#pragma section BACKUP
static	struct TRACE traceData[MAX_TRACE];
static	int			trace_top;
static	int			trace_end;
static	int			trace_get;
#pragma section


/*@@@E*/
/************************************************************************

	Function name	<< TraceClear >>
					トレース初期設定
	Syntax			TraceClear( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09

*************************************************************************/
int TraceClear( void )
{
		trace_top = 0;
		trace_end = 0;
		trace_get = 0;
		memset( traceData, 0, sizeof(traceData) );
}

/************************************************************************

	Function name	<< TraceAdd >>	
					トレース追加設定
	Syntax			TraceAdd( event );
	Argument		char event : i : イベントコード
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09

*************************************************************************/
int TraceAdd( char event )
{
		struct TRACE	*tracep;
		set_imask_ccr(1);

		tracep = &traceData[trace_top];
		tracep->tm_mon = timedata.tm_mon;
		tracep->tm_day = timedata.tm_day;
		tracep->tm_hour = timedata.tm_hour;
		tracep->tm_min = timedata.tm_min;
		tracep->tm_sec = timedata.tm_sec;
		tracep->tm_msec = timedata.tm_msec/10;

		tracep->eventID[0] = event;
		tracep->eventID[1] = backup_data.nowRouteCode;
		tracep->eventID[2] = backup_data.nowStationName;
		tracep->eventID[3] = backup_data.stationNumber;
		tracep->eventID[4] = backup_data.nowEMGCode;
		tracep->eventID[5] = backup_data.nowSPCCode;
		if( DUALPORT.ev5000_request.Request ){
			tracep->eventID[6] = (DUALPORT.ev5000_request.Sequence[0].sendRequest.sendCode>>8);
			tracep->eventID[7] = (DUALPORT.ev5000_request.Sequence[0].sendRequest.sendCode);
		}
		else {
			tracep->eventID[6] = 0;
			tracep->eventID[7] = 0;
		}

		trace_top++;
		if( trace_top>=MAX_TRACE ){
			trace_top=0;
		}
		if( trace_top==trace_end ){
			trace_end++;
			if( trace_end>=MAX_TRACE ){
				trace_end=0;
			}
		}
		if( trace_top==trace_get ){
			trace_get++;
			if( trace_get>=MAX_TRACE ){
				trace_get=0;
			}
		}
		set_imask_ccr(0);
}

/************************************************************************

	Function name	<< TraceTrouble >>
					トラブルトレース追加設定
	Syntax			TraceTrouble( event, troubleBit );
	Argument		char event               : i : イベントコード
				unsigned char troubleBit : i : 発生したトラブルビット
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/11/13

*************************************************************************/
int TraceTrouble( char event, unsigned char troubleBit )
{
		struct TRACE	*tracep;
		set_imask_ccr(1);

		tracep = &traceData[trace_top];
		tracep->tm_mon = timedata.tm_mon;
		tracep->tm_day = timedata.tm_day;
		tracep->tm_hour = timedata.tm_hour;
		tracep->tm_min = timedata.tm_min;
		tracep->tm_sec = timedata.tm_sec;
		tracep->tm_msec = timedata.tm_msec/10;

		tracep->eventID[0] = event;
		tracep->eventID[1] = backup_data.nowRouteCode;
		tracep->eventID[2] = backup_data.nowStationName;
		tracep->eventID[3] = backup_data.stationNumber;
		tracep->eventID[4] = backup_data.nowEMGCode;
		tracep->eventID[5] = backup_data.nowSPCCode;
		tracep->eventID[6] = troubleBit;
		tracep->eventID[7] = 0;

		trace_top++;
		if( trace_top>=MAX_TRACE ){
			trace_top=0;
		}
		if( trace_top==trace_end ){
			trace_end++;
			if( trace_end>=MAX_TRACE ){
				trace_end=0;
			}
		}
		if( trace_top==trace_get ){
			trace_get++;
			if( trace_get>=MAX_TRACE ){
				trace_get=0;
			}
		}
		set_imask_ccr(0);
}

/************************************************************************

	Function name	<< TraceGet >>	
					トレース取り出し
	Syntax			info = TraceGet( first );
	Argument		int first          : i : 1=最初の取り出し
						         0=次の取り出し
				struct TRACE *info : o : トレースデータ
							NULL = データなし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09

*************************************************************************/
struct TRACE *TraceGet( int first )
{
		int		getno;
		int		trace_on;

		trace_on = 1;
		set_imask_ccr(1);
		if( first ){
			trace_get = trace_end;
			getno = trace_get;
		}
		else {
			getno = trace_get;
			getno++;
		}
		if( getno>=MAX_TRACE ){
			getno = 0;
		}
		if( getno==trace_top )
			trace_on = 0;

		if( traceData[getno].eventID[0]==0 )
			trace_on = 0;

		if( trace_on ){
			trace_get = getno;
		}
		set_imask_ccr(0);

		if( trace_on ){
			return &traceData[getno];
		}

		return( NULL );
}

/************************************************************************

	Function name	<< TraceText >>	
					トレース情報のテキスト化
	Syntax			TraceText( getp, buffer );
	Argument		struct TRACE *getp : i : トレース情報
				char *buffer       : o : テキスト情報
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
				a01 2000/11/13  トラブル情報の追加

*************************************************************************/
void TraceText( struct TRACE *getp, char *buffer )
{
		int		i;
		int		mm,dd,hh,nn,ss,msec;

		mm = getp->tm_mon;
		dd = getp->tm_day;
		hh = getp->tm_hour;
		nn = getp->tm_min;
		ss = getp->tm_sec;
		msec = getp->tm_msec;
		if( msec>=10 ){
			msec-=10;
			ss++;
		}
		if( ss>=60 ){
			ss-=60;
		}
		if( nn>=60 ){
			nn-=60;
		}
		if( hh>=24 ){
			hh-=24;
		}
		if( dd>=32 ){
			dd-=32;
		}
		if( mm>=13 ){
			mm-=12;
		}
		buffer += sprintf( buffer, "%02u/%02u %02u:%02u:%02u.%01u ",
				mm,dd,hh,nn,ss,msec );

		if( getp->eventID[0] & 0x80 ){		// Trouble Trace
			buffer += sprintf( buffer, "R%02X S%02X N%02X E%02X S%02X T%02X ",
					getp->eventID[1],
					getp->eventID[2],
					getp->eventID[3],
					getp->eventID[4],
					getp->eventID[5],
					getp->eventID[6] );

			if( (getp->eventID[0] & 0xF0)==0x80 ){
				if( getp->eventID[0] & 0x01 ){
					buffer += sprintf( buffer, "tbl:DVAU2 " );
				}
				else {
					buffer += sprintf( buffer, "tbl:DVAU1 " );
				}
				if( getp->eventID[6] & 0x01 ){
					buffer += sprintf( buffer, "24V Signal " );
				}
				if( getp->eventID[6] & 0x02 ){
					buffer += sprintf( buffer, "Voice Signal " );
				}
			}
			if( (getp->eventID[0] & 0xF0)==0x90 ){
				buffer += sprintf( buffer, "tbl:PAA%u ", (getp->eventID[0] & 0x0F)+1 );
				if( getp->eventID[6] & 0x01 ){
					buffer += sprintf( buffer, "AMP1 " );
				}
				if( getp->eventID[6] & 0x02 ){
					buffer += sprintf( buffer, "AMP2 " );
				}
				if( getp->eventID[6] & 0x04 ){
					buffer += sprintf( buffer, "SP1 " );
				}
				if( getp->eventID[6] & 0x08 ){
					buffer += sprintf( buffer, "SP2 " );
				}
				if( getp->eventID[6] & 0x10 ){
					buffer += sprintf( buffer, "SP3 " );
				}
				if( getp->eventID[6] & 0x20 ){
					buffer += sprintf( buffer, "SP4 " );
				}
				if( getp->eventID[6] & 0x40 ){
					buffer += sprintf( buffer, "SP5 " );
				}
			}
			if( (getp->eventID[0] & 0xF0)==0xB0 ){
				buffer += sprintf( buffer, "tbl:Comm Error " );
				if( getp->eventID[6] & 0x01 ){
					buffer += sprintf( buffer, "FRONT DVAU " );
				}
				if( getp->eventID[6] & 0x02 ){
					buffer += sprintf( buffer, "REAR DVAU " );
				}
				if( getp->eventID[6] & 0x04 ){
					buffer += sprintf( buffer, "FRONT MCP " );
				}
				if( getp->eventID[6] & 0x08 ){
					buffer += sprintf( buffer, "REAR MCP " );
				}
			}
			if( (getp->eventID[0] & 0xF0)==0xC0 ){
				buffer += sprintf( buffer, "tbl:Comm Error " );
				for( i=0; i<8; i++ ){
					if( getp->eventID[6] & (0x01<<i) ){
						buffer += sprintf( buffer, "PAA%u ", i+1 );
					}
				}
			}
			return;
		}

		buffer += sprintf( buffer, "R%02X S%02X N%02X E%02X S%02X V%02X%02X ",
				getp->eventID[1],
				getp->eventID[2],
				getp->eventID[3],
				getp->eventID[4],
				getp->eventID[5],
				getp->eventID[6],
				getp->eventID[7] );

		switch( getp->eventID[0] ){
		case EVENT_INIT_TEST:
			buffer += sprintf( buffer, "INIT TEST MODE" );
			break;
		case EVENT_INIT_TEST2:
			buffer += sprintf( buffer, "INIT TEST MODE(Standby)" );
			break;
		case EVENT_SELF_TEST:
			buffer += sprintf( buffer, "SELF TEST MODE" );
			break;
		case EVENT_TEST:
			buffer += sprintf( buffer, "TEST MODE" );
			break;
		case EVENT_STANDBY:
			buffer += sprintf( buffer, "STANDBY MODE" );
			break;
		case EVENT_IDLE:
			buffer += sprintf( buffer, "IDLE MODE" );
			break;
		case EVENT_AUTO:
			buffer += sprintf( buffer, "AUTO MODE" );
			break;
		case EVENT_TRS:
			buffer += sprintf( buffer, "TRS MODE" );
			break;
		case EVENT_EMGMA:
			buffer += sprintf( buffer, "EMGMA MODE" );
			break;
		case EVENT_SPCMA:
			buffer += sprintf( buffer, "SPCMA MODE" );
			break;
		case EVENT_EMGHL:
			buffer += sprintf( buffer, "EMGHL MODE" );
			break;

		case EVENT_REPEAT:
			buffer += sprintf( buffer, "REPEAT VOICE" );
			break;
		case EVENT_EMG:
			buffer += sprintf( buffer, "EMG VOICE" );
			break;
		case EVENT_SPC:
			buffer += sprintf( buffer, "SPC VOICE" );
			break;
		case EVENT_STN:
			buffer += sprintf( buffer, "STN VOICE" );
			break;
		case EVENT_CAB:
			buffer += sprintf( buffer, "CAB IN" );
			break;
		case EVENT_MAN:
			buffer += sprintf( buffer, "MAN IN" );
			break;
		case EVENT_REL:
			buffer += sprintf( buffer, "CAB/MAN OUT" );
			break;

		case EVENT_TMS_NEXT:
			buffer += sprintf( buffer, "NEXT STATION VOICE" );
			break;
		case EVENT_TMS_SPC:
			buffer += sprintf( buffer, "TMS SPC VOICE" );
			break;
		case EVENT_TMS_STN:
			buffer += sprintf( buffer, "STATION VOICE" );
			break;

		case EVENT_DOOR_OPENACHIME:
			buffer += sprintf( buffer, "DOOR OPEN A CHIME" );
			break;
		case EVENT_DOOR_CLOSEACHIME:
			buffer += sprintf( buffer, "DOOR CLOSE A CHIME" );
			break;
		case EVENT_DOOR_OPENAANNOUNCE:
			buffer += sprintf( buffer, "DOOR OPEN A ANNOUNCE" );
			break;
		case EVENT_DOOR_CLOSEAANNOUNCE:
			buffer += sprintf( buffer, "DOOR OPEN A ANNOUNCE" );
			break;
		case EVENT_DOOR_OPENBCHIME:
			buffer += sprintf( buffer, "DOOR OPEN B CHIME" );
			break;
		case EVENT_DOOR_CLOSEBCHIME:
			buffer += sprintf( buffer, "DOOR CLOSE B CHIME" );
			break;
		case EVENT_DOOR_OPENBANNOUNCE:
			buffer += sprintf( buffer, "DOOR OPEN B ANNOUNCE" );
			break;
		case EVENT_DOOR_CLOSEBANNOUNCE:
			buffer += sprintf( buffer, "DOOR OPEN B ANNOUNCE" );
			break;

		case EVENT_DOWNLOAD_CONFIG:
			buffer += sprintf( buffer, "DOWNLOAD CONFIG" );
			break;
		case EVENT_DOWNLOAD_ROUTE:
			buffer += sprintf( buffer, "DOWNLOAD ROUTE" );
			break;
		case EVENT_DOWNLOAD_PROG:
			buffer += sprintf( buffer, "DOWNLOAD PROGRAM" );
			break;
		case EVENT_DOWNLOAD_RECONF:
			buffer += sprintf( buffer, "RECONFIG" );
			break;
		case EVENT_DOWNLOAD_REBOOT:
			buffer += sprintf( buffer, "REBOOT" );
			break;

		case EVENT_VOICE:
			buffer += sprintf( buffer, "VOICE" );
			break;
		case EVENT_VOICE_OK:
			buffer += sprintf( buffer, "VOICE END" );
			break;
		case EVENT_VOICE_NG:
			buffer += sprintf( buffer, "VOICE ERR" );
			break;
		case EVENT_VOICE_ABORT:
			buffer += sprintf( buffer, "VOICE ABORT" );
			break;
		case EVENT_EVCOMM_NG:
			buffer += sprintf( buffer, "EV5000 COMM ERROR" );
			break;
		}
}

void debugTime( void )
{
		printf( "%02u/%02u %02u:%02u:%02u.%01u ",
				timedata.tm_mon, timedata.tm_day,
				timedata.tm_hour, timedata.tm_min, timedata.tm_sec,
				timedata.tm_msec/10 );
}

