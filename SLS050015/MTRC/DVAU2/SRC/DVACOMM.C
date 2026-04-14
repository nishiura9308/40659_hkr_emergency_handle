/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	編成間通信制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/07		K.Honda
				a02. 2002/12/17		h.nisiura
				a03. 2004/09/13		h.nisiura
				a04. 2007/01/11		h.nisiura
				a05. 2021/02/02 h.nisiura RTTF追加
				a06. 2023/03/07 h.nisiura RTTF+STN手動放送追加
				a07. 2024/01/19 h.nisiura RTTF+STN手動放送時外のTMSコードクリアを削除
				a08. 2024/02/01 h.nisiura RTTFのスペシャルメッセージコード変換削除
				a09, 2024/03/11 h,nishiura  RTTF用の放送中情報を追加
*********************************************************************/

#include	<string.h>
#include	"iodefine.h"
#include	"table.h"

#include	"timer.h"
#include	"commdef.h"
/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/** 送信エリア **/
struct	POLLING		sendarea;

unsigned char	recvActive;

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#define		STX			(0x02)				/* テキスト開始			*/
#define		ETX			(0x03)				/* テキスト終了			*/
#define		DLE			(0x10)				/* ＤＬＥ拡張			*/

#define	COMM_WAIT_STX		0		/* ＳＴＸ待ち */
#define	COMM_WAIT_ETX		1		/* ＥＴＸ待ち */
#define	COMM_WAIT_BCC		2		/* ＢＣＣ待ち */


#define	MAX_RECEIVE_TIME1	12		// 120mS
#define	MAX_RECEIVE_TIME2	15		// 150mS
#define	MAX_RECEIVE_TIME3	10		// 100msec.(１６両編成用）
#define	MAX_CHANGE_TIME		100		// 1000 mSec.

#define	POLLCHECK_TIME		1000		// 10sec.
#define	INITIAL_POLLCHECK_TIME		6000	// 60sec.
#define	COUPLINGCHECK_TIME		90		// 1sec.

// #define	MAX_RETRY			3
#define	MAX_RETRY			12

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/** 受信エリア **/
union RECEIVE_COMM {
	struct MCPSEND	mcparea;
	struct PAASEND	paaarea;
	struct DVAUSEND	dvauarea;
	struct POLLING  polling;
	struct DOWNLOAD	upload;
	struct DOWNLOAD_ACK	download_ack;
};


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

static	const unsigned short	adrTable[] = {
	 0,		// DVAU
	10,		// MCP-1
	11,		// MCP-2
	12,		// MCP-X
	 1,		// PAA-1
	13,		// RTTF
	 0,		// DVAU
	12,		// MCP-X
	10,		// MCP-1
	11,		// MCP-2
	12,		// MCP-X
	 2,		// PAA-2
	13,		// RTTF
	 0,		// DVAU
	12,		// MCP-X
	10,		// MCP-1
	11,		// MCP-2
	12,		// MCP-X
	 3,		// PAA-3
	13,		// RTTF
	 0,		// DVAU
	12,		// MCP-X
	10,		// MCP-1
	11,		// MCP-2
	12,		// MCP-X
	 4,		// PAA-4
	13,		// RTTF
	 0,		// DVAU
	12,		// MCP-X
	10,		// MCP-1
	11,		// MCP-2
	12,		// MCP-X
	 5,		// PAA-5
	13,		// RTTF
	 0,		// DVAU
	12,		// MCP-X
	10,		// MCP-1
	11,		// MCP-2
	12,		// MCP-X
	 6,		// PAA-6
	13,		// RTTF
	 0,		// DVAU
	12,		// MCP-X
	10,		// MCP-1
	11,		// MCP-2
	12,		// MCP-X
	 7,		// PAA-7
	13,		// RTTF
	 0,		// DVAU
	12,		// MCP-X
	10,		// MCP-1
	11,		// MCP-2
	12,		// MCP-X
	 8,		// PAA-8
	13,		// RTTF
};

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	struct	TIMER_CONTROL	timedata;

extern	void asci_0_Initialize( int baud, char data, char parity, char stop );
extern	void asci_0_tx_enable( void );
extern	void asci_0_tx_disable( char tend );
extern	void asci_0_transmit( unsigned char data );

extern int rtifmessage_flag;				/* RTIFバッファ状態フラグ 0:データ無し/1:データ有り/2:編成間通信へ送出済み */
extern unsigned char rtifmessage[256];		/* RTIFメッセージ */
extern int rtifmessage_size;				/* RTIFメッセージサイズ */

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	struct POLLING	sendpolling;
static	struct DVAUSEND	senddvau;

static	struct DOWNLOAD	senddownload;
static	struct DOWNLOAD_ACK	senddownload_ack;

static	char	adrcnt;					// ポーリングアドレスカウンター

static	unsigned char	recvbuff[512];		/*受信バッファー*/
static	unsigned short	recvlen;			/*受信バイト数*/
static	unsigned char	recvphase;			/*受信フェーズ*/
static	unsigned char	recvdle;			/*受信ＤＬＥフラグ*/
static	unsigned char	recvbcc;			/*受信ＢＣＣ計算エリア*/
static	unsigned char	sendbuff[512];		/*送信バッファー*/
static	unsigned short	sendlen;			/*送信バイト数*/
static	unsigned short	sendcnt;			/*送信カウンター*/

static	unsigned char	recvretry[32];		// 受信リトライカウンター（各機器別）

static	char	sendactive;					// 送信可能フラグ
static	char	changing;					// アクティブＤＶＡＵ切替中

static	unsigned char	mcpsw[4][3];		// 前回受信したＭＣＰのスイッチ情報

static	short	downloading;				// ファイルダウンロード中
static	short	uploading;					// ファイルデータ受付中

static	char	befActiveDVAU;				// 前回アクティブのとき１

static	unsigned short	befCarNo;			// 前回までの車両番号
static	char	befCouplingCheck;			// 前回の１６両編成チェックフラグ
static	char	otherDVAUPolling;			// １６両編成時の別車両へポーリングするとき
static	char	findOtherDVAUPollingCnt;	// 他の編成のポーリングを見つけたカウント 2004/09/13 h.nishiura １６両編成新チェックロジック用

static	int		max_receive_time;

static	unsigned char	rtifsendbuff[256];	// RTIF用送信バッファー


static	short dryContactCtrl_ph[8];					// ドライコントロールの管理フェーズ 0:スタンバイ/1:応答後
static	short dryContactCtrl_checkin[8];			// ドライコントロールの非常ハンドル受付済記録
static	short dryContactCtrl_polingrequest;			// ドライコントロールポーリングリクエスト

/*@@@E*/
/************************************************************************

	Function name	<< asci_0_send_ir >>
					送信割り込み処理
	Syntax			asci_0_send_ir( int tend );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/09/07	送信終了・継続を追加

*************************************************************************/
void asci_0_send_ir( char tend )
{
		if( sendcnt==sendlen ){	/*すべて送信したとき*/
			asci_0_tx_disable( tend );		/*受信へ切り替え*/
			return;
		}

		asci_0_transmit( sendbuff[sendcnt++] );	/*１バイトの送信*/
}

/************************************************************************

	Function name	<< dvauCommInitialize >>
					編成間通信初期設定
	Syntax			dvauCommInitialize( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
					a01 2001/01/22 車両番号の強制送信
					a02 004/09/13 h.nishiura １６両編成新チェックロジック用変数追加
*************************************************************************/
void dvauCommInitialize( void )
{
		int i;

		asci_0_Initialize( 9600, 8, 'E', 0 );

		max_receive_time = 1000;
		timedata.CommRecvTimer = max_receive_time;
		timedata.CommCheckTimer = INITIAL_POLLCHECK_TIME;
		timedata.carNoSendTimer = 500;		// ５秒間の送信開始
		sendcnt = 0;
		sendlen = 0;
		changing = 0;
		memset( recvretry, 0, sizeof(recvretry) );
		downloading = 0;
		uploading = 0;
		otherDVAUPolling=0;
		findOtherDVAUPollingCnt=0; // 2004/09/13 h.nishiura １６両編成新チェックロジック用
		befCarNo = 0x0FFFF;		// 前回の車両番号をあり得ない値にして強制送信する
		
		for( i = 0 ; i < 8 ; i++ )
		{
			dryContactCtrl_ph[i] = 0;		// ドライコントロールの管理フェーズ
			dryContactCtrl_checkin[i] = 0;	// ドライコントロールの非常ハンドル受付済記録
			dryContactCtrl_polingrequest = 1; // ドライコントロールポーリングリクエスト
		}
}

/************************************************************************

	Function name	<< dvauSend >>
					送信準備処理
	Syntax			dvauSend( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void dvauSend( unsigned char *senddata, int length )
{
		unsigned char	*p;			/*送信データポインター*/
		unsigned char	sendbcc;	/*送信ＢＣＣ計算エリア*/
		int		i;

		if( DUALPORT2.mainActive==0 ){
			return;
		}

		asci_0_tx_enable();		/*送信開始  RTS=ON*/

		sendcnt = 0;					/* 送信ポインター０クリア */
		sendlen = 0;					/* 送信バイト数０クリア*/

//		sendbuff[sendlen++] = 0;		// 送信前のダミーデータ（ＲＴＳ切り替えタイミング）
//		sendbuff[sendlen++] = 0;

			/* DLE+STX を設定*/
		sendbuff[sendlen++] = DLE;
		sendbuff[sendlen++] = STX;

		sendbcc = 0;					/* 送信ＢＣＣの計算クリア */
		p = senddata;					/* 送信エリアの内容をそのまま送信 */

		sendbcc ^= length;						/* 送信ＢＣＣの計算 */
		if( length==STX || length==ETX || length==DLE ){
			sendbuff[sendlen++] = DLE;			/*データのＤＬＥ拡張*/
			sendbuff[sendlen++] = length + 0x40;
		}
		else {
			sendbuff[sendlen++] = length;
		}

		for( i=0; i<length; i++, p++ ){
			sendbcc ^= *p;				/* 送信ＢＣＣの計算 */
			if( *p==STX || *p==ETX || *p==DLE ){
				sendbuff[sendlen++] = DLE;			/*データのＤＬＥ拡張*/
				sendbuff[sendlen++] = *p + 0x40;
			}
			else {
				sendbuff[sendlen++] = *p;
			}
		}
			/* DLE+ETX を設定*/
		sendbuff[sendlen++] = DLE;
		sendbuff[sendlen++] = ETX;
		sendbcc ^= ETX;			/* 送信ＢＣＣの計算*/

		if( sendbcc==STX || sendbcc==ETX || sendbcc==DLE ){
			sendbuff[sendlen++] = DLE;			/* ＢＣＣのＤＬＥ拡張 */
			sendbuff[sendlen++] = sendbcc + 0x40;
		}
		else {
			sendbuff[sendlen++] = sendbcc;		/* ＢＣＣの設定 */
		}

		sendbuff[sendlen++] = 0;		/*ＲＴＳ切り替えタイミング用ダミー*/

		recvActive = 0;

		asci_0_transmit( sendbuff[sendcnt++] );		/*最初の１バイトを送信*/
}

/************************************************************************

	Function name	<< dvauDownloadSend >>
					ダウンロードデータの送信処理
	Syntax			dvauDownloadSend( void )
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/10/10

*************************************************************************/
void dvauDownloadSend( void )
{
		int		i;

		adrcnt = 0;

		senddownload.adr = 0;	// 相手ＤＶＡＵのみ
		if( DUALPORT2.download_active==1 ){
			senddownload.kind = 'D';
		}
		else {
			senddownload.kind = 'E';
		}
		senddownload.seq = DUALPORT2.download_seq;
		for( i=0; i<16; i++ ){
			senddownload.filename[i] = DUALPORT2.download_filename[i];
		}
		senddownload.filedate = DUALPORT2.download_date;
		senddownload.filetime = DUALPORT2.download_time;
		senddownload.datasize = DUALPORT2.download_size;
		for( i=0; i<64; i++ ){
			senddownload.data[i] = DUALPORT2.download_data[i];
		}

		DUALPORT2.download_active = 0;		// メインＣＰＵからの要求を解除
		DUALPORT2.download_active = 0;

		downloading = 1;		// ダウンロードデータ処理中

		max_receive_time = 200;
		timedata.CommRecvTimer = max_receive_time;
		dvauSend( (unsigned char *)&senddownload, sizeof( senddownload ) );
}

/************************************************************************

	Function name	<< dvauRTTFSend >>
					ＲＴＴＦデータの送信処理
	Syntax			dvauRTTFSend( void )
	Argument		none
	Author			TOA Corporation  h.nishiura
	Revision		a00 2020/09/17
					a01 2024/11/05 RTTFの0x0d,0x0aの前後デリミタ削除
*************************************************************************/
void dvauRTTFSend( unsigned char *senddata, int length, unsigned char *senddata_rttf, int length_rttf)
{
		unsigned char	*p;			/*送信データポインター*/
		unsigned char	sendbcc;	/*送信ＢＣＣ計算エリア*/
		int		i;

		if( DUALPORT2.mainActive==0 ){
			return;
		}

		asci_0_tx_enable();		/*送信開始  RTS=ON*/

		sendcnt = 0;					/* 送信ポインター０クリア */
		sendlen = 0;					/* 送信バイト数０クリア*/

//		sendbuff[sendlen++] = 0;		// 送信前のダミーデータ（ＲＴＳ切り替えタイミング）
//		sendbuff[sendlen++] = 0;

			/* DLE+STX を設定*/
		sendbuff[sendlen++] = DLE;
		sendbuff[sendlen++] = STX;

		sendbcc = 0;					/* 送信ＢＣＣの計算クリア */
		p = senddata;					/* 送信エリアの内容をそのまま送信 */

		sendbcc ^= length;						/* 送信ＢＣＣの計算 */
		if( length==STX || length==ETX || length==DLE ){
			sendbuff[sendlen++] = DLE;			/*データのＤＬＥ拡張*/
			sendbuff[sendlen++] = length + 0x40;
		}
		else {
			sendbuff[sendlen++] = length;
		}

		for( i=0; i<length; i++, p++ ){
			sendbcc ^= *p;				/* 送信ＢＣＣの計算 */
			if( *p==STX || *p==ETX || *p==DLE ){
				sendbuff[sendlen++] = DLE;			/*データのＤＬＥ拡張*/
				sendbuff[sendlen++] = *p + 0x40;
			}
			else {
				sendbuff[sendlen++] = *p;
			}
		}
			/* DLE+ETX を設定*/
		sendbuff[sendlen++] = DLE;
		sendbuff[sendlen++] = ETX;
		sendbcc ^= ETX;			/* 送信ＢＣＣの計算*/

		if( sendbcc==STX || sendbcc==ETX || sendbcc==DLE ){
			sendbuff[sendlen++] = DLE;			/* ＢＣＣのＤＬＥ拡張 */
			sendbuff[sendlen++] = sendbcc + 0x40;
		}
		else {
			sendbuff[sendlen++] = sendbcc;		/* ＢＣＣの設定 */
		}

		recvActive = 0;

		if( length_rttf != 0 ){
/*			sendbuff[sendlen++] = 0x0d;
			sendbuff[sendlen++] = 0x0a;
			sendbuff[sendlen++] = 0x0d;
			sendbuff[sendlen++] = 0x0a;	*/

			for( i=0; i<length_rttf; i++ ){
				sendbuff[sendlen++] = senddata_rttf[i];
			}

/*			sendbuff[sendlen++] = 0x00;
			sendbuff[sendlen++] = 0x00;
			sendbuff[sendlen++] = 0x0d;
			sendbuff[sendlen++] = 0x0a;
			sendbuff[sendlen++] = 0x0d;
			sendbuff[sendlen++] = 0x0a;
			sendbuff[sendlen++] = 0x0d;
			sendbuff[sendlen++] = 0x0a;	*/
		}

//		sendbuff[sendlen++] = 0;		/*ＲＴＳ切り替えタイミング用ダミー*/

		max_receive_time = sendlen*12/10/10+3;
		timedata.CommRecvTimer = max_receive_time;

		recvActive = 0;

		asci_0_transmit( sendbuff[sendcnt++] );		/*最初の１バイトを送信*/
}

/************************************************************************

	Function name	<< dvauPadResetDryContactCmdSend >>
					PadReset & DryContactコマンドの送信処理
	Syntax			dvauPadResetDryContactCmdSend( void )
	Argument		none
	Author			TOA Corporation  h.nishiura
	Revision		a00 2025/06/23
*************************************************************************/
void dvauPadResetDryContactSend( unsigned char *senddata )
{
		unsigned char	*p;			/*送信データポインター*/
		unsigned char	sendbcc;	/*送信ＢＣＣ計算エリア*/
		int		i,length;

		if( DUALPORT2.mainActive==0 ){
			return;
		}

		asci_0_tx_enable();		/*送信開始  RTS=ON*/

		length = 10;
	
		sendcnt = 0;					/* 送信ポインター０クリア */
		sendlen = 0;					/* 送信バイト数０クリア*/

		/* DLE+STX を設定*/
		sendbuff[sendlen++] = DLE;
		sendbuff[sendlen++] = STX;
	
		sendbcc = 0;					/* 送信ＢＣＣの計算クリア */
		p = senddata;					/* 送信エリアの内容をそのまま送信 */

		/* データ長 [0] */
		sendbcc ^= length;						/* 送信ＢＣＣの計算 */
		if( length==STX || length==ETX || length==DLE ){
			sendbuff[sendlen++] = DLE;			/*データのＤＬＥ拡張*/
			sendbuff[sendlen++] = length + 0x40;
		}
		else {
			sendbuff[sendlen++] = length;
		}

		/* アドレス [1-2] */
		for( i=0; i<2; i++, p++ ){
			sendbcc ^= *p;				/* 送信ＢＣＣの計算 */
			if( *p==STX || *p==ETX || *p==DLE ){
				sendbuff[sendlen++] = DLE;			/*データのＤＬＥ拡張*/
				sendbuff[sendlen++] = *p + 0x40;
			}
			else {
				sendbuff[sendlen++] = *p;
			}
		}
	
		/* PadReset,DryContact[3-10] */
		for( i=0; i<8; i++ ){
			p = &DUALPORT2.PadRseetDryContact[i].BYTE;
			sendbcc ^= *p;				/* 送信ＢＣＣの計算 */
			if( *p==STX || *p==ETX || *p==DLE ){
				sendbuff[sendlen++] = DLE;			/*データのＤＬＥ拡張*/
				sendbuff[sendlen++] = *p + 0x40;
			}
			else {
				sendbuff[sendlen++] = *p;
			}
		}
	
		/* DLE+ETX を設定*/
		sendbuff[sendlen++] = DLE;
		sendbuff[sendlen++] = ETX;
		sendbcc ^= ETX;			/* 送信ＢＣＣの計算*/

		if( sendbcc==STX || sendbcc==ETX || sendbcc==DLE ){
			sendbuff[sendlen++] = DLE;			/* ＢＣＣのＤＬＥ拡張 */
			sendbuff[sendlen++] = sendbcc + 0x40;
		}
		else {
			sendbuff[sendlen++] = sendbcc;		/* ＢＣＣの設定 */
		}

		max_receive_time = sendlen*12/10/10+3;
		timedata.CommRecvTimer = max_receive_time;

		recvActive = 0;

		asci_0_transmit( sendbuff[sendcnt++] );		/*最初の１バイトを送信*/
}

/************************************************************************

	Function name	<< dvauReceive >>
					受信処理
	Syntax			dvauReceive( unsigned char *data, char len );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
					a01 2001/01/23 車両番号登録を許可
					a02 2004/09/13 h.nishiura
								   １６両編成新チェックロジック
									アドレスＥ／ＦのＤＶＡＵからは
									自車両と異なるCarNo.のみ取り込むように変更
*************************************************************************/
char dvauReceive( unsigned char *data, char len )
{
		union RECEIVE_COMM *p;
		unsigned char	no;
		int		i,j;

		p = (union RECEIVE_COMM *)data;

		if( len==sizeof(p->polling) 		// ポーリングデータか？
			&& ( p->polling.kind=='N' || p->polling.kind=='M' || p->polling.kind=='S' || p->polling.kind=='C' || p->polling.kind=='I' || p->polling.kind=='T' || p->polling.kind=='R' )){

#if 0		// 2004/09/13 h.nishiura １６両編成チェックロジック変更により破棄
			// 別車両からの応答
			if( DUALPORT2.couplingCheck==2 ){		// １６両編成チェック中
				DUALPORT2.couplingCheck = 4;			// 受け付けた
				otherDVAUPolling=0;
				adrcnt = sizeof(adrTable)/sizeof(adrTable[0]);	// 故障車のポーリングをおこなう
				recvretry[16]=0;
				DUALPORT2.dummy[16]=0;

				if( ((p->mcparea.adr>>4) & 0x0FFF) != (DUALPORT2.pollingData.carNo & 0x0FFF) ){
					DUALPORT2.answerData.otherCarNo = p->mcparea.adr>>4;
				}
			}
#endif		// 2004/09/13 h.nishiura １６両編成チェックロジック変更により破棄

			timedata.CommCheckTimer= POLLCHECK_TIME;

			if( DUALPORT2.couplingCheck==0 ){

//DUALPORT2.dummy[100]=p->polling.kind;
//DUALPORT2.dummy[101]=len;
//for( i = 0 ; i < len ; i++ ){
//DUALPORT2.dummy[102+i]=data[i];
//}
				switch( p->polling.kind ){
				case 'N':			// 通常ポーリング
					changing = 0;
					DUALPORT2.activeDVAU = 0;
					DUALPORT2.changeDVAU = 0;
					break;

				case 'M':			// 通常ポーリング
					if( ((p->mcparea.adr>>4) & 0x0FFF) == (DUALPORT2.pollingData.carNo & 0x0FFF) ){
						changing = 0;
						DUALPORT2.activeDVAU = 0;
						DUALPORT2.changeDVAU = 0;
					}else{
						// 2004/09/13 h.nishiura １６両編成新チェックロジック
						if( (DUALPORT2.nowSignal2 & 0x01)==1 ){	// HCR SW On
							DUALPORT2.couplingCheck = 4;			// 強制１６両モード
							otherDVAUPolling=0;
							adrcnt = sizeof(adrTable)/sizeof(adrTable[0]);	// 故障車のポーリングをおこなう
							recvretry[16]=0;
							DUALPORT2.dummy[16]=0;
							DUALPORT2.answerData.otherCarNo = (p->mcparea.adr>>4) & 0x0FFF;
						}else{
							findOtherDVAUPollingCnt++;
							if( findOtherDVAUPollingCnt > 3 ){
								changing = 0;
								DUALPORT2.activeDVAU = 0;
								DUALPORT2.changeDVAU = 0;
								timedata.CommCheckTimer= INITIAL_POLLCHECK_TIME/3;
							}
						}
						// 2004/09/13 h.nishiura １６両編成新チェックロジック
					}
					break;

				case 'S':			// 車両番号登録ポーリング
					changing = 0;
					DUALPORT2.activeDVAU = 0;
					DUALPORT2.changeDVAU = 0;
					DUALPORT2.pollingData.carNo = p->polling.carno;
					break;

				case 'C':			// アクティブ切り替え
					DUALPORT2.activeDVAUrequest = 1;		// アクティブ切り替え要求
					DUALPORT2.activeDVAU = 0;
					DUALPORT2.changeDVAU = 0;
					changing = 0;
					break;

				case 'I':			// テスト要求
					DUALPORT2.activeDVAUrequest = 2;		// アクティブ切り替え要求
					DUALPORT2.activeDVAU = 0;
					DUALPORT2.changeDVAU = 0;
					changing = 0;
					break;

				case 'T':			// テスト要求
					DUALPORT2.activeDVAUrequest = 3;		// アクティブ切り替え要求
					DUALPORT2.activeDVAU = 0;
					DUALPORT2.changeDVAU = 0;
					changing = 0;
					break;

				case 'R':			// RESET
					DUALPORT2.activeDVAUrequest = 4;		// アクティブ切り替え要求
					DUALPORT2.activeDVAU = 0;
					DUALPORT2.changeDVAU = 0;
					changing = 0;
					break;
				}

				// 非アクティブ側なので、スイッチ情報をクリアしておく
				DUALPORT2.answerData.dvau1 = 0;
				DUALPORT2.answerData.dvau2 = 0;
				DUALPORT2.answerData.dvau3 = 0;
				DUALPORT2.answerData.dvau_trouble = 0;
				for( i=0; i<4; i++ ){
					for( j=0; j<3; j++ ){
						DUALPORT2.answerData.mcpSW[i].BYTE.BYTE[j] = 0;
					}
				}

				// バックアップのためのデータ保存
				DUALPORT2.pollingData.dataID = p->polling.dataid;
				DUALPORT2.pollingData.nextStation = p->polling.nextstation;
				DUALPORT2.pollingData.destination = p->polling.destination;
				DUALPORT2.pollingData.routeCode = p->polling.route;
				for( i=0; i<9; i++ ){
					DUALPORT2.pollingData.voiceBus.BYTE.BYTE[i] = p->polling.voicebus[i];
				}
				for( i=0; i<31; i++ ){
					DUALPORT2.pollingData.mcpDisplay.BYTE.BYTE[i] = p->polling.display[i];
				}
				for( i=0; i<8; i++ ){
					DUALPORT2.pollingData.voiceLevel[i] = p->polling.vlevel[i];
				}
			}
		}

		switch( (p->mcparea.adr & 0x000F) ){
		case 0x0000:		/* 相手ＤＶＡＵ */
			if( len==sizeof(p->download_ack) ){	// ダウンロードに対する応答のとき
				timedata.CommCheckTimer= POLLCHECK_TIME;

				if( downloading ){
					if( senddownload.seq != p->download_ack.seq ){
						max_receive_time = 200;
						timedata.CommRecvTimer = max_receive_time;
						dvauSend( (unsigned char *)&senddownload, sizeof( senddownload ) );
						break;			// シーケンスが違うときは　再送信
					}
					if( p->download_ack.kind=='W' ){		//ウェイト時
						max_receive_time = 200;
						timedata.CommRecvTimer = max_receive_time;
						break;
					}
				}
				downloading = 0;
				if( p->download_ack.kind=='E' ){		//エラー時
					DUALPORT2.download_active = 3;
					DUALPORT2.download_active = 3;
					break;
				}
				if( DUALPORT2.download_active==1 || DUALPORT2.download_active==2 ){		// ダウンロードの要求があるときは、ここで送信する
					dvauDownloadSend();
					break;
				}
				break;
			}

			if( len==sizeof(p->upload) ){		// アップロード時
				timedata.CommCheckTimer= POLLCHECK_TIME;

				if( DUALPORT2.upload_active==0 ){
					for( i=0; i<16; i++ ){
						DUALPORT2.upload_filename[i] = p->upload.filename[i];
					}
					DUALPORT2.upload_date = p->upload.filedate;
					DUALPORT2.upload_time = p->upload.filetime;
					DUALPORT2.upload_size = p->upload.datasize;
					for( i=0; i<64; i++ ){
						DUALPORT2.upload_data[i] = p->upload.data[i];
					}
					DUALPORT2.upload_seq = p->upload.seq;
					if( p->upload.kind=='D' ){
						DUALPORT2.upload_active = 1;
					}
					else {
						DUALPORT2.upload_active = 2;
					}
					uploading = 1;
				}
				senddownload_ack.adr = 0;
				senddownload_ack.seq = p->upload.seq;
				senddownload_ack.kind = 'W';
				dvauSend( (unsigned char *)&senddownload_ack, sizeof( senddownload_ack ) );
				max_receive_time = 50;
				timedata.CommRecvTimer = max_receive_time;
				break;
			}

			if( len==sizeof(p->polling) ){		// ポーリング時
				if( downloading ){
					max_receive_time = 200;
					timedata.CommRecvTimer = max_receive_time;
					dvauSend( (unsigned char *)&senddownload, sizeof( senddownload ) );
					break;
				}
				if( DUALPORT2.download_active==1 || DUALPORT2.download_active==2 ){		// ダウンロードの要求があるときは、送信する
					dvauDownloadSend();
					break;
				}
				if( uploading ){
					break;
				}

				if( ((p->mcparea.adr>>4) & 0x0FFF) != (DUALPORT2.pollingData.carNo & 0x0FFF) ){
					break;			// 車両が違う
				}

				// ポーリングに対する相手ＤＶＡＵへの応答
				senddvau.adr = (DUALPORT2.pollingData.carNo << 4)& 0x0FFF0;
				if( DUALPORT2.nowSignal2 & 0x04 ){	// Odd Address
					senddvau.dataid = DUALPORT2.answerData.dataID[0];
					senddvau.version = DUALPORT2.answerData.version[0];
				}
				else {
					senddvau.dataid = DUALPORT2.answerData.dataID[1];
					senddvau.version = DUALPORT2.answerData.version[1];
				}
				senddvau.signal1 = DUALPORT2.nowSignal1;
				senddvau.signal2 = DUALPORT2.nowSignal2;
				senddvau.signal3 = DUALPORT2.nowSignal3;
				senddvau.trouble = DUALPORT2.nowTrouble;

				for( i=0; i<8; i++ ){
					senddvau.carNo[i] = DUALPORT2.tms_request.carInfo[i].carNo;
				}
				dvauSend( (unsigned char *)&senddvau, sizeof(senddvau) );
				break;
			}

			if( len==sizeof(p->dvauarea) ){
				downloading = 0;

				if( ((p->mcparea.adr>>4) & 0x0FFF) != (DUALPORT2.pollingData.carNo & 0x0FFF) ){
					break;
				}
				// 相手ＤＶＡＵからの応答データ
				/** 必要なデータをコピーする **/
				if( DUALPORT2.nowSignal2 & 0x04 ){	// Odd Address
					DUALPORT2.answerData.version[1] = p->dvauarea.version;
					DUALPORT2.answerData.dataID[1] = p->dvauarea.dataid;
				}
				else {
					DUALPORT2.answerData.version[0] = p->dvauarea.version;
					DUALPORT2.answerData.dataID[0] = p->dvauarea.dataid;
				}

				DUALPORT2.answerData.dvau1 = p->dvauarea.signal1;
				DUALPORT2.answerData.dvau2 = p->dvauarea.signal2;
				DUALPORT2.answerData.dvau3 = p->dvauarea.signal3;
				DUALPORT2.answerData.dvau_trouble = p->dvauarea.trouble;
				break;
			}
				/*受信バイト数が異常のときは処理しない*/
			break;

		case 0x000E:		/* 相手ＤＶＡＵ（１６編成奇数アドレス） */
			if( len==sizeof(p->polling) ){		// ポーリング時
				if( (DUALPORT2.nowSignal2 & 0x04)==0 ){	// Even Address
					break;
				}

				// ポーリングに対する相手ＤＶＡＵへの応答
				senddvau.adr = p->mcparea.adr;

				if( DUALPORT2.nowSignal2 & 0x04 ){	// Odd Address
					senddvau.dataid = DUALPORT2.answerData.dataID[0];
					senddvau.version = DUALPORT2.answerData.version[0];
				}
				else {
					senddvau.dataid = DUALPORT2.answerData.dataID[1];
					senddvau.version = DUALPORT2.answerData.version[1];
				}
				senddvau.signal1 = DUALPORT2.nowSignal1;
				senddvau.signal2 = DUALPORT2.nowSignal2;
				senddvau.signal3 = DUALPORT2.nowSignal3;
				senddvau.trouble = DUALPORT2.nowTrouble;

				for( i=0; i<8; i++ ){
					senddvau.carNo[i] = DUALPORT2.tms_request.carInfo[i].carNo;
				}

//senddvau.carNo[0] = 0x101;
//senddvau.carNo[1] = 0x102;
//senddvau.carNo[2] = 0x103;
//senddvau.carNo[3] = 0x104;
//senddvau.carNo[4] = 0x105;
//senddvau.carNo[5] = 0x106;
//senddvau.carNo[6] = 0x107;
//senddvau.carNo[7] = 0x108;

				dvauSend( (unsigned char *)&senddvau, sizeof(senddvau) );
				break;
			}

			if( len==sizeof(p->dvauarea) ){
				downloading = 0;

				DUALPORT2.answerData.otherCarNo = p->mcparea.adr>>4;

				DUALPORT2.answerData.otherCar[0].dvau1 = p->dvauarea.signal1;
				DUALPORT2.answerData.otherCar[0].dvau2 = p->dvauarea.signal2;
				DUALPORT2.answerData.otherCar[0].dvau3 = p->dvauarea.signal3;
				DUALPORT2.answerData.otherCar[0].dvau_trouble = p->dvauarea.trouble;

				if( p->dvauarea.carNo[0] != DUALPORT2.pollingData.carNo ){
					// 2004/09/13 h.nishiura 異なるCarNo.のみ取り込むように変更
					for( i=0; i<8; i++ ){
						DUALPORT2.answerData.carInfo[i].carNo = p->dvauarea.carNo[i];
					}
				}
				break;
			}
				/*受信バイト数が異常のときは処理しない*/
			break;
		case 0x000F:		/* 相手ＤＶＡＵ （１６両編成偶数アドレス）*/
			if( len==sizeof(p->polling) ){		// ポーリング時
				if( DUALPORT2.nowSignal2 & 0x04 ){	// Odd Address
					break;
				}

				// ポーリングに対する相手ＤＶＡＵへの応答
				senddvau.adr = p->mcparea.adr;

				if( DUALPORT2.nowSignal2 & 0x04 ){	// Odd Address
					senddvau.dataid = DUALPORT2.answerData.dataID[0];
					senddvau.version = DUALPORT2.answerData.version[0];
				}
				else {
					senddvau.dataid = DUALPORT2.answerData.dataID[1];
					senddvau.version = DUALPORT2.answerData.version[1];
				}
				senddvau.signal1 = DUALPORT2.nowSignal1;
				senddvau.signal2 = DUALPORT2.nowSignal2;
				senddvau.signal3 = DUALPORT2.nowSignal3;
				senddvau.trouble = DUALPORT2.nowTrouble;

				for( i=0; i<8; i++ ){
					senddvau.carNo[i] = DUALPORT2.tms_request.carInfo[i].carNo;
				}

//senddvau.carNo[0] = 0x201;
//senddvau.carNo[1] = 0x202;
//senddvau.carNo[2] = 0x203;
//senddvau.carNo[3] = 0x204;
//senddvau.carNo[4] = 0x205;
//senddvau.carNo[5] = 0x206;
//senddvau.carNo[6] = 0x207;
//senddvau.carNo[7] = 0x208;

				dvauSend( (unsigned char *)&senddvau, sizeof(senddvau) );
				break;
			}

			if( len==sizeof(p->dvauarea) ){
				downloading = 0;

				DUALPORT2.answerData.otherCarNo = p->mcparea.adr>>4;

				DUALPORT2.answerData.otherCar[1].dvau1 = p->dvauarea.signal1;
				DUALPORT2.answerData.otherCar[1].dvau2 = p->dvauarea.signal2;
				DUALPORT2.answerData.otherCar[1].dvau3 = p->dvauarea.signal3;
				DUALPORT2.answerData.otherCar[1].dvau_trouble = p->dvauarea.trouble;

				if( p->dvauarea.carNo[0] != DUALPORT2.pollingData.carNo ){
					// 2004/09/13 h.nishiura 異なるCarNo.のみ取り込むように変更
					for( i=0; i<8; i++ ){
						DUALPORT2.answerData.carInfo[i].carNo = p->dvauarea.carNo[i];
					}
				}
				break;
			}
				/*受信バイト数が異常のときは処理しない*/
			break;

		case 0x0001:		/* PAA-1 */
		case 0x0002:		/* PAA-2 */
		case 0x0003:		/* PAA-3 */
		case 0x0004:		/* PAA-4 */
		case 0x0005:		/* PAA-5 */
		case 0x0006:		/* PAA-6 */
		case 0x0007:		/* PAA-7 */
		case 0x0008:		/* PAA-8 */
			if( len!=sizeof(p->paaarea) ){
				/*受信バイト数が異常のときは処理しない*/
				return 0;
			}

			no = p->paaarea.adr & 0x000F;
			if( ((p->mcparea.adr>>4) & 0x0FFF) != (DUALPORT2.pollingData.carNo & 0x0FFF) ){
				no += 8;
			}
			else {
				DUALPORT2.answerData.version[no+1] = p->paaarea.version;
				DUALPORT2.answerData.dataID[no+1] = p->paaarea.dataid;
			}
			DUALPORT2.answerData.rscSW[no-1].BYTE = p->paaarea.rscsw;
			DUALPORT2.answerData.trouble[no-1].BYTE = p->paaarea.trouble;
			break;

		case 0x000A:		/* MCP-1 */
		case 0x000B:		/* MCP-2 */
			if( len!=sizeof(p->mcparea) ){
				/*受信バイト数が異常のときは処理しない*/
				return 0;
			}

			no = p->mcparea.adr & 0x000F;
			if( ((p->mcparea.adr>>4) & 0x0FFF) != (DUALPORT2.pollingData.carNo & 0x0FFF) ){
				no += 2;
			}
			else {
				DUALPORT2.answerData.version[no+1] = p->mcparea.version;
				DUALPORT2.answerData.dataID[no+1] = p->mcparea.dataid;
			}
			if( mcpsw[no-10][0] != p->mcparea.mcpsw[0] ){
				mcpsw[no-10][0] = (unsigned char)p->mcparea.mcpsw[0];
				DUALPORT2.answerData.mcpSW[no-10].BYTE.BYTE[2] |= (unsigned char)p->mcparea.mcpsw[0];
			}
			if( mcpsw[no-10][1] != p->mcparea.mcpsw[1] ){
				mcpsw[no-10][1] = (unsigned char)p->mcparea.mcpsw[1];
				DUALPORT2.answerData.mcpSW[no-10].BYTE.BYTE[1] |= (unsigned char)p->mcparea.mcpsw[1];
			}
			if( mcpsw[no-10][2] != p->mcparea.mcpsw[2] ){
				mcpsw[no-10][2] = (unsigned char)p->mcparea.mcpsw[2];
				DUALPORT2.answerData.mcpSW[no-10].BYTE.BYTE[0] |= (unsigned long)p->mcparea.mcpsw[2];
			}
			break;
		}

		return 0;
}


/************************************************************************

	Function name	<< asci_0_recv_ir >>
					受信割り込み処理
	Syntax			asci_0_recv_ir( char data, char status );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void asci_0_recv_ir( char data, char status )
{
/*.	---------------------　処理詳細説明  ----------------------------	*/
/*  フェーズ毎に受信データを取り込む									*/
/*. ------------------------------------------------------------------  */

		timedata.CommRecvTimer = max_receive_time;

		if( status & 0x38 ){		// Framing Error, Parity Error?
			return;
		}

		if( data == DLE ){		/* ＤＬＥ受信 */
			recvdle = 1;
			return;
		}
		if( data == STX ){		/* ＳＴＸ受信 */
			if( recvdle ){		/*  DLE+STX */
				recvdle = 0;
				recvphase = COMM_WAIT_ETX;
				recvlen = 0;
				recvbcc = 0;
			}
			return;
		}

		switch( recvphase ){
		case COMM_WAIT_STX:		/* ＳＴＸ待ち */
			break;

		case COMM_WAIT_ETX:
			if( recvdle ){		/* ＤＬＥ処理 */
				recvdle = 0;
				if( data == ETX ){		/* ＥＴＸの受信 ( DLE+ETX ) */
					recvbcc ^= data;
					recvphase = COMM_WAIT_BCC;
					break;
				}
				data = data - 0x40;	/* データのＤＬＥ拡張 */
			}

			if( recvlen<sizeof(recvbuff) ){
				recvbcc ^= data;
				recvbuff[recvlen++] = data;
			}
			else {
				recvphase = COMM_WAIT_STX;
			}
			break;

		case COMM_WAIT_BCC:
			if( recvdle ){		/* ＤＬＥ処理 */
				recvdle = 0;
				data = data - 0x40;	/* データのＤＬＥ拡張 */
			}
			recvbcc ^= data;
			if( recvbcc==0 ){
				if( recvlen==recvbuff[0]+1 ){
					dvauReceive( &recvbuff[1], recvlen-1 );
				}
			}

			if( changing==0 ){
				recvActive = 1;			/* 受信完了 */
			}

			recvphase = COMM_WAIT_STX;
			break;
		}
}

/************************************************************************

	Function name	<< FilterPriorityMCPNo >>
					送受信制御
	Syntax			r=FilterPriorityMCPNo( adr );
	Argument		unsigned short adr	i : ユニットアドレス番号
					unsigned short r	o : 優先MCPユニットアドレス番号
					Author			TOA Corporation  h.nishiura
	Revision		a00 2002/12/17
					a01 2007/01/11  mcp1とmcp2を入れ替える
*************************************************************************/
unsigned short FilterPriorityMCPNo( unsigned short adr )
{

	if( adr == 12 ){
		/*. 優先ＭＣＰ仮アドレス */
		if( (DUALPORT2.nowSignal2 & 0x04)==0 ){	// Even Address
			if( (DUALPORT2.nowSignal2 & 0x01)==1 ){	// HCR SW On
				return (unsigned short)11;			// MCP-2
			}else{
				return (unsigned short)10;			// MCP-1
			}
		}else{
												// Odd Address
			if( (DUALPORT2.nowSignal2 & 0x01)==1 ){	// HCR SW On
				return (unsigned short)10;			// MCP-1
			}else{
				return (unsigned short)11;			// MCP-2
			}
		}
	}else{
		/*. 通常アドレス */
		return adr;
	}

}

/************************************************************************

	Function name	<< PADResetCtrl >>
					PADリセット制御
	Syntax			r=CheckPADReset();
	Argument		unsigned cher r	o 	  : PADリセット要求 1:変化あり / 0:変化なし
					Author			TOA Corporation  h.nishiura
	Revision		a00 2025/06/23
*************************************************************************/
unsigned char PADResetDrycontCtrl()
{
	int i;
	unsigned char pad_reset[8];
	unsigned char ret = 0;
	
	for( i = 0 ; i < 8 ; i++ )
	{
		pad_reset[i] = DUALPORT2.tms_request.PadReset[i];
		DUALPORT2.tms_request.PadReset[i]= 0;
	}
	for( i = 0 ; i < 8 ; i++ )
	{
		pad_reset[i] |= DUALPORT2.mcpMode.PadReset[i];
		DUALPORT2.mcpMode.PadReset[i]= 0;
	}

	for( i = 0 ; i < 8 ; i++ )
	{
//		if( ((( DUALPORT2.PadRseetDryContact[i].BYTE & 0x3f ) ^ pad_reset[i]) & pad_reset[i] ) != 0 )
		if( (( DUALPORT2.PadRseetDryContact[i].BYTE & 0x3f ) ^ pad_reset[i]) != 0 )
		{
			ret = 1;
		}
	}
	
	for( i = 0 ; i < 8 ; i++ )
	{
		DUALPORT2.PadRseetDryContact[i].BYTE = pad_reset[i] |( DUALPORT2.PadRseetDryContact[i].BYTE & 0xc0 );
	}
	
	return ret;
}

/************************************************************************

	Function name	<< DryContactCtrl >>
					ドライコンタクト制御
	Syntax			r=CheckPADReset();
	Argument		unsigned cher r	o 	  : ドライコンタクト状態 1:変化あり / 0:変化なし
					Author			TOA Corporation  h.nishiura
	Revision		a00 2025/06/23
*************************************************************************/
unsigned char DryContactCtrl()
{
	int i;
	unsigned char diffbit,ret = 0;
	
	
	for( i = 0 ; i < 8 ; i++ )
	{
		switch(dryContactCtrl_ph[i])
		{
			case 0:
				// スタンバイ
				if( DUALPORT2.answerData.rscSW[i].BYTE == 0 )
				{	// 非常ハンドル操作がなければDrycontactがopen(1)ならclose(0)へ
					if( DUALPORT2.PadRseetDryContact[i].BIT.DryContact1 == 1 )
					{
						DUALPORT2.PadRseetDryContact[i].BIT.DryContact1 = 0;
						ret = 1;
					}
					break;
				}
				else
				{
					dryContactCtrl_ph[i] = 1;
				}
			case 1:
				diffbit = DUALPORT2.nowEMGHL[i] ^ DUALPORT2.answerData.rscSW[i].BYTE;
				if ( diffbit == 0 )
				{
					/* トークバックとハンドルリクエストが一致 */
					if ( DUALPORT2.answerData.rscSW[i].BYTE != 0 ) /* ハンドルリクエスト有り */
					{
						if( DUALPORT2.PadRseetDryContact[i].BIT.DryContact1 == 1 )	/* Drycontactがopen(1)ならclose(0)へ */
						{
							DUALPORT2.PadRseetDryContact[i].BIT.DryContact1 = 0;
							ret = 1;
							dryContactCtrl_checkin[i] |= DUALPORT2.nowEMGHL[i];		/* 受付済み記録 */
						}
					}
					else	/* ハンドルリクエスト無し */
					{
						dryContactCtrl_checkin[i] = 0; /* 受付済み記録クリア */
						dryContactCtrl_ph[i] = 0;	/* スタンバイへ */
					}
				}
				else
				{	
					/* トークバックとハンドルリクエストが不一致 */
					/* 現在のトークバックを除く受付済み記録でマスク */
					diffbit = diffbit & ~(dryContactCtrl_checkin[i] & ~DUALPORT2.nowEMGHL[i] );
					if( diffbit != 0)
					{	/* 受付済み以外で差がある */
						if( DUALPORT2.PadRseetDryContact[i].BIT.DryContact1 == 0 )	/* Drycontactがclose(0)ならopen(1)へ */
						{
							DUALPORT2.PadRseetDryContact[i].BIT.DryContact1 = 1;
							ret = 1;
						}
					}
					else
					{	/* 受付済み以外で差がない */
						if( DUALPORT2.PadRseetDryContact[i].BIT.DryContact1 == 1 )	/* Drycontactがopen(1)ならclose(0)へ */
						{
							DUALPORT2.PadRseetDryContact[i].BIT.DryContact1 = 0;
							ret = 1;
							dryContactCtrl_checkin[i] |= DUALPORT2.nowEMGHL[i];		/* 受付済み記録 */
						}
					}
				}
				break;
		}
	}
	
	if (dryContactCtrl_polingrequest == 1 )
	{
		dryContactCtrl_polingrequest = 0;
		ret = 1;
	}
	
	return ret;
}

/************************************************************************

	Function name	<< dvaCommProc >>
					送受信制御
	Syntax			dvaCommProc( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
					a01 2001/04/18	１６両編成間のポーリングタイミング変更
					a02 2002/12/17	優先ＭＣＰ処理追加
					a03 2004/09/13	h.nishiura
									優先ＭＣＰには故障車情報を送らないように修正
									１６両編成新チェックロジック用変数クリア追加
					a05. 2021/02/02 h.nisiura RTTF追加
					a06. 2023/03/07 h.nisiura RTTF+STN手動放送追加
					a07. 2024/01/19 h.nisiura RTTF+STN手動放送時外のTMSコードクリアを削除
					a08. 2024/02/01 h.nisiura RTTFのスペシャルメッセージコード変換削除
					a09. 2024/03/11 h,nishiura  RTTF用の放送中情報を追加
*************************************************************************/
void dvaCommProc( void )
{
		int		i;
		int		timevalue;

			// 受信完了時の処理
		if( recvActive ){
			recvActive = 0;

			if( otherDVAUPolling==0 ){
				switch( FilterPriorityMCPNo(adrTable[adrcnt]) ){
				case 0:		// DVAU
					DUALPORT2.tms_answer.dvaCommError.BIT.dvauCommError1 = 0;
					DUALPORT2.tms_answer.dvaCommError.BIT.dvauCommError2 = 0;
					recvretry[0] = 0;
					break;

				case 1:		// PAA1
					if(DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError1 == 1)
					{
						dryContactCtrl_polingrequest = 1;
					}
					DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError1 = 0;
					recvretry[1] = 0;
					break;
				case 2:		// PAA2
					if(DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError2 == 1)
					{
						dryContactCtrl_polingrequest = 1;
					}
					DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError2 = 0;
					recvretry[2] = 0;
					break;
				case 3:		// PAA3
					if(DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError3 == 1)
					{
						dryContactCtrl_polingrequest = 1;
					}
					DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError3 = 0;
					recvretry[3] = 0;
					break;
				case 4:		// PAA4
					if(DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError4 == 1)
					{
						dryContactCtrl_polingrequest = 1;
					}
					DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError4 = 0;
					recvretry[4] = 0;
					break;
				case 5:		// PAA5
					if(DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError5 == 1)
					{
						dryContactCtrl_polingrequest = 1;
					}
					DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError5 = 0;
					recvretry[5] = 0;
					break;
				case 6:		// PAA6
					if(DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError6 == 1)
					{
						dryContactCtrl_polingrequest = 1;
					}
					DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError6 = 0;
					recvretry[6] = 0;
					break;
				case 7:		// PAA7
					if(DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError7 == 1)
					{
						dryContactCtrl_polingrequest = 1;
					}
					DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError7 = 0;
					recvretry[7] = 0;
					break;
				case 8:		// PAA8
					if(DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError8 == 1)
					{
						dryContactCtrl_polingrequest = 1;
					}
					DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError8 = 0;
					recvretry[8] = 0;
					break;

				case 10:	// MCP1
					DUALPORT2.tms_answer.dvaCommError.BIT.mcpCommError1 = 0;
					recvretry[10] = 0;
					break;
				case 11:	// MCP2
					DUALPORT2.tms_answer.dvaCommError.BIT.mcpCommError2 = 0;
					recvretry[11] = 0;
					break;
				}
				sendactive = 1;
			}
			else {
				if( adrTable[adrcnt]==0 ){
					recvretry[16] = 0;
					DUALPORT2.dummy[16]=0;
				}
				DUALPORT2.dummy[16+FilterPriorityMCPNo(adrTable[adrcnt])]=0;
				sendactive = 1;
			}
		}
		else if( timedata.CommRecvTimer==0 ){
			// タイムアウト時
			sendactive = 1;

			if( DUALPORT2.activeDVAU && changing==0 && (DUALPORT2.couplingCheck==0 || DUALPORT2.couplingCheck==4) ){

				if( otherDVAUPolling==0 ){
					if( FilterPriorityMCPNo(adrTable[adrcnt])>=0 && FilterPriorityMCPNo(adrTable[adrcnt])<=11 ){
						if( recvretry[FilterPriorityMCPNo(adrTable[adrcnt])]<MAX_RETRY ){
							recvretry[FilterPriorityMCPNo(adrTable[adrcnt])]++;

							if( downloading ){		// ダウンロード中であれば
								max_receive_time = 200;
								timedata.CommRecvTimer = max_receive_time;
								dvauSend( (unsigned char *)&senddownload, sizeof( senddownload ) );
								return;
							}
						}
						else {
							if( downloading ){		// ダウンロード中であれば
								DUALPORT2.download_active = 3;
								DUALPORT2.download_active = 3;
								downloading = 0;
								return;
							}

							switch( FilterPriorityMCPNo(adrTable[adrcnt]) ){
							case 0:		// DVAU
								if( DUALPORT2.nowSignal2 & 0x04 ){	// Odd Address
									DUALPORT2.tms_answer.dvaCommError.BIT.dvauCommError1 = 0;
									DUALPORT2.tms_answer.dvaCommError.BIT.dvauCommError2 = 1;
								}
								else {
									DUALPORT2.tms_answer.dvaCommError.BIT.dvauCommError1 = 1;
									DUALPORT2.tms_answer.dvaCommError.BIT.dvauCommError2 = 0;
								}
								DUALPORT2.answerData.dvau1 = 0;
								DUALPORT2.answerData.dvau2 = 0;
								DUALPORT2.answerData.dvau3 = 0;
								DUALPORT2.answerData.dvau_trouble = 0;
								break;
							case 1:		// PAA1
								DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError1 = 1;
								DUALPORT2.answerData.rscSW[0].BYTE = 0;
								DUALPORT2.answerData.trouble[0].BYTE = 0;
								break;
							case 2:		// PAA2
								DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError2 = 1;
								DUALPORT2.answerData.rscSW[1].BYTE = 0;
								DUALPORT2.answerData.trouble[1].BYTE = 0;
								break;
							case 3:		// PAA3
								DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError3 = 1;
								DUALPORT2.answerData.rscSW[2].BYTE = 0;
								DUALPORT2.answerData.trouble[2].BYTE = 0;
								break;
							case 4:		// PAA4
								DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError4 = 1;
								DUALPORT2.answerData.rscSW[3].BYTE = 0;
								DUALPORT2.answerData.trouble[3].BYTE = 0;
								break;
							case 5:		// PAA5
								DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError5 = 1;
								DUALPORT2.answerData.rscSW[4].BYTE = 0;
								DUALPORT2.answerData.trouble[4].BYTE = 0;
								break;
							case 6:		// PAA6
								DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError6 = 1;
								DUALPORT2.answerData.rscSW[5].BYTE = 0;
								DUALPORT2.answerData.trouble[5].BYTE = 0;
								break;
							case 7:		// PAA7
								DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError7 = 1;
								DUALPORT2.answerData.rscSW[6].BYTE = 0;
								DUALPORT2.answerData.trouble[6].BYTE = 0;
								break;
							case 8:		// PAA8
								DUALPORT2.answerData.rscSW[7].BYTE = 0;
								DUALPORT2.answerData.trouble[7].BYTE = 0;
								DUALPORT2.tms_answer.dvaCommError.BIT.paaCommError8 = 1;
								break;
							case 10:	// MCP1
								DUALPORT2.tms_answer.dvaCommError.BIT.mcpCommError1 = 1;
								break;
							case 11:	// MCP2
								DUALPORT2.tms_answer.dvaCommError.BIT.mcpCommError2 = 1;
								break;
							}
						}
					}
				}
				else {
					DUALPORT2.dummy[16+FilterPriorityMCPNo(adrTable[adrcnt])]++;

					if( adrTable[adrcnt]==0 ){
						if( recvretry[16]<MAX_RETRY*2 ){
							recvretry[16]++;
						}
						else {
							DUALPORT2.couplingCheck = 0;		// 16両編成切断
							findOtherDVAUPollingCnt=0;			// 2004/09/13 h.nishiura １６両編成新チェックロジック用追加

						}
					}
				}
			}

			if( changing ){
				changing = 0;
				DUALPORT2.changeDVAU = 0;
			}
		}

if( downloading )
DUALPORT2.dummy[4]=1;
else
DUALPORT2.dummy[4]=0;
if( uploading )
DUALPORT2.dummy[5]=1;
else
DUALPORT2.dummy[5]=0;


		if( uploading ){
			recvActive = 0;
			sendactive = 0;
			if( DUALPORT2.upload_active==0 && timedata.CommRecvTimer<48 ){
				senddownload_ack.kind = 'D';
				uploading = 0;
			}
			else if( DUALPORT2.upload_active==3 ){
				senddownload_ack.kind = 'E';
				DUALPORT2.upload_active=0;
				uploading = 0;
			}
			else if( timedata.CommRecvTimer!=0 ){
				return;
			}
			else {
				senddownload_ack.kind = 'W';
			}
			dvauSend( (unsigned char *)&senddownload_ack, sizeof( senddownload_ack ) );
			max_receive_time = 50;
			timedata.CommRecvTimer = max_receive_time;
			timedata.CommCheckTimer= POLLCHECK_TIME;
			return;
		}


		/*すべて送信したとき　判断する */
		if( DUALPORT2.activeDVAU==0 ){
			if( befActiveDVAU ){			// 前回アクティブか？
				timedata.CommCheckTimer= POLLCHECK_TIME;	// アクティブＤＶＡＵ監視タイマー開始
				befActiveDVAU=0;
			}
			else if( timedata.CommCheckTimer==0 ){		// アクティブＤＶＡＵ監視のタイムアウト時
				timedata.CommCheckTimer= POLLCHECK_TIME;
				DUALPORT2.activeDVAUrequest = 5;			// メインＣＰＵへアクティブになる要求を発行
				DUALPORT2.changeDVAU = 0;
				DUALPORT2.couplingCheck = 0;				// １６両編成終了
				otherDVAUPolling=0;
				findOtherDVAUPollingCnt=0;					// 2004/09/13 h.nishiura １６両編成新チェックロジック用追加
			}
			sendactive = 0;
		}
		else {
			befActiveDVAU = 1;
		}

		if( DUALPORT2.mainActive!=2 ){	// RESET
			if( downloading ){		// ダウンロード中であれば
				return;
			}
		}


		if( sendactive && changing==0 ){
			if( DUALPORT2.activeDVAU && DUALPORT2.activeDVAUrequest==0 ){	// 切り替え中でないこと
				sendactive = 0;

				// アクティブＤＶＡＵ側であれば、次の送信をおこなう
				if( DUALPORT2.download_active==1 || DUALPORT2.download_active==2 ){		// ダウンロード要求であれば
					dvauDownloadSend();			//ダウンロードデータを送信する
					return;
				}


				// １６両編成チェック処理
				// 連結チェックが開始したときのチェック
				if( DUALPORT2.couplingCheck!=befCouplingCheck ){
					befCouplingCheck = DUALPORT2.couplingCheck;

					if( DUALPORT2.couplingCheck==1 ){		// 連結開始時
						DUALPORT2.couplingCheck=2;			// 連結開始確認
						timedata.CommCheckTimer= COUPLINGCHECK_TIME;
					}
					if( DUALPORT2.couplingCheck==2 ){		// 連結開始確認時
						timedata.CommCheckTimer= COUPLINGCHECK_TIME;
					}
				}
				else if( timedata.CommCheckTimer==0 ){
					timedata.CommCheckTimer= POLLCHECK_TIME;
					if( DUALPORT2.couplingCheck==2 ){
						DUALPORT2.couplingCheck=3;		// 通信タイムアウト（１６両なし）
														// メインＣＰＵの処理待ち
					}
				}

				if( DUALPORT2.couplingCheck==1 ){		// １６両編成開始時は以下の処理はしない
					return;
				}
				if( DUALPORT2.couplingCheck==2 ){		// １６両編成チェック中は無視
					return;
				}
				if( DUALPORT2.couplingCheck==3 ){		// １６両編成なし時、メインＣＰＵが受け付けるまで無視
					return;
				}

				if( DUALPORT2.couplingCheck==0 ){		// １６両編成でないときは、エリアをクリアする
					for( i=0; i<8; i++ ){
						DUALPORT2.answerData.rscSW[i+8].BYTE = 0;
					}
					DUALPORT2.answerData.otherCarNo = 0;
					for( i=0; i<2; i++ ){
						DUALPORT2.answerData.otherCar[i].dvau1 = 0;
						DUALPORT2.answerData.otherCar[i].dvau2 = 0;
						DUALPORT2.answerData.otherCar[i].dvau3 = 0;
						DUALPORT2.answerData.otherCar[i].dvau_trouble = 0;
					}
				}


				if( DUALPORT2.couplingCheck && otherDVAUPolling ){	// 故障車ＤＶＡＵへのポーリング時
					if( adrTable[adrcnt]==0 ){			// 故障車ＤＶＡＵへのポーリング時
						if( (sendpolling.adr & 0x000F)==0x000F ){	// 故障車ＤＶＡＵ偶数アドレス側にする
							adrcnt++;			// アドレステーブルにしたがって、順次要求する
						}
					}
					else {
						adrcnt++;			// アドレステーブルにしたがって、順次要求する
					}
				}
				else {
					adrcnt++;			// アドレステーブルにしたがって、順次要求する
				}
				if( adrcnt>=sizeof(adrTable)/sizeof(adrTable[0]) ){
					adrcnt=0;
					if( DUALPORT2.couplingCheck==4 ){
						otherDVAUPolling ^=1;
					}
					else {
						otherDVAUPolling =0;
					}
				}

				if( DUALPORT2.couplingCheck && otherDVAUPolling ){	// 故障車ＤＶＡＵへのポーリング時
					if( adrTable[adrcnt]==0 ){			// 故障車ＤＶＡＵへのポーリング時
						if( (sendpolling.adr & 0x000F)==0x000E ){
							sendpolling.adr = 0x000F
								| ((DUALPORT2.answerData.otherCarNo << 4)& 0x0FFF0);
						}
						else {							// 偶数アドレスＤＶＡＵへのポーリング
							sendpolling.adr = 0x000E
								| ((DUALPORT2.answerData.otherCarNo << 4)& 0x0FFF0);
						}
					}
					else {
						sendpolling.adr = (adrTable[adrcnt] & 0x000F)
								| ((DUALPORT2.answerData.otherCarNo << 4)& 0x0FFF0);
					}
DUALPORT2.dummy[32] = (sendpolling.adr>>8) & 0x0FF;
DUALPORT2.dummy[33] = sendpolling.adr & 0x0FF;
				}
				else {
					sendpolling.adr = (adrTable[adrcnt] & 0x000F)
								| ((DUALPORT2.pollingData.carNo << 4)& 0x0FFF0);
				}

				changing = 0;
//				sendpolling.kind = 'N';		// 通常送信
				sendpolling.kind = 'M';		// 通常送信
				if( DUALPORT2.changeDVAU==1 ){
					sendpolling.kind = 'C';		// アクティブ切り替え
					changing = 1;
					adrcnt=0;
					sendpolling.adr = (adrTable[adrcnt] & 0x000F)
							| ((DUALPORT2.pollingData.carNo << 4)& 0x0FFF0);
				}
				else if( DUALPORT2.changeDVAU==2 ){
					sendpolling.kind = 'I';		// テストモード切り替え
					changing = 1;
					adrcnt=0;
					sendpolling.adr = (adrTable[adrcnt] & 0x000F)
							| ((DUALPORT2.pollingData.carNo << 4)& 0x0FFF0);
				}
				else if( DUALPORT2.changeDVAU==3 ){
					sendpolling.kind = 'T';		// テストモード切り替え
					changing = 1;
					adrcnt=0;
					sendpolling.adr = (adrTable[adrcnt] & 0x000F)
							| ((DUALPORT2.pollingData.carNo << 4)& 0x0FFF0);
				}
				else if( DUALPORT2.mainActive==2 ){	// RESET
					sendpolling.kind = 'R';
					changing = 1;
					adrcnt=0;
					sendpolling.adr = (adrTable[adrcnt] & 0x000F)
							| ((DUALPORT2.pollingData.carNo << 4)& 0x0FFF0);
				}
				else {
					if( DUALPORT2.couplingCheck==0 || otherDVAUPolling==0 ){	// 故障車ＤＶＡＵへのポーリング時
						if( DUALPORT2.pollingData.carNo != befCarNo ){
							befCarNo = DUALPORT2.pollingData.carNo;
							timedata.carNoSendTimer = 500;		// ５秒間の送信開始
							sendpolling.kind = 'S';
							sendpolling.adr = (adrTable[adrcnt] & 0x000F)
									| ((DUALPORT2.pollingData.carNo << 4)& 0x0FFF0);
						}
						else if( timedata.carNoSendTimer ){
							sendpolling.kind = 'S';
							sendpolling.adr = (adrTable[adrcnt] & 0x000F)
									| ((DUALPORT2.pollingData.carNo << 4)& 0x0FFF0);
						}
					}
				}

				if( adrTable[adrcnt] == 12 ){
					if( (PADResetDrycontCtrl() == 1 ) || ( DryContactCtrl() == 1 ) )
					{	/* Padリセットまたはドライコンタクト制御が必要な場合は、
						PadReset & DryContactコマンドの送信を優先 */
						dvauPadResetDryContactSend((unsigned char *)&sendpolling);
						dryContactCtrl_polingrequest = 0;
						return;
					}
					else
					{
						/* 優先MCP-Xをポーリング */
						sendpolling.adr = (FilterPriorityMCPNo(adrTable[adrcnt]) & 0x000F)
								| ((DUALPORT2.pollingData.carNo << 4)& 0x0FFF0);
					}
				}

				sendpolling.dataid = DUALPORT2.pollingData.dataID;
				sendpolling.carno = DUALPORT2.pollingData.carNo;
				sendpolling.nextstation = DUALPORT2.pollingData.nextStation;
				sendpolling.destination = DUALPORT2.pollingData.destination;
				sendpolling.route = DUALPORT2.pollingData.routeCode;
				sendpolling.door = DUALPORT2.pollingData.doorInfo.BYTE;
				for( i=0; i<9; i++ ){
					sendpolling.voicebus[i] = DUALPORT2.pollingData.voiceBus.BYTE.BYTE[i];
				}
				for( i=0; i<32; i++ ){
					sendpolling.display[i] = DUALPORT2.pollingData.mcpDisplay.BYTE.BYTE[i];
				}
				for( i=0; i<8; i++ ){
					sendpolling.vlevel[i] = DUALPORT2.pollingData.voiceLevel[i];
				}

				if( DUALPORT2.couplingCheck && otherDVAUPolling ){	// 故障車ＤＶＡＵへのポーリング時
					if( adrTable[adrcnt] != 12 ){					// 2004/09/13 h.nishiura MCP-Xには故障車情報を送らないように修正
						for( i=1; i<9; i++ ){
							sendpolling.voicebus[i] = DUALPORT2.pollingData.voiceBus.BYTE.BYTE[i+8];
						}
					}
				}

				if( changing ){
					max_receive_time = MAX_CHANGE_TIME;
					timedata.CommRecvTimer = max_receive_time;
				}
				else {
					if( DUALPORT2.couplingCheck ){
						max_receive_time = MAX_RECEIVE_TIME3;
						timedata.CommRecvTimer = max_receive_time;
					}
					else {
						if( DUALPORT2.nowSignal2 & 0x04 ){	// Odd Address
							max_receive_time = MAX_RECEIVE_TIME1;
							timedata.CommRecvTimer = max_receive_time;
						}
						else {
							max_receive_time = MAX_RECEIVE_TIME2;
							timedata.CommRecvTimer = max_receive_time;
						}
					}
				}
				if( adrTable[adrcnt] == 13 )	// RTTF
				{

					if( DUALPORT2.mn_stn_busy == 1 )
					{
						sendpolling.nextstation = DUALPORT2.mn_nextStation;
						sendpolling.destination = DUALPORT2.mn_destination;
						sendpolling.route = DUALPORT2.mn_routeCode;
					}
					else
					{
						// 上位通信が無効の場合はルートコード、駅コードを０とする。
						if( DUALPORT2.tms_request.Active == 0 )
						{
							sendpolling.nextstation = 0;
							sendpolling.destination = 0;
							sendpolling.route = 0;
						}
					}

					if(DUALPORT2.tms_answer.dvaStatus.BIT.specialMode)
					{
						sendpolling.door = DUALPORT2.nowSPCCode+1;		// SPC Code
						sendpolling.voicebus[1] &= 0x06;
						sendpolling.voicebus[1] |= 0x01;				// SPC 放送中
					}
					else
					{
						sendpolling.door = 0;							// SPC Code
						sendpolling.voicebus[1] &= 0x06;				// SPC 未放送
					}

					if(DUALPORT2.tms_answer.dvaStatus.BIT.emgMode)
					{
						sendpolling.voicebus[0] = DUALPORT2.nowEMGCode+1;	// EMG Code
						sendpolling.voicebus[1] &= 0x05;
						sendpolling.voicebus[1] |= 0x02;				// EMG 放送中
					}
					else
					{
						sendpolling.voicebus[0] = 0;					// EMG Code
						sendpolling.voicebus[1] &= 0x05;				// EMG 未放送
					}

					if( DUALPORT2.rttf_an_busy == 1 )					// a09 RTTF用放送中情報（0:停止中/1:放送中)
					{
						sendpolling.voicebus[2] = DUALPORT2.rttf_an_sendCode;	// Announcement message code
						sendpolling.voicebus[1] &= 0x03;
						sendpolling.voicebus[1] |= 0x04;				// Announcement 放送中
					}
					else
					{
						sendpolling.voicebus[2] = 0;					// Announcement message code
						sendpolling.voicebus[1] &= 0x03;				// Announcement 未放送
					}

					if( rtifmessage_flag == 1)
					{
						dvauRTTFSend( (unsigned char *)&sendpolling, 13, (unsigned char *)rtifmessage, rtifmessage_size );
						rtifmessage_flag = 2;
					}
					else
					{
						dvauRTTFSend( (unsigned char *)&sendpolling, 13, (unsigned char *)0, 0 );
					}
				}
				else
				{
					dvauSend( (unsigned char *)&sendpolling, sizeof( sendpolling ) );
				}
			}
		}
}


