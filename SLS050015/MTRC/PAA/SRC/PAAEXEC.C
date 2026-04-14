/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	実行制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/06/27		K.Honda
				a01. 2000/11/07		K.Honda
				a02. 2001/05/23		K.Honda
				a03. 2003/07/07		h.nisiura
*********************************************************************/

#include "IOFUNC.h"

#include "COMMDEF.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
/** 音声バスモード **/
char	voice_bus_mode;

char	emg_rsc;				/*非常通報器番号フラグ*/

char	initial_voice_level;	/*基本音量データ
									下位６ビットのみ使用
								*/

short	noise_checkCount;		/* 騒音検出カウンター */
char	amp1Active;
char	amp2Active;
short	ampFailCount1;		/* アンプフェイルカウンターＣＨ。１ */
short	ampFailCount2;		/* アンプフェイルカウンターＣＨ。２ */
short	testCount;			/* テスト用カウンター */
char	ledCount;			/* ＬＥＤ点滅カウンター */

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
/** 操作モード **/
#define	OPE_INIT	0
#define	OPE_CAB		1
#define	OPE_MAN		2
#define	OPE_EMG		3
#define	OPE_TRS		4
#define	OPE_AUTO	5
#define	OPE_TEST	6


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカルテーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/** スイッチデータ **/
extern	short	nowReadIO;
#define	IN_24V	0x1000
#define	AMPON1	0x0400
#define	AMPON2	0x0800
#define	MICON	0x0020

/** 送信エリア **/
extern	struct PAASEND	sendarea;


extern	unsigned short	address;			/* 上位１２ビット＝Ｃａｒ　Ｎｏ．
										   下位４ビット＝ＤＩＰＳＷアドレス */

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	short	checkAmp;			/* 騒音検出するアンプ */
static	char	befNoiseLevel;		/* 前回ノイズレベル */
static	char	befOpeMode;			/* 前回の音声バスモード */
static	char	testRSCNo;			/* テスト用非常通報器番号 */
static	char	befTestRSCNo;		/* 前回テスト用非常通報器番号 */
static	char	testTrouble;		/* テスト用非常通報器ビット */
static	char	ampok;
static	char	bef_emg_rsc;		/* 前回の非常通報器ビット */
static	char	testPhase;			/* 音声マイク検出フェーズ */

/** 操作モード **/
char	opeMode;				/*操作モード*/

/*@@@E*/
/************************************************************************

	Function name	<< paaGetMode >>
					操作モードの取得
	Syntax			paaGetMode( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaGetMode( void )
{
#if 0
		io_output_off4( 0x07 );
		io_output_on4( voice_bus_mode & 0x07 );
#else
		io_output_off4( 0x0F );
		io_output_on4( (address >> 8) & 0x0F );
#endif

		switch( voice_bus_mode & 0x00F ){
		case	0x00:		/* CAB/CAB */
			opeMode = OPE_CAB;
			break;
		case	0x01:		/* Manual */
			opeMode = OPE_MAN;
			break;
		case	0x02:		/* Emergency */
			opeMode = OPE_EMG;
			break;
		case	0x03:		/* TRS */
			opeMode = OPE_TRS;
			break;
		case	0x04:		/* AUTO */
			opeMode = OPE_AUTO;
			break;
		case	0x05:		/* TEST */
			opeMode = OPE_TEST;
			break;
		case	0x0F:		/* Initial */
			opeMode = OPE_INIT;
			break;
		}

}

/************************************************************************

	Function name	<< paaSetVolume >>
					操作モードの取得
	Syntax			paaSetVolume( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27
					a01 2000/10/10  CH.1 のチェックのみ
					a02 2003/07/07	h.nisiura AGCステップを6→4に変更
*************************************************************************/
void paaSetVolume( void )
{
		char	noiseLevel,nowNoise;
		char	level;
		unsigned short	setValue;

		if( opeMode!=OPE_AUTO ){
			noise_checkCount=0;
			return;
		}

			/* 音声バス２４Ｖを認識中は、音量調整しない */
		if( nowReadIO & IN_24V ){
			noise_checkCount=0;
			return;
		}

		if( noise_checkCount<30 ){
			return;
		}
		noise_checkCount=0;

		noiseLevel = (nowReadIO & 0x0300)>>8;

		nowNoise = noiseLevel;
		if( befNoiseLevel < noiseLevel ){
			nowNoise = befNoiseLevel;
		}

		if( nowNoise==0 ){		/* 最小 */
			level = initial_voice_level + 8;		/* -8dB */
		}
		if( nowNoise==1 ){		/* 中 */
			level = initial_voice_level + 4;		/* -4dB */
		}
		if( nowNoise==2 ){		/* 最大 */
			level = initial_voice_level + 0;		/* -0dB */
		}
		if( nowNoise==3 ){		/* 最大 */
			level = initial_voice_level + 0;		/* -0dB */
		}
		if( level>0x3E ){
			level = 0x3E;
		}
		befNoiseLevel = noiseLevel;

#if 0
		io_output_off4( 0x07 );
		io_output_on4( befNoiseLevel & 0x07 );
#endif

		setValue = ((unsigned short)level<<2)+0x1803;	/* Left,Right Set, CS=L */
		csio_output( setValue );		/* 出力 ( 13 bit )*/

#if 0
		checkAmp ^= 1;
		if( checkAmp ){
#endif
			io_output_off3( 0x01 );		/* AMP1 Noise Level ON */
			io_output_on3( 0x02 );		/* AMP2 Noise Level OFF */
#if 0
		}
		else {
			io_output_on3( 0x01 );		/* AMP1 Noise Level OFF */
			io_output_off3( 0x02 );		/* AMP2 Noise Level ON */
		}
#endif
}

/************************************************************************

	Function name	<< paaAMPCheck >>
					アンプ故障のチェック
	Syntax			paaAMPCheck( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaAMPCheck( void )
{
			/* 放送中 */
		if( nowReadIO & IN_24V ){
			if( ampFailCount1>=6000 ){
				ampFailCount1 = 6000;
				sendarea.trouble | 0x01;
			}
			else {
				sendarea.trouble &= ~0x01;
			}
			if( ampFailCount2>=6000 ){
				ampFailCount2 = 6000;
				sendarea.trouble | 0x02;
			}
			else {
				sendarea.trouble &= ~0x02;
			}
		}
}


/************************************************************************

	Function name	<< paaExecuteAUTO >>
					車内放送モードの処理
	Syntax			paaExecuteAUTO( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaExecuteAUTO( void )
{
		if( befOpeMode!=opeMode ){
			befOpeMode = opeMode;
			io_output_on2( 0xC0 );			/* AMP1, AMP2 on */
			io_output_on4( 0xC0 );
			io_output_on1( 0x1F );			/* RSC Gate OFF */
			io_output_on2( 0x3F );			/* All Speaker ON */
			io_output_on1( 0x20 );			/* RSC Switch gate ON */
			amp1Active = 1;
			amp2Active = 1;
			ampFailCount1 = 0;
			ampFailCount2 = 0;
		}

		paaAMPCheck();
}

/************************************************************************

	Function name	<< paaExecuteMAN >>
					マニュアル放送モードの処理
	Syntax			paaExecuteMAN( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaExecuteMAN( void )
{
		if( befOpeMode!=opeMode ){
			befOpeMode = opeMode;
			io_output_on2( 0xC0 );			/* AMP1, AMP2 on */
			io_output_on4( 0xC0 );
			io_output_on1( 0x1F );			/* RSC Gate OFF */
			io_output_on2( 0x3F );			/* All Speaker ON */
			io_output_on1( 0x20 );			/* RSC Switch gate ON */
			amp1Active = 1;
			amp2Active = 1;
			ampFailCount1 = 0;
			ampFailCount2 = 0;
		}

		paaAMPCheck();
}


/************************************************************************

	Function name	<< paaExecuteCAB >>
					ＣＡＢ／ＣＡＢモードの処理
	Syntax			paaExecuteCAB( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27
					a01 2001/05/23 スピーカー６のＯＦＦを行なっていなかったのを修正
*************************************************************************/
void paaExecuteCAB( void )
{
		if( befOpeMode!=opeMode ){
			befOpeMode = opeMode;
			io_output_off2( 0xC0 );			/* AMP1, AMP2 off */
			io_output_off4( 0xC0 );
			io_output_on1( 0x1F );			/* RSC Gate OFF */
			io_output_off2( 0x3F );			/* All Speaker OFF */
			io_output_on1( 0x20 );			/* RSC Switch gate ON */
		}

		paaAMPCheck();
}

/************************************************************************

	Function name	<< paaExecuteEMG >>
					非常通報モードの処理
	Syntax			paaExecuteEMG( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaExecuteEMG( void )
{
		if( befOpeMode!=opeMode ){
			befOpeMode = opeMode;
			io_output_on2( 0xC0 );			/* AMP1, AMP2 on */
			io_output_on4( 0xC0 );
			io_output_on1( 0x1F );			/* RSC Gate OFF */
			io_output_off2( 0x3F );			/* All Speaker OFF */
			io_output_on1( 0x20 );			/* RSC Switch gate ON */

			bef_emg_rsc = 0;
		}
		if( emg_rsc & 0x07 ){	/* EMGP1-3 */
			io_output_on2( 0x40 );			/* AMP1 on */
			io_output_on4( 0x40 );
			amp1Active = 1;
		}
		else {
			amp1Active = 0;
			io_output_off2( 0x40 );			/* AMP1 off */
			io_output_off4( 0x40 );
		}
		if( emg_rsc & 0x18 ){	/* EMGP4-5 */
			amp2Active = 1;
			io_output_on2( 0x80 );			/* AMP2 on */
			io_output_on4( 0x80 );
		}
		else {
			amp2Active = 0;
			io_output_off2( 0x80 );			/* AMP2 off */
			io_output_off4( 0x80 );
		}

		if( emg_rsc!=bef_emg_rsc ){
			bef_emg_rsc = emg_rsc;

			io_output_on1( 0x1F );			/* RSC Gate OFF */
			io_output_off2( 0x3F );			/* All Speaker OFF */

			io_output_off1( emg_rsc );		/* RSC Gate ON by Request */
			io_output_on2( emg_rsc );	/* Speaker ON by Request */
		}

		paaAMPCheck();
}

/************************************************************************

	Function name	<< paaExecuteTEST >>
					テストモードの処理
	Syntax			paaExecuteTEST( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27
				a01 2000/11/07 ５秒間のチェック間隔
				a02 2001/04/16 スピーカーを切り替えてから２秒間ウェイト
				a03 2001/05/23 スピーカー６のＯＦＦを行なっていなかったのを修正
*************************************************************************/
void paaExecuteTEST( void )
{
		if( befOpeMode!=opeMode ){
			befOpeMode = opeMode;
			io_output_on2( 0xC0 );			/* AMP1, AMP2 on */
			io_output_on4( 0xC0 );
			io_output_on1( 0x1F );			/* RSC Gate OFF */
			io_output_off2( 0x3F );			/* All Speaker OFF */
			io_output_off1( 0x20 );			/* RSC Switch gate OFF */

			sendarea.trouble = 0;

			testCount = 0;
			testRSCNo = 0;
			befTestRSCNo = 1;

			amp1Active = 1;
			amp2Active = 1;
			ampok = 0;
			testPhase = 0;
		}

		if( testRSCNo >= 5 ){
			testRSCNo = 0;
		}

		if( (nowReadIO & IN_24V)==0 ){
			return;
		}

		if( nowReadIO & AMPON1 ){
			ampok |= AMPON1;
			sendarea.trouble &= ~0x01;
		}
		if( nowReadIO & AMPON2 ){
			ampok |= AMPON2;
			sendarea.trouble &= ~0x02;
		}

		/* 放送中 */
		if( (ampok & AMPON1)==0 ){
			if( ampFailCount1>=100 ){
				ampFailCount1 = 100;
				sendarea.trouble | 0x01;
			}
			else {
				sendarea.trouble &= ~0x01;
			}
		}
		if( (ampok & AMPON2)==0 ){
			if( ampFailCount2>=100 ){
				ampFailCount2 = 100;
				sendarea.trouble | 0x02;
			}
			else {
				sendarea.trouble &= ~0x02;
			}
		}

		if( befTestRSCNo != testRSCNo ){
			befTestRSCNo = testRSCNo;

			testPhase = 0;
		}

		if( testPhase==0 ){		/* スピーカーとマイクをすべてＯＦＦにし、音声が入っていないことを確認する*/
			io_output_on1( 0x1F );			/* RSC Gate OFF */
			io_output_off2( 0x3F );			/* All Speaker OFF */

			if( (nowReadIO & MICON)==0 ){		/* EMG Microphone InActive? */
				testPhase++;
			}
			return;
		}

		if( testPhase==1 ){		/* スピーカーとマイクをＯＮにする */
			testTrouble = 0x01<<testRSCNo;
			io_output_on2( testTrouble );	/* Speaker ON by TEST RSC No. */
			if( testRSCNo==5 ){
				io_output_off1( 0x10 );	/* RSC Gate ON by TEST RSC No. */
			}
			else {
				io_output_off1( testTrouble );	/* RSC Gate ON by TEST RSC No. */
			}
			testCount = 0;

			testPhase=2;
			return;
		}

		if( testPhase==2 ){		/* ２秒間はチェックしない */
			if( testCount<200 ){		/* ２秒までは無視する */
				return;
			}
			testPhase = 3;
			testCount = 0;
		}
								/* マイクに音声が入ることを確認する */
		if( nowReadIO & MICON ){		/* EMG Microphone Active? */
			testTrouble = 0;
		}

		if( testCount>=500 ){	/* ５秒間の確認時間 */
			testCount = 0;
			if( testTrouble ){
				sendarea.trouble |= (testTrouble<<2);
			}
			else {
				testTrouble = 0x01<<testRSCNo;
				sendarea.trouble &= ~(testTrouble<<2);
			}
			testRSCNo++;
		}
}

/************************************************************************

	Function name	<< paaExecuteINIT >>
					初期設定
	Syntax			paaExecuteINIT( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaExecuteINIT( void )
{
		befOpeMode = 0;
		io_output_on2( 0xC0 );			/* AMP1, AMP2 on */
		io_output_on4( 0xC0 );
		io_output_on1( 0x1F );			/* RSC Gate OFF */
		io_output_on2( 0x3F );			/* All Speaker ON */
		io_output_on1( 0x20 );			/* RSC Switch gate ON */

		voice_bus_mode = 4;				/* Initial Test Mode */
}

/************************************************************************

	Function name	<< paaExecuteMode >>
					操作モードの別処理
	Syntax			paaExecuteMode( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaExecuteMode( void )
{
		switch( opeMode ){
		case	OPE_AUTO:		/*車内放送中*/
			paaExecuteAUTO();
			break;
		case	OPE_MAN:		/* Manual */
			paaExecuteMAN();
			break;
		case	OPE_CAB:		/*CAB/CAB Mode*/
			paaExecuteCAB();
			break;
		case	OPE_EMG:		/*非常通報*/
			paaExecuteEMG();	
			break;
		case	OPE_TEST:		/*自己診断中*/
			paaExecuteTEST();
			break;
		case	OPE_INIT:
			paaExecuteINIT();
			break;
		}
}

/************************************************************************

	Function name	<< paaLEDOUT >>
					操作モードの取得
	Syntax			paaLEDOUT( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaLEDOUT( void )
{
		/* アンプ出力中のときは、ＡＭＰ出力をおこなう */
		if( nowReadIO & (AMPON1|AMPON2) ){
			io_output_on1( 0x80 );
#if 0
			io_output_on4( 0x10 );
#endif
		}
		else {
			io_output_off1( 0x80 );
#if 0
			io_output_off4( 0x10 );
#endif
		}

		/* 非常通報時要求のときは、デバッグＬＥＤを点灯 */
		if( sendarea.rscsw ){
			io_output_on4( 0x08 );
		}
		else {
			io_output_off4( 0x08 );
		}

		/* RUN LED の点滅 */
		if( ledCount==25 ){
			io_output_on1( 0x40 );
			io_output_on4( 0x20 );
		}
		if( ledCount==50 ){
			io_output_off1( 0x40 );
			io_output_off4( 0x20 );
		}
		if( ledCount>=50 ){
			ledCount=0;
		}
}

