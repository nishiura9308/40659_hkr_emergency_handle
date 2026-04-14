/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	時間制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/06/27		K.Honda

*********************************************************************/

#include "IOFUNC.h"

#include "COMMDEF.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

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
extern	short	noise_checkCount;		/* 騒音検出カウンター */
extern	char	amp1Active;
extern	char	amp2Active;
extern	short	ampFailCount1;		/* アンプフェイルカウンターＣＨ。１ */
extern	short	ampFailCount2;		/* アンプフェイルカウンターＣＨ。２ */
extern	short	testCount;			/* テスト用カウンター */
extern	char	ledCount;			/* ＬＥＤ点滅カウンター */

/** スイッチデータ **/
extern	short	nowReadIO;
#define	IN_24V	0x1000
#define	AMPON1	0x0400
#define	AMPON2	0x0800
#define	MICON	0x0020

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/


/*@@@E*/
/************************************************************************

	Function name	<< timer_0_func >>
					１０ｍＳタイマー割り込み処理
	Syntax			timer_0_func( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void timer_0_func( void )
{
extern	void paaSignalIn( void );
extern	void paaSignalOut( void );

		paaSignalIn();

		noise_checkCount++;		/* 騒音検出カウンター */

		if( nowReadIO & IN_24V ){		/* DC24V ON?*/
			if( amp1Active )
				ampFailCount1++;
			if( amp2Active )
				ampFailCount2++;
		}
		else {
			ampFailCount1=0;
			ampFailCount2=0;
		}

		/* アンプがＯＮになれば　正常 */
		if( nowReadIO & AMPON1 ){
			ampFailCount1 = 0;
		}
		if( nowReadIO & AMPON2 ){
			ampFailCount2 = 0;
		}

		testCount++;
		ledCount++;
}

/************************************************************************

	Function name	<< timer_1_func >>
					１ｍＳタイマー割り込み処理
	Syntax			timer_1_func( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void timer_1_func( void )
{

}

