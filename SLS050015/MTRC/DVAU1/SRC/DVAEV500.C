/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	音声制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/07/27		K.Honda
	Revision	a01. 2000/09/07		K.Honda
	Revision	a02. 2000/11/13		K.Honda
				a03. 2001/04/18		K.Honda
				a04. 2003/04/01		h.nishiura  複数言語同時設定を可能に変更
				a05. 2024/03/11		h,nishiura  RTTF用放送中情報をDualPortRamにアップデート
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include	"iodefine.h"
#include	"table.h"

#include	"timer.h"
#include	"trace.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

unsigned char	ev5000_time_adjust;

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#define	SEND_SETUP			0		// セットアップ待ち
#define	SEND_INITIAL		1		// 接続チェック送信
#define	SEND_INITIAL_RECV	2		// 接続チェック受信
#define	SEND_WAIT			3		// 放送前時間待ち
#define	SEND_WAITUP			4		// 放送前時間待ち終了チェック
#define	SEND_VOICE			5		// 放送文送信
#define	SEND_VOICE_RECV		6		// 放送文受信チェック
#define	SEND_VOICE_END		7		// 放送文終了チェック
#define	SEND_BUSY			9	// 放送中チェック
#define	SEND_BUSY_RECV		10		// 放送中受信
#define	NEXT_VOICE			11	// 次回放送検索
#define	SEND_ABORT			12	// 放送キャンセル
#define	SEND_ABORT_RECV		13		// 放送キャンセル受信
#define	SEND_ABORTBUSY		14	// 放送キャンセル終了
#define	SEND_ABORTBUSY_RECV	15	// 放送キャンセル終了待ち
#define	SEND_TIME_GET		16		// 日時要求
#define	SEND_TIME_GET_RECV	17		// 日時受信
#define	SEND_TIME_SET		18		// 日時設定
#define	SEND_TIME_SET_RECV	19		// 日時設定受信

#define	MAX_EV5000_TIMEOUT	200		// 受信タイムアウト(x10mS)
#define	MAX_EV5000_RETRYTIMEOUT	3	// 受信タイムアウトリトライ回数
#define	MAX_EV5000_RETRYERROR	3	// 受信エラーリトライ回数
#define	MAX_EV5000_BUSYTIME	100		// 放送中チェックタイム(x10mS)
#define	MAX_EV5000_ABORTRETRY	3	// 放送キャンセルリトライ回数
#define	MAX_EV5000_ABORTLOOPRETRY	3	// 放送キャンセルリトライ回数

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	void asci_0_Initialize( int baud, char data, char parity, char stop );
extern	void asci_0_tx_enable( void );
extern	void asci_0_tx_disable( char tend );
extern	void asci_0_transmit( unsigned char data );

extern	struct	CONFIG_INFO		config_data;		// 設定情報

extern	struct	TIMER_CONTROL	timedata;		// タイマー情報

extern	struct	BACKUP_DATA		backup_data;		// バックアップデータ

extern	struct st_ReadSIGNAL nowSignal;			// 最新読みだし値

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	char	phase;				// 処理フェーズ
static	char	requestNo;			// 要求放送番号
static	char	timeout;			// タイムアウトカウンター
static	char	errcnt;				// エラーカウンター
static	char	recvbuff[32];		// 受信バッファー
static	char	recvlen;			// 受信バイト数
static	char	recv_complete;		// 受信完了フラグ
static	char	sendbuff[32];		// 送信バッファー
static	char	*sendp;				// 送信ポインター
static	char	sendbuff_tmp[32];	// 送信バッファーテンポラリ

static	char	recvsts;
static	char	abortcnt;			// 放送キャンセルリトライ回数

/*@@@E*/
/************************************************************************

	Function name	<< dvauEV5000Iniialize >>
					ＥＶ５０００初期設定
	Syntax			dvaEV5000Initialize( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27
	Revision		a01 2000/10/21		2400bps に変更、１秒間隔送受信に変更
	Revision		a02 2000/11/13		日時設定・取得の追加
				a03 2001/02/15		日時有効・無効フラグのクリア
*************************************************************************/
void dvauEV5000Initialize( void )
{
		asci_0_Initialize( 2400, 8, 'E', 0 );
		sendp = "";
		phase = SEND_SETUP;
		DUALPORT.ev5000_request.Initial=1;
		DUALPORT.ev5000_request.Abort=0;
		DUALPORT.ev5000_request.Complete=0;
		DUALPORT.ev5000_request.Request=0;
		DUALPORT.ev5000_request.Repeat=0;
		DUALPORT.ev5000_request.ev5000_error = 0;

		timedata.dateActive = 0;

		ev5000_time_adjust = 0;
}

/************************************************************************

	Function name	<< asci_0_send_ir >>
					送信割り込み処理
	Syntax			asci_0_send_ir( tend );
	Argument		int tend : i : 送信終了フラグ
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_0_send_ir( int tend )
{
		if( *sendp=='\0' ){	/*すべて送信したとき*/
			asci_0_tx_disable( tend );		/*受信へ切り替え*/
			return;
		}

		asci_0_transmit( *sendp++ );	/*１バイトの送信*/
}

/************************************************************************

	Function name	<< asci_0_recv_ir >>
					受信割り込み処理
	Syntax			asci_0_recv_ir( data, status );
	Argument		char data   : i : 受信データ
				char status : i : 受信ステータス
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_0_recv_ir( char data, char status )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・ＥＶ５０００からの受信データを取り込む
　・受信データにＣＲ(0x0D)があれば受信完了とする
　・ただし、受信ステータスが受信エラーのときは、受信完了としない
	------------------------------------------------------------------[@@@E]*/

		recvsts |= status;
		if( recvlen<sizeof(recvbuff)-1 ){
			recvbuff[recvlen++] = data;
			recvbuff[recvlen]='\0';
		}
		if( data=='\r' && (recvsts & 0x38)==0 ){
			recv_complete = 1;
		}
}


/************************************************************************

	Function name	<< dvauEV5000Cyclic >>
					ＥＶ５０００処理
	Syntax			dvaEV5000Cyclic( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27
	Revision		a01 2000/10/21		2400bps に変更、１秒間隔送受信に変更
	Revision		a02 2000/11/13		日時設定・取得の追加
					a03 2001/04/18		Ｂ(Busy) コマンドを２秒間隔にする
							ハードウェアビジー信号によるチェックを追加
					a04 2002/12/11		複数言語同時設定を可能に変更
					a05. 2024/03/11		h,nishiura  RTTF用放送中情報をDualPortRamにアップデート
*************************************************************************/
void dvauEV5000Cyclic( void )
{
		int		i;

		switch( phase ){
		case SEND_SETUP:		/* 送信データ準備処理 */
			timeout=0;	// タイムアウトカウントのリセット
			errcnt=0;	// エラーカウントのリセット
			DUALPORT.rttf_an_busy = 0;		// a05 RTTF用放送中情報（0:停止中/1:放送中) － SETUPフェーズは初期化を兼ねて放送停止中

			if( DUALPORT.ev5000_request.Initial ){	// 初期設定要求受け付け
				phase = SEND_INITIAL;		// 初期設定処理へ
				break;
			}
			if( DUALPORT.ev5000_request.Abort ){	// 放送中止受け付け
				DUALPORT.ev5000_request.Abort=0;
				if( DUALPORT.ev5000_request.Request==0 ){
					DUALPORT.ev5000_request.Complete = 1;	// 放送中断時、次の要求がなければ　完了したとする

#if 0
printf("EV5000 Request END\r\n");
#endif
				}
				break;
			}
			if( DUALPORT.ev5000_request.Request ){	// 放送要求受付
				DUALPORT.ev5000_request.Request=0;

				dvaIOOut(
					OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
					| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
					,
					OUTSIGNAL_audioBusDisable		// 車内放送許可
				);

				requestNo = 0;
				phase = SEND_WAIT;		// 最初のウェイトへ
				break;
			}
			else if( DUALPORT.ev5000_request.Complete==0 ){
				DUALPORT.ev5000_request.Complete = 1;	// 放送終了

#if 0
printf("EV5000 Request END\r\n");
#endif

				dvaIOOut(
					OUTSIGNAL_audioBusDisable		// 車内放送禁止
					,
					OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
					| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
				);
			}

			if( ev5000_time_adjust ){		// 日時設定受付
				ev5000_time_adjust = 0;
				phase = SEND_TIME_SET;		// 日時設定へ
			}
			break;

		case SEND_WAIT:		// 放送前ウェイト
			timedata.ev5000timer = DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.timewait * 10;
			if( timedata.ev5000timer==0 )
				timedata.ev5000timer=10;
			phase = SEND_WAITUP;
			break;

		case SEND_WAITUP:	// 放送前ウェイト終了チェック
			if( timedata.ev5000timer==0 ){

				// 放送文有無チェック
				for( ; requestNo<MAX_SEQUENCE; requestNo++ ){
					if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.en ){
						if( config_data.lang.en ){
							break;
						}
					}
					if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.cn ){
						if( config_data.lang.cn ){
							break;
						}
					}
					if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.ma ){
						if( config_data.lang.ma ){
							break;
						}
					}
				}

				if( requestNo>=MAX_SEQUENCE ){
					DUALPORT.ev5000_request.Complete=1;	//放送文終了
					phase = SEND_SETUP;

#if 0
printf("EV5000 Request END\r\n");
#endif

					dvaIOOut(
						0
						,
						OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
						| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
						);

					break;
				}

				phase = SEND_VOICE;		// １つの言語の放送	2002/12/11 複数言語同時可に変更
			}
			if( DUALPORT.ev5000_request.Abort ){		// 放送キャンセル
				phase = SEND_SETUP;
				TraceAdd( EVENT_VOICE_ABORT );
				break;
			}
			break;

		case SEND_VOICE:			// １つの言語の放送文送信	2002/12/11 複数言語同時可に変更
			if( DUALPORT.ev5000_request.Abort ){		// 放送キャンセル
				phase = SEND_SETUP;
				TraceAdd( EVENT_VOICE_ABORT );
				break;
			}

#if 0
debugTime();
printf("VOICE\r\n");
#endif
			backup_data.ev5000code.sendcode = DUALPORT.ev5000_request.Sequence[requestNo].BYTE.Code[1];
			DUALPORT.rttf_an_sendCode = (unsigned char)( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.sendCode) & 0xFF;
																			// a05 放送コードをDualPortRamにもアップデート
																			// RTTFパケットには8bit枠しか無いためbit 9は無視
#if 0
			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.cn ){
				sprintf( sendbuff, "M000,0%03X,%03X,000,000\r\n",
					DUALPORT.ev5000_request.SentenceNo,
					(DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.sendCode) & 0x1FF );
			}
			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.en ){
				sprintf( sendbuff, "M000,0%03X,000,%03X,000\r\n",
					DUALPORT.ev5000_request.SentenceNo,
					(DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.sendCode) & 0x1FF );
			}
			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.ma ){
				sprintf( sendbuff, "M000,0%03X,000,000,%03X\r\n",
					DUALPORT.ev5000_request.SentenceNo,
					(DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.sendCode) & 0x1FF );
			}
#else
			sprintf( sendbuff, "M000,0%03X,", DUALPORT.ev5000_request.SentenceNo );

			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.cn ){
				if( config_data.lang.cn ){
					sprintf( sendbuff_tmp, "%03X,",
					(DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.sendCode) & 0x1FF );
				}
			}else{
				strcpy( sendbuff_tmp, "000," );
			}
			strcat( sendbuff,sendbuff_tmp);

			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.en ){
				if( config_data.lang.en ){
					sprintf( sendbuff_tmp, "%03X,",
					(DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.sendCode) & 0x1FF );
				}
			}else{
				strcpy( sendbuff_tmp, "000," );
			}
			strcat( sendbuff,sendbuff_tmp);

			if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.ma ){
				if( config_data.lang.ma ){
					sprintf( sendbuff_tmp, "%03X\r\n",
					(DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.sendCode) & 0x1FF );
				}
			}else{
				strcpy( sendbuff_tmp, "000\r\n" );
			}
			strcat( sendbuff,sendbuff_tmp);

#endif


//			TraceAdd( EVENT_VOICE );
			recvlen = 0;
			recv_complete = 0;
			recvsts = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_0_transmit( *sendp++ );
			phase = SEND_VOICE_RECV;
			break;

		case SEND_VOICE_RECV:		// 放送分受信チェック
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
					if( recvsts!=0 ){			// 何らかの受信があれば、受信したものとみなす
						timedata.ev5000timer=MAX_EV5000_BUSYTIME;
						phase = SEND_BUSY;		// 放送中チェック
						break;
					}

					timedata.ev5000timer=MAX_EV5000_BUSYTIME;
					DUALPORT.ev5000_request.ev5000_error = 1;
					phase = SEND_BUSY;
					TraceAdd( EVENT_EVCOMM_NG );
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='A' ){		/* EV5000 が ＡＣＫをかえした */
#if 0
debugTime();
printf("Voice Ack\r\n");
#endif
				// 正常処理
				timedata.ev5000timer=MAX_EV5000_BUSYTIME;
				phase = SEND_BUSY;		// 放送中チェック
				break;
			}

			if( recvbuff[0]=='E' ){
				TraceAdd( EVENT_VOICE_NG );

				// 定義異常処理
				phase = NEXT_VOICE;		// 次の放送文へ
				break;
			}

			// 異常受信処理
			errcnt++;
			if( errcnt<3 ){
				phase = SEND_VOICE;
				break;
			}

			TraceAdd( EVENT_EVCOMM_NG );
			DUALPORT.ev5000_request.ev5000_error = 2;
			phase = SEND_SETUP;
			break;

		case SEND_BUSY:			// 放送中チェックの送信
			DUALPORT.rttf_an_busy = 1; // a05 RTTF用放送中情報（0:停止中/1:放送中) － 実際に放送しているフェーズで放送中ステート
			if( DUALPORT.ev5000_request.Abort ){
				phase = SEND_ABORT;
				timedata.ev5000timer = 0;
				abortcnt = 0;
				break;
			}
			if( timedata.ev5000timer!=0 )
				break;

#if 0
debugTime();
printf("BUSY Send\r\n");
#endif
			strcpy( sendbuff, "B\r\n" );
			recvlen = 0;
			recv_complete = 0;
			recvsts = 0;
			timedata.ev5000timer = MAX_EV5000_BUSYTIME;
			sendp = sendbuff;
			asci_0_transmit( *sendp++ );
			phase = SEND_BUSY_RECV;
			break;

		case SEND_BUSY_RECV:			// 放送中チェック
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
#if 0
debugTime();
printf("BUSY Time Out\r\n");
#endif
					timeout++;
					if( timeout>MAX_EV5000_RETRYTIMEOUT ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						DUALPORT.nowTrouble |= 0x04;
						if( nowSignal.SIGNAL3.RBIT.ev5000Busy==0 ){		// ＥＶ５０００からの信号も停止しているとき
							phase = SEND_SETUP;
							TraceAdd( EVENT_EVCOMM_NG );
							break;
						}
					}
					timedata.ev5000timer = MAX_EV5000_BUSYTIME;
					phase = SEND_BUSY;
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='Y' ){
				if( recvbuff[1]!='0' || recvbuff[2]!='0' ){	/* EV5000 が　ＢＵＳＹのとき */
#if 0
debugTime();
printf("BUSY Ack\r\n");
#endif
					// ビジー処理
					timedata.ev5000timer = MAX_EV5000_BUSYTIME;
					phase = SEND_BUSY;
					break;
				}
				/* EV5000 が ＡＣＫをかえした */
				// 正常処理
//				TraceAdd( EVENT_VOICE_OK );

				DUALPORT.nowTrouble &= ~0x04;

				if( nowSignal.SIGNAL3.RBIT.ev5000Busy==0 ){		// ＥＶ５０００からの信号も停止しているとき
#if 0
debugTime();
printf("BUSY Signal END\r\n");
#endif
					phase = NEXT_VOICE;		// 次の放送文へ
					DUALPORT.rttf_an_busy = 0;		// a05 RTTF用放送中情報（0:停止中/1:放送中) － 放送完了で停止中ステート
					break;
				}
#if 0
debugTime();
printf("BUSY!!\r\n");
#endif
				// ビジー処理
				timedata.ev5000timer = MAX_EV5000_BUSYTIME;
				phase = SEND_BUSY;
				break;
			}

			// 異常受信処理
			errcnt++;

			if( errcnt<MAX_EV5000_RETRYERROR ){
				phase = SEND_BUSY;
				timedata.ev5000timer = MAX_EV5000_TIMEOUT;
				break;
			}

			TraceAdd( EVENT_EVCOMM_NG );
			DUALPORT.ev5000_request.ev5000_error = 2;
			DUALPORT.nowTrouble |= 0x04;
			phase = SEND_SETUP;
			break;

		case NEXT_VOICE:
			// 次の放送文有無チェック
			for( requestNo++; requestNo<MAX_SEQUENCE; requestNo++ ){
				if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.en ){
					if( config_data.lang.en ){
						break;
					}
				}
				if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.cn ){
					if( config_data.lang.cn ){
						break;
					}
				}
				if( DUALPORT.ev5000_request.Sequence[requestNo].sendRequest.ma ){
					if( config_data.lang.ma ){
						break;
					}
				}
			}

			if( requestNo>=MAX_SEQUENCE ){
				phase = SEND_SETUP;
				break;
			}

			phase = SEND_WAIT;		// 放送前ウェイトへ
			break;


		case SEND_ABORT:		// 放送キャンセル
			if( timedata.ev5000timer!=0 )
				break;

#if 0
debugTime();
printf("ABORT SEND\r\n");
#endif
			strcpy( sendbuff, "M200\r\n" );
			recvlen = 0;
			recv_complete = 0;
			recvsts = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_0_transmit( *sendp++ );
			phase = SEND_ABORT_RECV;
			break;

		case SEND_ABORT_RECV:	// 放送キャンセル確認
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
#if 0
debugTime();
printf("ABORT Ack Timeout\r\n");
#endif
					timeout++;
					if( timeout>MAX_EV5000_RETRYTIMEOUT ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						if( nowSignal.SIGNAL3.RBIT.ev5000Busy==0 ){		// ＥＶ５０００からの信号も停止しているとき
							phase = SEND_ABORTBUSY;
							TraceAdd( EVENT_EVCOMM_NG );
							break;
						}
					}
					phase = SEND_ABORT;
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='A' ){		/* EV5000 が ＡＣＫをかえした */
#if 0
debugTime();
printf("ABORT Ack\r\n");
#endif
				phase = SEND_ABORTBUSY;
				timedata.ev5000timer = MAX_EV5000_BUSYTIME;
				break;
			}
			if( recvbuff[0]=='E' ){
				TraceAdd( EVENT_VOICE_NG );

				// 定義異常処理
				phase = SEND_ABORTBUSY;
				break;
			}

			// 異常受信処理
			errcnt++;

			if( errcnt<MAX_EV5000_RETRYERROR ){
				phase = SEND_ABORT;
				timedata.ev5000timer = MAX_EV5000_TIMEOUT;
				break;
			}

			TraceAdd( EVENT_EVCOMM_NG );
			DUALPORT.ev5000_request.ev5000_error = 2;

			phase = SEND_ABORTBUSY;
			break;


		case SEND_ABORTBUSY:	// 放送キャンセル後の放送停止確認
			if( timedata.ev5000timer!=0 )
				break;

#if 0
debugTime();
printf("BUSY Check\r\n");
#endif
			strcpy( sendbuff, "B\r\n" );
			recvlen = 0;
			recv_complete = 0;
			recvsts = 0;
			timedata.ev5000timer = MAX_EV5000_BUSYTIME;
			sendp = sendbuff;
			asci_0_transmit( *sendp++ );
			phase = SEND_BUSY_RECV;
			break;

		case SEND_ABORTBUSY_RECV:			// 放送中チェック
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
#if 0
debugTime();
printf("BUSY Time Out\r\n");
#endif
					timeout++;
					if( timeout>MAX_EV5000_RETRYTIMEOUT ){
						abortcnt++;
						if( abortcnt>MAX_EV5000_ABORTLOOPRETRY ){
							phase = SEND_ABORT;
							break;
						}
						DUALPORT.ev5000_request.ev5000_error = 1;
						DUALPORT.nowTrouble |= 0x04;
						if( nowSignal.SIGNAL3.RBIT.ev5000Busy==0 ){		// ＥＶ５０００からの信号も停止しているとき
							phase = SEND_SETUP;
							TraceAdd( EVENT_EVCOMM_NG );
							break;
						}
					}
					timedata.ev5000timer = MAX_EV5000_BUSYTIME;
					phase = SEND_ABORTBUSY;
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='Y' ){
				if( recvbuff[1]!='0' || recvbuff[2]!='0' ){	/* EV5000 が　ＢＵＳＹのとき */
#if 0
debugTime();
printf("BUSY!!\r\n");
#endif
					// ビジー処理
					timedata.ev5000timer = MAX_EV5000_BUSYTIME;
					phase = SEND_ABORTBUSY;
					break;
				}
				/* EV5000 が ＡＣＫをかえした */
				// 正常処理

				DUALPORT.nowTrouble &= ~0x04;

				if( nowSignal.SIGNAL3.RBIT.ev5000Busy==0 ){		// ＥＶ５０００からの信号も停止しているとき
					phase = SEND_SETUP;
					break;
				}
				// ビジー処理
				timedata.ev5000timer = MAX_EV5000_BUSYTIME;
				phase = SEND_ABORTBUSY;
				break;
			}

			// 異常受信処理
			errcnt++;

			if( errcnt<MAX_EV5000_RETRYERROR ){
				phase = SEND_ABORTBUSY;
				timedata.ev5000timer = MAX_EV5000_TIMEOUT;
				break;
			}

			TraceAdd( EVENT_EVCOMM_NG );
			DUALPORT.ev5000_request.ev5000_error = 2;
			DUALPORT.nowTrouble |= 0x04;
			phase = SEND_SETUP;
			break;

		case SEND_INITIAL:		// 接続チェック送信
			DUALPORT.ev5000_request.Abort=0;
			DUALPORT.ev5000_request.Request=0;

			if( timedata.ev5000timer!=0 )
				break;
			strcpy( sendbuff, "A\r\n" );
			recvlen = 0;
			recvsts = 0;
			recv_complete = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_0_transmit( *sendp++ );
			phase = SEND_INITIAL_RECV;
			break;

		case SEND_INITIAL_RECV:			// 接続チェック確認
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
					timeout++;
					if( timeout>2 ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						DUALPORT.nowTrouble |= 0x04;
						phase = SEND_SETUP;
						TraceAdd( EVENT_EVCOMM_NG );
						DUALPORT.ev5000_request.Initial=0;
					}
					else {
						phase = SEND_INITIAL;
					}
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='A' ){		/* EV5000 が ＡＣＫをかえした */
				// 正常処理
				DUALPORT.nowTrouble &= ~0x04;
//				phase = SEND_SETUP;
				phase = SEND_TIME_GET;
				break;
			}

			if( recvbuff[0]=='Y' ){
				if( recvbuff[1]!='0' || recvbuff[2]!='0' ){	/* EV5000 が　ＢＵＳＹのとき */
					// ビジー処理
					timedata.ev5000timer = MAX_EV5000_BUSYTIME;
					phase = SEND_INITIAL;
					break;
				}
				/* EV5000 が ＡＣＫをかえした */
				// 正常処理
//				phase = SEND_SETUP;
				phase = SEND_TIME_GET;
				break;
			}

			// 異常受信処理
			errcnt++;

			if( errcnt<3 ){
				phase = SEND_INITIAL;
				break;
			}

			TraceAdd( EVENT_EVCOMM_NG );
			DUALPORT.ev5000_request.ev5000_error = 2;
			DUALPORT.nowTrouble |= 0x04;
			phase = SEND_SETUP;
			DUALPORT.ev5000_request.Initial=0;
			break;

		case SEND_TIME_GET:		// 日時問い合わせ送信
			if( timedata.ev5000timer!=0 )
				break;
			strcpy( sendbuff, "TQ\r\n" );
			recvlen = 0;
			recvsts = 0;
			recv_complete = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_0_transmit( *sendp++ );
			phase = SEND_TIME_GET_RECV;
			break;

		case SEND_TIME_GET_RECV:		// 日時チェック確認
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
					timeout++;
					if( timeout>2 ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						phase = SEND_SETUP;
						TraceAdd( EVENT_EVCOMM_NG );
						DUALPORT.ev5000_request.Initial=0;
					}
					else {
						phase = SEND_TIME_GET;
					}
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='T' && recvbuff[1]=='A' ){
				for( i=2; i<14; i++ ){
					if( recvbuff[i]<'0' || recvbuff[i]>'9' ){

						TraceAdd( EVENT_EVCOMM_NG );

						// 異常受信処理
						errcnt++;

						if( errcnt<3 ){
							phase = SEND_TIME_GET;
							break;
						}
						phase = SEND_SETUP;
						DUALPORT.ev5000_request.Initial=0;
						break;
					}
				}
				if( i<14 )
					break;

printf("evdate=%s\r\n",recvbuff);
				timedata.tm_year = (recvbuff[2] & 0x0F) * 10 + (recvbuff[3] & 0x0F);
				timedata.tm_mon = (recvbuff[4] & 0x0F) * 10 + (recvbuff[5] & 0x0F);
				timedata.tm_day = (recvbuff[6] & 0x0F) * 10 + (recvbuff[7] & 0x0F);
				timedata.tm_hour = (recvbuff[8] & 0x0F) * 10 + (recvbuff[9] & 0x0F);
				timedata.tm_min = (recvbuff[10] & 0x0F) * 10 + (recvbuff[11] & 0x0F);
				timedata.tm_sec = (recvbuff[12] & 0x0F) * 10 + (recvbuff[13] & 0x0F);
				timedata.tm_msec = 0;
				timedata.dateActive = 1;

				phase = SEND_SETUP;
				DUALPORT.ev5000_request.Initial=0;
				break;
			}

			// 異常受信処理
			errcnt++;

			if( errcnt<3 ){
				phase = SEND_TIME_GET;
				break;
			}

			TraceAdd( EVENT_EVCOMM_NG );
			DUALPORT.ev5000_request.ev5000_error = 2;
			phase = SEND_SETUP;
			DUALPORT.ev5000_request.Initial=0;
			break;

		case SEND_TIME_SET:		// 日時設定送信
			if( timedata.ev5000timer!=0 )
				break;
			sprintf( sendbuff, "TM%02u%02u%02u%02u%02u%02u\r\n",
				timedata.tm_year,
				timedata.tm_mon,
				timedata.tm_day,
				timedata.tm_hour,
				timedata.tm_min,
				timedata.tm_sec );
//printf("set evdate=%s\r\n",sendbuff);


			recvlen = 0;
			recvsts = 0;
			recv_complete = 0;
			timedata.ev5000timer = MAX_EV5000_TIMEOUT;
			sendp = sendbuff;
			asci_0_transmit( *sendp++ );
			phase = SEND_TIME_SET_RECV;
			break;

		case SEND_TIME_SET_RECV:		// 日時チェック確認
			if( recv_complete==0 ){
				if( timedata.ev5000timer==0 ){
					timeout++;
					if( timeout>2 ){
						DUALPORT.ev5000_request.ev5000_error = 1;
						phase = SEND_SETUP;
						TraceAdd( EVENT_EVCOMM_NG );
					}
					else {
						phase = SEND_TIME_SET;
					}
				}
				break;
			}

			timeout=0;
			if( recvbuff[0]=='A' ){		/* EV5000 が ＡＣＫをかえした */
				// 正常処理
				phase = SEND_SETUP;
				break;
			}

			// 異常受信処理
			errcnt++;

			if( errcnt<3 ){
				phase = SEND_TIME_SET;
				break;
			}

			TraceAdd( EVENT_EVCOMM_NG );
			DUALPORT.ev5000_request.ev5000_error = 2;
			phase = SEND_SETUP;
			break;
		}
}

