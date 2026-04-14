/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	’ÊM§Œäƒ‚ƒWƒ…[ƒ‹												*
*-------------------------------------------------------------------*
*	Author		TOA Corporation									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

#include	<stdio.h>
#include    <machine.h>
#include    "vect.h"
#include    "iodefine.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
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
	// ƒu[ƒgƒ`ƒFƒbƒNƒGƒŠƒA
	unsigned short	check_date;
	unsigned short	check_time;
	unsigned long	check_all_bytes;
#pragma section

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< RAMCheck >>	
*					‰ŠúÝ’è
*	Syntax			RAMCheck( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int RAMCheck( void )
{
		volatile unsigned short	*p;
		long	cnt;
		unsigned short	data1,data2;

		p = (unsigned short *)RAMTOP_ADDRESS; /* RAM Top */
		for( cnt=0; cnt<RAM_SIZE/2; cnt++,p++ ){
			data1 = *p;
			*p = 0x55AA;
			data2 = *p;
			*p = data1;
			if( data2!=0x55AA ){
				return 1;
			}
		}
		p = (unsigned short *)RAMTOP_ADDRESS; /* RAM Top */
		for( cnt=0; cnt<RAM_SIZE/2; cnt++,p++ ){
			data1 = *p;
			*p = 0xAA55;
			data2 = *p;
			*p = data1;
			if( data2!=0xAA55 ){
				return 1;
			}
		}
//		p = (unsigned short *)RAMTOP_ADDRESS; /* RAM Top */
//		for( cnt=0; cnt<RAM_SIZE/2; cnt++,p++ ){
//			data1 = *p;
//			*p = 0xFFFF;
//			data2 = *p;
//			*p = data1;
//			if( data2!=0xFFFF ){
//				return 1;
//			}
//		}
//		p = (unsigned short *)RAMTOP_ADDRESS; /* RAM Top */
//		for( cnt=0; cnt<RAM_SIZE/2; cnt++,p++ ){
//			data1 = *p;
//			*p = 0x0000;
//			data2 = *p;
//			*p = data1;
//			if( data2!=0x0000 ){
//				return 1;
//			}
//		}

		return 0;
}

/************************************************************************
*																		*
*	Function name	<< ROMCheck >>	
*					‰ŠúÝ’è
*	Syntax			ROMCheck( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int ROMCheck( void )
{
		unsigned char	*p;
		long	cnt;
		unsigned char	data;

		p = (unsigned char *)ROMTOP_ADDRESS; /* ROM Top */
		data = 0;
		for( cnt=0; cnt<ROM_SIZE; cnt++,p++ ){
			data += (unsigned char)*p;
		}
		return data;
}


/************************************************************************
*																		*
*	Function name	<< HardwareSetup >>	
*					‰ŠúÝ’è
*	Syntax			HardwareSetup( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void HardwareSetup( void )
{
		watchDogON = watchDogFlag;
		watchDogFlag = 0;

		WDT = 0xA500;		/* WatchDog Disable */
		INTC.IER.BYTE = 0;	/* IRQ Use */

	/*** CPU Mode3 ***/

		BSC.BRCR.BIT.A21E=0;		// PA6 is A21
		BSC.BRCR.BIT.A22E=0;		// PA5 is A22
		BSC.BRCR.BIT.A23E=0;		// PA4 is A23

		IRQ4Jump=NULL;

		IMIA0Jump=NULL;
		IMIA1Jump=NULL;
		IMIA2Jump=NULL;
		IMIA3Jump=NULL;
		IMIA4Jump=NULL;

		ERI0Jump=NULL;
		RXI0Jump=NULL;
		TXI0Jump=NULL;
		TEI0Jump=NULL;
		ERI1Jump=NULL;
		RXI1Jump=NULL;
		TXI1Jump=NULL;
		TEI1Jump=NULL;

	/**** ƒ|[ƒg•ûŒüÝ’è ****/
		P4.DDR = 0;			/* P4 is Input( DIP-SW ) */
		P4.PCR.BYTE = 0;	/* P4 pullup is External */

		P6.DDR = 0x80;		/* P6 no use */

							/* P7 Input SCI0 status */

		P8.DDR = 0xFE;		/* P8 b4-b1=CS0-CS3, b0=IRQ0 */

		P9.DDR = 0xC0;		/* P9 b5=IRQ5, b4=IRQ4, b3=RxD1, b2=RxD0, b1=TxD1, b0=TxD0 */

		PA.DDR = 0xFF;		/* PA b7=A20, b6= , b5= , b4= , b3=DTR1, b2=RTS1, b1=DTR0, b0=RTS0 */

		SYSCR.BYTE = 0;
		SYSCR.BIT.SSBY  = 1;		/* Sleep –½—ßŒãAƒ\ƒtƒgƒEƒFƒAƒXƒ^ƒ“ƒoƒC */
		SYSCR.BIT.STS   = 7;		/* Žg—p‹ÖŽ~ */
		SYSCR.BIT.UE    = 0;		/* —DæŠ„‚èž‚Ý‚ ‚è */
		SYSCR.BIT.NMIEG = 0;
		SYSCR.BIT.RAME  = 1;		/*“à‘Ÿ‚q‚`‚l—LŒø */

		INTC.ISCR.BYTE = 0;
		INTC.ISCR.BIT.IRQ4SC = 0;	/* IRQ4 Level Int*/
		INTC.IER.BYTE = 0;

		if( INTC.ISR.BYTE )			// •Û—¯ó‘Ô‚ÌŠ„‚èž‚Ý‚ðƒNƒŠƒA
			INTC.ISR.BYTE = 0;

		if( ITU0.TSR.BYTE )
			ITU0.TSR.BYTE = 0;
		if( ITU1.TSR.BYTE )
			ITU1.TSR.BYTE = 0;
		if( ITU2.TSR.BYTE )
			ITU2.TSR.BYTE = 0;
		if( ITU3.TSR.BYTE )
			ITU3.TSR.BYTE = 0;
		if( ITU4.TSR.BYTE )
			ITU4.TSR.BYTE = 0;

		INTC.IPRA.BYTE = 0;
		INTC.IPRA.BIT.B3 = 1;  // Set WDT,RFSHC Priority Level 1

		INTC.IPRB.BYTE = 0;

		SCI0.SCR.BYTE = 0;
		SCI0.SMR.BYTE = 0;

		PA.DR.BIT.B1 = 1;		/* RTS OFF */
		PA.DR.BIT.B0 = 1;		/* RTS OFF */
}



/************************************************************************
*																		*
*	Function name	<< WatchDogReset >>	
*					‰ŠúÝ’è
*	Syntax			WatchDogReset( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void WatchDogReset( void )
{
							/* 14.7456MHz -> ƒ³
								ƒ³/4096 = 3600Hz
								0xFF= 70mS Interval */
		WDT = 0x5A00;		/* TCNT Clear */
		WDT = 0xA53F;		/* TCSR Set (Interval Timer) */
/*		WDT = 0xA55F;		* TCSR Set (Watch Dog Reset) */

		watchDogCount = 0;
}


/************************************************************************
*																		*
*	Function name	<< WatchDogDisable >>	
*					ƒEƒHƒbƒ`ƒhƒbƒO‚ð‹ÖŽ~‚·‚é
*	Syntax			WatchDogDisable( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void WatchDogDisable( void )
{
							/* 14.7456MHz -> ƒ³
								ƒ³/4096 = 3600Hz
								0xFF= 70mS Interval */
		WDT = 0x5A00;		/* TCNT Clear */
		WDT = 0xA500;		/* TCSR Reset */

		watchDogCount = 0;
}

//  vector 20 WOVI
void    INT_WOVI(void)
{
		if( WDT & 0x8000 ){
			watchDogCount++;
			if( watchDogCount>14 ){		/* –ñ‚P•b‚Ìƒ^ƒCƒ€ƒAƒEƒg */
				watchDogFlag = 1;
			}
		}
		WDT = 0xA51F;		/* TCSR Set (Interval Timer) */
		WDT = 0xA53F;		/* TCSR Set (Interval Timer) */
}



#pragma section IntPRG
//  vector 1 Reserved

//  vector 2 Reserved

//  vector 3 Reserved

//  vector 4 Reserved

//  vector 5 Reserved 

//  vector 6 Reserved

//  vector 7 NMI 
void    INT_NMI(void) {/* sleep(); */}
//  vector 8 TRAP
void    INT_TRAP1(void) {/* sleep(); */}
//  vector 9 TRAP
void    INT_TRAP2(void) {/* sleep(); */}
//  vector 10 TRAP
void    INT_TRAP3(void) {/* sleep(); */}


//  vector 12 IRQ0
void    INT_IRQ0(void) {/* sleep(); */}
//  vector 13 IRQ1
void    INT_IRQ1(void) {/* sleep(); */}
//  vector 14 IRQ2
void    INT_IRQ2(void) {/* sleep(); */}
//  vector 15 IRQ3
void    INT_IRQ3(void) {/* sleep(); */}
//  vector 16 IRQ4
void    INT_IRQ4(void) {
		if( IRQ4Jump!=NULL ){
			if( INTC.IPRA.BIT.B4==0 ){
				and_ccr(~0x40);
			}
			IRQ4Jump();
		}
		INTC.ISR.BIT.IRQ4F = 0;
}
//  vector 17 IRQ5
void    INT_IRQ5(void) {/* sleep(); */}
//  vector 18 Reserved

//  vector 19 Reserved

//  vector 21 CMI
void    INT_CMI(void) {/* sleep(); */}
//  vector 22 Reserved

//  vector 23 Reserved

//  vector 24 IMIA0
void    INT_IMIA0(void) {
		if( IMIA0Jump!=NULL ){
			if( INTC.IPRA.BIT.B2==0 ){
				and_ccr(~0x40);
			}
			IMIA0Jump();
		}
		if( ITU0.TSR.BIT.IMFA )
			ITU0.TSR.BIT.IMFA = 0;		/* ƒ^ƒCƒ}[Š„‚èž‚ÝƒNƒŠƒA */
}

//  vector 25 IMIB0
void    INT_IMIB0(void) {/* sleep(); */}
//  vector 26 OVI0
void    INT_OVI0(void) {/* sleep(); */}
//  vector 27 Reserved

//  vector 28 IMIA1
void    INT_IMIA1(void) {
		if( IMIA1Jump!=NULL ){
			if( INTC.IPRA.BIT.B1==0 ){
				and_ccr(~0x40);
			}
			IMIA1Jump();
		}
		if( ITU1.TSR.BIT.IMFA )
			ITU1.TSR.BIT.IMFA = 0;		/* ƒ^ƒCƒ}[Š„‚èž‚ÝƒNƒŠƒA */
}

//  vector 29 IMIB1
void    INT_IMIB1(void) {/* sleep(); */}
//  vector 30 OVI1
void    INT_OVI1(void) {/* sleep(); */}
//  vector 31 Reserved

//  vector 32 IMIA2
void    INT_IMIA2(void) {
		if( IMIA2Jump!=NULL ){
			if( INTC.IPRA.BIT.B0==0 ){
				and_ccr(~0x40);
			}
			IMIA2Jump();
		}
		if( ITU2.TSR.BIT.IMFA )
			ITU2.TSR.BIT.IMFA = 0;		/* ƒ^ƒCƒ}[Š„‚èž‚ÝƒNƒŠƒA */
}

//  vector 33 IMIB2
void    INT_IMIB2(void) {/* sleep(); */}
//  vector 34 OVI2
void    INT_OVI2(void) {/* sleep(); */}
//  vector 35 Reserved

//  vector 36 IMIA3
void    INT_IMIA3(void) {
		if( IMIA3Jump!=NULL ){
			if( INTC.IPRB.BIT.B7==0 ){
				and_ccr(~0x40);
			}
			IMIA3Jump();
		}
		if( ITU3.TSR.BIT.IMFA )
			ITU3.TSR.BIT.IMFA = 0;		/* ƒ^ƒCƒ}[Š„‚èž‚ÝƒNƒŠƒA */
}

//  vector 37 IMIB3
void    INT_IMIB3(void) {/* sleep(); */}
//  vector 38 OVI3
void    INT_OVI3(void) {/* sleep(); */}
//  vector 39 Reserved

//  vector 40 IMIA4
void    INT_IMIA4(void) {
		if( IMIA4Jump!=NULL ){
			if( INTC.IPRB.BIT.B6==0 ){
				and_ccr(~0x40);
			}
			IMIA4Jump();
		}
		if( ITU4.TSR.BIT.IMFA )
			ITU4.TSR.BIT.IMFA = 0;		/* ƒ^ƒCƒ}[Š„‚èž‚ÝƒNƒŠƒA */
}

//  vector 41 IMIB4
void    INT_IMIB4(void) {/* sleep(); */}
//  vector 42 OVI4
void    INT_OVI4(void) {/* sleep(); */}
//  vector 43 Reserved

//  vector 44 DEND0A
void    INT_DEND0A(void) {/* sleep(); */}
//  vector 45 DEND0B
void    INT_DEND0B(void) {/* sleep(); */}
//  vector 46 DEND1A
void    INT_DEND1A(void) {/* sleep(); */}
//  vector 47 DEND1B
void    INT_DEND1B(void) {/* sleep(); */}
//  vector 48 Reserved

//  vector 49 Reserved

//  vector 50 Reserved

//  vector 51 Reserved

//  vector 52 ERI0
void    INT_ERI0(void) {
		if( ERI0Jump!=NULL ){
			if( INTC.IPRB.BIT.B3==0 ){
				and_ccr(~0x40);
			}
			ERI0Jump();
		}
}
//  vector 53 RXI0
void    INT_RXI0(void) {
		if( RXI0Jump!=NULL ){
			if( INTC.IPRB.BIT.B3==0 ){
				and_ccr(~0x40);
			}
			RXI0Jump();
		}
}
//  vector 54 TXI0
void    INT_TXI0(void) {
		if( TXI0Jump!=NULL ){
			if( INTC.IPRB.BIT.B3==0 ){
				and_ccr(~0x40);
			}
			TXI0Jump();
		}
}
//  vector 55 TEI0
void    INT_TEI0(void) {
		if( TEI0Jump!=NULL ){
			if( INTC.IPRB.BIT.B3==0 ){
				and_ccr(~0x40);
			}
			TEI0Jump();
		}
}
//  vector 56 ERI1
void    INT_ERI1(void) {
		if( ERI1Jump!=NULL ){
			if( INTC.IPRB.BIT.B2==0 ){
				and_ccr(~0x40);
			}
			ERI1Jump();
		}
}
//  vector 57 RXI1
void    INT_RXI1(void) {
		if( RXI1Jump!=NULL ){
			if( INTC.IPRB.BIT.B2==0 ){
				and_ccr(~0x40);
			}
			RXI1Jump();
		}
}
//  vector 58 TXI1
void    INT_TXI1(void) {
		if( TXI1Jump!=NULL ){
			if( INTC.IPRB.BIT.B2==0 ){
				and_ccr(~0x40);
			}
			TXI1Jump();
		}
}
//  vector 59 TEI1
void    INT_TEI1(void) {
		if( TEI1Jump!=NULL ){
			if( INTC.IPRB.BIT.B2==0 ){
				and_ccr(~0x40);
			}
			TEI1Jump();
		}
}

//  vector 60 ADI
void    INT_ADI(void) {/* sleep(); */}


//  vector Dummy
void    Dummy(void) {/* sleep(); */}

