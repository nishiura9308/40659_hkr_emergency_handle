/********************************************************************
*	y`’n‰º“SŒü‚¯©“®—ñÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	’ÊM§Œäƒ‚ƒWƒ…[ƒ‹												*
*-------------------------------------------------------------------*
*	Author		TOA Corporation									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

#include	<machine.h>
#include	"iodefine.h"

#pragma stacksize	1024

#pragma entry EntryFunction

#pragma section EntryProgram

void EntryFunction( void )
{
	P8.DDR = 0xFE;		// CS3 Enable

	 set_imask_ccr(1);

	_INIT_IOLIB();					// Use SIM I/O

//	errno=0;						// Remove the comment when you use errno
//	srand(1);						// Remove the comment when you use rand()
//	_s1ptr=NULL;					// Remove the comment when you use strtok()

	HardwareSetup();				// Use Hardware Setup

	main();

	_CLOSEALL();					// Use SIM I/O
	
	sleep();
}

