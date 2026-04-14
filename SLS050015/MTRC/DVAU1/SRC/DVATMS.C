/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	通信制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/07		K.Honda
				a01. 2000/09/07		K.Honda
				a02. 2001/03/22		K.Honda
				a03. 2001/07/09		K.Honda
				a04  2021/02/07     h.nishiura RTTF対応

*********************************************************************/

#include	<stdio.h>
#include    <machine.h>

#include	"iodefine.h"
#include	"table.h"

#include	"timer.h"
#include	"trace.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

union SDRFORM	tms_sdr_data;		// ＴＭＳからの受信データ
union SDFORM	tms_sd_data;	// ＴＭＳへの送信データ

unsigned char	tms_config_ok;

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	struct	CONFIG_INFO		config_data;		// 設定情報

extern	struct	BACKUP_DATA		backup_data;	// バックアップデータ
extern	char	dataVersionCRCL;
extern	char	dataVersionCRCH;
extern	unsigned char	tms_config_write;

extern	struct st_ReadSIGNAL nowSignal;			// 最新読みだし値
extern	struct	TIMER_CONTROL	timedata;		// タイマー情報

extern	unsigned char	ev5000_time_adjust;

extern	void HDLC_Initialize( int baud, char lowadr, int send_length );
extern	void HDLC_RecvEnable( void );
extern	void HDLC_SendEnable( char *buff, int send_length );
extern	void HDLC_RecvSendChange( void );

extern	int (*HDLC_ReceiveEnd)( char *recvdata, short recvcnt, unsigned char status );
extern	int (*HDLC_ReceiveAbort)( void );
extern	int (*HDLC_SendEnd)( void );
extern	int (*HDLC_SendAbort)( void );
extern	int (*HDLC_SendStart)( void );

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

struct	CONFIG_INFO		tms_config_data;		// 設定情報

static	char			befAdjust;		// 時間設定の前回値
static	char			befTalkBack[8];	// 前回の非常通報器要求の状態

static	char			sdflag;			// ＳＤデータ送信要求

static	char			blockno;
static	char			*tracep;		// トレース取り出しポインター
static	short			rembytes;

static	char			firstFlag;		// 最初の通信フラグ

/*@@@E*/
/************************************************************************

	Function name	<< tms_sendsetup >>
					送信準備処理
	Syntax			tms_sendsetup( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07  ＣＲＣコードの追加

*************************************************************************/
int tms_sendsetup( void )
{
		int		i;

		tms_sd_data.SD.lowadr = 0x60;					// アドレス設定
		tms_sd_data.SD.highadr = 0x01;
		tms_sd_data.SD.control = 0x13;					// ＵＩコード設定

		tms_sd_data.SD.code = 0x30;						// ＳＤフォーマットのコード
		tms_sd_data.SD.version = config_data.version;		// 自ＤＶＡＵバージョン

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

		return 0;
}

/************************************************************************

	Function name	<< tms_receive_end >>
					受信処理
	Syntax			tms_receive_end( *recvdata,recvcnt,status );
	Argument		char *recvdata			i : 受信データ
					short recvcnt			i : 受信データ数
					unsigned char status	i : 受信ステータス
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07  受信終了で呼び出すよう変更
				a02 2001/03/22	最初のデータを初期値とする
				a03 2001/07/09  ドア信号は、ＴＥＸＴ３１に変更
				a04 2021/02/07 h.nishiura RTTF対応

*************************************************************************/
int tms_receive_end( char *recvdata, short recvcnt, unsigned char status )
{
		union SDRFORM	*recvp;
		int		i;
		unsigned char	onoff,on;

		// 通信エラーのときは　何もしない
		if( status & 0x60 ){	/* CRC ERROR? or Over RUN Error? */
			recvcnt = 0;
			HDLC_RecvEnable();
			return 1;
		}

		recvp = (union SDRFORM *)recvdata;

		// 受信アドレスチェック
		// 自アドレス以外は　何もしない
		if( recvp->SDR.lowadr!=0x60 ){
			recvcnt = 0;
			HDLC_RecvEnable();
			return 1;
		}
		if( recvp->SDR.highadr!=0x01 ){
			recvcnt = 0;
			HDLC_RecvEnable();
			return 1;
		}
		if( recvp->SDR.control!=0x13 ){
			recvcnt = 0;
			HDLC_RecvEnable();
			return 1;
		}

		// 受信データの解析
		switch( recvp->SDR.code ){
		case 0x20:		/* SDR Format */
			memcpy( &tms_sdr_data, recvdata, sizeof(tms_sdr_data) );

			// 受信データのフィールド確保

			// テスト開始の状態設定
			DUALPORT.tms_request.testExecRequest = recvp->SDR.testRequest.testExecRequest;
			// テスト要求の状態設定
			DUALPORT.tms_request.testStartRequest1 = recvp->SDR.testRequest.testStartRequest1;
			DUALPORT.tms_request.testStartRequest2 = recvp->SDR.testRequest.testStartRequest2;
			// テスト項目の設定
			DUALPORT.tms_request.itemNo = recvp->SDR.itemNo;

			// 日時情報が設定されていて、時刻設定のとき　内部時計を設定する
			if( recvp->SDR.activeText.timeDateValid ){
				if( recvp->SDR.activeText.timeDateAdjustment ){
					if( befAdjust==0 ){
						befAdjust = 1;
						timedata.tm_year = (recvp->SDR.timeDate.yy10 * 10 + recvp->SDR.timeDate.yy1 );
						timedata.tm_mon = recvp->SDR.timeDate.MM10 * 10 + recvp->SDR.timeDate.MM1;
						timedata.tm_day = recvp->SDR.timeDate.dd10 * 10 + recvp->SDR.timeDate.dd1;
						timedata.tm_hour = recvp->SDR.timeDate.hh10 * 10 + recvp->SDR.timeDate.hh1;
						timedata.tm_min = recvp->SDR.timeDate.mm10 * 10 + recvp->SDR.timeDate.mm1;
						timedata.tm_sec = recvp->SDR.timeDate.ss10 * 10 + recvp->SDR.timeDate.ss1;
						timedata.tm_msec = 0;

						ev5000_time_adjust = 1;
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
			if( recvp->SDR.activeText.carTypeValid ){
				for( i=0; i<8; i++ ){
					backup_data.carInfo[i].carType = recvp->SDR.carInfo2[i].carType;
					DUALPORT.tms_request.carInfo[i].carType = recvp->SDR.carInfo2[i].carType;
				}
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					DUALPORT.tms_request.carType = backup_data.carInfo[0].carType;
				}
				else {
					DUALPORT.tms_request.carType = backup_data.carInfo[7].carType;
				}
			}
			// 車両番号の設定
			if( recvp->SDR.activeText.carNoValid ){
				for( i=0; i<8; i++ ){
					backup_data.carInfo[i].carNo = recvp->SDR.carInfo2[i].carNo;
					DUALPORT.tms_request.carInfo[i].carNo = recvp->SDR.carInfo2[i].carNo;
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
			DUALPORT.tms_request.couplingUnitCode = recvp->SDR.couplingUnitCode;

			// ステーションコードの設定
			DUALPORT.tms_request.stationNumber = recvp->SDR.stationNumber;
			// 次駅コードの設定
			if( DUALPORT.tms_request.nextStationName != recvp->SDR.nextStationName ){
				DUALPORT.tms_request.nextStationName = recvp->SDR.nextStationName;
				DUALPORT.pollingData.nextStation = recvp->SDR.nextStationName;
				if( firstFlag ){
					DUALPORT.tms_request.change_nextStationName = 1;
				}
			}
			// 到着駅コードの設定
			DUALPORT.tms_request.destination = recvp->SDR.destination;
			DUALPORT.pollingData.destination = recvp->SDR.destination;
			// ルートコードの設定
			if( DUALPORT.tms_request.routeCode != recvp->SDR.routeCode ){
				DUALPORT.tms_request.routeCode = recvp->SDR.routeCode;
				DUALPORT.pollingData.routeCode = recvp->SDR.routeCode;
				if( firstFlag ){
					DUALPORT.tms_request.change_routeCode = 1;
				}
			}

			// スペシャル放送コードの設定
			DUALPORT.tms_request.specialCode = recvp->SDR.specialCode;

			// 非常通報器の接続要求  ( OFF->ON 要求 )
			for( i=0; i<8; i++ ){
				onoff = befTalkBack[i] ^ recvp->SDR.talkBack[i];
				on = onoff & recvp->SDR.talkBack[i];
				DUALPORT.tms_request.talkBack[i] |= on;
				befTalkBack[i] = recvp->SDR.talkBack[i];
			}

			// 自動放送の要求
			if( DUALPORT.tms_request.dbEffective != recvp->SDR.ATCpaActive.dbEffective ){
				DUALPORT.tms_request.dbEffective = recvp->SDR.ATCpaActive.dbEffective;
				if( firstFlag ){
					DUALPORT.tms_request.change_dbEffective = 1;
				}
			}
			// 自動放送の要求
			if( DUALPORT.tms_request.atcActive != recvp->SDR.ATCpaActive.atcActive ){
				DUALPORT.tms_request.atcActive = recvp->SDR.ATCpaActive.atcActive;
				if( firstFlag ){
					DUALPORT.tms_request.change_atcActive = 1;
				}
			}
			// 自動放送の禁止
			if( DUALPORT.tms_request.broadCastOff != recvp->SDR.door.broadCastOff ){
				DUALPORT.tms_request.broadCastOff = recvp->SDR.door.broadCastOff;
				if( firstFlag ){
					DUALPORT.tms_request.change_broadCastOff = 1;
				}
			}
			// ドア状態の設定
			if( recvp->SDR.doorStatus.doorClosed==0 ){
				DUALPORT.tms_request.doorStatus=1;
			}
			else {
				DUALPORT.tms_request.doorStatus=0;
			}

			if( DUALPORT.tms_request.doorOpenEffective ){
				if( DUALPORT.tms_request.doorOpen != recvp->SDR.door.doorOpen ){
					DUALPORT.tms_request.doorOpen = recvp->SDR.door.doorOpen;
					if( firstFlag ){
						DUALPORT.tms_request.change_doorOpen = 1;
					}
				}
			}

			if( firstFlag ){
				// ＴＭＳアクティブフラグ
				DUALPORT.tms_request.Active = 1;
			}

			// ＴＭＳ有効タイマー
			timedata.tmsActiveTimer = TMS_OFFLINE_TIME;

			HDLC_RecvSendChange();
			sdflag = 1;

			firstFlag = 1;

			// 応答データの準備
			tms_sendsetup();
			break;

		case 0x21:		// TDR

			HDLC_RecvSendChange();
			sdflag = 0;

			tms_sd_data.TD.lowadr = 0x60;					// アドレス設定
			tms_sd_data.TD.highadr = 0x01;
			tms_sd_data.TD.control = 0x13;					// ＵＩコード設定

			tms_sd_data.TD.code = 0x31;
			tms_sd_data.TD.traceNo = 0;

			if( recvp->TDR.blockNo==0 ){		// Initial Request

				blockno = 0;
				tms_sd_data.TD.blockNo = blockno;

				tms_sd_data.TD.data[0] = 0;		// fault
				tms_sd_data.TD.data[1] = 0;		// year
				tms_sd_data.TD.data[2] = 0;		// month
				tms_sd_data.TD.data[3] = 0;		// day
				tms_sd_data.TD.data[4] = 0;		// hour
				tms_sd_data.TD.data[5] = 0;		// min
				tms_sd_data.TD.data[6] = 0;		// sec

				tracep = (char *)TraceGet( 1 );
				rembytes = sizeof(struct TRACE);

				i= 7;
			}
			else {
				blockno++;
				if( blockno!=recvp->TDR.blockNo ){		// ブロックの再計算
					blockno = 0;
					i=0;
					tracep = (char *)TraceGet( 1 );
					while( tracep!=NULL ){
						i+= sizeof(struct TRACE);
						if( i>100 ){
							blockno++;
							i-=100;
							if( blockno==recvp->TDR.blockNo )
								break;
						}
						tracep = (char *)TraceGet( 0 );
					}
				}

				tms_sd_data.TD.blockNo = blockno;

				if( rembytes==0 ){
					tracep = (char *)TraceGet( 0 );
					rembytes = sizeof(struct TRACE);
				}
				i = 0;
			}

			for( ; i<100; i++ ){
				if( tracep==NULL ){
					tms_sd_data.TD.data[i] = 0;	// zero padding
					continue;
				}
				tms_sd_data.TD.data[i] = *tracep++;	// trace data
				rembytes--;
				if( rembytes==0 ){
					tracep = (char *)TraceGet( 0 );
					rembytes = sizeof(struct TRACE);
				}
			}
			break;

		case 0x22:		// LDR
			HDLC_RecvSendChange();
			sdflag = 2;

			if( (recvp->LDR.blockNo & 0x7F)==0 ){
				tracep = (char *)&tms_config_data;
				rembytes = sizeof(tms_config_data);
//				rembytes = 20;
			}
			if( rembytes>=101 ){
				for( i=0; i<101; i++ ){
					*tracep++ = recvp->LDR.data[i];
				}
//				memcpy( tracep, recvp->LDR.data, 101 );
//				tracep += 100;
				rembytes -= 101;
			}
			else {
				for( i=0; i<rembytes; i++ ){
					*tracep++ = recvp->LDR.data[i];
				}
//				memcpy( tracep, recvp->LDR.data, rembytes );
				dataVersionCRCL = recvp->LDR.data[i++];
				dataVersionCRCH = recvp->LDR.data[i++];

				tms_config_ok = 1;
			}

			tms_sd_data.LD.lowadr = 0x60;					// アドレス設定
			tms_sd_data.LD.highadr = 0x01;
			tms_sd_data.LD.control = 0x13;					// ＵＩコード設定

			tms_sd_data.LD.code = 0x32;
			tms_sd_data.LD.blockNo = (recvp->LDR.blockNo+1) & 0x7F;

			break;

		default:
			HDLC_RecvEnable();
			break;
		}
}


/************************************************************************

	Function name	<< tms_send_end >>
					送信終了処理
	Syntax			tms_send_end( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07  通信見直し

*************************************************************************/
int tms_send_end( void )
{
		HDLC_RecvEnable();
}


/************************************************************************

	Function name	<< tms_send_abort >>
					送信中断処理
	Syntax			tms_send_abort( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07  通信見直し

*************************************************************************/
int tms_send_abort( void )
{
		HDLC_RecvEnable();
}

/************************************************************************

	Function name	<< tms_recv_abort >>
					受信中断処理
	Syntax			tms_recv_abort( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07  通信見直し

*************************************************************************/
int tms_recv_abort( void )
{
		HDLC_RecvEnable();
}

/************************************************************************

	Function name	<< tms_send_start >>
					送信開始処理
	Syntax			tms_send_start( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07  通信見直し

*************************************************************************/
int tms_send_start( void )
{
		if( sdflag==1 ){
			HDLC_SendEnable( (char *)&tms_sd_data, sizeof(tms_sd_data.SD) );
		}
		else if( sdflag==0 ){
			HDLC_SendEnable( (char *)&tms_sd_data, sizeof(tms_sd_data.TD) );
		}
		else {
			HDLC_SendEnable( (char *)&tms_sd_data, sizeof(tms_sd_data.LD) );
		}

		if( tms_config_ok ){
			tms_config_ok = 0;
			config_data = tms_config_data;
			tms_config_write = 1;
		}
}

/************************************************************************

	Function name	<< tms_initialize >>
					初期設定
	Syntax			tms_initialize( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07  通信見直し
					a02 2001/07/09  EIDS Door Status Initialize

*************************************************************************/
void tms_initialize( void )
{
		firstFlag = 0;

		tms_config_ok = 0;

		/** ＨＤＬＣ通信部を初期設定 **/
		HDLC_Initialize( 9600, 0x60, sizeof(tms_sd_data) );

		// 送信情報の初期設定
		memset( &tms_sd_data, 0, sizeof(tms_sd_data) );	// 初期はリセット状態
		tms_sd_data.SD.lowadr = 0x60;					// アドレス設定
		tms_sd_data.SD.highadr = 0x01;
		tms_sd_data.SD.control = 0x13;					// ＵＩコード設定

		tms_sd_data.SD.code = 0x30;						// ＳＤフォーマットのコード
		tms_sd_data.SD.version = config_data.version;	// 自ＤＶＡＵバージョン

		HDLC_ReceiveEnd = tms_receive_end;
		HDLC_ReceiveAbort = tms_recv_abort;
		HDLC_SendEnd = tms_send_end;
		HDLC_SendAbort = tms_send_abort;
		HDLC_SendStart = tms_send_start;

		HDLC_RecvEnable();

		DUALPORT.tms_request.doorStatus = 1;			// 2001/07/09 EIDS のドア状態はクローズ
}

