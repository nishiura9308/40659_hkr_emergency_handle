/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	通信制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/07/27		K.Honda
	Revision	a01. 2000/09/07		K.Honda
	Revision	a02. 2000/11/13		K.Honda
				a03. 2001/04/18
				a04. 2002/04/08		h.nisiura
				a05. 2003/04/01		h.nisiura

				a07. 2004/09/13		h.nisiura
				a08. 2005/07/12		h.nisiura
				a09. 2013/03/26		h.nisiura
				a10. 2021/02/07		h.nisiura
				a11. 2023/03/07		h.nisiura
				a12. 2024/03/11		h.nisiura
				
*********************************************************************/

#include	<stdio.h>
#include    <machine.h>
#include    "iodefine.h"

#include	"table.h"

#include	"timer.h"

#define		VERSION_INFO	0x0305

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

unsigned short	VERSION;

unsigned char	tms_config_write;

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカルテーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	void dvaProcess_Init( void );
extern	void dvaMCPInitial( void );

extern	void dvaIOOut( unsigned long onbit, unsigned long offbit );
extern	void timer_0_Initialize( void );
extern	void dvauEV5000Initialize( void );
extern	void dvauMENTEInitialize( void );
extern	void tms_initialize( void );


extern	struct	BACKUP_DATA		backup_data;		// バックアップデータ
extern	struct	CONFIG_INFO		config_data;		// 設定情報
extern	struct st_ReadSIGNAL nowSignal;			// 最新Ｉ／Ｏ情報

extern	struct	TIMER_CONTROL	timedata;		// タイマー情報

extern	char	watchDogCount;		// ウォッチドッグカウンター
extern	char	watchDogFlag;		// ウォッチドッグフラグ
extern	char	watchDogON;		// ウォッチドッグ結果

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

extern	struct	BACKUP_DATA		backup_data;		// バックアップデータ
static	unsigned short magicCodeBack;

static	unsigned char	befSignal1;
static	unsigned char	befSignal2;
static	unsigned char	befSignal3;

extern	struct st_WriteSIGNAL	outSignal;
static	unsigned char	writeSignal1;
static	unsigned char	writeSignal2;
static	unsigned char	writeSignal3;

static	unsigned char	befTMSAct;
static	unsigned char	befActive;

/*@@@E*/
/************************************************************************

	Function name	<< main >>
					メイン関数
	Syntax			main( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27
	Revision		a01 2000/09/07  アドレス変更
				a02 2000/11/13	ＤＩＰ－ＳＷ変更
				a03 2001/04/18	ＥＶ５０００からの日時情報取得後、データ読み出し

*************************************************************************/
void main( void )
{
		int		i;
		int		ret;

		VERSION = VERSION_INFO;		// バージョンを設定

dvaIOOut( OUTSIGNAL_LED0, 0xFFFF );

		// 現在のＩ／Ｏ状況を読む
		for( i=0; i<50; i++ ){
			dvaIOIn();
		}

		if( nowSignal.SIGNAL2.RBIT.addressOdd ){	// 奇数アドレス時
			DUALPORT.answerData.version[0] = VERSION;
		}
		else {						// 偶数アドレス時
			DUALPORT.answerData.version[1] = VERSION;
		}

		DUALPORT.activeDVAU=0;
		DUALPORT.changeDVAU=0;
		DUALPORT.activeDVAUrequest=0;

			// 各モジュールの初期設定
		timer_0_Initialize();		// タイマー割り込みの初期設定

		dvauEV5000Initialize();		// ＥＶ５０００の初期設定
dvaIOOut( OUTSIGNAL_LED1, 0xFFFF );

		and_ccr(0x3F);						/* 割り込み全面許可 */
			// ＥＶ５０００からの日時情報の取り込み待ち
		while( DUALPORT.ev5000_request.Initial ){
			dvauEV5000Cyclic();		// 音声処理
			WatchDogReset();		// ウォッチドッグリセット
		}

		or_ccr(0xC0);				// 全面割り込み禁止
dvaIOOut( OUTSIGNAL_LED2, 0xFFFF );

		// バックアップ状況のチェック
		ret = dvaBackupCheck();
		if( ret<0 ){
			set_imask_ccr(1);
			printf("CONFIG Data Error\r\n");
			dvaIOOut( OUTSIGNAL_LED7|OUTSIGNAL_LED6, 0xFFFF );
			Halt();
		}

		if( ret>0 ){
			printf("BACKUP Broken  Reset\r\n");
		}

		if( nowSignal.SIGNAL2.RBIT.hcrON==0 ){
				// 初期設定状態
				// 仮アクティブＤＶＡＵの決定
			if( nowSignal.SIGNAL2.RBIT.addressOdd ){	// 奇数アドレス時
				backup_data.opeMode = OPEMODE_INITIAL_TEST;
				DUALPORT.activeDVAU=1;
				if( P4.DR.BIT.B1==0 ){
					backup_data.opeMode = OPEMODE_AUTO;
				}
			}
			else {						// 偶数アドレス時
//				backup_data.opeMode = OPEMODE_INITIAL_TEST2;
				backup_data.opeMode = OPEMODE_STANDBY;
				DUALPORT.activeDVAU=0;
//				DUALPORT.activeDVAU=1;
//				backup_data.opeMode = OPEMODE_IDLE;
			}
		}
		else {
				backup_data.opeMode = OPEMODE_INITIAL_TEST;
				DUALPORT.activeDVAU=1;
				if( P4.DR.BIT.B1==0 || P4.DR.BIT.B2==0 ){
					backup_data.opeMode = OPEMODE_AUTO;
				}
		}

			// 基本音量データの設定
		for( i=0; i<MAX_PAA; i++ ){
			DUALPORT.pollingData.voiceLevel[i] = config_data.paa[i].level;
		}

			// 前回車両番号の設定
		DUALPORT.pollingData.carNo = backup_data.carInfo[0].carNo;

		tms_config_write = 0;

			// 各モジュールの初期設定
		dvaProcess_Init();
		dvaMCPInitial();

		dvauMENTEInitialize();		// メンテナンスターミナルの初期設定

		tms_initialize();			// ＴＭＳ通信の初期設定

		printf("System Start\r\n");

		timedata.subActiveTimer = 1000;	// サブＣＰＵ起動チェックタイマースタート

dvaIOOut( OUTSIGNAL_LED3, 0xFFFF );
		and_ccr(0x3F);						/* 割り込み全面許可 */
		for( ;; ){
			DUALPORT.mainActive = 1;	// メインアクティブ設定
			dvaProcess();			// プロセス処理
			dvauEV5000Cyclic();		// 音声処理
			dvauMENTEMain();		// メンテナンス処理

			if( watchDogFlag ){
				printf("WatchDog Count = %d\n", watchDogCount );
				printf("watchDog ON = %d\n", watchDogON );
				watchDogFlag = 0;
			}

//			if( tms_config_write ){
//				dvaBackup();
//				tms_config_write=0;
//			}

			WatchDogReset();		// ウォッチドッグリセット
#if 0
if( backup_data.magicCode != magicCodeBack ){
	printf("before MagicCode = %X\r\n", magicCodeBack );
	printf("Now MagicCode = %X\r\n", backup_data.magicCode );
	magicCodeBack = backup_data.magicCode;
}
if( befSignal1!=nowSignal.SIGNAL1.BYTE ){
printf("600200 Change %X --> %X\r\n", befSignal1, nowSignal.SIGNAL1.BYTE );
befSignal1 = nowSignal.SIGNAL1.BYTE;
}
if( befSignal2!=nowSignal.SIGNAL2.BYTE ){
printf("600300 Change %X --> %X\r\n", befSignal2, nowSignal.SIGNAL2.BYTE );
befSignal2 = nowSignal.SIGNAL2.BYTE;
}
if( befSignal3!=nowSignal.SIGNAL3.BYTE ){
printf("600400 Change %X --> %X\r\n", befSignal3, nowSignal.SIGNAL3.BYTE );
befSignal3 = nowSignal.SIGNAL3.BYTE;
}

if( writeSignal1!=outSignal.SIGNAL1.BYTE ){
printf("600200 Write Change %X --> %X\r\n", writeSignal1, outSignal.SIGNAL1.BYTE );
writeSignal1 = outSignal.SIGNAL1.BYTE;
}
if( writeSignal2!=outSignal.SIGNAL2.BYTE ){
printf("600300 Write Change %X --> %X\r\n", writeSignal2, outSignal.SIGNAL2.BYTE );
writeSignal2 = outSignal.SIGNAL2.BYTE;
}
if( writeSignal3!=outSignal.SIGNAL3.BYTE ){
printf("600400 Write Change %X --> %X\r\n", writeSignal3, outSignal.SIGNAL3.BYTE );
writeSignal3 = outSignal.SIGNAL3.BYTE;
}

if( befTMSAct==0 && DUALPORT.tms_request.Active ){
befTMSAct = 1;
printf("TMS Active\r\n");
}
if( befTMSAct && DUALPORT.tms_request.Active==0 ){
befTMSAct = 0;
printf("TMS Off\r\n");
}
if( befActive!=DUALPORT.activeDVAU ){
printf("DVAU Active Change %X --> %X\r\n", befActive, DUALPORT.activeDVAU );
befActive = DUALPORT.activeDVAU;
}
#endif
		}

}

//struct st_dualport work;
