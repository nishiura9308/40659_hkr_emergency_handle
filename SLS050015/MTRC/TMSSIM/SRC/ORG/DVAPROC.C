/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	ƒvƒƒZƒX§Œäƒ‚ƒWƒ…[ƒ‹											*
*-------------------------------------------------------------------*
*	Author		Nissin system lab.									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

#include	<stdio.h>
#include    <machine.h>
#include    "iodefine.h"
#include    "table.h"

#include    "timer.h"

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
extern	struct ROUTEINFO *dvaGetDefaultRoute( void );
extern	struct ROUTEINFO *dvaSearchRoute( char routeCode );
extern	struct STATIONINFO *dvaSearchStation( char routeCode, char stationCode );
extern	struct STATIONINFO *dvaSearchStationNumber( char routeCode, char stationNumber );

extern	struct st_ReadSIGNAL nowSignal;			// ÅV“Ç‚Ý‚¾‚µ’l
extern	struct	TIMER_CONTROL	timedata;		// ƒ^ƒCƒ}[î•ñ

extern	struct	BACKUP_DATA		backup_data;		// ƒoƒbƒNƒAƒbƒvƒf[ƒ^
extern	struct	CONFIG_INFO		config_data;		// Ý’èî•ñ

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
static	unsigned char	befOpeMode;
static	unsigned char	test_stationCode;

static	unsigned char	befSignal1;

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< dvaInitialTEST >>								*
*					‰ŠúƒeƒXƒgƒV[ƒPƒ“ƒXˆ—											*
*	Syntax			dvaInitalTEST( void );										*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaInitialTEST( void )
{
		int		i;

		// ƒeƒXƒgó‹µ‚Ì‰Šú‰»
		if( befOpeMode!=OPEMODE_INITIAL_TEST ){
			befOpeMode = OPEMODE_INITIAL_TEST;

			DUALPORT.activeDVAU=1;

			for( i=0; i<16; i++ ){
				DUALPORT.answerData.version[i] = 0;
			}

			DUALPORT.ev5000_request.Initial=1;
			DUALPORT.ev5000_request.Complete=0;
			timedata.checkTimer = 1000;
		}

		if( timedata.checkTimer==0 ){
			// ƒ^ƒCƒ€ƒAƒEƒgˆ—
		}

		for( i=0; i<16; i++ ){
			if( DUALPORT.answerData.version[i]!=config_data.version[i] ){
				return;		// Še‹@Ší‚Ìƒo[ƒWƒ‡ƒ“‚n‚j
			}
		}

		if( DUALPORT.ev5000_request.Complete==0 ){
			return;
		}

		// ‚c‚u‚`‚tØ‚è‘Ö‚¦ƒRƒ}ƒ“ƒh
		DUALPORT.activeDVAU=0;
		DUALPORT.activeDVAUrequest = 1;

		backup_data.opeMode = OPEMODE_STANDBY;
}

/************************************************************************
*																		*
*	Function name	<< dvaTEST >>								*
*					ƒeƒXƒgƒV[ƒPƒ“ƒXˆ—											*
*	Syntax			dvaTEST( void );										*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaTEST( void )
{
		int		i;
		struct ROUTEINFO	*routeData;

		DUALPORT.pollingData.voiceBus.BIT.mode = 5;	// ‰¹ºƒoƒXŽg—pó‹µ‚ÍƒeƒXƒg

		routeData = dvaSearchRoute( backup_data.nowRouteCode );
		if( routeData==NULL ){
			routeData = dvaGetDefaultRoute();
			test_stationCode = 0;
		}
		else {
			test_stationCode = 0;
		}
		DUALPORT.ev5000_request.Sequence = routeData->station[test_stationCode].sequence;
		DUALPORT.ev5000_request.Complete = 0;
		DUALPORT.ev5000_request.Request = 1;

		IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;
		IOSIGNAL1.SIGNAL.WBIT.trsEnable = 0;
		IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
		IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
		IOSIGNAL1.SIGNAL.WBIT.trsOFF = 1;
		IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
		IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 0;
}

/************************************************************************
*																		*
*	Function name	<< dvaStandby >>								*
*					ƒXƒ^ƒ“ƒoƒCƒV[ƒPƒ“ƒXˆ—											*
*	Syntax			dvaStandby( void );										*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
//
//	ˆ—ŠT—vF
//			”ñƒAƒNƒeƒBƒu‚c‚u‚`‚t‚É‚È‚Á‚Ä‚¢‚éó‘Ô
//			ƒAƒNƒeƒBƒu‚c‚u‚`‚t‚©‚ç‚ÌØ‚è‘Ö‚¦—v‹‚ð‘Ò‚Â
//				Ø‘ÖŽž‚ÍA‚h‚c‚k‚dƒ‚[ƒh
//				‚Ü‚½‚ÍAƒeƒXƒgƒ‚[ƒh
//			‚P‚U—¼•Ò¬‚Ì‚½‚ß‚ÌƒQ[ƒgƒI[ƒvƒ“
//			‰¹ºƒoƒXM†‚Í@ŽóM‚Ì‚Ý
//			‚s‚q‚rM†‚Ì‘—M
//
void dvaStandby( void )
{
		if( befOpeMode != OPEMODE_STANDBY ){
			befOpeMode = OPEMODE_STANDBY;

			DUALPORT.activeDVAU=2;			// ”ñƒAƒNƒeƒBƒu‚c‚u‚`‚t
			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒN‚È‚µ

			// ’ÊMƒQ[ƒgƒI[ƒvƒ“
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 0;
			// ’ÊMƒ_ƒ~[’ïR‚n‚e‚e
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 1;
			// —ñŽÔ–³ü•ú‘—’âŽ~’†
			IOSIGNAL1.SIGNAL.WBIT.trsOFF = 1;
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			// ŽÔ“à•ú‘—‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 1;
		}

		// “ü—ÍM†‚ðƒAƒNƒeƒBƒu‚c‚u‚`‚t‚Ö’Ê’m
		DUALPORT.answerData.dvau.BYTE = nowSignal.SIGNAL2.BYTE;

		switch( DUALPORT.pollingData.voiceBus.BIT.mode ){
		case 0:
			// ‰¹ºƒoƒXŽg—pó‹µ‚ª‚b‚`‚a^‚b‚`‚a‚Ìê‡
			// ‚`‚b‚o‰¹ºo—Í‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 0;
			// ‚l‚b‚o‰¹ºo—Í‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 0;
			break;
		case 1:
			// ‰¹ºƒoƒXŽg—pó‹µ‚ªƒ}ƒCƒN•ú‘—‚Ìê‡
			// ‚`‚b‚o‰¹ºo—Í‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 0;
			// ‚l‚b‚o‰¹ºo—Í‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 0;
			break;
		case 2:
			break;
		case 3:
			// ‰¹ºƒoƒXŽg—pó‹µ‚ª—ñŽÔ–³ü‚Ìê‡
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			break;
		case 4:
			// ‰¹ºƒoƒXŽg—pó‹µ‚ªŽ©“®•ú‘—‚Ìê‡
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			break;
		}

		if( DUALPORT.changeDVAU==1 ){
			DUALPORT.changeDVAU = 0;
			backup_data.opeMode = OPEMODE_INITIAL_TEST;
		}
		if( DUALPORT.changeDVAU==2 ){
			DUALPORT.changeDVAU = 0;
			backup_data.opeMode = OPEMODE_IDLE;
		}
		if( DUALPORT.changeDVAU==3 ){
			DUALPORT.changeDVAU = 0;
			backup_data.opeMode = OPEMODE_TEST;
		}
}

/************************************************************************
*																		*
*	Function name	<< dvaCOUPLING >>									*
*					‚P‚U—¼•Ò¬ƒ`ƒFƒbƒNˆ—								*
*	Syntax			dvaCOUPLING( void );								*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
//
//	ˆ—ŠT—vF
//		‚P‚U—¼•Ò¬‚ÅA‰¹ºƒoƒX’ÊM‚ªˆÙí‚É‚È‚Á‚½‚Æ‚«A
//		Ž©‚c‚u‚`‚t‚Ì‚g‚b‚q‚ª‚n‚e‚e‚Å‚ ‚ê‚ÎA
//		ƒXƒ^ƒ“ƒoƒCƒ‚[ƒh‚ÉˆÚs‚·‚é
//
void dvaCOUPLING( void )
{
		switch( DUALPORT.couplingCheck ){
		case 0:		// ˜AŒ‹ƒ`ƒFƒbƒN‹N“®
			// ’ÊMƒQ[ƒgƒI[ƒvƒ“
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 0;
			// ’ÊMƒ_ƒ~[’ïR‚n‚e‚e
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 1;
			DUALPORT.couplingCheck = 1;		// ˜AŒ‹ƒ`ƒFƒbƒN‚ ‚è
			break;
		case 1:		// ˜AŒ‹ƒ`ƒFƒbƒN‹N“®’†
			break;
		case 2:		// ˜AŒ‹‚È‚µ‰ž“š
			// ’ÊMƒQ[ƒgƒNƒ[ƒY
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 1;
			// ’ÊMƒ_ƒ~[’ïR‚n‚m
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒNI—¹
			timedata.commGateTimer = 6000;	// ’ÊMƒQ[ƒgƒI[ƒvƒ“ƒ^ƒCƒ}[‹N“®
			break;
		case 3:		// ˜AŒ‹‚ ‚è‰ž“š
			// ’ÊMƒQ[ƒgƒNƒ[ƒY
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 1;
			// ’ÊMƒ_ƒ~[’ïR‚n‚m
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒNI—¹
			timedata.commGateTimer = 6000;	// ’ÊMƒQ[ƒgƒI[ƒvƒ“ƒ^ƒCƒ}[‹N“®

			if( nowSignal.SIGNAL2.RBIT.hcrON==0 ){
				DUALPORT.activeDVAU=0;		// ”ñƒAƒNƒeƒBƒu‚c‚u‚`‚t
				backup_data.opeMode = OPEMODE_STANDBY;
			}
			break;
		}
}


/************************************************************************
*																		*
*	Function name	<< dvaIDLE >>										*
*					‚h‚c‚k‚dƒV[ƒPƒ“ƒXˆ—								*
*	Syntax			dvaIDLE( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
//
//	ˆ—ŠT—vF
//		ƒCƒxƒ“ƒg‘Ò‚¿‚Ìó‘Ô
//
void dvaIDLE( void )
{
			// ƒ}ƒCƒN•ú‘—’†‚â‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Ì‚Æ‚«‚Íˆ—‚µ‚È‚¢
		if( DUALPORT.mcpMode.acpManual )
			return;
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// ƒ}ƒCƒN•ú‘—E‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚©‚ç‚Ì•œ‹AŽž
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// —ñŽÔ–³ü‚àƒLƒƒƒ“ƒZƒ‹
			befOpeMode = 0;
		}

			// —ñŽÔ–³ü•ú‘—’†‚ÍA’âŽ~‚·‚é‚Ì‚ð‘Ò‚Â
		if( DUALPORT.mcpMode.trsBUSY ){
			if( DUALPORT.answerData.dvau.BIT.trsRequest ){
				return;
			}
			// —ñŽÔ–³ü’âŽ~Žž‚ÍA•œ‹A‚³‚¹‚é
			DUALPORT.mcpMode.trsBUSY = 0;
			// Ž©“®•ú‘—‚ðƒLƒƒƒ“ƒZƒ‹—v‹‚ð‰ðœ
			DUALPORT.ev5000_request.Abort = 0;
			befOpeMode = 0;
		}


		if( befOpeMode != OPEMODE_IDLE ){
			befOpeMode = OPEMODE_IDLE;

			DUALPORT.activeDVAU=1;		// ƒAƒNƒeƒBƒu‚c‚u‚`‚tƒ‚[ƒh
			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒN‚È‚µ

			DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// Ž©“®•ú‘—ƒ‚[ƒh

			// ‚d‚u‚T‚O‚O‚O‚Ö•ú‘—ƒLƒƒƒ“ƒZƒ‹‚ð—v‹
			DUALPORT.ev5000_request.Abort = 1;

			// ’ÊMƒQ[ƒgƒNƒ[ƒY
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 1;
			// ’ÊMƒ_ƒ~[’ïR‚n‚m
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
			// —ñŽÔ–³ü•ú‘—‹N“®’†
			IOSIGNAL1.SIGNAL.WBIT.trsOFF = 0;
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			// ŽÔ“à•ú‘—‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;

			timedata.commGateTimer = 6000;	// ’ÊMƒQ[ƒgƒI[ƒvƒ“ƒ^ƒCƒ}[‹N“®

				// ‚l‚b‚o‚Ì‚q‚a‚k‚d‚c‚ðÁ“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RB=0;

				// ‚l‚b‚o‚Ì‹–‰ÂƒXƒCƒbƒ`‚ÌÝ’è
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
		}

		if( timedata.commGateTimer==0 ){	// ˜AŒ‹ƒ`ƒFƒbƒNƒ^ƒCƒ~ƒ“ƒO
			dvaCOUPLING();
		}

			// —ñŽÔ–³ü‹N“®‚ÌŠm”F
		if( DUALPORT.answerData.dvau.BIT.trsRequest ){
			backup_data.rootOpeMode = OPEMODE_IDLE;
			backup_data.opeMode = OPEMODE_TRS;
			return;
		}

			// ‚l‚b‚o‚©‚ç‚ÌŽ©“®•ú‘——v‹
		if( DUALPORT.mcpMode.ATCOn ){
			DUALPORT.mcpMode.ATCOn = 0;
			backup_data.opeMode = OPEMODE_AUTO;
		}

			// ‚l‚b‚o‚©‚ç‚ÌŽ©“®•ú‘—‰ðœ
		if( DUALPORT.mcpMode.ATCOff ){
			DUALPORT.mcpMode.ATCOff = 0;
		}

			// ‚l‚b‚o‚©‚ç‚Ì”ñí’Ê•ñ—v‹
		if( DUALPORT.mcpMode.EMGPOn ){
			backup_data.opeMode = OPEMODE_EMGHL;
		}

			// ‚l‚b‚o‚©‚ç‚Ì”ñí•ú‘——v‹
		if( DUALPORT.mcpMode.EMGOn ){
			backup_data.opeMode = OPEMODE_EMGMA;
		}

			// ‚l‚b‚o‚©‚ç‚ÌƒXƒyƒVƒƒƒ‹•ú‘——v‹
		if( DUALPORT.mcpMode.SPCOn ){
			backup_data.opeMode = OPEMODE_SPCMA;
		}

			// ‚l‚b‚o‚©‚ç‚ÌƒeƒXƒg—v‹
		if( DUALPORT.mcpMode.test ){
			DUALPORT.mcpMode.test = 0;
			backup_data.opeMode = OPEMODE_SELF_TEST;
		}

			// ‚s‚l‚r‚©‚ç‚ÌŽóMî•ñ‚ðŽó‚¯•t‚¯‚é
		if( DUALPORT.tms_request.Active ){		// ‚s‚l‚r—LŒø
			if( DUALPORT.tms_request.testStartRequest1 ){
				// ‚s‚l‚r‚©‚ç‚ÌƒeƒXƒg—v‹
				backup_data.opeMode = OPEMODE_TEST;
			}
			if( DUALPORT.tms_request.testStartRequest2 ){
				// ‚s‚l‚r‚©‚ç‚ÌƒeƒXƒg—v‹
				backup_data.opeMode = OPEMODE_TEST;
			}
				// ‚l‚b‚o‚Ì‚`‚s‚b@‚k‚d‚c‚ð“_–Å
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ATC=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ATC=1;

			DUALPORT.mcpMode.ATCEnable = 1;
		}
		else {									// ‚s‚l‚r–³Œø
				// ‚l‚b‚o‚Ì‚`‚s‚b@‚k‚d‚c‚ðÁ“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ATC=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ATC=0;

			DUALPORT.mcpMode.ATCEnable = 0;
		}
}

/************************************************************************
*																		*
*	Function name	<< dvaAUTO >>										*
*					Ž©“®•ú‘—ƒV[ƒPƒ“ƒXˆ—								*
*	Syntax			dvaAUTO( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaAUTO( void )
{
		int		i;
		struct ROUTEINFO *routeData;
		struct STATIONINFO *stationData;
		unsigned char	onoff;
		struct st_SIGNAL1 on;

			// ƒ}ƒCƒN•ú‘—’†‚â‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Ì‚Æ‚«‚Íˆ—‚µ‚È‚¢
		if( DUALPORT.mcpMode.acpManual )
			return;
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// ƒ}ƒCƒN•ú‘—E‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚©‚ç‚Ì•œ‹AŽž
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// —ñŽÔ–³ü‚àƒLƒƒƒ“ƒZƒ‹
			befOpeMode = 0;
		}

			// —ñŽÔ–³ü•ú‘—’†‚ÍA’âŽ~‚·‚é‚Ì‚ð‘Ò‚Â
		if( DUALPORT.mcpMode.trsBUSY ){
			if( DUALPORT.answerData.dvau.BIT.trsRequest ){
				return;
			}
			// —ñŽÔ–³ü’âŽ~Žž‚ÍA•œ‹A‚³‚¹‚é
			DUALPORT.mcpMode.trsBUSY = 0;
			// Ž©“®•ú‘—‚ðƒLƒƒƒ“ƒZƒ‹—v‹‚ð‰ðœ
			DUALPORT.ev5000_request.Abort = 0;
			befOpeMode = 0;
		}


		if( befOpeMode != OPEMODE_AUTO ){
			befOpeMode = OPEMODE_AUTO;

			DUALPORT.activeDVAU=1;		// ƒAƒNƒeƒBƒu‚c‚u‚`‚tƒ‚[ƒh
			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒN‚È‚µ

			DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// Ž©“®•ú‘—ƒ‚[ƒh

			// ’ÊMƒQ[ƒgƒNƒ[ƒY
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 1;
			// ’ÊMƒ_ƒ~[’ïR‚n‚m
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
			// —ñŽÔ–³ü•ú‘—‹N“®’†
			IOSIGNAL1.SIGNAL.WBIT.trsOFF = 0;
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			// ŽÔ“à•ú‘—‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;

			timedata.commGateTimer = 6000;	// ’ÊMƒQ[ƒgƒI[ƒvƒ“ƒ^ƒCƒ}[‹N“®

				// ‚l‚b‚o‚Ì‚q‚a‚k‚d‚c‚ðÁ“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RB=0;

				// ‚l‚b‚o‚Ì‚`‚s‚b‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ATC=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ATC=0;

				// ‚l‚b‚o‚Ì‹–‰ÂƒXƒCƒbƒ`‚ÌÝ’è
			DUALPORT.mcpMode.ATCEnable = 1;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
		}

		if( timedata.commGateTimer==0 ){	// ˜AŒ‹ƒ`ƒFƒbƒNƒ^ƒCƒ~ƒ“ƒO
			dvaCOUPLING();
		}

			// —ñŽÔ–³ü‹N“®‚ÌŠm”F
		if( DUALPORT.answerData.dvau.BIT.trsRequest ){
			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_TRS;
			return;
		}

			// ‚l‚b‚o‚©‚ç‚ÌŽ©“®•ú‘——v‹
		if( DUALPORT.mcpMode.ATCOn ){
			DUALPORT.mcpMode.ATCOn = 0;
		}

			// ‚l‚b‚o‚©‚ç‚ÌŽ©“®•ú‘—‰ðœ
		if( DUALPORT.mcpMode.ATCOff ){
			DUALPORT.mcpMode.ATCOff = 0;
			backup_data.opeMode = OPEMODE_IDLE;
		}

			// ‚l‚b‚o‚©‚ç‚Ì”ñí•ú‘—Žó‚¯•t‚¯
		if( DUALPORT.mcpMode.EMGOn ){
			DUALPORT.mcpMode.EMGOn = 0;

			backup_data.nowEMGCode = DUALPORT.mcpMode.EMGCode;
			// •ú‘—‹N“®
			DUALPORT.ev5000_request.Sequence = config_data.emgma[backup_data.nowEMGCode].sequence;
			DUALPORT.ev5000_request.Complete = 0;
			DUALPORT.ev5000_request.Abort = 1;
			DUALPORT.ev5000_request.Request = 1;

			backup_data.nowEMGCode = DUALPORT.mcpMode.EMGCode;
			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_EMGMA;
		}

			// ‚l‚b‚o‚©‚ç‚Ì”ñí’Ê•ñŠíŽó‚¯•t‚¯
		if( DUALPORT.mcpMode.EMGPOn ){
			DUALPORT.mcpMode.EMGPOn = 0;
			for( i=0; i<MAX_PAA; i++ ){
				backup_data.nowEMGHL[i] = DUALPORT.mcpMode.EMGPHL[i];
				DUALPORT.mcpMode.EMGPHL[i] = 0;
			}
			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_EMGHL;
		}

			// ‚l‚b‚o‚©‚ç‚ÌƒXƒyƒVƒƒƒ‹ƒƒbƒZ[ƒW•ú‘—Žó‚¯•t‚¯
		if( DUALPORT.mcpMode.SPCOn ){
			DUALPORT.mcpMode.SPCOn = 0;

			backup_data.nowSPCCode = DUALPORT.mcpMode.EMGCode;
			// •ú‘—‹N“®
			DUALPORT.ev5000_request.Sequence = config_data.spcma[backup_data.nowSPCCode].sequence;
			DUALPORT.ev5000_request.Complete = 0;
			DUALPORT.ev5000_request.Abort = 1;
			DUALPORT.ev5000_request.Request = 1;

			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_SPCMA;
		}

			// ‚l‚b‚o‚©‚ç‚ÌŽŸ‰wˆÄ“à—v‹
		if( DUALPORT.mcpMode.STNOn ){
			DUALPORT.mcpMode.STNOn = 0;

			if( DUALPORT.ev5000_request.Complete ){
				// •ú‘—‚ªI‚í‚Á‚Ä‚¢‚ê‚ÎŽó‚¯•t‚¯‚é

				stationData = backup_data.Station;
				if( stationData==NULL ){		// ‘O‰ñ•ú‘—•¶‚ª‚È‚¯‚ê‚Î@Œ»Ý‚Ì‰wƒR[ƒh‚©‚çLeaving ‚ð‹‚ß‚é
					stationData = dvaSearchStation( backup_data.nowRouteCode, backup_data.nowStationName );
				}
				if( stationData!=NULL ){
						// ƒf[ƒ^‚ª‚ ‚ê‚ÎA‚l‚b‚o‚Æ‚d‚h‚c‚r•\Ž¦A
					memcpy( DUALPORT.pollingData.mcpDisplay.BIT.lineDisplay[1], stationData->mcpStationChar, 6 );
					DUALPORT.eids.Station = stationData->eidsStationChar;

						// •ú‘—‚ðŠJŽn
					if( stationData!=NULL ){
						DUALPORT.ev5000_request.Sequence = stationData->sequence;
						DUALPORT.ev5000_request.Complete = 0;
						DUALPORT.ev5000_request.Request = 1;
					}
						// ŽŸ‰w‚ð‹‚ß‚é
					stationData++;
					if( stationData->stationCode==0 ){	
						// ÅI‚Ü‚Å‚É“ž’B‚µ‚½‚Æ‚«‚ÍAÅI‰w‚Å‚Æ‚ß‚é
						stationData--;
					}
					backup_data.Station = stationData;
				}
			}
		}

			// ‚l‚b‚o‚©‚ç‚ÌƒŠƒs[ƒg•ú‘——v‹
		if( DUALPORT.mcpMode.REPOn ){
			DUALPORT.mcpMode.REPOn = 0;

			if( DUALPORT.ev5000_request.Complete ){
				// •ú‘—‚ªI‚í‚Á‚Ä‚¢‚ê‚ÎŽó‚¯•t‚¯‚é

				if( DUALPORT.ev5000_request.Sequence!=NULL ){	
					// ‘O‰ñ•ú‘—‚ª—LŒø‚Ì‚Æ‚«
					DUALPORT.ev5000_request.Request = 1;
				}
			}
		}

			// ‚l‚b‚o‚©‚ç‚ÌƒeƒXƒg—v‹
		if( DUALPORT.mcpMode.test ){
			DUALPORT.mcpMode.test = 0;
			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_SELF_TEST;
		}

			// ‚s‚l‚r‚©‚ç‚ÌŽóMî•ñ‚ðŽó‚¯•t‚¯‚é
		if( DUALPORT.tms_request.Active ){			// ‚s‚l‚r—LŒø
			if( DUALPORT.tms_request.testStartRequest1 ){
				// ‚s‚l‚r‚©‚ç‚ÌƒeƒXƒg—v‹
				backup_data.rootOpeMode = OPEMODE_AUTO;
				backup_data.opeMode = OPEMODE_TEST;
			}
			if( DUALPORT.tms_request.testStartRequest2 ){
				// ‚s‚l‚r‚©‚ç‚ÌƒeƒXƒg—v‹
				backup_data.rootOpeMode = OPEMODE_AUTO;
				backup_data.opeMode = OPEMODE_TEST;
			}

			// ”ñí’Ê•ñŠí‘I‘ð‚ðŽó‚¯•t‚¯‚é
			for( i=0; i<MAX_PAA; i++ ){
				if( DUALPORT.tms_request.talkBack[i] ){
					// ‚s‚l‚r‚©‚ç‚Ì”ñí’Ê•ñ—v‹
					backup_data.nowEMGHL[i] = DUALPORT.tms_request.talkBack[i];
					DUALPORT.tms_request.talkBack[i] = 0;
					backup_data.rootOpeMode = OPEMODE_AUTO;
					backup_data.opeMode = OPEMODE_EMGHL;
				}
			}

			// •ú‘—‹ÖŽ~‚Ìˆ’u
			if( DUALPORT.tms_request.broadCastOff ){
				DUALPORT.tms_request.broadCastOff=0;
				DUALPORT.tms_request.change_broadcast=0;
				DUALPORT.tms_request.change_paActive=0;
				DUALPORT.tms_request.change_stationNumber=0;
				DUALPORT.tms_request.change_nextStationName=0;
				DUALPORT.tms_request.change_destination=0;
				DUALPORT.tms_request.change_routeCode=0;
			}

			// s‚«æˆÄ“à‚Ì•\Ž¦
			if( DUALPORT.tms_request.change_routeCode ){
				DUALPORT.tms_request.change_routeCode = 0;
				backup_data.nowRouteCode = DUALPORT.tms_request.routeCode;
				routeData = dvaSearchRoute( backup_data.nowRouteCode );
				if( routeData!=NULL ){
					dvaMCPDisplaySet( 0, routeData->mcpRouteChar );
				}
				DUALPORT.eids.Route = routeData->eidsRouteChar;
			}

			// Ž©“®•ú‘—‚Ì‹N“®ƒ`ƒFƒbƒN
			if( DUALPORT.tms_request.change_broadcast==0 
				&& DUALPORT.tms_request.change_paActive==0 ){
					;			// –¢•ú‘—ó‘Ô
			}
			else if( DUALPORT.tms_request.change_broadcast==1 
				&& DUALPORT.tms_request.change_paActive==0 ){
				DUALPORT.tms_request.change_broadcast = 0;

				if( DUALPORT.tms_request.change_nextStationName ){
					DUALPORT.tms_request.change_nextStationName = 0;
					backup_data.nowStationName = DUALPORT.tms_request.nextStationName;

						// ŽŸ‰wˆÄ“à•ú‘—‚Ì‹N“®
						// ƒ‹[ƒgƒR[ƒh‚Æ‰w–¼‚ÅŒŸõ‚·‚é
					stationData = dvaSearchStation( backup_data.nowRouteCode, backup_data.nowStationName );
					if( stationData!=NULL ){
							// ƒf[ƒ^‚ª‚ ‚ê‚ÎA‚l‚b‚o‚Æ‚d‚h‚c‚r•\Ž¦A
						dvaMCPDisplaySet( 1, stationData->mcpStationChar );
						DUALPORT.eids.Station = stationData->eidsStationChar;

							// •ú‘—‚ðŠJŽn
						backup_data.Station = stationData;
						DUALPORT.ev5000_request.Sequence = stationData->sequence;
						DUALPORT.ev5000_request.Complete = 0;
						DUALPORT.ev5000_request.Abort = 1;
						DUALPORT.ev5000_request.Request = 1;
					}
				}
			}
			else if( DUALPORT.tms_request.change_broadcast==0 
				&& DUALPORT.tms_request.change_paActive==1 ){
				DUALPORT.tms_request.change_broadcast = 0;
				DUALPORT.tms_request.change_paActive = 0;

				if( DUALPORT.tms_request.change_stationNumber ){
					DUALPORT.tms_request.change_stationNumber = 0;
						// “ž’…ˆÄ“à•ú‘—‚Ì‹N“®

							// ƒ‹[ƒg‚ÆƒXƒe[ƒVƒ‡ƒ“ƒiƒ“ƒo[‚ÅŒŸõ
					stationData = dvaSearchStationNumber( backup_data.nowRouteCode, DUALPORT.tms_request.stationNumber );
					if( stationData!=NULL ){
							// ƒf[ƒ^‚ª‚ ‚ê‚ÎA‰w–¼ƒR[ƒh‚ðÄ“xÝ’èiArrived •ú‘—‚Ì‚½‚ßj
						backup_data.nowStationName = stationData->stationCode;

							// ‚l‚b‚o‚Æ‚d‚h‚c‚r‚Ö•\Ž¦
						memcpy( DUALPORT.pollingData.mcpDisplay.BIT.lineDisplay[1], stationData->mcpStationChar, 6 );
						DUALPORT.eids.Station = stationData->eidsStationChar;

							// •ú‘—‚ðŠJŽn
						backup_data.Station = stationData;
						DUALPORT.ev5000_request.Sequence = stationData->sequence;
						DUALPORT.ev5000_request.Complete = 0;
						DUALPORT.ev5000_request.Abort = 1;
						DUALPORT.ev5000_request.Request = 1;
					}
				}
			}
			else if( DUALPORT.tms_request.change_broadcast==1 
				&& DUALPORT.tms_request.change_paActive==1 ){
				DUALPORT.tms_request.change_broadcast = 0;
				DUALPORT.tms_request.change_paActive = 0;

				// ƒXƒyƒVƒƒƒ‹ƒƒbƒZ[ƒW•ú‘—‚Ì‹N“®
				backup_data.nowSPCCode = DUALPORT.tms_request.specialCode;
				// •ú‘—‹N“®
				DUALPORT.ev5000_request.Sequence = config_data.spcma[backup_data.nowSPCCode].sequence;
				DUALPORT.ev5000_request.Complete = 0;
				DUALPORT.ev5000_request.Abort = 1;
				DUALPORT.ev5000_request.Request = 1;

				backup_data.rootOpeMode = OPEMODE_AUTO;
				backup_data.opeMode = OPEMODE_SPCMA;
			}

			DUALPORT.mcpMode.ATCEnable = 1;		// ‚`‚s‚bƒL[Žó‚¯•t‚¯‹–‰Â
			DUALPORT.mcpMode.STNEnable = 0;		// ‚r‚s‚mƒL[Žó‚¯•t‚¯‹ÖŽ~
			DUALPORT.mcpMode.REPEnable = 0;		// ‚q‚d‚oƒL[Žó‚¯•t‚¯‹ÖŽ~
		}
		else {
			DUALPORT.mcpMode.ATCEnable = 1;		// ‚`‚s‚bƒL[Žó‚¯•t‚¯‹–‰Â
			DUALPORT.mcpMode.STNEnable = 1;		// ‚r‚s‚mƒL[Žó‚¯•t‚¯‹–‰Â
			DUALPORT.mcpMode.REPEnable = 1;		// ‚q‚d‚oƒL[Žó‚¯•t‚¯‹–‰Â
		}

			// “ž’…‰w•ú‘—(Arriving)‚ªI—¹‚µ‚½‚çA“ž’…‰w•ú‘—(Arrived)‚ð‹N“®‚·‚é
		if( backup_data.Station->Arriving ){
			if( DUALPORT.ev5000_request.Complete ){
				// •ú‘—I—¹ó‘Ô
				stationData = backup_data.Station;
				stationData++;
				if( stationData->Arrived  ){	// Arrived ‚ ‚èH
					// “ž’…‰w•ú‘—(Arrived)‚Ì•ú‘—
					DUALPORT.ev5000_request.Sequence = stationData->sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
					backup_data.Station = stationData;
				}
				DUALPORT.ev5000_request.Complete=0;
			}
		}

			// ƒhƒAÚ“_“ü—Í‚ÌŽó‚¯•t‚¯
		onoff = nowSignal.SIGNAL1.BYTE ^ befSignal1;
		if( onoff ){
			on.SIGNAL.BYTE = onoff & nowSignal.SIGNAL1.BYTE;
			if( DUALPORT.ev5000_request.Complete ){
				// •ú‘—‚ªI—¹‚µ‚Ä‚¢‚é‚Æ‚«
				// ƒhƒA‚`‚Ìƒ`ƒƒƒCƒ€
				if( on.SIGNAL.RBIT.doorOpenAChime ){
					DUALPORT.ev5000_request.Sequence = config_data.door[DOOR_OPEN_A].chime_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
				if( on.SIGNAL.RBIT.doorCloseAChime ){
					DUALPORT.ev5000_request.Sequence = config_data.door[DOOR_CLOSE_A].chime_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
				if( on.SIGNAL.RBIT.doorOpenAAnnounce ){
					DUALPORT.ev5000_request.Sequence = config_data.door[DOOR_OPEN_A].announce_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
				if( on.SIGNAL.RBIT.doorCloseAAnnounce ){
					DUALPORT.ev5000_request.Sequence = config_data.door[DOOR_CLOSE_A].announce_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
				if( on.SIGNAL.RBIT.doorOpenBChime ){
					DUALPORT.ev5000_request.Sequence = config_data.door[DOOR_OPEN_B].chime_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
				if( on.SIGNAL.RBIT.doorCloseBChime ){
					DUALPORT.ev5000_request.Sequence = config_data.door[DOOR_CLOSE_B].chime_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
				if( on.SIGNAL.RBIT.doorOpenBAnnounce ){
					DUALPORT.ev5000_request.Sequence = config_data.door[DOOR_OPEN_B].announce_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
				if( on.SIGNAL.RBIT.doorCloseBAnnounce ){
					DUALPORT.ev5000_request.Sequence = config_data.door[DOOR_CLOSE_B].announce_sequence;
					DUALPORT.ev5000_request.Complete = 0;
					DUALPORT.ev5000_request.Request = 1;
				}
			}
			befSignal1 = nowSignal.SIGNAL1.BYTE;
		}
}

/************************************************************************
*																		*
*	Function name	<< dvaTRS >>										*
*					—ñŽÔ–³ü•ú‘—ƒV[ƒPƒ“ƒXˆ—							*
*	Syntax			dvaTRS( void );										*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaTRS( void )
{
		int		i;

			// ƒ}ƒCƒN•ú‘—’†‚â‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Ì‚Æ‚«‚Íˆ—‚µ‚È‚¢
		if( DUALPORT.mcpMode.acpManual )
			return;
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// ƒ}ƒCƒN•ú‘—E‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚©‚ç‚Ì•œ‹AŽž
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// —ñŽÔ–³ü‚àƒLƒƒƒ“ƒZƒ‹
			befOpeMode = 0;
		}

		if( befOpeMode != OPEMODE_TRS ){
			befOpeMode = OPEMODE_TRS;

			DUALPORT.activeDVAU=1;		// ƒAƒNƒeƒBƒu‚c‚u‚`‚tƒ‚[ƒh

			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒN‚È‚µ

			DUALPORT.pollingData.voiceBus.BIT.mode = 3;	// ‰¹ºƒoƒXŽg—pó‹µ‚Í—ñŽÔ–³ü•ú‘—

			// ‚d‚u‚T‚O‚O‚O‚Ö•ú‘—ƒLƒƒƒ“ƒZƒ‹‚ðŽwŽ¦
			DUALPORT.ev5000_request.Abort = 1;

			// ’ÊMƒQ[ƒgƒNƒ[ƒY
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 1;
			// ’ÊMƒ_ƒ~[’ïR‚n‚m
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
			// —ñŽÔ–³ü•ú‘—‹N“®’†
			IOSIGNAL1.SIGNAL.WBIT.trsOFF = 0;
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			// ŽÔ“à•ú‘—‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;

			timedata.commGateTimer = 6000;	// ’ÊMƒQ[ƒgƒI[ƒvƒ“ƒ^ƒCƒ}[‹N“®

				// ‚l‚b‚o‚Ì‚q‚a‚k‚d‚c‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RB=0;

				// ‚l‚b‚o‚Ì‹–‰ÂƒXƒCƒbƒ`‚ÌÝ’è
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;

			DUALPORT.mcpMode.trsBUSY=1;		// —ñŽÔ–³ü•ú‘—’†
		}

			// ‚l‚b‚o‚©‚ç‚Ì”ñí•ú‘—Žó‚¯•t‚¯
		if( DUALPORT.mcpMode.EMGOn ){
			DUALPORT.mcpMode.EMGOn = 0;

			backup_data.nowEMGCode = DUALPORT.mcpMode.EMGCode;
			// •ú‘—‹N“®
			DUALPORT.ev5000_request.Sequence = config_data.emgma[backup_data.nowEMGCode].sequence;
			DUALPORT.ev5000_request.Complete = 0;
			DUALPORT.ev5000_request.Abort = 1;
			DUALPORT.ev5000_request.Request = 1;

			backup_data.nowEMGCode = DUALPORT.mcpMode.EMGCode;
			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_EMGMA;
		}

			// ‚l‚b‚o‚©‚ç‚Ì”ñí’Ê•ñŠíŽó‚¯•t‚¯
		if( DUALPORT.mcpMode.EMGPOn ){
			DUALPORT.mcpMode.EMGPOn = 0;
			for( i=0; i<MAX_PAA; i++ ){
				backup_data.nowEMGHL[i] = DUALPORT.mcpMode.EMGPHL[i];
				DUALPORT.mcpMode.EMGPHL[i] = 0;
			}
			backup_data.rootOpeMode = OPEMODE_AUTO;
			backup_data.opeMode = OPEMODE_EMGHL;
		}

			// —ñŽÔ–³ü•ú‘—’†‚ÍA’âŽ~‚·‚é‚Ì‚ð‘Ò‚Â
		if( DUALPORT.answerData.dvau.BIT.trsRequest ){
			return;
		}

		// —ñŽÔ–³ü’âŽ~Žž‚ÍA•œ‹A‚³‚¹‚é
		DUALPORT.mcpMode.trsBUSY = 0;
		// Ž©“®•ú‘—‚ðƒLƒƒƒ“ƒZƒ‹—v‹‚ð‰ðœ
		DUALPORT.ev5000_request.Abort = 0;

		// ŒÄoŒ³‚Ö•œ‹A‚·‚é
		backup_data.opeMode = backup_data.rootOpeMode;
}

/************************************************************************
*																		*
*	Function name	<< dvaEMGMA >>										*
*					”ñí•ú‘—ƒV[ƒPƒ“ƒXˆ—								*
*	Syntax			dvaEMGMA( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaEMGMA( void )
{
			// ƒ}ƒCƒN•ú‘—’†‚â‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Ì‚Æ‚«‚Íˆ—‚µ‚È‚¢
		if( DUALPORT.mcpMode.acpManual )
			return;
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// ƒ}ƒCƒN•ú‘—E‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚©‚ç‚Ì•œ‹AŽž
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// —ñŽÔ–³ü‚àƒLƒƒƒ“ƒZƒ‹
			befOpeMode = 0;
		}

		if( befOpeMode != OPEMODE_EMGMA ){
			befOpeMode = OPEMODE_EMGMA;

			DUALPORT.activeDVAU=1;		// ƒAƒNƒeƒBƒu‚c‚u‚`‚tƒ‚[ƒh
			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒN‚È‚µ

			DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// Ž©“®•ú‘—ƒ‚[ƒh

			// ’ÊMƒQ[ƒgƒNƒ[ƒY
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 1;
			// ’ÊMƒ_ƒ~[’ïR‚n‚m
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
			// —ñŽÔ–³ü•ú‘—’âŽ~’†
			IOSIGNAL1.SIGNAL.WBIT.trsOFF = 1;
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			// ŽÔ“à•ú‘—‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;

			timedata.commGateTimer = 6000;	// ’ÊMƒQ[ƒgƒI[ƒvƒ“ƒ^ƒCƒ}[‹N“®

				// ‚l‚b‚o‚Ì‚q‚a‚k‚d‚c‚ðÁ“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RB=0;

				// ‚l‚b‚o‚Ì‹–‰ÂƒXƒCƒbƒ`‚ÌÝ’è
			DUALPORT.mcpMode.ATCEnable = 1;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 1;
		}

			// ‚l‚b‚o‚©‚ç‚ÌƒŠƒs[ƒg—v‹
		if( DUALPORT.mcpMode.REPOn ){
			DUALPORT.mcpMode.REPOn = 0;

			DUALPORT.ev5000_request.Abort = 1;
			DUALPORT.ev5000_request.Sequence = config_data.emgma[backup_data.nowEMGCode].sequence;
			DUALPORT.ev5000_request.Request = 1;
		}

			// ‚d‚u‚T‚O‚O‚O‚©‚ç‚Ì•ú‘—‚ªI—¹‚µ‚½‚çAŒÄoŒ³‚Ìƒ‚[ƒh‚Ö–ß‚é
		if( DUALPORT.ev5000_request.Complete ){
			backup_data.opeMode = backup_data.rootOpeMode;
		}
}

/************************************************************************
*																		*
*	Function name	<< dvaSPCMA >>										*
*					ƒXƒyƒVƒƒƒ‹•ú‘—ƒV[ƒPƒ“ƒXˆ—						*
*	Syntax			dvaSPCMA( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaSPCMA( void )
{
			// ƒ}ƒCƒN•ú‘—’†‚â‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Ì‚Æ‚«‚Íˆ—‚µ‚È‚¢
		if( DUALPORT.mcpMode.acpManual )
			return;
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// ƒ}ƒCƒN•ú‘—E‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚©‚ç‚Ì•œ‹AŽž
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// —ñŽÔ–³ü‚àƒLƒƒƒ“ƒZƒ‹
			befOpeMode = 0;
		}

		if( befOpeMode != OPEMODE_SPCMA ){
			befOpeMode = OPEMODE_SPCMA;

			DUALPORT.activeDVAU=1;		// ƒAƒNƒeƒBƒu‚c‚u‚`‚tƒ‚[ƒh
			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒN‚È‚µ

			DUALPORT.pollingData.voiceBus.BIT.mode = 4;		// Ž©“®•ú‘—ƒ‚[ƒh

			// ’ÊMƒQ[ƒgƒNƒ[ƒY
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 1;
			// ’ÊMƒ_ƒ~[’ïR‚n‚m
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
			// —ñŽÔ–³ü•ú‘—‹N“®’†
			IOSIGNAL1.SIGNAL.WBIT.trsOFF = 0;
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			// ŽÔ“à•ú‘—‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;

			timedata.commGateTimer = 6000;	// ’ÊMƒQ[ƒgƒI[ƒvƒ“ƒ^ƒCƒ}[‹N“®

				// ‚l‚b‚o‚Ì‚q‚a‚k‚d‚c‚ðÁ“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RB=0;

				// ‚l‚b‚o‚Ì‹–‰ÂƒXƒCƒbƒ`‚ÌÝ’è
			DUALPORT.mcpMode.ATCEnable = 1;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 1;
		}

			// —ñŽÔ–³ü‹N“®‚ÌŠm”F
		if( DUALPORT.answerData.dvau.BIT.trsRequest ){
			backup_data.opeMode = OPEMODE_TRS;
			return;
		}

			// ‚l‚b‚o‚©‚ç‚ÌƒŠƒs[ƒg—v‹
		if( DUALPORT.mcpMode.REPOn ){
			DUALPORT.mcpMode.REPOn = 0;

			DUALPORT.ev5000_request.Abort = 1;
			DUALPORT.ev5000_request.Sequence = config_data.spcma[backup_data.nowSPCCode].sequence;
			DUALPORT.ev5000_request.Request = 1;
		}

			// ‚d‚u‚T‚O‚O‚O‚©‚ç‚Ì•ú‘—‚ªI—¹‚µ‚½‚çAŒÄoŒ³‚Ìƒ‚[ƒh‚Ö–ß‚é
		if( DUALPORT.ev5000_request.Complete ){
			backup_data.opeMode = backup_data.rootOpeMode;
		}
}

/************************************************************************
*																		*
*	Function name	<< dvaEMGHL >>										*
*					”ñí’Ê•ñƒV[ƒPƒ“ƒXˆ—								*
*	Syntax			dvaEMGHL( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaEMGHL( void )
{
		int		i;

			// ƒ}ƒCƒN•ú‘—’†‚â‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Ì‚Æ‚«‚Íˆ—‚µ‚È‚¢
		if( DUALPORT.mcpMode.acpManual )
			return;
		if( DUALPORT.mcpMode.mcpManual )
			return;
		if( DUALPORT.mcpMode.Cab )
			return;

			// ƒ}ƒCƒN•ú‘—E‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚©‚ç‚Ì•œ‹AŽž
		if( DUALPORT.mcpMode.Release ){
			DUALPORT.mcpMode.Release = 0;
			DUALPORT.mcpMode.trsBUSY = 0;		// —ñŽÔ–³ü‚àƒLƒƒƒ“ƒZƒ‹
			befOpeMode = 0;
		}

		if( befOpeMode != OPEMODE_EMGHL ){
			befOpeMode = OPEMODE_EMGHL;

			DUALPORT.activeDVAU=1;		// ƒAƒNƒeƒBƒu‚c‚u‚`‚tƒ‚[ƒh
			DUALPORT.couplingCheck = 0;		// ˜AŒ‹ƒ`ƒFƒbƒN‚È‚µ

			DUALPORT.pollingData.voiceBus.BIT.mode = 2;	// ”ñí’Ê•ñƒ‚[ƒh

			DUALPORT.ev5000_request.Abort = 1;

			// ’ÊMƒQ[ƒgƒNƒ[ƒY
			IOSIGNAL1.SIGNAL.WBIT.commGateOFF = 1;
			// ’ÊMƒ_ƒ~[’ïR‚n‚m
			IOSIGNAL1.SIGNAL.WBIT.commDummyOFF = 0;
			// —ñŽÔ–³ü•ú‘—’âŽ~
			IOSIGNAL1.SIGNAL.WBIT.trsOFF = 1;
			// ‚`‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;
			// ‚l‚b‚o‰¹ºo—Í‹ÖŽ~
			IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;
			// ŽÔ“à•ú‘—‹–‰Â
			IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;

			timedata.commGateTimer = 6000;	// ’ÊMƒQ[ƒgƒI[ƒvƒ“ƒ^ƒCƒ}[‹N“®

				// ‚l‚b‚o‚Ì‚q‚a‚k‚d‚c‚ðÁ“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RB=0;

				// ‚l‚b‚o‚Ì‹–‰ÂƒXƒCƒbƒ`‚ÌÝ’è
			DUALPORT.mcpMode.ATCEnable = 0;
			DUALPORT.mcpMode.STNEnable = 0;
			DUALPORT.mcpMode.REPEnable = 0;
		}

			// ’Ê•ñŠí‚Ì‰¹ºƒQ[ƒg‚ðƒI[ƒvƒ“
		for( i=0; i<MAX_PAA; i++ ){
			DUALPORT.pollingData.voiceBus.BYTE.BYTE[i+1] = backup_data.nowEMGHL[i];
		}

			// ‚·‚×‚Ä‚Ì’Ê•ñŠí‚ÌƒXƒCƒbƒ`‚ª‚n‚e‚e‚É‚È‚Á‚½‚çAŒÄ‚Ño‚µŒ³‚Ìƒ‚[ƒh‚Ö–ß‚é
		for( i=0; i<MAX_PAA; i++ ){
			if( DUALPORT.answerData.rscSW[i].BYTE )
				break;
		}
		if( i==MAX_PAA ){
			backup_data.opeMode = backup_data.rootOpeMode;
		}
}

/************************************************************************
*																		*
*	Function name	<< dvaProcess >>									*
*					ƒV[ƒPƒ“ƒXˆ—										*
*	Syntax			dvaProcess( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaProcess( void )
{
		if( DUALPORT.activeDVAU ){
			switch( backup_data.opeMode ){
			case OPEMODE_INITIAL_TEST:
				dvaInitialTEST();
				break;
			case OPEMODE_TEST:
				dvaTEST();
				break;
			case OPEMODE_IDLE:
				dvaIDLE();
				break;
			case OPEMODE_AUTO:
				dvaAUTO();
				break;
			case OPEMODE_EMGMA:
				dvaEMGMA();
				break;
			case OPEMODE_EMGHL:
				dvaEMGHL();
				break;
			case OPEMODE_TRS:
				dvaTRS();
				break;
			}
		}
		else {
			dvaStandby();
		}
}

