/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	通信制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/09		K.Honda
	Revision	a01. 2000/09/07		K.Honda
																	*
*********************************************************************/

#include <string.h>

#include    <machine.h>
#include    "iodefine.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

extern	void	(*ERI0Jump)(void);
extern	void	(*RXI0Jump)(void);
extern	void	(*TXI0Jump)(void);
extern	void	(*TEI0Jump)(void);

extern	void	(*ERI1Jump)(void);
extern	void	(*RXI1Jump)(void);
extern	void	(*TXI1Jump)(void);
extern	void	(*TEI1Jump)(void);

extern	void asci_0_recv_ir( char data, char status );
extern	void asci_0_send_ir( char tend );

extern	void asci_1_recv_ir( char data, char status );
extern	void asci_1_send_ir( char tend );

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*@@@E*/

/************************************************************************

	Function name	<< asci_0_RX >>
		正常受信割り込み処理
	Syntax			asci_0_RX( void );
	Argument		none
	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/07/27

*************************************************************************/
void asci_0_RX( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．０の受信割り込みハンドラー
　・受信割り込み処理を呼び出す
	------------------------------------------------------------------[@@@E]*/
		unsigned char	status;
		unsigned char	data;

		status = SCI0.SSR.BYTE;			/*割り込みステータスの読込み*/
		if( status & 0x40 ){			/*受信データ有り？*/
			data = SCI0.RDR;
			SCI0.SSR.BYTE = (unsigned char)(status & ~0x78); 	/*受信エラーを含むステータスクリア*/
			asci_0_recv_ir( data, status );	/*受信データとステータスを渡す*/
		}
}

/************************************************************************

	Function name	<< asci_0_ER >>
		異常受信割り込み処理
	Syntax			asci_0_ER( void );
	Argument		none
	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/07/27

*************************************************************************/
void asci_0_ER( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．０の受信割り込みハンドラー
　・受信割り込み処理を呼び出す
	------------------------------------------------------------------[@@@E]*/
		unsigned char	status;
		unsigned char	data;

		status = SCI0.SSR.BYTE;	/*割り込みステータスの読込み*/
		if( status & 0x40 ){	/*受信データ有り？*/
			data = SCI0.RDR;
			asci_0_recv_ir( data, status );	/*受信データとステータスを渡す*/
		}
		SCI0.SSR.BYTE = (unsigned char)(status & ~0x78); 	/*受信エラーを含むステータスクリア*/
}

/************************************************************************

	Function name	<< asci_0_TX >>
		送信割り込み処理
	Syntax			asci_0_TX( void );
	Argument		none
	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/07/27
				a01 2000/09/07
					送信終了・継続を追加

*************************************************************************/
void asci_0_TX( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．０の送信割り込みハンドラー
　・送信割り込み処理を呼び出す
	------------------------------------------------------------------[@@@E]*/
		if( SCI0.SSR.BIT.TEND ){
			asci_0_send_ir( 1 );	/*送信終了割り込み*/
		}
		else if( SCI0.SSR.BIT.TDRE ){
			asci_0_send_ir( 0 );	/*送信継続割り込み*/
		}
}

/************************************************************************

	Function name	<< asci_0_TE >>
		送信終了割り込み処理
	Syntax			asci_0_TE( void );
	Argument		none
	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/07/27
				a01 2000/09/07
					送信終了・継続を追加

*************************************************************************/
void asci_0_TE( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．０の送信割り込みハンドラー
　・送信割り込み処理を呼び出す
	------------------------------------------------------------------[@@@E]*/
		if( SCI0.SSR.BIT.TEND ){
			asci_0_send_ir( 1 );	/*送信終了割り込み*/
		}
		else if( SCI0.SSR.BIT.TDRE ){
			asci_0_send_ir( 0 );	/*送信継続割り込み*/
		}
}

/************************************************************************

	Function name	<< asci_1_RX >>
		正常受信割り込み処理
	Syntax			asci_1_RX( void );
	Argument		none
	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/07/27

*************************************************************************/
void asci_1_RX( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．１の受信割り込みハンドラー
　・受信割り込み処理を呼び出す
	------------------------------------------------------------------[@@@E]*/
		unsigned char	status;
		unsigned char	data;

		status = SCI1.SSR.BYTE;			/*割り込みステータスの読込み*/
		if( status & 0x40 ){			/*受信データ有り？*/
			data = SCI1.RDR;
			SCI1.SSR.BYTE = (unsigned char)(status & ~0x78); 	/*受信エラーを含むステータスクリア*/
			asci_1_recv_ir( data, status );
		}
}

/************************************************************************

	Function name	<< asci_1_ER >>
		異常受信割り込み処理
	Syntax			asci_1_ER( void );
	Argument		none
	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/07/27

*************************************************************************/
void asci_1_ER( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．１の受信割り込みハンドラー
　・受信割り込み処理を呼び出す
	------------------------------------------------------------------[@@@E]*/
		unsigned char	status;
		unsigned char	data;

		status = SCI1.SSR.BYTE;	/*割り込みステータスの読込み*/
		if( status & 0x40 ){	/*受信データ有り？*/
			data = SCI1.RDR;
			asci_1_recv_ir( data, status );
		}
		SCI1.SSR.BYTE = (unsigned char)(status & ~0x78); 	/*受信エラーを含むステータスクリア*/
}

/************************************************************************

	Function name	<< asci_1_TX >>
		送信割り込み処理
	Syntax			asci_1_TX( void );
	Argument		none
	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/07/27
				a01 2000/09/07
					送信終了・継続を追加

*************************************************************************/
void asci_1_TX( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．１の送信割り込みハンドラー
　・送信割り込み処理を呼び出す
	------------------------------------------------------------------[@@@E]*/
		if( SCI1.SSR.BIT.TEND ){
			asci_1_send_ir( 1 );	/*送信終了割り込み*/
		}
		else if( SCI1.SSR.BIT.TDRE ){
			asci_1_send_ir( 0 );	/*送信継続割り込み*/
		}
}

/************************************************************************

	Function name	<< asci_1_TE >>
		送信終了割り込み処理
	Syntax			asci_1_TE( void );
	Argument		none
	Author			TOA Corporation  K.Honda							*
	Revision		a00 2000/07/27
				a01 2000/09/07
					送信終了・継続を追加

*************************************************************************/
void asci_1_TE( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．１の送信割り込みハンドラー
　・送信割り込み処理を呼び出す
	------------------------------------------------------------------[@@@E]*/
		if( SCI1.SSR.BIT.TEND ){
			asci_1_send_ir( 1 );	/*送信終了割り込み*/
		}
		else if( SCI1.SSR.BIT.TDRE ){
			asci_1_send_ir( 0 );	/*送信継続割り込み*/
		}
}

/************************************************************************

	Function name	<< asci_0_Initialize >>
					初期設定
	Syntax			asci_0_Initialize( baud, data, parity, stop );
	Argument		int baud   : i : ボーレート 1200-9600
				int data   : i : データ長　8 or 7
				int parity : i : パリティー 'E'=Even, 'O'=Odd, 'N'=None
				int stop   : i : ストップ長　0 or 1
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_0_Initialize( int baud, char data, char parity, char stop )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．０の初期設定
	------------------------------------------------------------------[@@@E]*/
		SCI0.SMR.BYTE = 0;
		SCI0.SMR.BIT.CA   = 0;	/* 調歩同期 */
		if( data==8 ){
			SCI0.SMR.BIT.CHR  = 0;	/* 8bit data */
		}
		else {
			SCI0.SMR.BIT.CHR  = 1;	/* 7bit data */
		}
		if( parity=='E' ){
			SCI0.SMR.BIT.PE   = 1;	/* Even Parity */
			SCI0.SMR.BIT.OE   = 0;
		}
		else if( parity=='O' ){
			SCI0.SMR.BIT.PE   = 1;	/* Odd Parity */
			SCI0.SMR.BIT.OE   = 1;
		}
		else {
			SCI0.SMR.BIT.PE   = 0;	/* Non Parity */
		}
		if( stop==0 ){
			SCI0.SMR.BIT.STOP = 0;	/* 1 Stop Bit */
		}
		else {
			SCI0.SMR.BIT.STOP = 1;	/* 2 Stop Bit */
		}
		SCI0.SMR.BIT.MP   = 0;
		if( baud>=2400 ){
			SCI0.SMR.BIT.CKS  = 0;					/* 1/1 Clock */
			SCI0.BRR = 191/( baud / 2400 );		/* 14.7456MHz */
		}
		else {
			SCI0.SMR.BIT.CKS  = 1;					/* 1/4 Clock */
			SCI0.BRR = 191/( baud / 600 );		/* 14.7456MHz */
		}

		PA.DR.BIT.B1 = 1;					/* RTS OFF */

		ERI0Jump = asci_0_ER;		/* 異常受信割り込みベクター設定 */
		RXI0Jump = asci_0_RX;		/* 正常受信割り込みベクター設定 */
		TXI0Jump = asci_0_TX;		/* 送信割り込み割り込みベクター設定*/
		TEI0Jump = asci_0_TE;		/* 送信終了割り込みベクター設定 */

		SCI0.SCR.BIT.RIE  = 1;		/* 受信割り込みあり*/
		SCI0.SCR.BIT.TE   = 1;		/* 送信許可 */
		SCI0.SCR.BIT.RE   = 1;		/* 受信許可 */
}

/************************************************************************

	Function name	<< asci_0_tx_enable >>
					送信許可処理
	Syntax			asci_0_tx_enable( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_0_tx_enable( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．０の送信許可
　・ＲＴＳをＯＮにして、送信を許可する
	------------------------------------------------------------------[@@@E]*/
		SCI0.SCR.BIT.RE  = 0;
		SCI0.SCR.BIT.RIE  = 0;
		PA.DR.BIT.B1 = 0;		/* RTS ON */
}

/************************************************************************
																		*
	Function name	<< asci_0_tx_disable >>
					送信禁止処理
	Syntax			asci_0_tx_disable( char tend );
	Argument		tend : 送信終了フラグ
					1 = 送信終了
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_0_tx_disable( char tend )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．０の送信禁止
　・送信終了時は、送信を禁止し、ＲＴＳをＯＦＦにし、受信割り込みを許可する
　・送信継続時は、送信割り込みを禁止するのみ
	------------------------------------------------------------------[@@@E]*/
		SCI0.SCR.BIT.TIE = 0;			/* TIE off */
		if( tend ){
			SCI0.SCR.BIT.TEIE = 0;		/* TEIE off */
			PA.DR.BIT.B1 = 1;			/* RTS OFF */
			SCI0.SCR.BIT.RE  = 1;
			SCI0.SCR.BIT.RIE  = 1;
		}
}

/************************************************************************

	Function name	<< asci_0_transmit >>
					１バイト送信処理
	Syntax			asci_0_transmit( data );
	Argument		unsigned char data : i : 送信データ
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_0_transmit( unsigned char data )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．０の送信
　・送信データを送信ポートへ書き込む
	------------------------------------------------------------------[@@@E]*/
		SCI0.TDR = data; 				/*送信データセット*/
		SCI0.SSR.BIT.TDRE = 0;			/*送信データ有りセット*/
		SCI0.SCR.BIT.TIE = 1;			/* TIE on */
		SCI0.SCR.BIT.TEIE = 1;			/* TEIE on */
}



/************************************************************************

	Function name	<< asci_1_Initialize >>	
					初期設定
	Syntax			asci_1_Initialize( baud, data, parity, stop );
	Argument		int baud   : i : ボーレート 1200-9600
				int data   : i : データ長　8 or 7
				int parity : i : パリティー 'E'=Even, 'O'=Odd, 'N'=None
				int stop   : i : ストップ長　0 or 1
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_1_Initialize( int baud, char data, char parity, char stop )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．１の初期設定
	------------------------------------------------------------------[@@@E]*/
		SCI1.SMR.BYTE = 0;
		SCI1.SMR.BIT.CA   = 0;	/* 調歩同期 */
		if( data==8 ){
			SCI1.SMR.BIT.CHR  = 0;	/* 8bit data */
		}
		else {
			SCI1.SMR.BIT.CHR  = 1;	/* 7bit data */
		}
		if( parity=='E' ){
			SCI1.SMR.BIT.PE   = 1;	/* Even Parity */
			SCI1.SMR.BIT.OE   = 0;
		}
		else if( parity=='O' ){
			SCI1.SMR.BIT.PE   = 1;	/* Odd Parity */
			SCI1.SMR.BIT.OE   = 1;
		}
		else {
			SCI1.SMR.BIT.PE   = 0;	/* Non Parity */
		}
		if( stop==0 ){
			SCI1.SMR.BIT.STOP = 0;	/* 1 stop bit */
		}
		else {
			SCI1.SMR.BIT.STOP = 1;	/* 2 stop bit */
		}
		SCI1.SMR.BIT.MP   = 0;
		if( baud>=2400 ){
			SCI1.SMR.BIT.CKS  = 0;					/* 1/1 Clock */
			SCI1.BRR = 191/( baud / 2400 );		/* 14.7456MHz */
		}
		else {
			SCI1.SMR.BIT.CKS  = 1;					/* 1/4 Clock */
			SCI1.BRR = 191/( baud / 600 );		/* 14.7456MHz */
		}

		PA.DR.BIT.B0 = 1;		/* RTS OFF */

		ERI1Jump = asci_1_ER;		/* 異常割り込みベクター設定 */
		RXI1Jump = asci_1_RX;		/* 正常割り込みベクター設定 */
		TXI1Jump = asci_1_TX;		/* 送信割り込みベクター設定 */
		TEI1Jump = asci_1_TE;		/* 送信終了割り込みベクター設定 */

		SCI1.SCR.BIT.RIE  = 1;		/* 受信割り込み許可 */
		SCI1.SCR.BIT.TE   = 1;		/* 送信許可 */
		SCI1.SCR.BIT.RE   = 1;		/* 受信許可 */
}


/************************************************************************

	Function name	<< asci_1_tx_enable >>
					送信許可処理
	Syntax			asci_1_tx_enable( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_1_tx_enable( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．１の送信許可
　・ＲＴＳをＯＮにして、送信を許可する
	------------------------------------------------------------------[@@@E]*/
		SCI1.SCR.BIT.RE  = 0;
		SCI1.SCR.BIT.RIE  = 0;
		PA.DR.BIT.B0 = 0;		/* RTS ON */
}

/************************************************************************

	Function name	<< asci_1_tx_disable >>	
					送信禁止処理
	Syntax			asci_1_tx_disable( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void asci_1_tx_disable( char tend )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．１の送信禁止
　・送信終了時は、送信を禁止し、ＲＴＳをＯＦＦにし、受信割り込みを許可する
　・送信継続時は、送信割り込みを禁止するのみ
	------------------------------------------------------------------[@@@E]*/
		SCI1.SCR.BIT.TIE = 0;			/* TIE off */
		if( tend ){
			SCI1.SCR.BIT.TEIE = 0;			/* TEIE off */
			SCI1.SCR.BIT.RE  = 1;
			SCI1.SCR.BIT.RIE  = 1;
			PA.DR.BIT.B0 = 1;		/* RTS OFF */
		}
}

/************************************************************************

	Function name	<< asci_1_transmit >>
					１バイト送信処理
	Syntax			asci_1_transmit( unsigned char data );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27
																		*
*************************************************************************/
void asci_1_transmit( unsigned char data )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非同期　ＣＨ．１の送信
　・送信データを送信ポートへ書き込む
	------------------------------------------------------------------[@@@E]*/
		SCI1.TDR = data; 				/*送信データセット*/
		SCI1.SSR.BIT.TDRE = 0;			/*送信データ有りセット*/
		SCI1.SCR.BIT.TIE = 1;			/* TIE on */
		SCI1.SCR.BIT.TEIE = 1;			/* TEIE on */
}

