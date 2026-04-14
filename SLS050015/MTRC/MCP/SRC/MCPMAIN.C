/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	メイン制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/06/27		K.Honda

*********************************************************************/

#include	<stdio.h>
#include	<string.h>

#include "IOFUNC.h"

#include "COMMDEF.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#define	SWLED_HL		0x0020


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカルテーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	void mcpInitialize( void );
extern	void mcpDynamicScanDisplay( void );

/** 自アドレス **/
extern	unsigned short	address;			/* 上位１２ビット＝Ｃａｒ　Ｎｏ．
										   下位４ビット＝ＤＩＰＳＷアドレス */

/** 送信エリア **/
extern	struct MCPSEND	sendarea;

/** スイッチデータ **/
extern	short	sw_led;				/*スイッチＬＥＤ*/
extern	char	matrix_led[4][6];	/*６ｘ４マトリックスＬＥＤ*/
extern	short	sw_blink;			/*スイッチ点滅データ*/
extern	char	matrix_blink[4];	/*６ｘ４マトリックス点滅データ*/

extern	short	now_sw_led;				/*現在表示中、スイッチＬＥＤ*/
extern	char	now_matrix_led[4][6];	/*現在表示中、６ｘ４マトリックスＬＥＤ*/
extern	short	now_sw_blink;
extern	char	now_matrix_blink[4];

extern	char	rawData[16][10];			/* ＲＡＷデータ */
extern	char	blinkPositionData[16][10];	/*点滅位置データ*/


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

*************************************************************************/
void main( void )
{
		int		dipsw;
		union {
			unsigned short	WORD;
			unsigned char	BYTE[2];
		} swapdata;
		unsigned char	data;

		io_output_off( 0x00, 0xFF );	/* ALL I/O OFF(LED off) */
		io_output_on( 0x00, 0x05 );

		mcpInitialize();

		io_output_on( 0x00, 0x80 );		/* Watch Dog Enable */

		dipsw = read_dipsw();

		address &= 0x0F0FF;
		if( (dipsw & 0x20)==0 ){		/* ODD Car */
			address |= 0x0A00;
		}
		else {					/* Even Car */
			address |= 0x0B00;
		}

		sendarea.mcpsw[0] = 0;
		sendarea.mcpsw[1] = 0;
		sendarea.mcpsw[2] = (char)dipsw;

		swapdata.WORD = prg_version;
		data = swapdata.BYTE[0];
		swapdata.BYTE[0] = swapdata.BYTE[1];
		swapdata.BYTE[1] = data;
		sendarea.version = swapdata.WORD;


		sw_led &= SWLED_HL;
		memset( matrix_led, 0, sizeof(matrix_led) );
		sw_blink = 0;
		memset( matrix_blink, 0, sizeof(matrix_blink) );
		now_sw_led = 0xFFFF;
		now_sw_blink = 0xFFFF;
		memset( now_matrix_led, 0xFF, sizeof(now_matrix_led) );
		memset( now_matrix_blink, 0xFF, sizeof(now_matrix_blink) );

		memset( rawData, 0, sizeof(rawData) );
		memset( blinkPositionData, 0, sizeof(blinkPositionData) );

		if( sw_led & SWLED_HL ){
			io_output_off( 0x00, 0x08 ); /* 高輝度設定 */
			io_output_on( 0x00, 0x02 ); /* 高輝度設定 */
		}
		else {
			io_output_off( 0x00, 0x02 ); /* 低輝度設定 */
			io_output_on( 0x00, 0x08 ); /* 低輝度設定 */
		}

		__enable();

		for( ;; ){
			mcpDynamicScanDisplay(); 

			watch_dog_reset();
		}
}

