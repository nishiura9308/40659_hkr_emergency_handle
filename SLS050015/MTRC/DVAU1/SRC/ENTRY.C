/********************************************************************
*	【香港地下鉄向け自動列車放送制御装置】							*
*-------------------------------------------------------------------*
*	通信制御モジュール												*
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
	or_ccr(0xC0);				// 全面割り込み禁止
	set_imask_ccr(1);

	_INIT_IOLIB();					// Use SIM I/O

	HardwareSetup();				// Use Hardware Setup

	main();

	_CLOSEALL();					// Use SIM I/O

	sleep();
}

