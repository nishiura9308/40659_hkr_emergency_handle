/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	メイン制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/06/27		K.Honda
				a01. 2003/07/07		h.nisiura

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

extern	void paaInitialize(void);
extern	void paaGetMode(void);
extern	void paaSetVolume(void);
extern	void paaExecuteMode(void);
extern	void paaLEDOUT(void);

/** 自アドレス **/
extern	unsigned short	address;			/* 上位１２ビット＝Ｃａｒ　Ｎｏ．
										   下位４ビット＝ＤＩＰＳＷアドレス */

extern	unsigned short	address2;			/* 上位１２ビット＝Ｃａｒ　Ｎｏ．
										   下位４ビット＝ＤＩＰＳＷアドレス */

/** 送信エリア **/
extern	struct PAASEND	sendarea;

/** スイッチデータ **/
extern	short	nowReadIO;
#define	IN_24V	0x1000
#define	AMPON1	0x0400
#define	AMPON2	0x0800
#define	MICON	0x0020

/** 音声バスモード **/
extern	char	voice_bus_mode;
extern	char	initial_voice_level;	/*基本音量データ
									下位６ビットのみ使用
								*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	char	readCount;
static	short	preReadIO;

/*@@@E*/
/************************************************************************

	Function name	<< main >>
					メイン関数
	Syntax			main( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27
					a01 2003/07/07	h.nisiura AGC初期値を12→8に変更
*************************************************************************/
void main( void )
{
		int		dipsw;
		unsigned short	swdata;
		union {
			unsigned short	WORD;
			unsigned char	BYTE[2];
		} swapdata;
		unsigned char	data;
		unsigned short	setValue;

		io_output_off1( 0xFF );		/* ALL I/O OFF */
		io_output_off2( 0xFF );
		io_output_off3( 0xFF );
		io_output_off4( 0xFF );

		/** 初期Ｉ／Ｏ値 **/
		io_output_on1( 0xFF );
		io_output_on2( 0xFF );
		io_output_on3( 0x00 );

		paaInitialize();

		io_output_on3( 0x80 );		/* Watch Dog Enable */

		dipsw = read_dipsw();
		swdata = switch_read();

		address2 = ((swdata>>13)+1) & 0x0F;

		address &= 0x0F0FF;
		address |= (address2 << 8) & 0x0F00;

		sendarea.dataid = 0;
		sendarea.rscsw = 0;
		sendarea.trouble = 0;
		swapdata.WORD = prg_version;
		data = swapdata.BYTE[0];
		swapdata.BYTE[0] = swapdata.BYTE[1];
		swapdata.BYTE[1] = data;
		sendarea.version = swapdata.WORD;

		voice_bus_mode = 0xFF;

		initial_voice_level = 8;			/* 音量の初期値は　-8dB */
		setValue = ((unsigned short)initial_voice_level<<2)+0x1803;	/* Left,Right Set */
		csio_output( setValue );		/* 出力 ( 13 bit )*/

		__enable();

		for( ;; ){
			paaGetMode();
			paaSetVolume();
			paaExecuteMode();
			paaLEDOUT();

			watch_dog_reset();
		}
}

