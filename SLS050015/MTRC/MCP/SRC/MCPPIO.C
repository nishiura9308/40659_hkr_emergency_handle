/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	Ｉ／Ｏ入力制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/06/27		K.Honda
	Revision	a00. 2000/11/07		K.Honda

*********************************************************************/

#include <stdio.h>

#include "IOFUNC.h"

#include "COMMDEF.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/** スイッチデータ **/
short	onReadIO;
short	offReadIO;
short	nowReadIO;

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

extern	char	mcp_sendactive;

/** 送信エリア **/
extern	struct MCPSEND	sendarea;

/** 表示データ **/
extern	short	sw_led;				/*スイッチＬＥＤ*/

#define	SW_EMGPRG	0x0001
#define	SW_CABCAB	0x0002
#define	SW_RBM		0x0004
#define	SW_TEST		0x0008
#define	SW_ATC		0x0010
#define	SW_HL		0x0020
#define	SW_EMG		0x0040
#define	SW_SPC		0x0080
#define	SW_REP		0x0100
#define	SW_LEFT		0x0200
#define	SW_RIGHT	0x0400
#define	SW_SEL		0x0800
#define	SW_RST		0x1000
#define	SW_ENT		0x2000
#define	SW_STN		0x4000

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


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	char	readCount;
static	short	preReadIO;

/*@@@E*/
void debug_Display( void )
{
			if( sw_led & SWLED_HL ){
				sw_led ^= onReadIO;
			if( onReadIO & 0x01 ){
				onReadIO &= ~0x01;
				{
					int	i;
					for( i=0; i<6; i++ ){
						matrix_led[0][i] = 0x20+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[1][i] = 0x26+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[2][i] = 0x2C+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[3][i] = 0x32+i;
					}
				}
			}
			if( onReadIO & 0x02 ){
				onReadIO &= ~0x02;
				{
					int	i;
					for( i=0; i<6; i++ ){
						matrix_led[0][i] = 0x30+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[1][i] = 0x36+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[2][i] = 0x3C+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[3][i] = 0x42+i;
					}
				}
			}
			if( onReadIO & 0x04 ){
				onReadIO &= ~0x04;
				{
					int	i;
					for( i=0; i<6; i++ ){
						matrix_led[0][i] = 0x50+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[1][i] = 0x56+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[2][i] = 0x5C+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[3][i] = 0x62+i;
					}
				}
			}
			if( onReadIO & 0x08 ){
				onReadIO &= ~0x08;
				{
					int	i;
					for( i=0; i<6; i++ ){
						matrix_led[0][i] = 0x60+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[1][i] = 0x66+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[2][i] = 0x6C+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[3][i] = 0x72+i;
					}
				}
			}
			if( onReadIO & 0x10 ){
				onReadIO &= ~0x10;
				{
					int	i;
					for( i=0; i<6; i++ ){
						matrix_led[0][i] = 0x70+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[1][i] = 0x76+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[2][i] = 0x7C+i;
					}
					for( i=0; i<6; i++ ){
						matrix_led[3][i] = 0x02+i;
					}
				}
			}
			}
			else if( onReadIO ){
				sprintf( matrix_led[0], "%02X", onReadIO );
				onReadIO = 0;
			}
}

/************************************************************************

	Function name	<< mcpSwitchIn >>
					スイッチ状態入力処理
	Syntax			mcpSwitchIn( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/06/27
				a01 2000/11/07 スイッチ押下状態をＤＶＡＵへ送信するまで蓄積する

*************************************************************************/
void mcpSwitchIn( void )
{
		short	readIO;
		union {
			short	ddata;
			char	bdata[2];
		} data;

		readIO = switch_read();

		if( mcp_sendactive ){
			mcp_sendactive = 0;
			sendarea.mcpsw[0] = 0;
			sendarea.mcpsw[1] = 0;
		}

		if( readIO!=preReadIO ){
			readCount = 0;
		}
		else if( readCount>=2 ){	/* 50mS 経過しても同一のとき */
			onReadIO = (nowReadIO ^ readIO) & readIO;
			offReadIO = (nowReadIO ^ readIO) & nowReadIO;
			nowReadIO = readIO;

			if( readCount<30 ){
				readCount++;
				nowReadIO &= ~SW_ENT;		/* ENT キーのみ 300mS 経過時に有効 */
			}

			data.ddata = nowReadIO;
			sendarea.mcpsw[0] |= data.bdata[0];
			sendarea.mcpsw[1] |= data.bdata[1];

			if( onReadIO & SW_HL ){
				onReadIO &= ~SW_HL;
				sw_led ^= SWLED_HL;
				if( sw_led & SWLED_HL ){
					io_output_off( 0x00, 0x08 ); /* 高輝度設定 */
					io_output_on( 0x00, 0x02 ); /* 高輝度設定 */
				}
				else {
					io_output_off( 0x00, 0x02 ); /* 低輝度設定 */
					io_output_on( 0x00, 0x08 ); /* 低輝度設定 */
				}
			}

			if( (read_dipsw() & 0x01)==0 ){
				debug_Display();
			}
		}
		else {
			readCount++;
		}
		preReadIO = readIO;
}

