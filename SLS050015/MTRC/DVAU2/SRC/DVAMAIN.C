/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	メイン制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/06/27		K.Honda

*********************************************************************/

#include	<stdio.h>
#include    <machine.h>
#include    "iodefine.h"
#include	"table.h"

#include	"timer.h"

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
extern	void dvauCommInitialize( void );	// 音声バス通信初期設定
extern	void dvaCommProc( void );		// 音声バス通信処理

extern	void dvauEIDSInitialize( void );	// ＥＩＤＳ通信初期設定
extern	void dvauEIDSSendCheck( void );	// ＥＩＤＳ通信処理

extern	struct	TIMER_CONTROL	timedata;

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

static	int	bef_active;

/*@@@E*/
/************************************************************************

	Function name	<< main >>
					メイン関数
	Syntax			main( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void main( void )
{
		/* 初期設定 */
		timer_0_Initialize();
		dvauCommInitialize();
		dvauEIDSInitialize();

		DUALPORT2.mainActive = 0;

		set_imask_ccr(0);		/* 割り込み解除 */

		bef_active=0;

		strcpy( DUALPORT2.dummy, "END " );

		for( ;; ){
			if( DUALPORT2.mainActive ){
				set_imask_ccr(0);		/* 割り込み解除 */
				dvauEIDSSendCheck();
				dvaCommProc();
			}
			if( DUALPORT2.mainActive ){
				if( DUALPORT2.subActive==1 ){
					if( timedata.mainActiveTimer==0 ){
						DUALPORT2.mainActive = 0;
						timedata.mainActiveTimer=1000;
					}
				}
				else {
					DUALPORT2.subActive=1;
					timedata.mainActiveTimer=1000;
				}
			}
			WatchDogReset();
		}
}

struct st_dualport work;
