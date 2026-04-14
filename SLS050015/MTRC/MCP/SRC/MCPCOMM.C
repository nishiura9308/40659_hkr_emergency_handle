/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	編成間通信制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/06/27		K.Honda
				a01. 2000/12/06
*********************************************************************/

#include <string.h>

#include "IOFUNC.h"

#include "COMMDEF.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/** 自アドレス **/
unsigned short	address;			/* 上位１２ビット＝Ｃａｒ　Ｎｏ．
								   下位４ビット＝ＤＩＰＳＷアドレス */

/** 送信エリア **/
struct MCPSEND	sendarea;

char	mcp_active;

char	mcp_sendactive;

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#define		STX			(0x02)				/* テキスト開始			*/
#define		ETX			(0x03)				/* テキスト終了			*/
#define		DLE			(0x10)				/* ＤＬＥ拡張			*/

#define	COMM_WAIT_STX		0		/* ＳＴＸ待ち */
#define	COMM_WAIT_ETX		1		/* ＥＴＸ待ち */
#define	COMM_WAIT_BCC		2		/* ＢＣＣ待ち */


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/** 表示データ **/
extern	short	sw_led;				/*スイッチＬＥＤ*/
#define	SWLED_EMGPRG	0x0001
#define	SWLED_CABCAB	0x0002
#define	SWLED_RBM		0x0004
#define	SWLED_TEST		0x0008
#define	SWLED_ATC		0x0010
#define	SWLED_HL		0x0020
#define	SWLED_EMG		0x0040
#define	SWLED_SPC		0x0080
#define	SWLED_REP		0x0100
#define	SWLED_LEFT		0x0200
#define	SWLED_RIGHT		0x0400
#define	SWLED_SEL		0x0800
#define	SWLED_RST		0x1000
#define	SWLED_ENT		0x2000
#define	SWLED_STN		0x4000
#define	SWLED_SPC2		0x8000	/* Same SWLED_SPC */


extern	char	matrix_led[4][6];	/*６ｘ４マトリックスＬＥＤ*/
extern	short	sw_blink;			/*スイッチ点滅データ*/
extern	char	matrix_blink[4];	/*６ｘ４マトリックス点滅データ*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	unsigned char	recvbuff[64];		/*受信バッファー*/
static	unsigned char	recvlen;			/*受信バイト数*/
static	unsigned char	recvphase;			/*受信フェーズ*/
static	unsigned char	recvdle;			/*受信ＤＬＥフラグ*/
static	unsigned char	recvbcc;			/*受信ＢＣＣ計算エリア*/
static	unsigned char	sendbuff[64];		/*送信バッファー*/
static	unsigned char	sendlen;			/*送信バイト数*/
static	unsigned char	sendcnt;			/*送信カウンター*/

/*@@@E*/
/************************************************************************

	Function name	<< asci_0_send_ir >>
					送信割り込み処理
	Syntax			asci_0_send_ir( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void asci_0_send_ir( void )
{
		if( sendcnt==sendlen ){	/*すべて送信したとき*/
			asci_0_tx_disable();		/*受信へ切り替え*/
			return;
		}

		asci_0_transmit( sendbuff[sendcnt++] );	/*１バイトの送信*/
}

/************************************************************************

	Function name	<< mcpSend >>
					送信準備処理
	Syntax			mcpSend( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void mcpSend( void )
{
		unsigned char	*p;			/*送信データポインター*/
		unsigned char	sendbcc;	/*送信ＢＣＣ計算エリア*/
		int		i;
		unsigned char	len;

		asci_0_tx_enable(1);		/*送信開始  RTS=ON*/

		mcp_sendactive = 1;

		sendarea.adr = address;

		sendcnt = 0;					/* 送信ポインター０クリア */
		sendlen = 0;					/* 送信バイト数０クリア*/

			/* DLE+STX を設定*/
		sendbuff[sendlen++] = DLE;
		sendbuff[sendlen++] = STX;

		sendbcc = 0;					/* 送信ＢＣＣの計算クリア */
		p = (unsigned char *)&sendarea;			/* 送信エリアの内容をそのまま送信 */

		len = sizeof(sendarea);
		sendbcc ^= len;						/* 送信ＢＣＣの計算 */
		if( len==STX || len==ETX || len==DLE ){
			sendbuff[sendlen++] = DLE;			/*データのＤＬＥ拡張*/
			sendbuff[sendlen++] = len + 0x40;
		}
		else {
			sendbuff[sendlen++] = len;
		}

		for( i=0; i<sizeof(sendarea); i++, p++ ){
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

		__disable();

		asci_0_tx_enable(0);		/*送信開始  RTS=ON*/

		asci_0_send_ir();		/*最初の１バイトを送信*/
}

/************************************************************************

	Function name	<< mcpReceive >>
					受信処理
	Syntax			r=mcpReceive(*data,len );
	Argument		unsigned char *data	o	: 受信データ
					char len			i	: 要求データサイズ
					char r				o	: 1=応答必要 / 0=応答不要
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27
				a01 2000/12/06  １６両編成時は自分宛のデータのみ取り込む

*************************************************************************/
char mcpReceive( unsigned char *data, char len )
{
		struct POLLING	*p;
		unsigned short	leddata;

		p = (struct POLLING *)data;

		if( len!=sizeof(*p) ){
			/*受信バイト数が異常のときは処理しない*/
			return 0;
		}

		/** 必要なデータをコピーする **/

		mcp_active = (p->door>>6) & 0x03;
		if( sendarea.mcpsw[2] & 0x20 ){
			if( mcp_active & 0x02 ){
				memcpy( &leddata, p->display, 2 );
				sw_led = (sw_led & SWLED_HL) | (leddata & ~SWLED_HL);	/* SW-LED */
				memcpy( matrix_led, &p->display[2], 4*6 );			/* Matrix-LED */
				memcpy( &sw_blink, &p->display[2+4*6], 2 );			/* SW-Blink */
				memcpy( matrix_blink, &p->display[2+4*6+2], 4 );	/* Matrix Blink */
			}
			else {
				memcpy( &leddata, p->display, 2 );
				leddata &= SWLED_CABCAB;
				sw_led = (sw_led & SWLED_HL) | (leddata & ~SWLED_HL);	/* SW-LED */
				memset( matrix_led, 0, 4*6 );			/* Matrix-LED */
				sw_blink = 0;
				memset( matrix_blink, 0, 4 );	/* Matrix Blink */
			}
		}
		else {
			if( mcp_active & 0x01 ){
				memcpy( &leddata, p->display, 2 );
				sw_led = (sw_led & SWLED_HL) | (leddata & ~SWLED_HL);	/* SW-LED */
				memcpy( matrix_led, &p->display[2], 4*6 );			/* Matrix-LED */
				memcpy( &sw_blink, &p->display[2+4*6], 2 );			/* SW-Blink */
				memcpy( matrix_blink, &p->display[2+4*6+2], 4 );	/* Matrix Blink */
			}
			else {
				memcpy( &leddata, p->display, 2 );
				leddata &= SWLED_CABCAB;
				sw_led = (sw_led & SWLED_HL) | (leddata & ~SWLED_HL);	/* SW-LED */
				memset( matrix_led, 0, 4*6 );			/* Matrix-LED */
				sw_blink = 0;
				memset( matrix_blink, 0, 4 );	/* Matrix Blink */
			}
		}

		if( p->kind=='S' ){		/* 車両番号の登録 */
			/*
				address
					-------------- +1 -------------  ------------- +0 -------------
		 64180 Side	b15 b14 b13 b12 b11 b10  b9  b8  b7  b6  b5  b4  b3  b2  b1  b0
			H8 Side	 b7  b6  b5  b4  b3  b2  b1  b0 b15 b14 b13 b12 b11 b10  b9  b8
					-- Car No.--    ----- AD -----   -----Car No.--  -- Car No.----
					    3210                             BA98             7654

				carNo
					-------------- +1 -------------  ------------- +0 -------------
		 64180 Side	b15 b14 b13 b12 b11 b10  b9  b8  b7  b6  b5  b4  b3  b2  b1  b0
			H8 Side	 b7  b6  b5  b4  b3  b2  b1  b0 b15 b14 b13 b12 b11 b10  b9  b8
					-- Car No.--    -- Car No.  --                  -- Car No.----
					    7654           3210                              BA98
			*/
			address &= 0x0F00;
			address |= ((p->carno & 0x00F00)<<4)  & 0x0F000;	/* car No. 3210 */
			address |= ((p->carno & 0x0F000)>>12) & 0x0000F;	/* car No. 7654 */
			address |= ((p->carno & 0x0000F)<<4)  & 0x000F0;	/* car No. BA98 */
		}

		if( p->kind=='N' ){		/* 一時的処置 */
			if( (address & 0x0F00) == p->adr ){
				return 1;
			}
		}
		/** 応答が必要な場合、送信をおこなう */
		if( address == p->adr ){
			return 1;
		}
		return 0;
}


/************************************************************************

	Function name	<< asci_0_recv_ir >>
					受信割り込み処理
	Syntax			asci_0_recv_ir( data, status );
	Argument		char data	i	:受信データ
					char status	i	:受信ステータス
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void asci_0_recv_ir( char data, char status )
{
/*.	---------------------　処理詳細説明  ----------------------------	*/
/*  フェーズ毎に受信データを取り込む									*/
/*. ------------------------------------------------------------------  */
status=0;
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
					asci_0_rx_disable();
					__enable();
					if( mcpReceive( &recvbuff[1], recvlen-1 ) ){
						mcpSend();
					}
					else {
						__disable();
						asci_0_rx_enable();
					}
				}
			}
			recvphase = COMM_WAIT_STX;
			break;
		}
}

