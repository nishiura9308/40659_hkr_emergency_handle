/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	ＥＩＤＳ制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/07		K.Honda

*********************************************************************/

#include    <machine.h>
#include	<string.h>
#include	<limits.h>

#include	"timer.h"
#include	"table.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#define		STX			(0x02)				/* テキスト開始			*/
#define		ETX			(0x03)				/* テキスト終了			*/
#define		DLE			(0x10)				/* ＤＬＥ拡張			*/
#define		ACK			(0x06)				/* 正常送信応答			*/
#define		NAK			(0x15)				/* 異常送信応答			*/
#define		SPC			(0x20)				/* データの区切り		*/


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	struct	TIMER_CONTROL	timedata;

extern	void asci_1_Initialize( int baud, char data, char parity, char stop );
extern	void asci_1_tx_enable( void );
extern	void asci_1_tx_disable( char tend );
extern	void asci_1_transmit( unsigned char data );


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
/** 送信エリア **/
static	struct	EIDS_FORMAT		eidsarea;
static	unsigned char	*sendp;				/*送信バッファー*/
static	unsigned char	sendlen;			/*送信バイト数*/
static	unsigned char	sendcnt;			/*送信カウンター*/

static	unsigned char	recvbuff[512];		/*受信バッファー*/
static	unsigned short	recvlen;			/*受信バイト数*/
static	unsigned char	recvphase;			/*受信フェーズ*/
static	unsigned char	recvdle;			/*受信ＤＬＥフラグ*/
static	unsigned char	recvbcc;			/*受信ＢＣＣ計算エリア*/


static	struct EIDS_MSG befEIDS;		// 前回送信情報
static	unsigned char	befMessage;		// 前回送信メッセージコード
static	unsigned char	eidsResetCounter;	// リセット信号５秒間保持カウンター 2001/07/04
static	unsigned char	eidsMessageCounter;	// メッセージ５秒間保持カウンター	2001/07/04



// for RTIF
															// 1+1+1+12+11+1+2+1=30
static	const unsigned char	rtifexample[30] = { 	0x2c,		// 1
													0x26,		// 1
													0x52,		// 1
													0x2d,0x3b,0x34,0x64,0x26,0x61,0x45,0x4b,0x24,0x3d,0x25,0x71,	// 12
													0x68,0x65,0x6c,0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,			// 11
													0x1f,		// 1
													0x50,0x20,	// 2
													0x25 };		// 1
unsigned char rtifarea[192];		/* RTIF受信バッファー */
int rtifarea_recvsize = 0;			/* RTIF受信サイズ */

void dvauRTIFSend( void );


/*@@@E*/
/************************************************************************

	Function name	<< asci_1_recv_ir >>
					受信割り込み処理
	Syntax			asci_1_recv_ir( char data, char status );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void asci_1_recv_ir( char data, char status )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・ハンドラーの定義のみ、使用していない
	------------------------------------------------------------------[@@@E]*/
		if( status & 0x38 ){		// Framing Error, Parity Error?
			return;
		}
		recvbuff[0] = data;

}

/************************************************************************

	Function name	<< asci_1_send_ir >>
					送信割り込み処理
	Syntax			asci_1_send_ir( int tend );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07	送信終了・継続を追加

*************************************************************************/
void asci_1_send_ir( char tend )
{
		if( sendcnt==sendlen ){	/*すべて送信したとき*/
			asci_1_tx_disable( tend );		/*受信へ切り替え*/
			return;
		}

		sendcnt++;
		asci_1_transmit( *sendp++ );	/*１バイトの送信*/
}

/************************************************************************

	Function name	<< dvauEIDSInitialize >>
					ＥＩＤＳ通信初期設定
	Syntax			dvauEIDSInitialize( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
					a01 2001/07/04  比較エリアの初期設定を追加
									ドア信号のクローズ

*************************************************************************/
void dvauEIDSInitialize( void )
{

		asci_1_Initialize( 1200, 7, 'E', 0 );

		memset( &eidsarea, 0, sizeof(eidsarea) );

		eidsarea.Messages = 0x20;
		eidsarea.status.BIT.All1 = 1;
		eidsarea.status.BIT.reset = 0;
		eidsarea.status.BIT.doorstatus = 1;

		memset( &befEIDS, 0, sizeof(befEIDS) );	// 2001/07/04 比較エリアの初期設定

		timedata.eidstimer = 20;
}

/************************************************************************

	Function name	<< dvauEIDSSend >>
					送信準備処理
	Syntax			dvauEIDSSend( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void dvauEIDSSend( void )
{
		asci_1_tx_enable();		/*送信開始  RTS=ON*/

		sendcnt = 0;					/* 送信ポインター０クリア */

		eidsarea.stx = STX;
		eidsarea.etx = ETX;

		sendp = (unsigned char *)&eidsarea;
		sendlen = sizeof(eidsarea);

		sendcnt++;
		asci_1_transmit( *sendp++ );	/*最初の１バイトを送信*/
}

/************************************************************************

	Function name	<< dvauEIDSSendCheck >>
					２００ｍＳ毎の送信起動
	Syntax			dvauEIDSSendCheck( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/11/13 ５秒間の送信後、データが変化しなければ送信停止
				a02 2001/01/15 非常放送・スペシャル放送のコードを送信していなかった
							   ５秒間の送信後、非常放送・スペシャル放送のコードを２０ｈに戻していなかった
				a03 2001/07/04 連続して送信するように変更
								リセット信号とメッセージは、変化を確認後、リセットする
								リセット信号の状態をメインＣＰＵへ通知する
								リセット信号がＯＮになれば、メッセージを２０ｈにする
								逆にメッセージが２０ｈ以外になれば、リセット信号をＯＦＦにする

*************************************************************************/
void dvauEIDSSendCheck( void )
{
		if( DUALPORT2.activeDVAU && DUALPORT2.activeDVAUrequest==0 ){	// 切り替え中でないこと

			if( timedata.eidstimer==0 ){
				timedata.eidstimer = 20;		// 200mS Wait

				// リセット信号の確認
				if( befEIDS.Reset != DUALPORT2.eids.Reset ){	// リセット信号変化時 2001/07/04
					befEIDS.Reset = DUALPORT2.eids.Reset;
					DUALPORT2.eids.Reset = 0;		// リセット信号の終了
					if( befEIDS.Reset ){			// Reset 0->1 Detect
						if( eidsarea.status.BIT.reset==0 ){
							eidsResetCounter = 5*5;		// タイマーのリセット
						}
						else {
							eidsResetCounter = 2;		// タイマーのリセット
						}
						eidsarea.status.BIT.reset = 1;	// リセット信号の送信

						DUALPORT2.eids.ResetStatus = 1;	// リセット信号の通知 2001/07/10 add

						eidsarea.Messages = 0x20;	// メッセージのＯＦＦ
						DUALPORT2.eids.Message = 0;		// メッセージの終了
					}
				}

				// リセット信号の終了チェック
				if( eidsResetCounter ){
					eidsResetCounter--;
					if( eidsResetCounter==0 ){		// リセット信号のタイムアップ
						if( DUALPORT2.eids.Reset==0 ){
							eidsarea.status.BIT.reset = 0;	// リセット信号のＯＦＦ
							DUALPORT2.eids.ResetStatus = 0;	// リセット信号の通知 2001/07/10 add
						}
						else {
							DUALPORT2.eids.Reset = 0;		// リセット信号の終了
							eidsResetCounter=1;				// 次回に再度チェックする
						}
					}
				}

				// メッセージと駅案内の確認
				if( DUALPORT2.eids.sendreq ){		// 送信要求あり
					DUALPORT2.eids.sendreq = 0;

					if( befEIDS.Message != DUALPORT2.eids.Message ){	// メッセージ変化あり 2001/07/04
						befEIDS.Message = DUALPORT2.eids.Message;
						if( befEIDS.Message!=0 ){
							eidsMessageCounter = 5*5;		// メッセージタイマーのリセット
							eidsarea.Messages = befEIDS.Message;	// メッセージの送信

							eidsarea.status.BIT.reset = 0;	// リセット信号のＯＦＦ
							DUALPORT2.eids.ResetStatus = 0;	// リセット信号の通知 2001/07/10 add
						}
					}

					eidsarea.Station = DUALPORT2.eids.Station;				// 駅名情報を送信
					eidsarea.Route = DUALPORT2.eids.Route;					// ルート情報を送信
				}

				// メッセージの終了チェック
				if( eidsMessageCounter ){
					eidsMessageCounter--;
					if( eidsMessageCounter==0 ){		// メッセージのタイムアップ
						eidsarea.Messages = 0x20;	// メッセージのＯＦＦ
						DUALPORT2.eids.Message = 0;		// メッセージの終了
						befEIDS.Message = 0;
					}
				}

				eidsarea.status.BIT.doorstatus = DUALPORT2.eids.Door;	// ドア情報を送信

//				dvauEIDSSend();			// ２００ｍＳ毎の送信起動
				dvauRTIFSend();
			}
		}
}

/************************************************************************

	Function name	<< dvauRTIF >>
					ＲＴＩＦメッセージ
	Syntax			dvauRTIF( void );
	Argument		none
	Author			TOA Corporation  h.nishiura
	Revision		a00 2020/09/16

*************************************************************************/

static const unsigned long crc32tab[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};


unsigned long crc32(unsigned char *p, int len)
{
	unsigned long crc = 0;
	int i;

	for( i=0 ; i<len; i++,p++)
	{
		crc = (crc>>8)^crc32tab[*p^(crc&0xff)];
	}
	return crc;
}

void dvauRTIFSend( void )
{
	int i;
	unsigned long crc;

	for( i = 0 ; i < sizeof(rtifarea) ; i++ )
	{
		rtifarea[i] = 0x00;
	}

	for( i = 0 ; i < sizeof(rtifexample) ; i++ )
	{
		rtifarea[i] = rtifexample[i];
	}
	crc = crc32(&rtifarea[1],26);

	rtifarea[sizeof(rtifexample)] = (unsigned char)((crc >> 24) & 0xff);
	rtifarea[sizeof(rtifexample)+1] = (unsigned char)((crc >> 16) & 0xff);
	rtifarea[sizeof(rtifexample)+2] = (unsigned char)((crc >> 8) & 0xff);
	rtifarea[sizeof(rtifexample)+3] = (unsigned char)((crc) & 0xff);

	sendp = (unsigned char *)&rtifarea;
	sendlen = sizeof(rtifexample)+12+115;	// +CRC12byte
	rtifarea_recvsize = sendlen;

	asci_1_tx_enable();		/*送信開始  RTS=ON*/

	sendcnt = 0;					/* 送信ポインター０クリア */
	sendcnt++;
	asci_1_transmit( *sendp++ );	/*最初の１バイトを送信*/
}

