/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	プロセス制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/07		K.Honda
				a01. 2001/04/18

				a03．2002/04/08 h.nisiura アクティブが切り替り時（HCR Off)
				                                 ＥＩＤＳ表示クリア追加
				a04. 2003/04/03 h.nisiura ＭＣＰ次駅表示タイミング（自動・手動）
										  ドアチャイムペンディング修正

				a06. 2004/09/13	h.nishiura  常にcommGateオープン
											HCR ONと連動してダミー抵抗ＯＮに変更

				a07. 2013/05/10 h.nishiura 到着案内放送の起動(Extra timing)の追加
				a08. 2014/02/17 h.nishiura ATCアクティブ信号は放送中は無視に変更
				a09. 2021/02/07 h.nishiura RTTF対応
				a10. 2023/03/07 h.nishiura RTTF+STN手動放送追加
*********************************************************************/

#include	<stdio.h>
#include    <machine.h>
#include    "iodefine.h"
#include    "table.h"

#include    "timer.h"
#include    "trace.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
int		ReturnAutoMode;		// 2001/07/02 自動放送モードへ戻るフラグ
							// 自動放送に入ったときは　１になる

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
extern	void dvaIOOut( unsigned long onbit, unsigned long offbit );

extern	struct ROUTEINFO *dvaGetDefaultRoute( void );
extern	struct ROUTEINFO *dvaSearchRoute( unsigned char routeCode );
extern	struct STATIONINFO *dvaSearchStation( unsigned char routeCode, unsigned char stationCode );
extern	struct STATIONINFO *dvaSearchStationNumber( unsigned char routeCode, unsigned char stationNumber );
extern	struct STATIONINFO *dvaSearchStationNumberExt( unsigned char routeCode, unsigned char stationCode );
extern	struct STATIONINFO *dvaSearchDestination( unsigned char routeCode );

extern	struct st_ReadSIGNAL nowSignal;			// 最新読みだし値
extern	struct st_ReadSIGNAL onSignal;			// ＯＮになった信号
extern	struct	TIMER_CONTROL	timedata;		// タイマー情報

extern	struct	BACKUP_DATA		backup_data;		// バックアップデータ
extern	struct	CONFIG_INFO		config_data;		// 設定情報
extern	struct	ANNOUNCE_INFO	announce_data;		// 放送設定情報
extern	struct	ROUTEINFO		route;				// 駅情報

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	unsigned char	befOpeMode;
static	union EV5000_SEQUENCE Sequence[MAX_SEQUENCE];	// 放送シーケンスコード
static	unsigned char	testno;
static	unsigned char	testexec;
static	unsigned char	testReq;

static	unsigned char	audioSignal;

static	unsigned char	troubleNowTrouble;
static	unsigned char	troubleAnswerTrouble;
static	unsigned char	troublePAA[8];
static	unsigned char	troubleCommError1;
static	unsigned char	troubleCommError2;

static	unsigned char	pend_DR_Opn_A_Chime;
static	unsigned char	pend_DR_Cls_A_Chime;
static	unsigned char	pend_DR_Opn_B_Chime;
static	unsigned char	pend_DR_Cls_B_Chime;

static	unsigned char	busy_STNon;
static	unsigned char	busy_AUTOon;

static	unsigned char	BroadCast_count;	/* ブロードキャストカウンタ */

/*@@@E*/
/************************************************************************

	Function name	<< dvaProcess_Init >>
					プロセス制御初期設定処理
	Syntax			dvaProcess_Init( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	スピーカー制御初期設定
				a02 2001/07/02  自動放送モードへ戻るフラグのクリア
				a03 2001/07/10  ＥＩＤＳドアステータスはクローズ状態
				a04 2013/05/10  ブロードキャストカウンタクリア

*************************************************************************/
void dvaProcess_Init( void )
{
		befOpeMode = 0;

		DUALPORT.nowTrouble = 0;
		audioSignal = 0;

		DUALPORT.mcpMode.mcpManual=0;
		DUALPORT.mcpMode.Cab=0;

		backup_data.rootOpeMode=0;

		timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

		ReturnAutoMode = 0;				// 初期値は　ＩＤＬＥモード

		DUALPORT.eids.Door = 1;				// 2001/07/10 EIDS のドア状態はクローズ

		dvaIOOut(
			OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＦＦ
			| OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＦＦ
			| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
			| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
			,
			0
		);

		pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
		pend_DR_Cls_A_Chime = 0;	//
		pend_DR_Opn_B_Chime = 0;	//
		pend_DR_Cls_B_Chime = 0;	//

		busy_STNon = 0;				// 遅延ＭＣＰ次駅表示用
		busy_AUTOon = 0;			// 遅延ＭＣＰ次駅表示用

		BroadCast_count = 0;
}

/************************************************************************

	Function name	<< dvaInitialTEST >>
					初期テストシーケンス処理
	Syntax			dvaInitalTEST( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/04/18	スピーカー制御
						1分間の放送

*************************************************************************/
void dvaInitialTEST( void )
{
		int		i;
		struct ROUTEINFO	*routeData;
		char	buff[BUFSIZ];
		int		skip;
		int		ttl;

			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
		}

		DUALPORT.pollingData.voiceBus.BIT.mode = 5;	// 音声バス使用状況はテスト


		if( befOpeMode != OPEMODE_INITIAL_TEST ){
			befOpeMode = OPEMODE_INITIAL_TEST;

			DUALPORT.activeDVAU=1;

			backup_data.rootOpeMode=0;

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

			TraceAdd( EVENT_INIT_TEST );

			dvaMCPDisplaySet( 0, " TEST " );
			dvaMCPDisplaySet( 1, "      " );
			dvaMCPDisplaySet( 2, "      " );
			dvaMCPDisplaySet( 3, "      " );

			dvaIOOut(
				OUTSIGNAL_trsDisable			// 列車無線起動停止
				| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
				,
				OUTSIGNAL_audioBusDisable		// 車内放送中
				| OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				);

			dvaIOOut( OUTSIGNAL_LED0, 0xFF );

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCOnEnable = 0;
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 1;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			DUALPORT.mcpMode.testRelease=0;

				// 放送起動
			DUALPORT.ev5000_request.SentenceNo = SENTENCE_SPECIAL;
			DUALPORT.ev5000_request.Sequence = announce_data.spcma[TEST_SPCMA].sequence;
			DUALPORT.ev5000_request.Complete = 0;
			DUALPORT.ev5000_request.Abort = 1;
			DUALPORT.ev5000_request.Request = 1;

			timedata.testExecTimer = 6000;

			audioSignal = 0;
		}

		if( DUALPORT.mcpMode.testRelease ){		// テスト終了要求時
			DUALPORT.mcpMode.testRelease = 0;
			backup_data.opeMode = OPEMODE_AUTO;
			DUALPORT.ev5000_request.Abort = 1;
			return;
		}

		if( DUALPORT.activeDVAU ){
			if( DUALPORT.changeDVAU==0 ){
				if( timedata.testExecTimer==0 ){		// テスト終了時
					if( DUALPORT.ev5000_request.Complete==0 ){
						DUALPORT.ev5000_request.Abort = 1;
					}
					if( audioSignal != 0x03 ){
						DUALPORT.nowTrouble &= 0x04;
						DUALPORT.nowTrouble |= (~audioSignal) & 0x03;
					}
					else {
						DUALPORT.nowTrouble = 0;
					}
					return;
				}

				if( DUALPORT.ev5000_request.Complete ){	// 放送終了時
					// 放送起動
					DUALPORT.ev5000_request.SentenceNo = SENTENCE_SPECIAL;
					DUALPORT.ev5000_request.Sequence = announce_data.spcma[TEST_SPCMA].sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Abort = 1;
					DUALPORT.ev5000_request.Request = 1;
				}
				else {		// 放送中
					if( nowSignal.SIGNAL2.RBIT.audioBus ){
						audioSignal |= 0x01;
					}
					if( nowSignal.SIGNAL3.RBIT.audioSignalDetect ){
						audioSignal |= 0x02;
					}
				}
			}
		}
		else {
			backup_data.opeMode = OPEMODE_INITIAL_TEST2;
		}

}

/************************************************************************

	Function name	<< dvaSELFTEST >>
					セルフテストシーケンス処理
	Syntax			dvaSELFTEST( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/04/18	スピーカー制御
						1分間の放送

*************************************************************************/
void dvaSELFTEST( void )
{
		int		i;
		struct ROUTEINFO	*routeData;
		char	buff[BUFSIZ];
		int		skip;
		int		ttl;
		union st_READSignal2 signal2;

			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
		}

		DUALPORT.pollingData.voiceBus.BIT.mode = 5;	// 音声バス使用状況はテスト

		if( befOpeMode != OPEMODE_SELF_TEST ){
			befOpeMode = OPEMODE_SELF_TEST;

			DUALPORT.activeDVAU=1;

			backup_data.rootOpeMode=0;

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

			TraceAdd( EVENT_SELF_TEST );

			// 放送起動
			DUALPORT.ev5000_request.SentenceNo = SENTENCE_SPECIAL;
			DUALPORT.ev5000_request.Sequence = announce_data.spcma[TEST_SPCMA].sequence;
			DUALPORT.ev5000_request.Complete = 0;
			DUALPORT.ev5000_request.Abort = 1;
			DUALPORT.ev5000_request.Request = 1;

			dvaMCPDisplaySet( 0, " TEST " );
			dvaMCPDisplaySet( 1, "      " );
			dvaMCPDisplaySet( 2, "      " );
			dvaMCPDisplaySet( 3, "      " );

			dvaIOOut(
				OUTSIGNAL_trsDisable			// 列車無線起動停止
				| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
				,
				OUTSIGNAL_audioBusDisable		// 車内放送中
				| OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				);

			dvaIOOut( OUTSIGNAL_LED0, 0xFF );

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCOnEnable = 0;
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 1;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			DUALPORT.mcpMode.testRelease=0;

			timedata.testExecTimer = 6000;

			audioSignal = 0;
		}

			// 列車無線起動の確認
		signal2.BYTE = nowSignal.SIGNAL2.BYTE;
		signal2.BYTE |= DUALPORT.answerData.dvau2;
		if( signal2.RBIT.trsRequest ){
			backup_data.rootOpeMode = OPEMODE_IDLE;
			backup_data.opeMode = OPEMODE_TRS;
			return;
		}

		if( DUALPORT.mcpMode.testRelease ){		// テスト終了要求時
			DUALPORT.mcpMode.testRelease = 0;
			backup_data.opeMode = OPEMODE_IDLE;
			DUALPORT.ev5000_request.Abort = 1;
			return;
		}

		if( DUALPORT.activeDVAU ){
			if( DUALPORT.changeDVAU==0 ){
				if( timedata.testExecTimer==0 ){		// テスト終了時
					if( audioSignal != 0x03 ){
						DUALPORT.nowTrouble &= 0x04;
						DUALPORT.nowTrouble |= (~audioSignal) & 0x03;
					}
					else {
						DUALPORT.nowTrouble = 0;
					}

					timedata.testExecTimer = 6000;
				}

				if( DUALPORT.ev5000_request.Complete ){
					// 放送起動
					DUALPORT.ev5000_request.SentenceNo = SENTENCE_SPECIAL;
					DUALPORT.ev5000_request.Sequence = announce_data.spcma[TEST_SPCMA].sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Abort = 1;
					DUALPORT.ev5000_request.Request = 1;
				}
				else {		// 放送中
					if( nowSignal.SIGNAL2.RBIT.audioBus ){
						audioSignal |= 0x01;
					}
					if( nowSignal.SIGNAL3.RBIT.audioSignalDetect ){
						audioSignal |= 0x02;
					}
				}
			}
		}
		else {
			backup_data.opeMode = OPEMODE_STANDBY;
		}

}

/************************************************************************

	Function name	<< dvaTEST >>
					ＴＭＳテストシーケンス処理
	Syntax			dvaTEST( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	スピーカー制御

*************************************************************************/
void dvaTEST( void )
{
			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
		}

		if( befOpeMode != OPEMODE_TEST ){
			befOpeMode = OPEMODE_TEST;

			DUALPORT.activeDVAU=1;
			backup_data.rootOpeMode=0;

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

			TraceAdd( EVENT_TEST );

			DUALPORT.testResponce.testExecutionStatus = 0;
			DUALPORT.testResponce.testStartResponce1 = 0;
			DUALPORT.testResponce.testStartResponce2 = 0;
			if( DUALPORT.tms_request.testStartRequest1 ){
				DUALPORT.testResponce.testStartResponce1 = 1;
				DUALPORT.testItemNo = DUALPORT.tms_request.itemNo;
			}
			else if( DUALPORT.tms_request.testStartRequest2 ){
				DUALPORT.testResponce.testStartResponce2 = 1;
				DUALPORT.testItemNo = DUALPORT.tms_request.itemNo;
			}

			testno = 0;
			testexec = 0;

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCOnEnable = 0;
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 1;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			testReq = DUALPORT.tms_request.testExecRequest;	// 初期値
		}

		if( DUALPORT.tms_request.testStartRequest1 ){
			DUALPORT.testResponce.testStartResponce1 = 1;
			DUALPORT.testItemNo = DUALPORT.tms_request.itemNo;
		}
		else if( DUALPORT.tms_request.testStartRequest2 ){
			DUALPORT.testResponce.testStartResponce2 = 1;
			DUALPORT.testItemNo = DUALPORT.tms_request.itemNo;
		}

		if( testReq != DUALPORT.tms_request.testExecRequest ){
			testReq = DUALPORT.tms_request.testExecRequest;
			if( DUALPORT.tms_request.testExecRequest ){
				DUALPORT.testItemResult.OK = 0;
				DUALPORT.testItemResult.NG = 0;
				DUALPORT.testSubItemResult = 0;

				if( DUALPORT.testResponce.testStartResponce1 ){
					DUALPORT.testResponce.testExecutionStatus = 1;

					timedata.testExecTimer = 1000;
					testexec = 1;
				}
				else if( DUALPORT.testResponce.testStartResponce2 ){

					DUALPORT.testResponce.testExecutionStatus = 1;

					// 放送起動
					DUALPORT.ev5000_request.SentenceNo = SENTENCE_SPECIAL;
					DUALPORT.ev5000_request.Sequence = announce_data.spcma[TEST_SPCMA].sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Abort = 1;
					DUALPORT.ev5000_request.Request = 1;

					DUALPORT.pollingData.voiceBus.BIT.mode = 5;	// 音声バス使用状況はテスト

					timedata.testExecTimer = 6000;
					testexec = 1;
				}
			}
		}

		if( testexec==1 ){
			if( timedata.testExecTimer==0 ){
				if( DUALPORT.testResponce.testStartResponce1 ){
					if( DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError1 ){
						DUALPORT.testSubItemResult |= 0x0001;
					}
					if( DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError2 ){
						DUALPORT.testSubItemResult |= 0x0002;
					}
					if( DUALPORT.tms_answer.dvaCommError.BIT.mcpCommError1 ){
						DUALPORT.testSubItemResult |= 0x0004;
					}
					if( DUALPORT.tms_answer.dvaCommError.BIT.mcpCommError2 ){
						DUALPORT.testSubItemResult |= 0x0008;
					}
				}

				if( DUALPORT.testResponce.testStartResponce2 ){
					if( DUALPORT.tms_answer.dvaCommError.BYTE[1] & (0x01<<(DUALPORT.testItemNo-1)) ){
						DUALPORT.testSubItemResult |= 0x003F;
					}
					if( DUALPORT.answerData.trouble[DUALPORT.testItemNo-1].BIT.AMP1 ){
						DUALPORT.testSubItemResult |= 0x0001;
					}
					if( DUALPORT.answerData.trouble[DUALPORT.testItemNo-1].BIT.AMP2 ){
						DUALPORT.testSubItemResult |= 0x0001;
					}
					if( DUALPORT.answerData.trouble[DUALPORT.testItemNo-1].BIT.SP1 ){
						DUALPORT.testSubItemResult |= 0x0002;
					}
					if( DUALPORT.answerData.trouble[DUALPORT.testItemNo-1].BIT.SP2 ){
						DUALPORT.testSubItemResult |= 0x0004;
					}
					if( DUALPORT.answerData.trouble[DUALPORT.testItemNo-1].BIT.SP3 ){
						DUALPORT.testSubItemResult |= 0x0008;
					}
					if( DUALPORT.answerData.trouble[DUALPORT.testItemNo-1].BIT.SP4 ){
						DUALPORT.testSubItemResult |= 0x0010;
					}
					if( DUALPORT.answerData.trouble[DUALPORT.testItemNo-1].BIT.SP5 ){
						DUALPORT.testSubItemResult |= 0x0020;
					}
				}

				if( DUALPORT.testSubItemResult ){
					DUALPORT.testItemResult.NG = 1;
				}
				else {
					DUALPORT.testItemResult.OK = 1;
				}

				DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// 自動放送モード

				DUALPORT.testResponce.testExecutionStatus = 2;
				DUALPORT.ev5000_request.Abort = 1;
				testexec = 0;
			}

			if( DUALPORT.testResponce.testStartResponce2 ){
				if( DUALPORT.ev5000_request.Complete ){
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
			}
		}

		if( DUALPORT.tms_request.Active==0 ){
			DUALPORT.testResponce.testExecutionStatus = 0;
			DUALPORT.testResponce.testStartResponce1 = 0;
			DUALPORT.testResponce.testStartResponce2 = 0;
			backup_data.opeMode = OPEMODE_AUTO;
			return;
		}

		if( DUALPORT.tms_request.testStartRequest1==0
			&& DUALPORT.tms_request.testStartRequest2==0 ){
			DUALPORT.testResponce.testExecutionStatus = 0;
			DUALPORT.testResponce.testStartResponce1 = 0;
			DUALPORT.testResponce.testStartResponce2 = 0;
			backup_data.opeMode = OPEMODE_AUTO;
			return;
		}

		dvaIOOut(
			OUTSIGNAL_trsDisable			// 列車無線起動停止
			| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
			| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
			,
			OUTSIGNAL_audioBusDisable		// 車内放送中
			| OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
			| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
			);

		dvaIOOut( OUTSIGNAL_LED0, 0xFF );
}

/************************************************************************

	Function name	<< dvaInitialStandby >>
					スタンドバイシーケンス処理（for InitialTest)
	Syntax			dvaInitialStandby( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/04/18	スピーカー制御

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]
	処理概要：
			非アクティブＤＶＡＵになっている状態
			アクティブＤＶＡＵからの切り替え要求を待つ
				切替時は、ＩＤＬＥモード
				または、テストモード
			１６両編成のためのゲートオープン
			音声バス信号は　受信のみ
			ＴＲＳ信号の送信

	------------------------------------------------------------------[@@@E]*/
void dvaInitialStandby( void )
{
		if( befOpeMode != OPEMODE_INITIAL_TEST2 ){
			befOpeMode = OPEMODE_INITIAL_TEST2;

			TraceAdd( EVENT_INIT_TEST2 );

			backup_data.rootOpeMode=0;

			DUALPORT.activeDVAU=0;			// 非アクティブＤＶＡＵ

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

			DUALPORT.mcpMode.ATCOnEnable = 0;
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 0;

			dvaIOOut(
				OUTSIGNAL_audioBusDisable		// 車内放送禁止
				| OUTSIGNAL_trsDisable			// 列車無線起動許可
				,
				OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
				| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＦＦ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＦＦ
				);
		}

		dvaIOOut( OUTSIGNAL_LED1, 0xFF );
}

/************************************************************************

	Function name	<< dvaStandby >>
					スタンドバイシーケンス処理
	Syntax			dvaStandby( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/04/18 音声バスモードが列車無線で
						ないときは、列車無線の放送を禁止し、
						列車無線をアクティブＤＶＡＵが
						受け付けたときのみ許可する
				a02 2001/07/02  自動放送モードへ戻るフラグのクリア
				a03 2001/07/10  EIDS Door Status is Closed
*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]
	処理概要：
			非アクティブＤＶＡＵになっている状態
			アクティブＤＶＡＵからの切り替え要求を待つ
				切替時は、ＩＤＬＥモード
				または、テストモード
			１６両編成のためのゲートオープン
			音声バス信号は　受信のみ
			ＴＲＳ信号の送信

	------------------------------------------------------------------[@@@E]*/
void dvaStandby( void )
{
		int		i;

		if( befOpeMode != OPEMODE_STANDBY ){
			befOpeMode = OPEMODE_STANDBY;

			TraceAdd( EVENT_STANDBY );

			backup_data.rootOpeMode=0;

			DUALPORT.activeDVAU=0;			// 非アクティブＤＶＡＵ

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

			DUALPORT.mcpMode.ATCOnEnable = 0;
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 0;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;
		}

		DUALPORT.eids.Door = 1;				// 2001/07/10 EIDS のドア状態はクローズ

		ReturnAutoMode = 0; 		// 2001/07/02 自動放送モードに戻るのを止める

		for( i=0; i<8; i++ ){
			DUALPORT.tms_answer.passengerAlarm[i] = DUALPORT.answerData.rscSW[i].BYTE;
		}
		switch( DUALPORT.pollingData.voiceBus.BIT.mode & 0x0F ){
		case 0:		// CAB/CAB
				// CAB mode のときは、マイクスイッチによってスピーカーを制御
			if( nowSignal.SIGNAL3.RBIT.mcpPTTSW ){
				dvaIOOut(
					OUTSIGNAL_audioBusDisable		// 車内放送禁止
					,
					OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
					| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
					| OUTSIGNAL_mcpMONSPON		// ＭＣＰスピーカーＯＦＦ
				);
			}
			else {
				dvaIOOut(
					OUTSIGNAL_mcpMONSPON		// ＭＣＰスピーカーＯＮ
					,
					OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
					| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
				);
			}
			if( nowSignal.SIGNAL3.RBIT.acpPTTSW ){
				dvaIOOut(
					OUTSIGNAL_audioBusDisable		// 車内放送禁止
					,
					OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
					| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
					| OUTSIGNAL_acpMONSPON		// ＡＣＰスピーカーＯＦＦ
				);
			}
			else {
				dvaIOOut(
					OUTSIGNAL_acpMONSPON		// ＡＣＰスピーカーＯＮ
					,
					OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
					| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
				);
			}

			if( nowSignal.SIGNAL3.RBIT.mcpPTTSW==0 && nowSignal.SIGNAL3.RBIT.acpPTTSW==0 ){
				dvaIOOut(
					OUTSIGNAL_audioBusDisable		// 車内放送禁止
					| OUTSIGNAL_trsDisable			// 列車無線起動停止
					,
					OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
					| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				);
			}
			break;

		case 1:		// Microphone
			// 音声バス使用状況が列車無線放送以外の場合
			dvaIOOut(
				OUTSIGNAL_audioBusDisable		// 車内放送禁止
				| OUTSIGNAL_trsDisable			// 列車無線起動停止
				| OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
				,
				OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＦＦ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＦＦ
				);
			break;

		case 3:		// CPA-Radio
			dvaIOOut(
				OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				,
				OUTSIGNAL_audioBusDisable		// 車内放送禁止
				| OUTSIGNAL_trsDisable			// 列車無線起動許可
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＦＦ
			);
			break;

		default:
			// 音声バス使用状況が列車無線放送以外の場合
			dvaIOOut(
				OUTSIGNAL_audioBusDisable		// 車内放送禁止
				| OUTSIGNAL_trsDisable			// 列車無線起動停止
				| OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
				,
				OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＦＦ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＦＦ
				);
		}

		dvaIOOut( OUTSIGNAL_LED1, 0xFF );
}


/************************************************************************

	Function name	<< dvaIDLE >>
					ＩＤＬＥシーケンス処理
	Syntax			dvaIDLE( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	スピーカー制御
				a02 2001/07/02  自動放送モードへ戻るフラグのクリア
				a03 2001/07/10  EIDS Door Closed
								EIDS Reset 信号がアクティブのときは、
								ＥＶ５０００の放送中と同じく、ＲＥＰ，ＳＴＮは機能しない
				a04 2002/12/17  ＳＴＮ押下時現在駅の次駅放送後次の駅をＭＣＰ表示するよう変更
								ドアチャイムペンディング処理追加
				a05 2003/05/29 h.nihiura ＴＭＳへの送信情報の準備を無条件実行に変更
				a06 2023/03/07 h.nisiura RTTF+STN手動放送追加
*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		イベント待ちの状態
		マニュアルモードの処理を行う

	------------------------------------------------------------------[@@@E]*/
void dvaIDLE( void )
{
		int		i;
		struct ROUTEINFO *routeData;
		struct STATIONINFO *stationData;
		union st_READSignal2 signal2;
		unsigned char tmp_nowStationNo;

			// ＴＭＳへの送信情報の準備
		for( i=0; i<8; i++ ){
			DUALPORT.tms_answer.passengerAlarm[i] = DUALPORT.answerData.rscSW[i].BYTE;
		}

			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
			befOpeMode = 0;
		}

			// 列車無線放送中は、停止するのを待つ
//		signal2.BYTE = nowSignal.SIGNAL2.BYTE;
//		signal2.BYTE |= DUALPORT.answerData.dvau2;
//		if( DUALPORT.mcpMode.trsBUSY ){
//			if( signal2.RBIT.trsRequest ){
//				return;
//			}
//			// 列車無線停止時は、復帰させる
//			DUALPORT.mcpMode.trsBUSY = 0;
//			// 自動放送をキャンセル要求を解除
//			DUALPORT.ev5000_request.Abort = 0;
//			befOpeMode = 0;
//		}


		if( befOpeMode != OPEMODE_IDLE ){
			befOpeMode = OPEMODE_IDLE;

			dvaMCPInitial();

			TraceAdd( EVENT_IDLE );

			DUALPORT.activeDVAU=1;		// アクティブＤＶＡＵモード

			DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// 自動放送モード

			// ＥＶ５０００へ放送キャンセルを要求
			DUALPORT.ev5000_request.Abort = 1;

			dvaIOOut(
				OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
				| OUTSIGNAL_trsDisable			// 列車無線起動停止
				,
				OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				| OUTSIGNAL_audioBusDisable		// 車内放送許可
				);

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCOnEnable = 0;
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 1;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			DUALPORT.tms_request.change_dbEffective=0;
			DUALPORT.tms_request.change_atcActive=0;
			DUALPORT.tms_request.change_nextStationName=0;
			DUALPORT.tms_request.change_destination=0;
			DUALPORT.tms_request.change_routeCode=0;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			DUALPORT.ev5000_request.Repeat = 0;

			busy_STNon = 0;				// 遅延ＭＣＰ次駅表示用
		}

		DUALPORT.eids.Door = 1;				// 2001/07/10 EIDS のドア状態はクローズ

		ReturnAutoMode = 0; 		// 2001/07/02 自動放送モードに戻るのを止める
		backup_data.rootOpeMode = 0;

			// 列車無線起動の確認
		signal2.BYTE = nowSignal.SIGNAL2.BYTE;
		signal2.BYTE |= DUALPORT.answerData.dvau2;
		if( signal2.RBIT.trsRequest ){
			backup_data.rootOpeMode = OPEMODE_IDLE;
			backup_data.opeMode = OPEMODE_TRS;
			return;
		}

			// ＭＣＰからの自動放送要求
		if( DUALPORT.mcpMode.ATCOn ){
			DUALPORT.mcpMode.ATCOn = 0;
			backup_data.opeMode = OPEMODE_AUTO;
		}

			// ＭＣＰからの自動放送解除
		if( DUALPORT.mcpMode.ATCOff ){
			DUALPORT.mcpMode.ATCOff = 0;
		}

			// ＭＣＰからの非常通報要求
		if( DUALPORT.mcpMode.EMGPOn ){
			DUALPORT.mcpMode.EMGPOn = 0;

			for( i=0; i<16; i++ ){
				backup_data.nowEMGHL[i] = DUALPORT.mcpMode.EMGPHL[i];
//				DUALPORT.mcpMode.EMGPHL[i] = 0;
			}
			backup_data.rootOpeMode = OPEMODE_IDLE;
			backup_data.opeMode = OPEMODE_EMGHL;
		}

			// ＭＣＰからのテスト要求
		if( DUALPORT.mcpMode.test ){
			DUALPORT.mcpMode.test = 0;
			backup_data.opeMode = OPEMODE_SELF_TEST;
			audioSignal = 0;
			return;
		}

			// ＭＣＰからのリピート放送要求
		if( DUALPORT.mcpMode.REPOn ){
			DUALPORT.mcpMode.REPOn = 0;

			if( DUALPORT.ev5000_request.Complete && DUALPORT.eids.ResetStatus==0 ){
				// 放送が終わっていれば受け付ける	ＥＩＤＳのリセット出力がないとき

					// ルートコードと駅名で検索する
				tmp_nowStationNo = backup_data.nowStationNo;
				stationData = dvaSearchStation( backup_data.nowRouteCode, backup_data.nowStationName );
				backup_data.nowStationNo = tmp_nowStationNo;

					// 放送を開始
				DUALPORT.ev5000_request.SentenceNo = SENTENCE_STATION;
				DUALPORT.ev5000_request.Sequence = stationData->sequence;
				DUALPORT.ev5000_request.Complete = 0;
				DUALPORT.ev5000_request.Request = 1;
				DUALPORT.ev5000_request.Repeat = 1;
				TraceAdd( EVENT_REPEAT );

//				DUALPORT.eids.Route = route.eidsRouteChar;
//				DUALPORT.eids.Station = route.station[backup_data.nowStationNo].eidsStationChar;
//				if( DUALPORT.eids.Station ){
//					DUALPORT.eids.sendreq = 1;
//				}

//				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=1;
				dvaMCPDisplaySet( 0, "  REP " );
				DUALPORT.mcpMode.INUSE = 1;
				DUALPORT.tms_answer.dvaStatus.BIT.autoMode = 1;
			}
		}
		if( DUALPORT.ev5000_request.Repeat ){
			if( DUALPORT.ev5000_request.Complete && DUALPORT.eids.ResetStatus==0 ){
				// 放送が終わっていれば受け付ける	ＥＩＤＳのリセット出力がないとき
				DUALPORT.ev5000_request.Repeat = 0;

//				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;

				dvaMCPInitial();
			}
		}

			// ＭＣＰからの次駅案内要求
		if( DUALPORT.mcpMode.STNOn ){

			if( DUALPORT.ev5000_request.Complete && DUALPORT.eids.ResetStatus==0 ){
				// 放送が終わっていれば受け付ける	ＥＩＤＳのリセット出力がないとき

				if( busy_STNon == 1 ){
					busy_STNon = 0;
					DUALPORT.mcpMode.STNOn = 0;
					DUALPORT.mn_stn_busy = 0;

						// 次駅を求める
					backup_data.nowStationNo++;
					if( backup_data.nowStationNo>=MAX_STATION ){
						backup_data.nowStationNo--;
					}
					if( route.station[backup_data.nowStationNo].stationCode==0 ){
						// 最終までに到達したときは、最終駅でとめる
						backup_data.nowStationNo--;
					}

						// データがあれば、ＭＣＰ表示、
					dvaMCPDisplaySet( 1, route.station[backup_data.nowStationNo].mcpStationChar );
					DUALPORT.tms_answer.dvaStatus.BIT.autoMode = 1;

				}else{

					if( route.station[backup_data.nowStationNo].stationCode!=0 ){
						DUALPORT.mcpMode.REPEnable = 1;

							// 放送を開始
						backup_data.nowStationName = route.station[backup_data.nowStationNo].stationCode;
						DUALPORT.ev5000_request.SentenceNo = SENTENCE_STATION;
						DUALPORT.ev5000_request.Sequence = route.station[backup_data.nowStationNo].sequence;
						DUALPORT.ev5000_request.Complete = 0;
						DUALPORT.ev5000_request.Request = 1;
						TraceAdd( EVENT_STN );

						// データがあれば、ＥＩＤＳ表示、
						DUALPORT.eids.Route = route.eidsRouteChar;
						DUALPORT.eids.Station = route.station[backup_data.nowStationNo].eidsStationChar;
						if( DUALPORT.eids.Station ){
							DUALPORT.eids.sendreq = 1;
						}

						busy_STNon = 1;
						DUALPORT.mn_stn_busy = 1;
						DUALPORT.mn_nextStation = backup_data.nowStationName;
						stationData = dvaSearchDestination(backup_data.nowRouteCode);
						DUALPORT.mn_destination = stationData->stationCode;
						DUALPORT.mn_routeCode = backup_data.nowRouteCode;

					}else{
						DUALPORT.mcpMode.STNOn = 0;
						busy_STNon = 0;
						DUALPORT.mn_stn_busy = 0;
					}
				}

			}
		}

			// ＴＭＳからの受信情報を受け付ける
		if( DUALPORT.tms_request.Active ){		// ＴＭＳ有効
			if( DUALPORT.tms_request.testStartRequest1 ){
				// ＴＭＳからのテスト要求
				backup_data.opeMode = OPEMODE_TEST;
			}
			if( DUALPORT.tms_request.testStartRequest2 ){
				// ＴＭＳからのテスト要求
				backup_data.opeMode = OPEMODE_TEST;
			}

			DUALPORT.mcpMode.ATCEnable = 1;
			DUALPORT.mcpMode.ATCOnEnable = 0;

			dvaIOOut( OUTSIGNAL_LED2|OUTSIGNAL_LED3, 0xFF );
		}
		else {									// ＴＭＳ無効
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.ATCOnEnable = 0;

			dvaIOOut( OUTSIGNAL_LED2, 0xFF );
		}

		// ドア接点入力の受け付け
												// ボタン判定処理
		if( onSignal.SIGNAL1.BYTE ||
					pend_DR_Opn_A_Chime ||
					pend_DR_Cls_A_Chime ||
					pend_DR_Opn_B_Chime ||
					pend_DR_Cls_B_Chime 	){

			if( DUALPORT.ev5000_request.Complete && DUALPORT.eids.ResetStatus==0 ){
				// 放送が終わっていれば受け付ける	ＥＩＤＳのリセット出力がないとき
				// ドアＡのチャイム
				if( onSignal.SIGNAL1.RBIT.doorOpenAChime || pend_DR_Opn_A_Chime ){
					memset( Sequence, 0, sizeof(Sequence) );
					Sequence[0] = config_data.chime_sequence;
					Sequence[0].sendRequest.sendCode = 0;
					DUALPORT.ev5000_request.SentenceNo = config_data.chime_sequence.sendRequest.sendCode;
					DUALPORT.ev5000_request.Sequence = Sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 1;

					dvaMCPDisplaySet( 2, "CHIME " );

					TraceAdd( EVENT_DOOR_OPENACHIME );

					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}
				if( onSignal.SIGNAL1.RBIT.doorCloseAChime || pend_DR_Cls_A_Chime ){
					memset( Sequence, 0, sizeof(Sequence) );
					Sequence[0] = config_data.chime_sequence;
					Sequence[0].sendRequest.sendCode = 0;
					DUALPORT.ev5000_request.SentenceNo = config_data.chime_sequence.sendRequest.sendCode;
					DUALPORT.ev5000_request.Sequence = Sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 1;

					dvaMCPDisplaySet( 2, "CHIME " );

					TraceAdd( EVENT_DOOR_CLOSEACHIME );

					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}
				if( onSignal.SIGNAL1.RBIT.doorCloseAAnnounce ){
					DUALPORT.ev5000_request.SentenceNo = SENTENCE_DOOR;
					DUALPORT.ev5000_request.Sequence = announce_data.door[DOOR_CLOSE_A].announce_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorMode = 1;

					dvaMCPDisplaySet( 2, "DCA-A " );

					TraceAdd( EVENT_DOOR_CLOSEAANNOUNCE );
				}
				if( onSignal.SIGNAL1.RBIT.doorOpenBChime || pend_DR_Opn_B_Chime ){
					memset( Sequence, 0, sizeof(Sequence) );
					Sequence[0] = config_data.chime_sequence;
					Sequence[0].sendRequest.sendCode = 0;
					DUALPORT.ev5000_request.SentenceNo = config_data.chime_sequence.sendRequest.sendCode;
					DUALPORT.ev5000_request.Sequence = Sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 1;

					dvaMCPDisplaySet( 2, "CHIME " );

					TraceAdd( EVENT_DOOR_OPENBCHIME );

					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}
				if( onSignal.SIGNAL1.RBIT.doorCloseBChime || pend_DR_Cls_B_Chime ){
					memset( Sequence, 0, sizeof(Sequence) );
					Sequence[0] = config_data.chime_sequence;
					Sequence[0].sendRequest.sendCode = 0;
					DUALPORT.ev5000_request.SentenceNo = config_data.chime_sequence.sendRequest.sendCode;
					DUALPORT.ev5000_request.Sequence = Sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 1;

					dvaMCPDisplaySet( 2, "CHIME " );

					TraceAdd( EVENT_DOOR_CLOSEBCHIME );

					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}
				if( onSignal.SIGNAL1.RBIT.doorCloseBAnnounce ){
					DUALPORT.ev5000_request.SentenceNo = SENTENCE_DOOR;
					DUALPORT.ev5000_request.Sequence = announce_data.door[DOOR_CLOSE_B].announce_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorMode = 1;

					dvaMCPDisplaySet( 2, "DCA-B " );

					TraceAdd( EVENT_DOOR_CLOSEBANNOUNCE );
				}
			}else{
												// ドアチャイムボタンペンディング処理
				if( DUALPORT.ev5000_request.SentenceNo == SENTENCE_EMERGENCY ||
					DUALPORT.ev5000_request.SentenceNo == SENTENCE_SPECIAL 		){
					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}else{
					pend_DR_Opn_A_Chime |= onSignal.SIGNAL1.RBIT.doorOpenAChime;
					pend_DR_Cls_A_Chime |= onSignal.SIGNAL1.RBIT.doorCloseAChime;
					pend_DR_Opn_B_Chime |= onSignal.SIGNAL1.RBIT.doorOpenBChime;
					pend_DR_Cls_B_Chime |= onSignal.SIGNAL1.RBIT.doorCloseBChime;
				}
			}
			onSignal.SIGNAL1.BYTE = 0;
		}
		else {
			if( DUALPORT.ev5000_request.Complete ){
				if( DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode
					|| DUALPORT.tms_answer.dvaStatus.BIT.doorMode ){
					dvaMCPDisplaySet( 2, "      " );
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 0;
					DUALPORT.tms_answer.dvaStatus.BIT.doorMode = 0;
				}
			}
		}

		if( DUALPORT.ev5000_request.Complete ){
			DUALPORT.tms_answer.dvaStatus.BIT.autoMode = 0;
		}
}

/************************************************************************

	Function name	<< dvaAUTO >>
					自動放送シーケンス処理
	Syntax			dvaAUTO( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	スピーカー制御
				a02 2001/07/02  自動放送モードへ戻るフラグのセット
				a03 2001/07/10  EIDS Door Status is TMS Data
								EIDS Reset 信号がアクティブのときは、
								ＥＶ５０００の放送中と同じく、ＲＥＰ，ＳＴＮは機能しない
				a04 2003/04/01	自動放送後、ＭＣＰ表示のみさらに次駅へ進める。
								ドアチャイムペンディング処理追加
				a05 2003/05/29 h.nishiura ＴＭＳへの送信情報の準備を無条件実行に変更
				a06 2013/05/10 h.nishiura 到着案内放送の起動(Extra timing)の追加
				a07 2014/02/17 h.nishiura ATCアクティブ信号は放送中は無視に変更
				a08 2021/02/07 h.nishiura RTTF対応
*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		ＴＭＳからの指示による自動放送を行う

	------------------------------------------------------------------[@@@E]*/
void dvaAUTO( void )
{
		int		i;
		struct ROUTEINFO *routeData;
		struct STATIONINFO *stationData;
		union st_READSignal2 signal2;
		unsigned char tmp_nowStationNo;

			// ＴＭＳへの送信情報の準備
		for( i=0; i<8; i++ ){
			DUALPORT.tms_answer.passengerAlarm[i] = DUALPORT.answerData.rscSW[i].BYTE;
		}


			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
			befOpeMode = 0;
		}

//			// 列車無線放送中は、停止するのを待つ
//		signal2.BYTE = nowSignal.SIGNAL2.BYTE;
//		signal2.BYTE |= DUALPORT.answerData.dvau2;
//		if( DUALPORT.mcpMode.trsBUSY ){
//			if( signal2.RBIT.trsRequest ){
//				return;
//			}
//			// 列車無線停止時は、復帰させる
//			DUALPORT.mcpMode.trsBUSY = 0;
//			// 自動放送をキャンセル要求を解除
//			DUALPORT.ev5000_request.Abort = 0;
//			befOpeMode = 0;
//		}


		if( befOpeMode != OPEMODE_AUTO ){
			befOpeMode = OPEMODE_AUTO;

			dvaMCPInitial();

			TraceAdd( EVENT_AUTO );

			DUALPORT.activeDVAU=1;		// アクティブＤＶＡＵモード

			DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// 自動放送モード

			dvaIOOut(
				OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
				| OUTSIGNAL_trsDisable			// 列車無線起動停止
				,
				OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				| OUTSIGNAL_audioBusDisable		// 車内放送許可
				);

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCOnEnable = 1;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 1;

				// ドア接点情報のクリア
			onSignal.SIGNAL1.BYTE = 0;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			DUALPORT.ev5000_request.Repeat = 0;

			busy_AUTOon = 0;			// 遅延ＭＣＰ次駅表示用
		}

		ReturnAutoMode = 1; 		// 2001/07/02 自動放送モードに戻る
		backup_data.rootOpeMode = 0;

			// 列車無線起動の確認
		signal2.BYTE = nowSignal.SIGNAL2.BYTE;
		signal2.BYTE |= DUALPORT.answerData.dvau2;
		if( signal2.RBIT.trsRequest ){
			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_TRS;
			return;
		}

			// ＭＣＰからの自動放送要求
		if( DUALPORT.mcpMode.ATCOn ){
			DUALPORT.mcpMode.ATCOn = 0;
		}

			// ＭＣＰからの自動放送解除
		if( DUALPORT.mcpMode.ATCOff ){
			DUALPORT.mcpMode.ATCOff = 0;
			backup_data.opeMode = OPEMODE_IDLE;
		}

			// ＭＣＰからの非常通報器受け付け
		if( DUALPORT.mcpMode.EMGPOn ){
			DUALPORT.mcpMode.EMGPOn = 0;
			for( i=0; i<16; i++ ){
				backup_data.nowEMGHL[i] = DUALPORT.mcpMode.EMGPHL[i];
//				DUALPORT.mcpMode.EMGPHL[i] = 0;
			}
			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_EMGHL;
		}

			// ＭＣＰからのリピート放送要求
		if( DUALPORT.mcpMode.REPOn ){
			DUALPORT.mcpMode.REPOn = 0;

			if( DUALPORT.ev5000_request.Complete && DUALPORT.eids.ResetStatus==0 ){
				// 放送が終わっていれば受け付ける	ＥＩＤＳのリセット出力がないとき

					// ルートコードと駅名で検索する
				tmp_nowStationNo = backup_data.nowStationNo;
				stationData = dvaSearchStation( backup_data.nowRouteCode, backup_data.nowStationName );
				backup_data.nowStationNo = tmp_nowStationNo;

					// 放送を開始
				DUALPORT.ev5000_request.SentenceNo = SENTENCE_STATION;
				DUALPORT.ev5000_request.Sequence = stationData->sequence;
				DUALPORT.ev5000_request.Complete = 0;
				DUALPORT.ev5000_request.Request = 1;
				DUALPORT.ev5000_request.Repeat = 1;
				TraceAdd( EVENT_REPEAT );

//				DUALPORT.eids.Route = route.eidsRouteChar;
//				DUALPORT.eids.Station = route.station[backup_data.nowStationNo].eidsStationChar;
//				if( DUALPORT.eids.Station ){
//					DUALPORT.eids.sendreq = 1;
//				}

//				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=1;
				dvaMCPDisplaySet( 0, "  REP " );
				DUALPORT.tms_answer.dvaStatus.BIT.autoMode = 1;
			}
		}
		if( DUALPORT.ev5000_request.Repeat ){
			if( DUALPORT.ev5000_request.Complete && DUALPORT.eids.ResetStatus==0 ){
				// 放送が終わっていれば受け付ける	ＥＩＤＳのリセット出力がないとき
				DUALPORT.ev5000_request.Repeat = 0;

//				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;

				dvaMCPInitial();
			}
		}

			// ＴＭＳからの受信情報を受け付ける
		if( DUALPORT.tms_request.Active ){			// ＴＭＳ有効
			if( DUALPORT.tms_request.testStartRequest1 ){
				// ＴＭＳからのテスト要求
				backup_data.rootOpeMode = OPEMODE_AUTO;
				backup_data.opeMode = OPEMODE_TEST;
			}
			if( DUALPORT.tms_request.testStartRequest2 ){
				// ＴＭＳからのテスト要求
				backup_data.rootOpeMode = OPEMODE_AUTO;
				backup_data.opeMode = OPEMODE_TEST;
			}

			// 非常通報器選択を受け付ける
			for( i=0; i<MAX_PAA; i++ ){
				if( DUALPORT.tms_request.talkBack[i] ){
					backup_data.rootOpeMode = OPEMODE_AUTO;
					backup_data.opeMode = OPEMODE_EMGHL;
				}
			}

			// 放送禁止の処置
			if( DUALPORT.tms_request.broadCastOff || DUALPORT.eids.ResetStatus ){
					// EIDS のリセット中は放送しない
				DUALPORT.ev5000_request.Abort = 1;

				DUALPORT.tms_request.change_dbEffective=0;
				DUALPORT.tms_request.change_atcActive=0;
				DUALPORT.tms_request.change_nextStationName=0;
				DUALPORT.tms_request.change_destination=0;
				DUALPORT.tms_request.change_routeCode=0;
			}

			// 行き先案内の表示
			if( DUALPORT.tms_request.change_routeCode
				|| DUALPORT.tms_request.change_nextStationName ){
				DUALPORT.tms_request.change_routeCode = 0;
				DUALPORT.tms_request.change_nextStationName = 0;

				BroadCast_count = 0;

				routeData = dvaSearchRoute( DUALPORT.tms_request.routeCode );
				if( routeData!=NULL ){
					backup_data.nowRouteCode = DUALPORT.tms_request.routeCode;
					if( DUALPORT.mcpMode.INUSE==0 ){
						dvaMCPDisplaySet( 0, routeData->mcpRouteChar );
					}

					// 次駅案内表示
					// ルートコードと駅名で検索する
					stationData = dvaSearchStation( backup_data.nowRouteCode, DUALPORT.tms_request.nextStationName );
					if( stationData!=NULL ){
						backup_data.nowStationName = DUALPORT.tms_request.nextStationName;
						// データがあれば、ＭＣＰとＥＩＤＳ表示、
						dvaMCPDisplaySet( 1, stationData->mcpStationChar );
						DUALPORT.eids.Route = routeData->eidsRouteChar;
						DUALPORT.eids.Station = stationData->eidsStationChar;
					}
				}
			}

			// 自動放送の起動チェック
			if( DUALPORT.tms_request.change_dbEffective ){
					// EIDS のリセット中は放送しない
				DUALPORT.tms_request.change_dbEffective = 0;
				if( DUALPORT.tms_request.dbEffective ){

					if( BroadCast_count == 1 ){

						// 到着案内放送の起動(Extra timing)
						routeData = dvaSearchRoute( backup_data.nowRouteCode );

						if( routeData!=NULL ){

							if( DUALPORT.mcpMode.INUSE==0 ){
								dvaMCPDisplaySet( 0, routeData->mcpRouteChar );
							}
							// ルートとステーションナンバーで検索
							stationData = dvaSearchStationNumberExt( backup_data.nowRouteCode, DUALPORT.tms_request.nextStationName );
							if( stationData!=NULL ){

								if(stationData->Extra==1)
								{
									backup_data.stationNumber = DUALPORT.tms_request.stationNumber;

										// データがあれば、駅名コードを再度設定（Arrived 放送のため）
									backup_data.nowStationName = stationData->stationCode;

									// データがあれば、ＭＣＰとＥＩＤＳ表示、
									dvaMCPDisplaySet( 1, stationData->mcpStationChar );
									DUALPORT.eids.Route = routeData->eidsRouteChar;
									DUALPORT.eids.Station = stationData->eidsStationChar;
									if( DUALPORT.eids.Station ){
										DUALPORT.eids.sendreq = 1;
									}

									// 放送を開始
									DUALPORT.ev5000_request.SentenceNo = SENTENCE_STATION;
									DUALPORT.ev5000_request.Sequence = stationData->sequence;
									DUALPORT.ev5000_request.Complete = 0;
									DUALPORT.ev5000_request.Abort = 1;
									DUALPORT.ev5000_request.Request = 1;

									BroadCast_count++;

									DUALPORT.tms_answer.dvaStatus.BIT.autoMode = 1;

									TraceAdd( EVENT_TMS_STN );
								}
							}
						}

					}else{

						routeData = dvaSearchRoute( DUALPORT.tms_request.routeCode );
						if( routeData!=NULL ){
							backup_data.nowRouteCode = DUALPORT.tms_request.routeCode;

							if( DUALPORT.mcpMode.INUSE==0 ){
								dvaMCPDisplaySet( 0, routeData->mcpRouteChar );
							}

							if( BroadCast_count == 0 ){

								// 次駅案内放送の起動
								stationData = dvaSearchStation( backup_data.nowRouteCode, DUALPORT.tms_request.nextStationName );
								if( stationData!=NULL ){
									backup_data.nowStationName = DUALPORT.tms_request.nextStationName;

									// データがあれば、ＭＣＰとＥＩＤＳ表示、
									dvaMCPDisplaySet( 1, stationData->mcpStationChar );
									// 放送を開始
									DUALPORT.ev5000_request.SentenceNo = SENTENCE_STATION;
									DUALPORT.ev5000_request.Sequence = stationData->sequence;
									DUALPORT.ev5000_request.Complete = 0;
									DUALPORT.ev5000_request.Abort = 1;
									DUALPORT.ev5000_request.Request = 1;

									BroadCast_count++;

									DUALPORT.tms_answer.dvaStatus.BIT.autoMode = 1;

									DUALPORT.eids.Route = routeData->eidsRouteChar;
									DUALPORT.eids.Station = stationData->eidsStationChar;
									if( DUALPORT.eids.Station ){
										DUALPORT.eids.sendreq = 1;
									}
									TraceAdd( EVENT_TMS_NEXT );
									busy_AUTOon = 1;
								}else{
									busy_AUTOon = 0;
								}
							}
						}else{
							busy_AUTOon = 0;
						}
					}
				}
			}

			if( busy_AUTOon == 1 ){
				/* 自動放送終了後 */
				if( DUALPORT.ev5000_request.Complete && DUALPORT.eids.ResetStatus==0 ){
					// 次駅を求める
					backup_data.nowStationNo++;
					if( backup_data.nowStationNo>=MAX_STATION ){
						backup_data.nowStationNo--;
					}
					if( route.station[backup_data.nowStationNo].stationCode==0 ){
						// 最終までに到達したときは、最終駅でとめる
						backup_data.nowStationNo--;
					}

					// データがあれば、ＭＣＰ表示、
					dvaMCPDisplaySet( 1, route.station[backup_data.nowStationNo].mcpStationChar );

					busy_AUTOon = 0;
				}
			}

				// 放送起動フラグがＯＮになったとき
			if( DUALPORT.tms_request.change_atcActive ){
					// EIDS のリセット中は放送しない
				DUALPORT.tms_request.change_atcActive = 0;
				if( busy_AUTOon == 0 ) // 2014/2/17 ATCアクティブの発生タイミングにより、放送中にもかかわらず、
				{					   //           MCPの表示が進行してしまう動作が現地で発生したため、
									   //			放送中はATCアクティブは見ないように変更。
					if( DUALPORT.tms_request.atcActive ){
						if( DUALPORT.tms_request.specialCode>0x70 && DUALPORT.tms_request.specialCode<0x80 ){
							routeData = dvaSearchRoute( backup_data.nowRouteCode );
							if( routeData!=NULL ){
								if( DUALPORT.mcpMode.INUSE==0 ){
									dvaMCPDisplaySet( 0, routeData->mcpRouteChar );
								}
								// ルートとステーションナンバーで検索
								stationData = &routeData->station[backup_data.nowStationNo];
								// データがあれば、ＭＣＰとＥＩＤＳ表示、
								dvaMCPDisplaySet( 1, stationData->mcpStationChar );
								DUALPORT.eids.Route = routeData->eidsRouteChar;
								DUALPORT.eids.Station = stationData->eidsStationChar;
							}

							// スペシャルメッセージ放送の起動
							backup_data.nowSPCCode = DUALPORT.tms_request.specialCode - 0x71 + ATC_SPCMA;
							DUALPORT.nowSPCCode = backup_data.nowSPCCode;
							backup_data.rootOpeMode = OPEMODE_AUTO;
							backup_data.opeMode = OPEMODE_SPCMA;

							TraceAdd( EVENT_TMS_SPC );
							DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=1;

							// 放送が終わっていれば受け付ける
							// 放送起動
							DUALPORT.ev5000_request.SentenceNo = SENTENCE_SPECIAL;
							DUALPORT.ev5000_request.Sequence = announce_data.spcma[backup_data.nowSPCCode].sequence;
							DUALPORT.ev5000_request.Complete = 0;
							DUALPORT.ev5000_request.Abort = 1;
							DUALPORT.ev5000_request.Request = 1;

							DUALPORT.eids.Message = announce_data.spcma[backup_data.nowSPCCode].eidsChar;
							DUALPORT.eids.sendreq = 1;

							DUALPORT.tms_answer.dvaStatus.BIT.specialMode = 1;

							TraceAdd( EVENT_SPC );
						}
						else {
							// 到着案内放送の起動

							routeData = dvaSearchRoute( DUALPORT.tms_request.routeCode );
							if( routeData!=NULL ){
								backup_data.nowRouteCode = DUALPORT.tms_request.routeCode;

								if( DUALPORT.mcpMode.INUSE==0 ){
									dvaMCPDisplaySet( 0, routeData->mcpRouteChar );
								}
								// ルートとステーションナンバーで検索
								stationData = dvaSearchStationNumber( backup_data.nowRouteCode, DUALPORT.tms_request.stationNumber );
								if( stationData!=NULL ){

									backup_data.stationNumber = DUALPORT.tms_request.stationNumber;
									if(stationData->Extra!=1)
									{
											// データがあれば、駅名コードを再度設定（Arrived 放送のため）
										backup_data.nowStationName = stationData->stationCode;

										// データがあれば、ＭＣＰとＥＩＤＳ表示、
										dvaMCPDisplaySet( 1, stationData->mcpStationChar );
										DUALPORT.eids.Route = routeData->eidsRouteChar;
										DUALPORT.eids.Station = stationData->eidsStationChar;
										if( DUALPORT.eids.Station ){
											DUALPORT.eids.sendreq = 1;
										}

										// 放送を開始
										DUALPORT.ev5000_request.SentenceNo = SENTENCE_STATION;
										DUALPORT.ev5000_request.Sequence = stationData->sequence;
										DUALPORT.ev5000_request.Complete = 0;
										DUALPORT.ev5000_request.Abort = 1;
										DUALPORT.ev5000_request.Request = 1;

										DUALPORT.tms_answer.dvaStatus.BIT.autoMode = 1;

										TraceAdd( EVENT_TMS_STN );
									}
								}
							}
						}
					}
				}
			}

			DUALPORT.eids.Door = DUALPORT.tms_request.doorStatus;	// 2001/07/10 EIDS のドア状態は　ＴＭＳに従う

			DUALPORT.mcpMode.ATCEnable = 1;		// ＡＴＣキー受け付け許可
			DUALPORT.mcpMode.STNEnable = 0;		// ＳＴＮキー受け付け禁止
			DUALPORT.mcpMode.REPEnable = 1;		// ＲＥＰキー受け付け許可

			dvaIOOut( OUTSIGNAL_LED3|OUTSIGNAL_LED0, 0xFF );
		}
		else {
			DUALPORT.mcpMode.ATCEnable = 1;		// ＡＴＣキー受け付け許可
			DUALPORT.mcpMode.STNEnable = 1;		// ＳＴＮキー受け付け許可
			DUALPORT.mcpMode.REPEnable = 1;		// ＲＥＰキー受け付け許可

			DUALPORT.eids.Door = 1;				// 2001/07/10 EIDS のドア状態はクローズ
			dvaIOOut( OUTSIGNAL_LED3, 0xFF );
		}


			// ドア接点入力の受け付け
		if( onSignal.SIGNAL1.BYTE ||
					pend_DR_Opn_A_Chime ||
					pend_DR_Cls_A_Chime ||
					pend_DR_Opn_B_Chime ||
					pend_DR_Cls_B_Chime 	){

			if( DUALPORT.ev5000_request.Complete && DUALPORT.eids.ResetStatus==0 ){
				// 放送が終わっていれば受け付ける	ＥＩＤＳのリセット出力がないとき
				// ドアＡのチャイム
				if( onSignal.SIGNAL1.RBIT.doorOpenAChime || pend_DR_Opn_A_Chime ){
					memset( Sequence, 0, sizeof(Sequence) );
					Sequence[0] = config_data.chime_sequence;
					Sequence[0].sendRequest.sendCode = 0;
					DUALPORT.ev5000_request.SentenceNo = config_data.chime_sequence.sendRequest.sendCode;
					DUALPORT.ev5000_request.Sequence = Sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 1;

					dvaMCPDisplaySet( 2, "CHIME " );

					TraceAdd( EVENT_DOOR_OPENACHIME );

					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}
				if( onSignal.SIGNAL1.RBIT.doorCloseAChime || pend_DR_Cls_A_Chime ){
					memset( Sequence, 0, sizeof(Sequence) );
					Sequence[0] = config_data.chime_sequence;
					Sequence[0].sendRequest.sendCode = 0;
					DUALPORT.ev5000_request.SentenceNo = config_data.chime_sequence.sendRequest.sendCode;
					DUALPORT.ev5000_request.Sequence = Sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 1;

					dvaMCPDisplaySet( 2, "CHIME " );

					TraceAdd( EVENT_DOOR_CLOSEACHIME );

					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}
				if( onSignal.SIGNAL1.RBIT.doorCloseAAnnounce ){
					DUALPORT.ev5000_request.SentenceNo = SENTENCE_DOOR;
					DUALPORT.ev5000_request.Sequence = announce_data.door[DOOR_CLOSE_A].announce_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorMode = 1;

					dvaMCPDisplaySet( 2, "DCA-A " );

					TraceAdd( EVENT_DOOR_CLOSEAANNOUNCE );
				}
				if( onSignal.SIGNAL1.RBIT.doorOpenBChime || pend_DR_Opn_B_Chime ){
					memset( Sequence, 0, sizeof(Sequence) );
					Sequence[0] = config_data.chime_sequence;
					Sequence[0].sendRequest.sendCode = 0;
					DUALPORT.ev5000_request.SentenceNo = config_data.chime_sequence.sendRequest.sendCode;
					DUALPORT.ev5000_request.Sequence = Sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 1;

					dvaMCPDisplaySet( 2, "CHIME " );

					TraceAdd( EVENT_DOOR_OPENBCHIME );

					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}
				if( onSignal.SIGNAL1.RBIT.doorCloseBChime || pend_DR_Cls_B_Chime ){
					memset( Sequence, 0, sizeof(Sequence) );
					Sequence[0] = config_data.chime_sequence;
					Sequence[0].sendRequest.sendCode = 0;
					DUALPORT.ev5000_request.SentenceNo = config_data.chime_sequence.sendRequest.sendCode;
					DUALPORT.ev5000_request.Sequence = Sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 1;

					dvaMCPDisplaySet( 2, "CHIME " );

					TraceAdd( EVENT_DOOR_CLOSEBCHIME );

					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}
				if( onSignal.SIGNAL1.RBIT.doorCloseBAnnounce ){
					DUALPORT.ev5000_request.SentenceNo = SENTENCE_DOOR;
					DUALPORT.ev5000_request.Sequence = announce_data.door[DOOR_CLOSE_B].announce_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					DUALPORT.tms_answer.dvaStatus.BIT.doorMode = 1;

					dvaMCPDisplaySet( 2, "DCA-B " );

					TraceAdd( EVENT_DOOR_CLOSEBANNOUNCE );
				}
			}else{
												// ドアチャイムボタンペンディング処理
				if( DUALPORT.ev5000_request.SentenceNo == SENTENCE_EMERGENCY ||
					DUALPORT.ev5000_request.SentenceNo == SENTENCE_SPECIAL 		){
					pend_DR_Opn_A_Chime = 0;	// ドアチャイムペンディング情報クリア
					pend_DR_Cls_A_Chime = 0;	//
					pend_DR_Opn_B_Chime = 0;	//
					pend_DR_Cls_B_Chime = 0;	//
				}else{
					pend_DR_Opn_A_Chime |= onSignal.SIGNAL1.RBIT.doorOpenAChime;
					pend_DR_Cls_A_Chime |= onSignal.SIGNAL1.RBIT.doorCloseAChime;
					pend_DR_Opn_B_Chime |= onSignal.SIGNAL1.RBIT.doorOpenBChime;
					pend_DR_Cls_B_Chime |= onSignal.SIGNAL1.RBIT.doorCloseBChime;
				}
			}
			onSignal.SIGNAL1.BYTE = 0;
		}
		else {
			if( DUALPORT.ev5000_request.Complete ){
				if( DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode
					|| DUALPORT.tms_answer.dvaStatus.BIT.doorMode ){
					dvaMCPDisplaySet( 2, "      " );
					DUALPORT.tms_answer.dvaStatus.BIT.doorChimeMode = 0;
					DUALPORT.tms_answer.dvaStatus.BIT.doorMode = 0;
				}
			}
		}

		if( DUALPORT.ev5000_request.Complete ){
			DUALPORT.tms_answer.dvaStatus.BIT.autoMode = 0;
		}
}

/************************************************************************

	Function name	<< dvaTRS >>
					列車無線放送シーケンス処理
	Syntax			dvaTRS( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	スピーカー制御
						ＡＴＣスイッチの入力許可
					a02 2001/07/11 スペシャル放送中にＴＲＳが
							入ると、rootOpeMode が IDLE になっているため
							ＡＴＣモードに戻らない
							ReturnAutoMode フラグを利用する
					a03 2003/05/28  h.nishiura ＴＭＳへの送信情報の準備追加
					a04 2003/05/29 h.nihiura ＴＭＳへの送信情報の準備を無条件実行に変更

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		列車無線放送要求時に呼び出される

	------------------------------------------------------------------[@@@E]*/
void dvaTRS( void )
{
		int		i;
		union st_READSignal2 signal2;

			// ＴＭＳへの送信情報の準備
		for( i=0; i<8; i++ ){
			DUALPORT.tms_answer.passengerAlarm[i] = DUALPORT.answerData.rscSW[i].BYTE;
		}

			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
			befOpeMode = 0;
		}

		if( befOpeMode != OPEMODE_TRS ){
			befOpeMode = OPEMODE_TRS;

			TraceAdd( EVENT_TRS );

			DUALPORT.activeDVAU=1;		// アクティブＤＶＡＵモード

			DUALPORT.pollingData.voiceBus.BIT.mode = 3;	// 音声バス使用状況は列車無線放送

			// ＥＶ５０００へ放送キャンセルを指示
			DUALPORT.ev5000_request.Abort = 1;

			dvaIOOut(
				OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				,
				OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				| OUTSIGNAL_audioBusDisable		// 車内放送許可
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＦＦ
				| OUTSIGNAL_trsDisable			// 列車無線放送許可
				);

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCEnable = 1;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 1;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			DUALPORT.ev5000_request.Repeat = 0;

			DUALPORT.mcpMode.trsBUSY=1;		// 列車無線放送中

//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[0] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[1] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[2] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[0] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[1] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[2] = 0;		// MCP Switch OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BYTE = 0;	// Switch LED OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BYTE = 0;	// Switch LED Blink OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[0]=0x00;	// Matrix LED Blink OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0x00;	// Matrix LED Blink OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;	// Matrix LED Blink OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0x00;	// Matrix LED Blink OFF
		}

			// ＭＣＰからの自動放送要求
		if( DUALPORT.mcpMode.ATCOn ){
			DUALPORT.mcpMode.ATCOn = 0;
		}

			// ＭＣＰからの自動放送解除
		if( DUALPORT.mcpMode.ATCOff ){
			DUALPORT.mcpMode.ATCOff = 0;
			backup_data.opeMode = OPEMODE_IDLE;
		}

			// ＭＣＰからの非常放送受け付け
		if( DUALPORT.mcpMode.EMGOn ){
			DUALPORT.mcpMode.EMGOn = 0;

			backup_data.nowEMGCode = DUALPORT.mcpMode.EMGCode;
			DUALPORT.nowEMGCode = backup_data.nowEMGCode;
			backup_data.opeMode = OPEMODE_EMGMA;
		}

			// ＭＣＰからの非常通報器受け付け
		if( DUALPORT.mcpMode.EMGPOn ){
			DUALPORT.mcpMode.EMGPOn = 0;
			for( i=0; i<16; i++ ){
				backup_data.nowEMGHL[i] = DUALPORT.mcpMode.EMGPHL[i];
//				DUALPORT.mcpMode.EMGPHL[i] = 0;
			}
			backup_data.opeMode = OPEMODE_EMGHL;
		}

			// 列車無線放送中は、停止するのを待つ
		signal2.BYTE = nowSignal.SIGNAL2.BYTE;
		if( signal2.RBIT.trsRequest ){		/* Front Active */
			if( DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError1==0
				&& DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError2==0 ){	/* Rear Normal */
				signal2.BYTE = DUALPORT.answerData.dvau2;
				if( signal2.RBIT.trsRequest ){	/* Rear Active */
					dvaIOOut(
						OUTSIGNAL_trsDisable			// 列車無線放送禁止
						,
						0
					);
					return;
				}

					/* Fron Only */
				dvaIOOut(
					0
					,
					OUTSIGNAL_trsDisable			// 列車無線放送許可
				);
				return;
			}

			/* Front Only */
			dvaIOOut(
				0
				,
				OUTSIGNAL_trsDisable			// 列車無線放送許可
			);

			return;
		}

		if( DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError1==0
			&& DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError2==0 ){	/* Rear Normal */
			signal2.BYTE = DUALPORT.answerData.dvau2;
			if( signal2.RBIT.trsRequest ){	/* Rear Active */
				dvaIOOut(
					OUTSIGNAL_trsDisable			// 列車無線放送禁止
					,
					0
				);
				return;
			}
		}

		// 列車無線停止時は、復帰させる
		DUALPORT.mcpMode.trsBUSY = 0;
		// 自動放送をキャンセル要求を解除
		DUALPORT.ev5000_request.Abort = 0;

		// 呼出元へ復帰する
		if( ReturnAutoMode ){		// 自動放送モードへ戻る処理
			backup_data.opeMode = OPEMODE_AUTO;
		}
		else {
			backup_data.opeMode = OPEMODE_IDLE;
		}
//		backup_data.opeMode = backup_data.rootOpeMode;


		dvaIOOut( OUTSIGNAL_LED5, 0xFF );
}

/************************************************************************

	Function name	<< dvaEMGMA >>
					非常放送シーケンス処理
	Syntax			dvaEMGMA( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
	Revision		a01 2000/10/10	放送連続処理
					a02 2001/03/22	スピーカー制御
					a03 2001/07/11  くり返し時のＥＩＤＳへの送信
					a04 2003/05/28  h.nishiura ＴＭＳへの送信情報の準備追加
					a05 2003/05/29  h.nihiura ＴＭＳへの送信情報の準備を無条件実行に変更

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		非常放送処理を行う
		放送は連続して行う

	------------------------------------------------------------------[@@@E]*/
void dvaEMGMA( void )
{
		int i;

			// ＴＭＳへの送信情報の準備
		for( i=0; i<8; i++ ){
			DUALPORT.tms_answer.passengerAlarm[i] = DUALPORT.answerData.rscSW[i].BYTE;
		}

			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
			befOpeMode = 0;
		}

		if( befOpeMode != OPEMODE_EMGMA ){
			befOpeMode = OPEMODE_EMGMA;

			TraceAdd( EVENT_EMGMA );

			DUALPORT.activeDVAU=1;		// アクティブＤＶＡＵモード

			DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// 自動放送モード

			dvaIOOut(
				OUTSIGNAL_trsDisable			// 列車無線停止中
				| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
				,
				OUTSIGNAL_audioBusDisable		// 車内放送許可
				| OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				);

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCEnable = 1;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 1;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			dvaMCPDisplaySet( 0, "EMG-MA" );
			DUALPORT.mcpMode.INUSE = 1;
			dvaMCPDisplaySet( 1, announce_data.emgma[backup_data.nowEMGCode].mcpChar2 );
			dvaMCPDisplaySet( 2, announce_data.emgma[backup_data.nowEMGCode].mcpChar3 );

//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[0] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[1] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[2] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[0] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[1] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[2] = 0;		// MCP Switch OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BYTE = 0;	// Switch LED OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BYTE = 0;	// Switch LED Blink OFF
		}

				// 放送が終わっていれば受け付ける
		if( DUALPORT.ev5000_request.Complete ){
			// ＥＶ５０００からの放送が終了したら、繰り返す
			if( DUALPORT.mcpMode.EMGRepeat && DUALPORT.eids.ResetStatus==0 ){
					// 途中で、マイク放送が入った場合の考慮
				DUALPORT.ev5000_request.SentenceNo = SENTENCE_EMERGENCY;
				DUALPORT.ev5000_request.Sequence = announce_data.emgma[backup_data.nowEMGCode].sequence;
				DUALPORT.ev5000_request.Complete = 0;
				DUALPORT.ev5000_request.Abort = 0;
				DUALPORT.ev5000_request.Request = 1;
				DUALPORT.tms_answer.dvaStatus.BYTE = 0;
				DUALPORT.tms_answer.dvaStatus.BIT.emgMode = 1;

				DUALPORT.eids.Message = announce_data.emgma[backup_data.nowEMGCode].eidsChar;
				DUALPORT.eids.sendreq = 1;			// 2001/07/11 add

				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=1;
			}

			// 放送要求
			if( DUALPORT.mcpMode.EMGOn ){
				DUALPORT.mcpMode.EMGOn = 0;

				backup_data.nowEMGCode = DUALPORT.mcpMode.EMGCode;
				DUALPORT.nowEMGCode = backup_data.nowEMGCode;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=1;

				// 放送起動
				DUALPORT.ev5000_request.SentenceNo = SENTENCE_EMERGENCY;
				DUALPORT.ev5000_request.Sequence = announce_data.emgma[backup_data.nowEMGCode].sequence;
				DUALPORT.ev5000_request.Complete = 0;
				DUALPORT.ev5000_request.Abort = 1;
				DUALPORT.ev5000_request.Request = 1;

				DUALPORT.eids.Message = announce_data.emgma[backup_data.nowEMGCode].eidsChar;
				DUALPORT.eids.sendreq = 1;

				DUALPORT.mcpMode.EMGRepeat = 1;

				DUALPORT.tms_answer.dvaStatus.BYTE = 0;
				DUALPORT.tms_answer.dvaStatus.BIT.emgMode = 1;

				TraceAdd( EVENT_EMG );
			}
		}

		dvaIOOut( OUTSIGNAL_LED6, 0xFF );
}

/************************************************************************

	Function name	<< dvaSPCMA >>
					スペシャル放送シーケンス処理
	Syntax			dvaSPCMA( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	スピーカー制御
					a02 2001/07/17　スペシャル放送中の非常ハンドルの受け付け
					a03 2003/05/28 h.nishiura ＴＭＳへの送信情報の準備追加
					a04 2003/05/29 h.nishiura ＴＭＳへの送信情報の準備を無条件実行に変更
					a05 2021/02/07 h.nishiura RTTF対応
*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		スペシャル放送処理を行う

	------------------------------------------------------------------[@@@E]*/
void dvaSPCMA( void )
{
		int		i;
		union st_READSignal2 signal2;

			// ＴＭＳへの送信情報の準備
		for( i=0; i<8; i++ ){
			DUALPORT.tms_answer.passengerAlarm[i] = DUALPORT.answerData.rscSW[i].BYTE;
		}

			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
			befOpeMode = 0;
		}

		if( befOpeMode != OPEMODE_SPCMA ){
			befOpeMode = OPEMODE_SPCMA;

			TraceAdd( EVENT_SPCMA );

			DUALPORT.activeDVAU=1;		// アクティブＤＶＡＵモード

			DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// 自動放送モード

			dvaIOOut(
				OUTSIGNAL_trsDisable			// 列車無線停止中
				| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
				,
				OUTSIGNAL_audioBusDisable		// 車内放送許可
				| OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
				);

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCEnable = 1;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 1;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;

			DUALPORT.ev5000_request.Repeat = 0;

			dvaMCPDisplaySet( 0, "SPC-MA" );
			DUALPORT.mcpMode.INUSE = 1;
			dvaMCPDisplaySet( 1, announce_data.spcma[backup_data.nowSPCCode].mcpChar2 );
			dvaMCPDisplaySet( 2, announce_data.spcma[backup_data.nowSPCCode].mcpChar3 );

//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[0] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[1] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[2] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[0] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[1] = 0;		// MCP Switch OFF
//			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[2] = 0;		// MCP Switch OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BYTE = 0;	// Switch LED OFF
//			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BYTE = 0;	// Switch LED Blink OFF
		}

			// 列車無線起動の確認
		signal2.BYTE = nowSignal.SIGNAL2.BYTE;
		signal2.BYTE |= DUALPORT.answerData.dvau2;
		if( signal2.RBIT.trsRequest ){
			if( backup_data.rootOpeMode==OPEMODE_AUTO ){
				backup_data.opeMode = backup_data.rootOpeMode;
			}
			else {
				backup_data.opeMode = OPEMODE_TRS;
			}
			return;
		}

			// ＭＣＰからの非常通報器受け付け
		if( DUALPORT.mcpMode.EMGPOn ){
			DUALPORT.mcpMode.EMGPOn = 0;
			for( i=0; i<16; i++ ){
				backup_data.nowEMGHL[i] = DUALPORT.mcpMode.EMGPHL[i];
//				DUALPORT.mcpMode.EMGPHL[i] = 0;
			}
			backup_data.opeMode = OPEMODE_EMGHL;
			DUALPORT.ev5000_request.Abort = 1;
			return;
		}

		if( DUALPORT.ev5000_request.Complete ){
			DUALPORT.tms_answer.dvaStatus.BIT.specialMode = 0;

			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;

			if( DUALPORT.ev5000_request.Repeat ){
				DUALPORT.ev5000_request.Repeat = 0;
				dvaMCPDisplaySet( 0, "SPC-MA" );
				DUALPORT.mcpMode.INUSE = 1;
				dvaMCPDisplaySet( 1, announce_data.spcma[backup_data.nowSPCCode].mcpChar2 );
				dvaMCPDisplaySet( 2, announce_data.spcma[backup_data.nowSPCCode].mcpChar3 );
//				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			}

			// 放送要求
			if( DUALPORT.mcpMode.SPCOn ){
				DUALPORT.mcpMode.SPCOn = 0;
				DUALPORT.nowSPCCode = backup_data.nowSPCCode;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=1;

				// 放送が終わっていれば受け付ける
				// 放送起動
				DUALPORT.ev5000_request.SentenceNo = SENTENCE_SPECIAL;
				DUALPORT.ev5000_request.Sequence = announce_data.spcma[backup_data.nowSPCCode].sequence;
				DUALPORT.ev5000_request.Complete = 0;
				DUALPORT.ev5000_request.Abort = 1;
				DUALPORT.ev5000_request.Request = 1;

				DUALPORT.eids.Message = announce_data.spcma[backup_data.nowSPCCode].eidsChar;
				DUALPORT.eids.sendreq = 1;

				DUALPORT.tms_answer.dvaStatus.BYTE = 0;
				DUALPORT.tms_answer.dvaStatus.BIT.specialMode = 1;

				TraceAdd( EVENT_SPC );
			}

				// ＭＣＰからのリピート要求
			if( DUALPORT.mcpMode.REPOn ){
				DUALPORT.mcpMode.REPOn = 0;

//				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=1;

				// 放送が終わっていれば受け付ける
				DUALPORT.ev5000_request.Abort = 1;
				DUALPORT.ev5000_request.SentenceNo = SENTENCE_SPECIAL;
				DUALPORT.ev5000_request.Sequence = announce_data.spcma[backup_data.nowSPCCode].sequence;
				DUALPORT.ev5000_request.Complete = 0;
				DUALPORT.ev5000_request.Request = 1;
				DUALPORT.ev5000_request.Repeat = 1;

				dvaMCPDisplaySet( 0, "  REP " );
				DUALPORT.mcpMode.INUSE = 1;

				DUALPORT.eids.Message = announce_data.spcma[backup_data.nowSPCCode].eidsChar;
				DUALPORT.eids.sendreq = 1;

				DUALPORT.tms_answer.dvaStatus.BYTE = 0;
				DUALPORT.tms_answer.dvaStatus.BIT.specialMode = 1;

				TraceAdd( EVENT_REPEAT );
			}
		}

			// ＥＶ５０００からの放送が終了したら、呼出元のモードへ戻る
		if( DUALPORT.ev5000_request.Complete ){
			if( backup_data.rootOpeMode==OPEMODE_AUTO ){
				backup_data.opeMode = backup_data.rootOpeMode;
				DUALPORT.tms_answer.dvaStatus.BIT.specialMode = 0;
			}
		}

		dvaIOOut( OUTSIGNAL_LED7, 0xFF );
}

/************************************************************************

	Function name	<< dvaEMGHL >>
					非常通報シーケンス処理
	Syntax			dvaEMGHL( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
					a01 2001/03/22	スピーカー制御
					a02 2003/05/29 h.nihiura ＴＭＳへの送信情報の準備を無条件実行に変更
*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		非常通報処理を行う

	------------------------------------------------------------------[@@@E]*/
void dvaEMGHL( void )
{
		int		i;
		int		j;

			// ＴＭＳへの送信情報の準備
		for( i=0; i<8; i++ ){
			DUALPORT.tms_answer.passengerAlarm[i] = DUALPORT.answerData.rscSW[i].BYTE;
		}

			// マイク放送中やＣＡＢ／ＣＡＢモードのときは処理しない
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// マイク放送・ＣＡＢ／ＣＡＢモードからの復帰時
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// 列車無線もキャンセル
			befOpeMode = 0;
		}

		if( befOpeMode != OPEMODE_EMGHL ){
			befOpeMode = OPEMODE_EMGHL;

			TraceAdd( EVENT_EMGHL );

			DUALPORT.activeDVAU=1;		// アクティブＤＶＡＵモード

			DUALPORT.pollingData.voiceBus.BIT.mode = 2;	// 非常通報モード

			DUALPORT.ev5000_request.Abort = 1;

			dvaIOOut(
				OUTSIGNAL_trsDisable			// 列車無線停止中
				| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
				| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
				,
				OUTSIGNAL_audioBusDisable		// 車内放送許可
				| OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
				| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
				);

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

				// ＭＣＰの許可スイッチの設定
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
			DUALPORT.mcpMode.MANEnable = 0;

			DUALPORT.tms_answer.dvaStatus.BYTE = 0;
		}

			// ＭＣＰからの非常通報器受け付け
		if( DUALPORT.mcpMode.EMGPOn ){
			DUALPORT.mcpMode.EMGPOn = 0;
			for( i=0; i<16; i++ ){
				backup_data.nowEMGHL[i] = DUALPORT.mcpMode.EMGPHL[i];
//				DUALPORT.mcpMode.EMGPHL[i] = 0;
			}
		}

			// 通報器の音声ゲートをオープン
		for( i=0; i<16; i++ ){
			DUALPORT.pollingData.voiceBus.BYTE.BYTE[i+1] = backup_data.nowEMGHL[i];
		}

			// すべての通報器のスイッチがＯＦＦになったら、呼び出し元のモードへ戻る
		for( i=0; i<16; i++ ){
			backup_data.nowEMGHL[i] &= DUALPORT.answerData.rscSW[i].BYTE;
			if( DUALPORT.answerData.rscSW[i].BYTE )
				break;
		}
		if( i==16 ){
			for( i=0; i<8; i++ ){
				DUALPORT.pollingData.voiceBus.BYTE.BYTE[i+1] = 0;
			}
			for( i=0; i<16; i++ ){
				backup_data.nowEMGHL[i] = 0;
			}
			backup_data.opeMode = backup_data.rootOpeMode;
		}
}

/************************************************************************

	Function name	<< dvaCOUPLING >>
					１６両編成チェック処理
	Syntax			dvaCOUPLING( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
					a01 2000/10/10	通信手順変更
					a02 2004/09/13	h.nishiura
									常にcommGateオープン
									HCR ONと連動してダミー抵抗ＯＮに変更
*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		１６両編成で、音声バス通信が異常になったとき、
		自ＤＶＡＵのＨＣＲがＯＦＦであれば、
		スタンバイモードに移行する

	------------------------------------------------------------------[@@@E]*/
void dvaCOUPLING( void )
{

												// 2004/09/13	h.nishiura
		if( nowSignal.SIGNAL2.RBIT.hcrON==0 ){
			dvaIOOut(
				0
				,
				OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＮ
				| OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＮ
			);
		}else{
			dvaIOOut(
				OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＦＦ
				,
				OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＮ
			);
		}
		return;
												// 2004/09/13	h.nishiura 以下は処理しない

		if( DUALPORT.activeDVAU==0 ){		// 非アクティブＤＶＡＵは

			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動

			if( DUALPORT.couplingCheck==4 ){			// １６両編成中は
				dvaIOOut(
					OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＦＦ
					,
					OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＦＦ
				);
				return;
			}

			DUALPORT.couplingCheck = 0;		// 連結チェックなし
			dvaIOOut(
				0
				,
				OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＮ
				| OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＮ
			);
			return;
		}

			// 連結チェックタイミング

		switch( DUALPORT.couplingCheck ){
		case 0:		// 連結チェック開始
			// アクティブＤＶＡＵは１分間通常通信をおこなう
			if( timedata.commGateTimer ){
				DUALPORT.couplingCheck = 0;		// 連結チェックなし
				dvaIOOut(
					OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＦＦ
					| OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＦＦ
					,
					0
				);
				return;
			}

			// ファイルを更新中のときは連結チェックしない
			if( DUALPORT.download_active ){
				DUALPORT.couplingCheck = 0;		// 連結チェック終了
				timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動
				return;
			}
			if( DUALPORT.upload_active ){
				DUALPORT.couplingCheck = 0;		// 連結チェック終了
				timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動
				return;
			}


			// ＨＣＲがＯＦＦのときはチェックしない
			if( nowSignal.SIGNAL2.RBIT.hcrON==0 ){
				DUALPORT.couplingCheck = 0;		// 連結チェック終了
				timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動
				return;
			}

			// アイドルモードか自動放送中のみ受け付け
			if( backup_data.opeMode!=OPEMODE_IDLE && backup_data.opeMode!=OPEMODE_AUTO ){
				DUALPORT.couplingCheck = 0;		// 連結チェック終了
				timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動
				return;
			}


//			if( P4.DR.BIT.B3==0 ){
//				return;		// １６両編成チェックをやめる
//			}

			DUALPORT.couplingCheck = 1;		// 連結チェックをサブＣＰＵへ通知
			break;

		case 1:		// 連結チェック開始確認中
			break;

		case 2:		// 連結チェック起動確認中
			// アクティブＤＶＡＵは
			dvaIOOut(
				OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＦＦ
				,
				OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＮ
			);
			break;

		case 3:		// 連結なし応答
			dvaIOOut(
				OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＦＦ
				| OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＦＦ
				,
				0
			);

			DUALPORT.couplingCheck = 0;		// 連結チェック終了
			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動
			break;

		case 4:		// 連結あり応答
			dvaIOOut(
				OUTSIGNAL_commDummyOFF			// １６両編成通信用ダミー抵抗ＯＦＦ
				,
				OUTSIGNAL_commGateOFF			// １６両編成通信用ゲートＯＦＦ
			);
			timedata.commGateTimer = 6000;	// 通信ゲートオープンタイマー起動
			break;
		}
}

/************************************************************************

	Function name	<< dvaProcess >>
					シーケンス処理
	Syntax			dvaProcess( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/10/10  ＭＣＰのＡＴＣ／ＲＢ　ＬＥＤ処理
				a02 2000/11/13  トラブル発生トレースの追加
				a03	2002/04/08 h.nisiura ＥＩＤＳ表示クリア追加
*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		シーケンス処理関数の呼び出しを行う
		列車無線の受け付けを行う
		相手ＤＶＡＵからの要求受け付けを行う
		ＭＣＰのＡＴＣ、ＲＢのＬＥＤ点灯・消灯処理を行う
		トラブル発生トレースを行う

	------------------------------------------------------------------[@@@E]*/
void dvaProcess( void )
{
		union st_READSignal2 signal2;
		int		i;
		unsigned char	onbit;

		if( DUALPORT.activeDVAU ){		// アクティブＤＶＡＵの処理
			dvaMCPOperation();

			if( nowSignal.SIGNAL2.RBIT.hcrON ){
				dvaIOOut(
					0
					,
					OUTSIGNAL_trsActive			// 列車無線受け付け禁止
					);
			}
			else {
				if( DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError1
					|| DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError2 ){
					dvaIOOut(
						OUTSIGNAL_trsActive			// 列車無線受け付け許可
						,
						0
					);
				}
				else {
					dvaIOOut(
						0
						,
						OUTSIGNAL_trsActive			// 列車無線受け付け禁止
						);
				}
			}
		}
		else {
			dvaMCPInitial();

			dvaIOOut(
				OUTSIGNAL_trsActive			// 列車無線受け付け許可
				,
				0
			);
		}

		// 相手ＤＶＡＵからの要求受け付け
		if( DUALPORT.activeDVAUrequest==1 ){		// アクティブ切り替え
			DUALPORT.activeDVAUrequest = 0;
			backup_data.opeMode = OPEMODE_AUTO;
		}
		else if( DUALPORT.activeDVAUrequest==2 ){		// イニシャルテスト要求
			DUALPORT.activeDVAUrequest = 0;
			backup_data.opeMode = OPEMODE_INITIAL_TEST;
		}
		else if( DUALPORT.activeDVAUrequest==3 ){		// 自己診断要求
			DUALPORT.activeDVAUrequest = 0;
			backup_data.opeMode = OPEMODE_SELF_TEST;
			audioSignal = 0;
			return;
		}
		else if( DUALPORT.activeDVAUrequest==4 ){		// リセット要求
			DUALPORT.activeDVAUrequest = 0;
			for( timedata.checkTimer = 500; timedata.checkTimer!=0; )
				DUALPORT.mainActive = 1;
			RESET();
		}
		else if( DUALPORT.activeDVAUrequest==5 ){		// タイムアウトによるアクティブ切り替え
			DUALPORT.activeDVAUrequest = 0;
printf("DVAU Timeout\r\n");
			if( backup_data.opeMode==OPEMODE_INITIAL_TEST2 ){
				backup_data.opeMode = OPEMODE_INITIAL_TEST;
			}
			else if( backup_data.opeMode==OPEMODE_STANDBY ){
				if( DUALPORT.tms_request.Active ){			// ＴＭＳ有効
					backup_data.opeMode = OPEMODE_AUTO;
				}
				else {
					backup_data.opeMode = OPEMODE_IDLE;
				}
			}
			dvaMCPInitial();
		}
		else {
			signal2.BYTE = DUALPORT.answerData.dvau2;
			if( DUALPORT.activeDVAU ){
				if( DUALPORT.changeDVAU==0 ){
					if( nowSignal.SIGNAL2.RBIT.hcrON==0 ){
						if( signal2.RBIT.hcrON ){		// アクティブが切り替わった

														// '02/04/08 h.nisiura ＥＩＤＳ表示クリア追加
							DUALPORT.eids.Route = (char)0x00;
							DUALPORT.eids.Station = (char)0x00;
							DUALPORT.eids.sendreq = 1;

							if( backup_data.opeMode == OPEMODE_INITIAL_TEST
								&& P4.DR.BIT.B1 && P4.DR.BIT.B2 ){
								DUALPORT.changeDVAU = 2;		// 切り替え要求
							}
							else {
								DUALPORT.changeDVAU = 1;		// 切り替え要求
							}
							DUALPORT.ev5000_request.Abort = 1;
						}
					}
				}
			}
			else if( backup_data.opeMode!=OPEMODE_STANDBY ){
				backup_data.opeMode = OPEMODE_STANDBY;
				DUALPORT.ev5000_request.Abort = 1;
			}
		}

		// 連結チェック
		dvaCOUPLING();


		switch( backup_data.opeMode ){
		case OPEMODE_INITIAL_TEST:
			dvaInitialTEST();
			break;
		case OPEMODE_INITIAL_TEST2:
			dvaInitialStandby();
			break;
		case OPEMODE_SELF_TEST:
			dvaSELFTEST();
			break;
		case OPEMODE_TEST:
			dvaTEST();
			break;
		case OPEMODE_IDLE:
			dvaIDLE();
			break;
		case OPEMODE_AUTO:
			dvaAUTO();
			break;
		case OPEMODE_EMGMA:
			dvaEMGMA();
			break;
		case OPEMODE_SPCMA:
			dvaSPCMA();
			break;
		case OPEMODE_EMGHL:
			dvaEMGHL();
			break;
		case OPEMODE_TRS:
			dvaTRS();
			break;
		case OPEMODE_STANDBY:
			dvaStandby();
			break;
		default:
			backup_data.opeMode = OPEMODE_AUTO;
			break;
		}

		if( backup_data.opeMode==OPEMODE_AUTO
			|| backup_data.rootOpeMode==OPEMODE_AUTO ){
			// ＭＣＰのＡＴＣを点灯
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ATC=1;
			if( DUALPORT.tms_request.Active ){			// ＴＭＳ有効
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ATC=0;
			}
			else {
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ATC=1;
			}
		}
		else {
				// ＭＣＰのＡＴＣ　ＬＥＤを消灯
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ATC=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ATC=0;
		}

			// 列車無線起動の確認
		signal2.BYTE = nowSignal.SIGNAL2.BYTE;
		signal2.BYTE |= DUALPORT.answerData.dvau2;
		if( signal2.RBIT.trsRequest ){
				// ＭＣＰのＲＢＬＥＤを点灯
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB=1;
			if( backup_data.opeMode==OPEMODE_TRS
				&& DUALPORT.mcpMode.Cab==0
				&& DUALPORT.mcpMode.mcpManual==0 ){
					//　列車無線放送中のときは、点灯
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RB=0;
			}
			else {
					//　列車無線要求中のときは、点滅
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RB=1;
			}
		}
		else {
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB=0;
		}

	// トラブルメッセージのトレース
		if( DUALPORT.nowTrouble != troubleNowTrouble ){
			onbit = (DUALPORT.nowTrouble ^ troubleNowTrouble) & DUALPORT.nowTrouble;
			troubleNowTrouble = DUALPORT.nowTrouble;
			if( onbit ){
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					TraceTrouble( EVENT_TROUBLE_DVAU, onbit );
				}
				else {
					TraceTrouble( EVENT_TROUBLE_DVAU+1, onbit );
				}
			}
		}
		if( DUALPORT.answerData.dvau_trouble != troubleAnswerTrouble ){
			onbit = (DUALPORT.answerData.dvau_trouble ^ troubleAnswerTrouble) & DUALPORT.answerData.dvau_trouble;
			troubleAnswerTrouble = DUALPORT.answerData.dvau_trouble;
			if( onbit ){
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					TraceTrouble( EVENT_TROUBLE_DVAU, onbit );
				}
				else {
					TraceTrouble( EVENT_TROUBLE_DVAU+1, onbit );
				}
			}
		}

		for( i=0; i<8; i++ ){
			if( DUALPORT.answerData.trouble[i].BYTE != troublePAA[i] ){
				onbit = (DUALPORT.answerData.trouble[i].BYTE ^ troublePAA[i]) & DUALPORT.answerData.trouble[i].BYTE;
				troublePAA[i] = DUALPORT.answerData.trouble[i].BYTE;
				if( onbit ){
					TraceTrouble( EVENT_TROUBLE_PAA+i, onbit );
				}
			}
		}

		if( DUALPORT.tms_answer.dvaCommError.BYTE[0] != troubleCommError1 ){
			onbit = (DUALPORT.tms_answer.dvaCommError.BYTE[0] ^ troubleCommError1) & DUALPORT.tms_answer.dvaCommError.BYTE[0];
			troubleCommError1 = DUALPORT.tms_answer.dvaCommError.BYTE[0];
			if( onbit ){
				TraceTrouble( EVENT_TROUBLE_COMMPAA, onbit );
			}
		}
		if( DUALPORT.tms_answer.dvaCommError.BYTE[1] != troubleCommError2 ){
			onbit = (DUALPORT.tms_answer.dvaCommError.BYTE[1] ^ troubleCommError2) & DUALPORT.tms_answer.dvaCommError.BYTE[1];
			troubleCommError2 = DUALPORT.tms_answer.dvaCommError.BYTE[1];
			if( onbit ){
				TraceTrouble( EVENT_TROUBLE_COMMMCP, onbit );
			}
		}

		// ＡＣＰ用　ＬＥＤの点灯
		if( DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.CAB ){
			if( DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.CAB ){
				if( timedata.acpBlink<25 ){
					dvaIOOut( OUTSIGNAL_acpCABLED | OUTSIGNAL_acpCabLED, 0 );
				}
				else {
					dvaIOOut( 0, OUTSIGNAL_acpCABLED | OUTSIGNAL_acpCabLED );
				}
			}
			else {
				dvaIOOut( OUTSIGNAL_acpCABLED | OUTSIGNAL_acpCabLED, 0 );
			}
		}
		else {
			dvaIOOut( 0, OUTSIGNAL_acpCABLED | OUTSIGNAL_acpCabLED );
		}
		if( DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP ){
			if( DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP ){
				if( timedata.acpBlink<25 ){
					dvaIOOut( OUTSIGNAL_acpREPLED, 0 );
				}
				else {
					dvaIOOut( 0, OUTSIGNAL_acpREPLED );
				}
			}
			else {
				dvaIOOut( OUTSIGNAL_acpREPLED, 0 );
			}
		}
		else {
			dvaIOOut( 0, OUTSIGNAL_acpREPLED );
		}
		if( DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN ){
			if( DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN ){
				if( timedata.acpBlink<25 ){
					dvaIOOut( OUTSIGNAL_acpSTNLED, 0 );
				}
				else {
					dvaIOOut( 0, OUTSIGNAL_acpSTNLED );
				}
			}
			else {
				dvaIOOut( OUTSIGNAL_acpSTNLED, 0 );
			}
		}
		else {
			dvaIOOut( 0, OUTSIGNAL_acpSTNLED );
		}
}

