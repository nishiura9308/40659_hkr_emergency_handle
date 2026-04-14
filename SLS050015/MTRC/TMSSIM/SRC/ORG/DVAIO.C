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

#include    <machine.h>
#include    "iodefine.h"

#include    "table.h"


/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

struct st_ReadSIGNAL nowSignal;			// ÅV“Ç‚Ý‚¾‚µ’l

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

static	struct st_ReadSIGNAL	befSignal;
static	struct st_WriteSIGNAL	outSignal;

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< dvaIOIn >>	
*					Ú“_“ü—Í
*	Syntax			dvaIOIn( void );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void    dvaIOIn(void)
{
		struct st_ReadSIGNAL signal;

			// Œ»Ý‚ÌÚ“_î•ñ‚ð“Ç‚Ýž‚Þ
		signal.SIGNAL1.BYTE = IOSIGNAL1.BYTE;
		signal.SIGNAL2.BYTE = IOSIGNAL2.BYTE;
		signal.SIGNAL3.BYTE = IOSIGNAL3.BYTE;

		if( signal.SIGNAL1.BYTE!=befSignal.SIGNAL1.BYTE ){
			// ‘O‰ñ‚Æˆá‚¤‚Æ‚«‚ÍA‘O‰ñ’l‚ðXV‚·‚é
			befSignal.SIGNAL1.BYTE = signal.SIGNAL1.BYTE;
		}
		else {
			// Œ»Ý‚ÌÚ“_î•ñ‚ðŒöŠJ‚·‚é
			nowSignal.SIGNAL1.BYTE = signal.SIGNAL1.BYTE;
		}

		if( signal.SIGNAL2.BYTE!=befSignal.SIGNAL2.BYTE ){
			// ‘O‰ñ‚Æˆá‚¤‚Æ‚«‚ÍA‘O‰ñ’l‚ðXV‚·‚é
			befSignal.SIGNAL2.BYTE = signal.SIGNAL2.BYTE;
		}
		else {
			// Œ»Ý‚ÌÚ“_î•ñ‚ðŒöŠJ‚·‚é
			nowSignal.SIGNAL2.BYTE = signal.SIGNAL2.BYTE;
		}

		if( signal.SIGNAL3.BYTE!=befSignal.SIGNAL3.BYTE ){
			// ‘O‰ñ‚Æˆá‚¤‚Æ‚«‚ÍA‘O‰ñ’l‚ðXV‚·‚é
			befSignal.SIGNAL3.BYTE = signal.SIGNAL3.BYTE;
		}
		else {
			// Œ»Ý‚ÌÚ“_î•ñ‚ðŒöŠJ‚·‚é
			nowSignal.SIGNAL3.BYTE = signal.SIGNAL3.BYTE;
		}

		DUALPORT.nowSignal2 = nowSignal.SIGNAL2.BYTE;
}

/************************************************************************
*																		*
*	Function name	<< dvaIOOut >>										*
*					‚h^‚no—Íî•ñÝ’èŠÖ”								*
*	Syntax			dvaIOOut( onbit, offbit );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaIOOut( unsigned short onbit, unsigned short offbit )
{
		outSignal.SIGNAL1.BYTE &= ~((unsigned char)(offbit>>8));
		outSignal.SIGNAL2.BYTE &= ~((unsigned char)(offbit));

		outSignal.SIGNAL1.BYTE |= (unsigned char)(onbit>>8);
		outSignal.SIGNAL2.BYTE |= (unsigned char)(onbit);

		IOSIGNAL1.BYTE = outSignal.SIGNAL1.BYTE;
		IOSIGNAL2.BYTE = ~outSignal.SIGNAL2.BYTE;
}


/*@@@E*/
