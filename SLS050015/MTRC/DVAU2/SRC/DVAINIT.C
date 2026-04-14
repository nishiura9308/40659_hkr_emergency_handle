/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	初期設定モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/07		K.Honda

*********************************************************************/

#include	<stdio.h>
#include    <machine.h>
#include    "iodefine.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#pragma section IntChgTable
	void	(*IRQ4Jump)(void);

	void	(*IMIA0Jump)(void);
	void	(*IMIA1Jump)(void);
	void	(*IMIA2Jump)(void);
	void	(*IMIA3Jump)(void);
	void	(*IMIA4Jump)(void);

	void	(*ERI0Jump)(void);
	void	(*RXI0Jump)(void);
	void	(*TXI0Jump)(void);
	void	(*TEI0Jump)(void);
	void	(*ERI1Jump)(void);
	void	(*RXI1Jump)(void);
	void	(*TXI1Jump)(void);
	void	(*TEI1Jump)(void);

#pragma section DataWatchDog
char	watchDogCount;
char	watchDogFlag;
char	watchDogON;

#pragma section BootCheck
	// ブートチェックエリア
	unsigned short	check_date;
	unsigned short	check_time;
	unsigned long	check_all_bytes;
#pragma section

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


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*@@@E*/
/************************************************************************

	Function name	<< HardwareSetup >>	
					初期設定
	Syntax			HardwareSetup( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void HardwareSetup( void )
{
	/*** CPU Mode3 ***/

	/**** ポート方向設定 ****/
		P4.DDR = 0;			/* P4 is Input( DIP-SW ) */
		P4.PCR.BYTE = 0;	/* P4 pullup is External */

		P6.DDR = 0x80;		/* P6 no use */

							/* P7 Input SCI0 status */

		P8.DDR = 0xFE;		/* P8 b4-b1=CS0-CS3, b0=IRQ0 */

		P9.DDR = 0xC0;		/* P9 b5=IRQ5, b4=IRQ4, b3=RxD1, b2=RxD0, b1=TxD1, b0=TxD0 */

		PA.DDR = 0xFF;		/* PA b7=A20, b6= , b5= , b4= , b3=DTR1, b2=RTS1, b1=DTR0, b0=RTS0 */

		SYSCR.BYTE = 0;
		SYSCR.BIT.SSBY  = 1;		/* Sleep 命令後、ソフトウェアスタンバイ */
		SYSCR.BIT.STS   = 7;		/* 使用禁止 */
		SYSCR.BIT.UE    = 0;		/* 優先割り込みあり */
		SYSCR.BIT.NMIEG = 0;
		SYSCR.BIT.RAME  = 1;		/*内臓ＲＡＭ有効 */

		INTC.ISCR.BYTE = 0;
		INTC.ISCR.BIT.IRQ4SC = 0;	/* IRQ4 Level Int*/
		INTC.IER.BYTE = 0;

		INTC.IPRA.BYTE = 0;
		INTC.IPRA.BIT.B3 = 1;  // Set WDT,RFSHC Priority Level 1

		INTC.IPRB.BYTE = 0;
}



/************************************************************************
																		*
	Function name	<< WatchDogReset >>
					初期設定
	Syntax			WatchDogReset( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void WatchDogReset( void )
{
							/* 14.7456MHz -> Φ
								Φ/4096 = 3600Hz
								0xFF= 70mS Interval */
		WDT = 0x5A00;		/* TCNT Clear */
		WDT = 0xA53F;		/* TCSR Set (Interval Timer) */
/*		WDT = 0xA55F;		* TCSR Set (Watch Dog Reset) */

		watchDogCount = 0;
}

/************************************************************************
																		*
	Function name	<< WatchDogDisable >>
					ウォッチドッグを禁止する
	Syntax			WatchDogDisable( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07

*************************************************************************/
void WatchDogDisable( void )
{
							/* 14.7456MHz -> Φ
								Φ/4096 = 3600Hz
								0xFF= 70mS Interval */
		WDT = 0x5A00;		/* TCNT Clear */
		WDT = 0xA500;		/* TCSR Reset */

		watchDogCount = 0;
}


