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

#include	"table.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
	
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
extern	void dvaIOOut( unsigned long onbit, unsigned long offbit );
extern	int RAMCheck( void );
extern	int DUALRAMCheck( void );
extern	int ROMCheck( void );
extern	int boot( char *filename );
extern	void jump_app( void );

extern	char	watchDogCount;
extern	char	watchDogFlag;
extern	char	watchDogON;

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
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void main( void )
{
		int		i;

		dvaIOOut( 0, 0xFFFF );

//		if( P4.DR.BIT.B0==0 ){
			if( RAMCheck() ){
				set_imask_ccr(1);
				printf("RAM Check Error\r\n");
				dvaIOOut( OUTSIGNAL_LED0, 0xFFFF );
				Halt();
			}
//		}

//		if( P4.DR.BIT.B0==0 ){
			if( ROMCheck() ){
				set_imask_ccr(1);
				printf("ROM Check Error\r\n");
				dvaIOOut( OUTSIGNAL_LED1, 0xFFFF );
				Halt();
			}
//		}

//		if( P4.DR.BIT.B0==0 ){
			if( DUALRAMCheck() ){
				set_imask_ccr(1);
				printf("DUALRAM Check Error\r\n");
				dvaIOOut( OUTSIGNAL_LED2, 0xFFFF );
				Halt();
			}
//		}

		memset( (char *)&DUALBOOT, 0, sizeof(DUALBOOT) );

		set_imask_ccr(0);

//		if( P4.DR.BIT.B2==0 ){
			for( i=0; i<5; i++ ){
				if( boot_cpu2( "A:\\DVAU2.MOT" )==0 ){
					break;
				}
			}
			if( i>=5 ){
				set_imask_ccr(1);
				printf("Boot DVAU2 Error\r\n");
				dvaIOOut( OUTSIGNAL_LED3, 0 );
				Halt();
			}
			memset( (char *)DUALBOOT.data, 0, sizeof(DUALBOOT.data) );
//		}

		if( boot( "A:\\DVAU1.MOT" ) ){
			set_imask_ccr(1);
			printf("Boot DVAU1 Error\r\n");
			dvaIOOut( OUTSIGNAL_LED4, 0 );
			Halt();
		}
		dvaIOOut( 0, 0xFFFF );

		WatchDogReset();

		jump_app();
}

