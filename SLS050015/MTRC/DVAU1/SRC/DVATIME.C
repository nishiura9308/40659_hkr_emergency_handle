/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	時間制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation
																	*
	Revision	a00. 2000/08/07		K.Honda

*********************************************************************/

#include    <machine.h>
#include    "iodefine.h"
#include	"table.h"

#include	"timer.h"


/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

struct	TIMER_CONTROL	timedata;		// タイマー情報

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
extern	void dvaIOIn(void);
extern	void dvaIOOut( unsigned long onbit, unsigned long offbit );

extern	void	(*IMIA0Jump)(void);

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

static	char	signalCheckTime;

/*@@@E*/
/************************************************************************

	Function name	<< timer_0_Intr >>
					タイマー割り込み
	Syntax			timer_0_Intr( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void    timer_0_Intr(void)
{
		// 接点信号チェック
		signalCheckTime++;
		if( signalCheckTime>4 ){
			signalCheckTime=0;
			dvaIOIn();
		}

		// タイマー割り込み処理

		// 日付・時刻のカウント処理
		timedata.tm_msec++;
		if( timedata.tm_msec>99 ){
			timedata.tm_msec=0;
			timedata.tm_sec++;
		}
		if( timedata.tm_sec>59 ){
			timedata.tm_sec=0;
			timedata.tm_min++;
		}
		if( timedata.tm_min>59 ){
			timedata.tm_min=0;
			timedata.tm_hour++;
		}
		if( timedata.tm_hour>59 ){
			timedata.tm_hour=0;
			timedata.tm_day++;
		}

		if( timedata.tm_day==0 ){
			timedata.tm_day=1;
		}
		if( timedata.tm_mon==0 ){
			timedata.tm_mon=1;
		}
		switch( timedata.tm_mon ){
		case 2:
			if( (timedata.tm_year%4)==0 ){
				if( timedata.tm_day>29 ){
					timedata.tm_day=1;
					timedata.tm_mon++;
				}
			}
			else {
				if( timedata.tm_day>28 ){
					timedata.tm_day=1;
					timedata.tm_mon++;
				}
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if( timedata.tm_day>30 ){
				timedata.tm_day = 1;
				timedata.tm_mon++;
			}
			break;
		default:
			if( timedata.tm_day>31 ){
				timedata.tm_day = 1;
				timedata.tm_mon++;
			}
			break;
		}

		if( timedata.tm_mon>12 ){
			timedata.tm_mon=1;
			timedata.tm_year++;
		}

		// プロセス用タイマー
		if( timedata.checkTimer ){
			timedata.checkTimer--;
		}

		// ＴＭＳ有効カウント
		if( timedata.tmsActiveTimer ){
			timedata.tmsActiveTimer--;
			if( timedata.tmsActiveTimer==0 ){
				DUALPORT.tms_request.Active = 0;		// ＴＭＳデータ無効
			}
		}

		// ＥＶ５０００通信タイムアウトカウント
		if( timedata.ev5000timer ){
			timedata.ev5000timer--;
		}

		// 連結通信タイムカウント
		if( timedata.commGateTimer ){
			timedata.commGateTimer--;
		}

		// テスト時間
		if( timedata.testExecTimer ){
			timedata.testExecTimer--;
		}

		// テスト結果表示間隔
		if( timedata.testDispTimer ){
			timedata.testDispTimer--;
		}

		// サブＣＰＵチェック間隔
		if( timedata.subActiveTimer ){
			timedata.subActiveTimer--;
		}

		if( DUALPORT.mainActive ){
			if( DUALPORT.subActive ){
				DUALPORT.subActive = 0;
				timedata.subActiveTimer = 1000;
			}
			else if( timedata.subActiveTimer==0 ){
				dvaIOOut( OUTSIGNAL_LED7|OUTSIGNAL_LED5, 0xFFFF );
				Halt();
			}
		}
		else {
			timedata.subActiveTimer = 1000;
		}

		if( timedata.acpBlink ){
			timedata.acpBlink--;
		}
		else {
			timedata.acpBlink = 50;
		}

		if( timedata.menteSendTimer ){
			timedata.menteSendTimer--;
		}
}

/************************************************************************

	Function name	<< timer_0_Initialize >>
					初期設定
	Syntax			timer_0_Initialize( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void timer_0_Initialize( void )
{
		/******* タイマー初期設定 *******/

		/*** タイマー０のみ使用 ***/

		IMIA0Jump = timer_0_Intr;

		ITU0.TCR.BYTE = 0;
		ITU0.TCR.BIT.CCLR = 1;		/* ＧＲＡカウントでクリア */
		ITU0.TCR.BIT.CKEG = 0;		/* クロックの裁ち上がりでカウント */
		ITU0.TCR.BIT.TPSC = 2;		/* Φ／４クロック */
		ITU0.TIOR.BYTE = 0;			/* コンペアマッチによる出力禁止 */
		ITU0.TIER.BYTE = 0;
		ITU0.TIER.BIT.IMIEA = 1;	/* ＩＭＦＡフラグによる割り込み */
		ITU0.TSR.BYTE = 0;			/* タイマーステータスクリア */

		ITU0.TCNT = 0;				/* カウンタークリア */
										/* 14.7456MHz -> Φ
										Φ／４ = 3.6864MHz
										0.27126uS
										10mS / 0.27126uS = 0x9000
									*/
		ITU0.GRA = 0x9000;

		/** タイマースタート **/
		ITU.TSTR.BYTE = 0;
		ITU.TSTR.BIT.STR0 = 1;
		ITU.TSNC.BYTE = 0;			/*タイマー同期なし */
		ITU.TSNC.BIT.SYNC4 = 0;
		ITU.TSNC.BIT.SYNC3 = 0;
		ITU.TSNC.BIT.SYNC2 = 0;
		ITU.TSNC.BIT.SYNC1 = 0;
		ITU.TSNC.BIT.SYNC0 = 0;
		ITU.TMDR.BYTE = 0;			/*位相計数なし*/
		ITU.TFCR.BYTE = 0;
		ITU.TOER.BYTE = 0;
		ITU.TOCR.BYTE = 0;

}


/*@@@E*/
