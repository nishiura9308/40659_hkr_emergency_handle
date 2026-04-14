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

#pragma stacksize	4096

#pragma entry EntryFunction

#pragma section EntryProgram

void EntryFunction( void )
{
	 set_imask_ccr(1);

	HardwareSetup();				// Use Hardware Setup

	main();

	sleep();
}

