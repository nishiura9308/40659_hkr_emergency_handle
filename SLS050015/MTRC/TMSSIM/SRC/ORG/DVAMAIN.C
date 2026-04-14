/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	’ÊM§Œäƒ‚ƒWƒ…[ƒ‹												*
*-------------------------------------------------------------------*
*	Author		Nissin system lab.									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

#include	<stdio.h>
#include    <machine.h>
#include    "iodefine.h"

#include	"timer.h"
#include	"table.h"

#define		VERSION_INFO	0x0001

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

char	tms_start;


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
extern	void tms_initialize( void );
extern	int tms_send( void );

extern	struct	TIMER_CONTROL	timedata;		// ƒ^ƒCƒ}[î•ñ

extern	char	watchDogCount;
extern	char	watchDogFlag;
extern	char	watchDogON;

extern	void dvaIOOut( unsigned short onbit, unsigned short offbit );

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< main >>											*
*					ƒƒCƒ“ŠÖ”											*
*	Syntax			main( void );										*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void main( void )
{
		int		i;

		memset( DUALBOOT.data, 0, sizeof(DUALBOOT.data) );

		tms_start = 0;
		if( P4.DR.BIT.B6 ){
			tms_start = 1;
		}

		dvaIOOut( 0, 0xFF );

//		and_ccr(0x3F);						/* Š„‚èž‚Ý‘S–Ê‹–‰Â */

		timer_0_Initialize();

		dvauMENTEInitialize();
		tms_initialize();

		tms_send_end();

//		or_ccr(0x80);

		and_ccr(0x3F);						/* Š„‚èž‚Ý‘S–Ê‹–‰Â */
		for( ;; ){
			DUALPORT.activeDVAU = 1;
			DUALPORT.activeDVAUrequest=0;

			if( tms_start ){
				if( timedata.tms_timer==0 && timedata.tmstimeout==0 ){
					and_ccr(0xc0);
					tms_send();
					and_ccr(0x3F);						/* Š„‚èž‚Ý‘S–Ê‹–‰Â */
				}
			}
			if( watchDogFlag ){
				printf("WatchDog %d %d %d\n", watchDogCount, watchDogFlag, watchDogON );
				watchDogFlag = 0;
			}

			dvauMENTEMain();

			WatchDogReset();
		}

}

void asci_0_send_ir( int tend )
{
}
void asci_0_recv_ir( char data, char status )
{
}

