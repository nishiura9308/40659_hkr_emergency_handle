/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	通信制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/07/27		K.Honda
				a01 2000/11/13
				a02 2001/03/22

*********************************************************************/

#include    <machine.h>
#include    "iodefine.h"

#include    "table.h"


/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

struct st_ReadSIGNAL nowSignal;			// 最新読みだし値
struct st_ReadSIGNAL onSignal;			// ＯＮになった信号

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

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

static	struct st_ReadSIGNAL	befSignal;
static	struct st_ReadSIGNAL	nowbefSignal;
		struct st_WriteSIGNAL	outSignal;

static	short	readCount1;
static	short	readCount2;
static	short	readCount3;

/*@@@E*/
/************************************************************************

	Function name	<< dvaIOOut >>
					Ｉ／Ｏ出力情報設定関数
	Syntax			dvaIOOut( onbit, offbit );
	Argument		unsigned long onbit  : i : I/O Output ON bit
				unsigned long offbit : i : I/O Output OFF bit
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27
	Revision		a01 2000/11/13  ACP LED のポート追加

*************************************************************************/
void dvaIOOut( unsigned long onbit, unsigned long offbit )
{
		outSignal.SIGNAL1.BYTE &= ~((unsigned char)(offbit>>8));
		outSignal.SIGNAL2.BYTE &= ~((unsigned char)(offbit));
		outSignal.SIGNAL3.BYTE &= ~((unsigned char)(offbit>>16));

		outSignal.SIGNAL1.BYTE |= (unsigned char)(onbit>>8);
		outSignal.SIGNAL2.BYTE |= (unsigned char)(onbit);
		outSignal.SIGNAL3.BYTE |= (unsigned char)(onbit>>16);

		IOSIGNAL1.BYTE = outSignal.SIGNAL1.BYTE;
		IOSIGNAL2.BYTE = ~outSignal.SIGNAL2.BYTE;
		IOSIGNAL3.BYTE = outSignal.SIGNAL3.BYTE;
}

/************************************************************************

	Function name	<< dvaIOIn >>
					接点入力
	Syntax			dvaIOIn( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27
	Revision		a01 2001/03/22  マイクのＰＴＴスイッチのよるモニターＳＰの制御
						（高速化のため）

*************************************************************************/
void    dvaIOIn(void)
{
		struct st_ReadSIGNAL signal;
		struct st_ReadSIGNAL onoff;

			// 現在の接点情報を読み込む
		signal.SIGNAL1.BYTE = IOSIGNAL1.BYTE;
		signal.SIGNAL2.BYTE = IOSIGNAL2.BYTE;
		signal.SIGNAL3.BYTE = IOSIGNAL3.BYTE;

		if( signal.SIGNAL1.BYTE!=befSignal.SIGNAL1.BYTE ){
			readCount1 = 0;
			// 前回と違うときは、前回値を更新する
			befSignal.SIGNAL1.BYTE = signal.SIGNAL1.BYTE;
		}
		else if( readCount1>=2 ){
			// 現在の接点情報を公開する
			nowSignal.SIGNAL1.BYTE = signal.SIGNAL1.BYTE;
		}
		else {
			readCount1++;
		}

		if( signal.SIGNAL2.BYTE!=befSignal.SIGNAL2.BYTE ){
			readCount2 = 0;
			// 前回と違うときは、前回値を更新する
			befSignal.SIGNAL2.BYTE = signal.SIGNAL2.BYTE;
		}
		else if( readCount2>=2 ){
			// 現在の接点情報を公開する
			nowSignal.SIGNAL2.BYTE = signal.SIGNAL2.BYTE;
		}
		else {
			readCount2++;
		}

		if( signal.SIGNAL3.BYTE!=befSignal.SIGNAL3.BYTE ){
			readCount3 = 0;
			// 前回と違うときは、前回値を更新する
			befSignal.SIGNAL3.BYTE = signal.SIGNAL3.BYTE;
		}
		else if( readCount3>=2 ){
			// 現在の接点情報を公開する
			nowSignal.SIGNAL3.BYTE = signal.SIGNAL3.BYTE;
		}
		else {
			readCount3++;
		}

		DUALPORT.nowSignal1 = nowSignal.SIGNAL1.BYTE;
		DUALPORT.nowSignal2 = nowSignal.SIGNAL2.BYTE;
		DUALPORT.nowSignal3 = nowSignal.SIGNAL3.BYTE;

		onoff.SIGNAL1.BYTE = nowSignal.SIGNAL1.BYTE ^ nowbefSignal.SIGNAL1.BYTE;
		onoff.SIGNAL2.BYTE = nowSignal.SIGNAL2.BYTE ^ nowbefSignal.SIGNAL2.BYTE;
		onoff.SIGNAL3.BYTE = nowSignal.SIGNAL3.BYTE ^ nowbefSignal.SIGNAL3.BYTE;
		nowbefSignal.SIGNAL1.BYTE = nowSignal.SIGNAL1.BYTE;
		nowbefSignal.SIGNAL2.BYTE = nowSignal.SIGNAL2.BYTE;
		nowbefSignal.SIGNAL3.BYTE = nowSignal.SIGNAL3.BYTE;
		onSignal.SIGNAL1.BYTE = onoff.SIGNAL1.BYTE & nowSignal.SIGNAL1.BYTE;
		onSignal.SIGNAL2.BYTE = onoff.SIGNAL1.BYTE & nowSignal.SIGNAL2.BYTE;
		onSignal.SIGNAL3.BYTE = onoff.SIGNAL1.BYTE & nowSignal.SIGNAL3.BYTE;

		if( DUALPORT.activeDVAU==0 )
			return;


			// アクティブＤＶＡＵの処理

		switch( DUALPORT.pollingData.voiceBus.BIT.mode & 0x0F ){
		case 0:		// CAB mode のときは、マイクスイッチによってスピーカーを制御
			if( nowSignal.SIGNAL3.RBIT.mcpPTTSW ){
//				DUALPORT.eids.Reset = 1;			// リセット信号アクティブ 2001/07/04

				dvaIOOut( 
					0
					,
					OUTSIGNAL_mcpMONSPON		// ＭＣＰスピーカーＯＦＦ
				);
			}
			else {
				dvaIOOut( 
					OUTSIGNAL_mcpMONSPON		// ＭＣＰスピーカーＯＮ
					,
					0
				);
			}
			if( nowSignal.SIGNAL3.RBIT.acpPTTSW ){
				DUALPORT.eids.Reset = 1;			// リセット信号アクティブ 2001/07/04

				dvaIOOut( 
					0
					,
					OUTSIGNAL_acpMONSPON		// ＡＣＰスピーカーＯＦＦ
				);
			}
			else {
				dvaIOOut( 
					OUTSIGNAL_acpMONSPON		// ＡＣＰスピーカーＯＮ
					,
					0
				);
			}
			break;

		case 1:		// マイク放送のときは、ＰＴＴを押したとき、ＯＦＦにする
			if( nowSignal.SIGNAL3.RBIT.acpPTTSW || nowSignal.SIGNAL3.RBIT.mcpPTTSW ){
				DUALPORT.eids.Reset = 1;			// リセット信号アクティブ 2001/07/04

				dvaIOOut( 
					0
					,
					OUTSIGNAL_mcpMONSPON		// ＭＣＰスピーカーＯＦＦ
					| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＦＦ
				);
			}
			break;

		case 3:		// CPA-Radio
			if( nowSignal.SIGNAL3.RBIT.acpPTTSW || nowSignal.SIGNAL3.RBIT.mcpPTTSW ){
				DUALPORT.eids.Reset = 1;			// リセット信号アクティブ 2001/07/04
			}

			dvaIOOut( 
				OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				,
				OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＦＦ
			);
			break;

		default:		// 以外のモードのときは、スピーカーをＯＦＦする
			dvaIOOut( 
				OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
				,
				0
			);
			break;
		}
}


/*@@@E*/
