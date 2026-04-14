/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	編成間通信制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/06/27		K.Honda
				a02. 2002/04/08		h.nisiura sendbuff 16->128サイズ変更
											  recvbuff 64->128サイズ変更
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

unsigned short	address2;			/* 上位１２ビット＝Ｃａｒ　Ｎｏ．
								   下位４ビット＝ＤＩＰＳＷアドレス */

/** 送信エリア **/
struct PAASEND	sendarea;

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

/** 音声バスモード **/
extern	char	voice_bus_mode;

extern	char	emg_rsc;				/*非常通報器番号フラグ*/

extern	char	initial_voice_level;	/*基本音量データ
									下位６ビットのみ使用
								*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	unsigned char	recvbuff[128];		/*受信バッファー*/		/* 2002/04/08 h.nisiura 64->128サイズ変更 */
static	unsigned char	recvlen;			/*受信バイト数*/
static	unsigned char	recvphase;			/*受信フェーズ*/
static	unsigned char	recvdle;			/*受信ＤＬＥフラグ*/
static	unsigned char	recvbcc;			/*受信ＢＣＣ計算エリア*/
static	unsigned char	sendbuff[128];		/*送信バッファー*/		/* 2002/04/08 h.nisiura 16->128サイズ変更 */
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

	Function name	<< paaSend >>
					送信準備処理
	Syntax			paaSend( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27

*************************************************************************/
void paaSend( void )
{
		unsigned char	*p;			/*送信データポインター*/
		unsigned char	sendbcc;	/*送信ＢＣＣ計算エリア*/
		int		i;
		unsigned char	len;

		sendarea.adr = address;

		asci_0_tx_enable();		/*送信開始  RTS=ON*/

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

		asci_0_send_ir();		/*最初の１バイトを送信*/
}

/************************************************************************

	Function name	<< paaReceive >>
					受信処理
	Syntax			r=paaReceive(*data,len );
	Argument		unsigned char *data	o	: 受信データ
					char len			i	: 要求データサイズ
					char r				o	: 1=応答必要 / 0=応答不要
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27
				a01 2001/04/17  １６両編成時は自分宛のデータのみ取り込む

*************************************************************************/
char paaReceive( unsigned char *data, char len )
{
		struct POLLING	*p;

		p = (struct POLLING *)data;

		if( len!=sizeof(*p) ){
			/*受信バイト数が異常のときは処理しない*/
			return 0;
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

		/** 必要なデータをコピーする **/
		voice_bus_mode = p->voicebus[0];				/*音声バスモード*/

		if( p->kind=='N' ){		/* 一時的処置 */
			if( (address & 0x0F00) == p->adr ){
				emg_rsc = p->voicebus[(address2 & 0x0F)];			/*非常通報器番号*/
				initial_voice_level = p->vlevel[(address2 & 0x0F)-1];	/* 自アドレスの基本音量データ */
				return 1;
			}
		}

		if( (address & 0x0F0FF) == (p->adr & 0x0F0FF) ){
			/* １６両編成時のときは、自分の車両番号のみデータを受け取る */
			emg_rsc = p->voicebus[(address2 & 0x0F)];			/*非常通報器番号*/
			initial_voice_level = p->vlevel[(address2 & 0x0F)-1];	/* 自アドレスの基本音量データ */
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
	Syntax			asci_0_recv_ir( char data, char status );
	Argument		none
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
			if( data == recvbcc ){
				if( recvlen==recvbuff[0]+1 ){
					if( paaReceive( &recvbuff[1], recvlen-1 ) ){
						paaSend();
					}
				}
			}
			recvphase = COMM_WAIT_STX;
			break;
		}
}

