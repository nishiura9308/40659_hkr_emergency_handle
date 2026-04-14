/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	時間制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

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

struct	TIMER_CONTROL	timedata;

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

extern	void	(*IMIA0Jump)(void);

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*@@@E*/
/************************************************************************
																		*
	Function name	<< timer_0_Intr >>
					タイマー割り込み
	Syntax			timer_0_Intr( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void    timer_0_Intr(void)
{
		// タイマー割り込み処理

			// ＥＩＤＳウェイトタイマーカウント
		if( timedata.eidstimer ){
			timedata.eidstimer--;
		}
			// ＲＴＩＦウェイトタイマーカウント
		if( timedata.rtiftimer ){
			timedata.rtiftimer--;
		}
			// 音声バス通信タイマーカウント
		if( timedata.CommRecvTimer ){
			timedata.CommRecvTimer--;
		}
		if( timedata.CommCheckTimer ){
			timedata.CommCheckTimer--;
		}
		if( timedata.carNoSendTimer ){
			timedata.carNoSendTimer--;
		}

		if( timedata.mainActiveTimer ){
			timedata.mainActiveTimer--;
		}
}

/************************************************************************
																		*
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
										271.27uS
										10mS / 271.27uS = 0x9000
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
