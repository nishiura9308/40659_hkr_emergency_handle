/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	‚l‚b‚o‘€ìƒ‚ƒWƒ…[ƒ‹											*
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

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
#define	MCP_RESET		0
#define	MCP_IDLE		1
#define	MCP_LINE_SELECT		2
#define	MCP_LINE_SELECT2		3
#define	MCP_LANG_SELECT		4
#define	MCP_LANG_SELECT2	5
#define	MCP_EMGP1		6
#define	MCP_EMGP2		7
#define	MCP_EMG1		8
#define	MCP_EMG2		9
#define	MCP_SPC1		10
#define	MCP_SPC2		11

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
// ƒ‹[ƒgŒŸõ
extern	struct ROUTEINFO *dvaSearchRoute( char routeCode );
// ‰w–¼ŒŸõ
extern	struct STATIONINFO *dvaSearchStation( char routeCode, char stationCode );
// ÅI‰wŒŸõ
extern	struct STATIONINFO *dvaSearchDestination( char routeCode );

extern	struct	BACKUP_DATA		backup_data;		// ƒoƒbƒNƒAƒbƒvƒf[ƒ^
extern	struct	CONFIG_INFO		config_data;		// Ý’èî•ñ

extern	struct st_ReadSIGNAL nowSignal;			// ÅV“Ç‚Ý‚¾‚µ’l

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

static	int		mcp_phase;				// ˆ—ƒtƒF[ƒY

static	unsigned char	select_rscSW_row;		// ”ñí’Ê•ñŠí”Ô†i‚o‚`‚`”Ô†j
static	unsigned char	select_rscSW_col;		// ”ñí’Ê•ñŠí”Ô†iƒnƒ“ƒhƒ‹”Ô†j
static	unsigned char	select_emgCode;			// ‘I‘ð’†”ñí•ú‘—ƒR[ƒh
static	unsigned char	select_spcCode;			// ‘I‘ð’†ƒXƒyƒVƒƒƒ‹•ú‘—ƒR[ƒh
static	unsigned char	select_routeCode;		// ‘I‘ð’†ƒ‹[ƒgƒR[ƒh
static	struct SELECT_LANG	select_lang;		// ‘I‘ð’†Œ¾Œê
static	int		item_skip;						// ‘I‘ð”Ô†

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< MCPDisplayBlank >>								*
*					‚l‚b‚o‚Öƒuƒ‰ƒ“ƒN‚ðÝ’è								*
*	Syntax			MCPDisplayBlank( line );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
static	void MCPDisplayBlank( char line )
{
		memcpy( DUALPORT.pollingData.mcpDisplay.BIT.lineDisplay[line], "      ", 6 );
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPDisplaySet >>								*
*					‚l‚b‚o‚Ö•\Ž¦î•ñ‚ðÝ’è								*
*	Syntax			dvaMCPDisplaySet( line, strings );					*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaMCPDisplaySet( char line, char *strings )
{
		memcpy( DUALPORT.pollingData.mcpDisplay.BIT.lineDisplay[line], strings, 6 );
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPOpeEMGPDisplay >>								*
*					‚l‚b‚o‚Ö”ñí’Ê•ñŠí‚ð•\Ž¦										*
*	Syntax			dvaMCPOpeEMGPDisplay( void );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int dvaMCPOpeEMGPDisplay( char skip )
{
		int		i;
		int		j;
		int		k;
		char	work[16];

		MCPDisplayBlank( 2 );
		MCPDisplayBlank( 3 );
		k=0;
		for( i=0; i<8; i++ ){
			for( j=0; j<8; j++ ){
				if( DUALPORT.answerData.rscSW[i].BYTE & (0x0001<j) ){
					if( k<2 && skip==0 ){
						sprintf( work, "EH%02u%02u", i+1, j+1 );
						dvaMCPDisplaySet( k+2, work );
						select_rscSW_row = i;
						select_rscSW_col = j;
					}
					if( skip )
						skip--;
					k++;
				}
			}
		}

		sprintf( work, "PAEH%2u", k );
		dvaMCPDisplaySet( 0, work );

		return( skip );
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPOpeEMGDisplay >>								*
*					‚l‚b‚o‚Ö”ñí•ú‘—‘I‘ð‚ð•\Ž¦										*
*	Syntax			dvaMCPOpeEMGDisplay( void );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int dvaMCPOpeEMGDisplay( char skip )
{
		int		i;
		int		j;
		int		k;
		char	work[16];

		MCPDisplayBlank( 2 );
		MCPDisplayBlank( 3 );
		k=0;
		for( i=0; i<32; i++ ){
			if( config_data.emgma[i].eidsChar!=0 ){
				if( k<1 && skip==0 ){
					sprintf( work, "EMG-%02u", i+1 );
					dvaMCPDisplaySet( k+2, work );
					select_emgCode = i;
				}
				if( skip )
					skip--;
				k++;
			}
		}

		dvaMCPDisplaySet( 0, "EMG-MA" );

		return( skip );
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPOpeSPCDisplay >>								*
*					‚l‚b‚o‚ÖƒXƒyƒVƒƒƒ‹•ú‘—‘I‘ð‚ð•\Ž¦										*
*	Syntax			dvaMCPOpeSPCDisplay( void );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int dvaMCPOpeSPCDisplay( char skip )
{
		int		i;
		int		j;
		int		k;
		char	work[16];

		MCPDisplayBlank( 2 );
		MCPDisplayBlank( 3 );
		k=0;
		for( i=0; i<32; i++ ){
			if( config_data.spcma[i].eidsChar!=0 ){
				if( k<1 && skip==0 ){
					sprintf( work, "SPC-%02u", i+1 );
					dvaMCPDisplaySet( k+2, work );
					select_spcCode = i;
				}
				if( skip )
					skip--;
				k++;
			}
		}

		dvaMCPDisplaySet( 0, "SPC-MA" );

		return( skip );
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPOpeLineDisplay >>							*
*					‚l‚b‚o‚Öƒ‰ƒCƒ“‘I‘ð‚ð•\Ž¦							*
*	Syntax			dvaMCPOpeLineDisplay( void );						*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int dvaMCPOpeLineDisplay( unsigned char *routeCode, int adv )
{
		int		i;
		int		j;
		int		k;
		char	work[16];
		struct ROUTEINFO	*routeData;
		struct STATIONINFO	*stationData;

		for( i=0; i<MAX_ROUTE; i++ ){
			routeData = dvaSearchRoute( *routeCode );
			stationData = dvaSearchDestination( *routeCode );
			if( routeData!=NULL && stationData!=NULL ){
				break;
			}
			if( adv ){
				*routeCode = *routeCode+1;
			}
			else {
				*routeCode = *routeCode-1;
			}
			if( *routeCode==0 ){
				*routeCode = 1;
			}
		}

		if( routeData!=NULL ){
			dvaMCPDisplaySet( 0, (char *)routeData->mcpRouteChar );
			if( stationData!=NULL ){
				dvaMCPDisplaySet( 2, (char *)stationData->mcpStationChar );
				return( 0 );
			}
		}

		return( 1 );
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPOpeLangDisplay >>							*
*					‚l‚b‚o‚ÖŒ¾Œê‘I‘ð‚ð•\Ž¦								*
*	Syntax			dvaMCPOpeLangDisplay( void );						*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int dvaMCPOpeLangDisplay( struct SELECT_LANG *lang, int adv )
{
		int		i;
		int		j;
		int		k;
		int		mode;

		// Œ»Ý‚Ìó‹µ‚ðƒ`ƒFƒbƒN
		if( config_data.lang.en ){
			if( config_data.lang.cn ){
				if( config_data.lang.ma ){
					mode = 3;		// E-C-M
				}
				else {
					mode = 1;		// EN-CN
				}
			}
			else if( config_data.lang.ma ){
				mode = 2;			// EN-MA
			}
			else {
				mode = 0;			// ENG
			}
		}
		else {
			mode = 0;				// ENG
		}

		// ŽŸ‚Ì‘I‘ð‚ðŒˆ’è
		mode += adv;
		if( mode>3 )
			mode=3;

		// ‘I‘ðŒã‚Ì•\Ž¦
		switch( mode ){
		case 0:		/* ENG */
			config_data.lang.en = 1;
			config_data.lang.cn = 0;
			config_data.lang.ma = 0;
			dvaMCPDisplaySet( 2, " ENG  " );
			break;
		case 1:		/* EN-CN */
			config_data.lang.en = 1;
			config_data.lang.cn = 1;
			config_data.lang.ma = 0;
			dvaMCPDisplaySet( 2, "EN-CN " );
			break;
		case 2:		/* EN-MA */
			config_data.lang.en = 1;
			config_data.lang.cn = 0;
			config_data.lang.ma = 1;
			dvaMCPDisplaySet( 2, "EN-MA " );
			break;
		default:
			config_data.lang.en = 1;
			config_data.lang.cn = 1;
			config_data.lang.ma = 1;
			dvaMCPDisplaySet( 2, "E-C-M " );
			break;
		}

		return( 1 );
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPCab >>									*
*					‚l‚b‚o^‚`‚b‚o‚É‚æ‚é‚b‚`‚aƒ‚[ƒhˆ—									*
*	Syntax			dvaMCPCab( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
//	‰^“]ÈŠÔ˜A—
void dvaMCPCab( void )
{
		IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 1;	// ŽÔ“à•ú‘—‹ÖŽ~
		IOSIGNAL1.SIGNAL.WBIT.acpDisable = 0;		// ‚`‚b‚o‰¹º‹–‰Â
		IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 0;		// ‚l‚b‚o‰¹º‹–‰Â
		IOSIGNAL1.SIGNAL.WBIT.trsOFF = 1;			// —ñŽÔ–³ü‹N“®’âŽ~
		IOSIGNAL1.SIGNAL.WBIT.trsEnable = 0;		// —ñŽÔ–³ü•ú‘—‹ÖŽ~

			// ‰¹ºƒoƒXŽg—pó‹µ‚Í@‚b‚`‚a^‚b‚`‚a
		DUALPORT.pollingData.voiceBus.BIT.mode = 0;		
			// ‚b‚`‚aƒXƒCƒbƒ`‚k‚d‚c‚ð“_“”
		DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.CAB=1;
		DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.CAB=0;
			// ‚`‚b‚oƒXƒCƒbƒ`‚k‚d‚c‚ð“_“”
		IOSIGNAL1.SIGNAL.WBIT.acpCabLED=1;
			// ‚l‚b‚o‚Ö‚b‚`‚a^‚b‚`‚a•ú‘—’†‚ð•\Ž¦
		dvaMCPDisplaySet( 0, "CA/CA " );

			// Ž©“®•ú‘—‚ðƒLƒƒƒ“ƒZƒ‹—v‹
		DUALPORT.ev5000_request.Abort = 1;

		// Œ³‚Ìó‘Ô‚Ö‚Ì•œ‹A—v‹‚Ì‰ðœ
		DUALPORT.mcpMode.Release = 0;	
			// ‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚ÌÝ’è
		DUALPORT.mcpMode.Cab = 1;

		mcp_phase = MCP_IDLE;
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPManual >>									*
*					‚l‚b‚o^‚`‚b‚o‚É‚æ‚éƒ}ƒjƒ…ƒAƒ‹ƒ‚[ƒhˆ—			*
*	Syntax			dvaMCPManual( void );								*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
//	ŽÔ“àƒ}ƒCƒN•ú‘—
void dvaMCPManual( void )
{
		IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;	// ŽÔ“à•ú‘—’†
		IOSIGNAL1.SIGNAL.WBIT.acpDisable = 0;		// ‚`‚b‚o‰¹º‹–‰Â
		IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 0;		// ‚l‚b‚o‰¹º‹–‰Â
		IOSIGNAL1.SIGNAL.WBIT.trsOFF = 1;			// —ñŽÔ–³ü‹N“®’âŽ~
		IOSIGNAL1.SIGNAL.WBIT.trsEnable = 0;		// —ñŽÔ–³ü•ú‘—‹ÖŽ~

			// ‰¹ºƒoƒXŽg—pó‹µ‚Í@ƒ}ƒCƒN•ú‘—
		DUALPORT.pollingData.voiceBus.BIT.mode = 1;		
			// ‚l‚b‚o‚Öƒ}ƒCƒN•ú‘—’†‚ð•\Ž¦
		dvaMCPDisplaySet( 0, "MANUAL" );

		// Ž©“®•ú‘—‚ðƒLƒƒƒ“ƒZƒ‹—v‹
		DUALPORT.ev5000_request.Abort = 1;

		// Œ³‚Ìó‘Ô‚Ö‚Ì•œ‹A—v‹‚Ì‰ðœ
		DUALPORT.mcpMode.Release = 0;	

		mcp_phase = MCP_IDLE;
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPRelease >>									*
*					‚l‚b‚o^‚`‚b‚o‚É‚æ‚é								*
*					‚b‚`‚a^‚b‚`‚aƒ}ƒjƒ…ƒAƒ‹ƒ‚[ƒh‰ðœˆ—				*
*	Syntax			dvaMCPManual( void );								*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
//	ŽÔ“à•ú‘—E‚`‚b‚o^‚l‚b‚o‰¹º•s‰Â
void dvaMCPRelease( void )
{
		IOSIGNAL1.SIGNAL.WBIT.audioBusDisable = 0;	// ŽÔ“à•ú‘—’†
		IOSIGNAL1.SIGNAL.WBIT.acpDisable = 1;		// ‚`‚b‚o‰¹º‹ÖŽ~
		IOSIGNAL1.SIGNAL.WBIT.mcpDisable = 1;		// ‚l‚b‚o‰¹º‹ÖŽ~

			// ‚b‚`‚aƒXƒCƒbƒ`‚k‚d‚c‚ðÁ“”
		DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.CAB=0;
		DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.CAB=0;
			// ‚`‚b‚oƒXƒCƒbƒ`‚k‚d‚c‚ðÁ“”
		IOSIGNAL1.SIGNAL.WBIT.acpCabLED=0;

			// Ž©“®•ú‘—‚ðƒLƒƒƒ“ƒZƒ‹—v‹‚ð‰ðœ
		DUALPORT.ev5000_request.Abort = 0;

			// ‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Ì‰ðœ
		DUALPORT.mcpMode.Cab = 0;
			// ‚l‚b‚o‚É‚æ‚éƒ}ƒjƒ…ƒAƒ‹ƒ‚[ƒh‚ð‰ðœ
		DUALPORT.mcpMode.mcpManual = 0;	
			// ‚`‚b‚o‚É‚æ‚éƒ}ƒjƒ…ƒAƒ‹ƒ‚[ƒh‚ð‰ðœ
		DUALPORT.mcpMode.acpManual = 0;	
			// Œ³‚Ìó‘Ô‚Ö‚Ì•œ‹A—v‹
		DUALPORT.mcpMode.Release = 1;

		mcp_phase = MCP_RESET;
}

/************************************************************************
*																		*
*	Function name	<< dvaMCPSWIn >>									*
*					‚l‚b‚oƒXƒCƒbƒ`ˆ—ií‚ÉŽó‚¯•t‚¯‚é‚à‚Ìj			*
*	Syntax			dvaMCPSWIn( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaMCPSWIn( void )
{
		int		i;

		//
		// ‚`‚b‚oƒXƒCƒbƒ`”»’è
		//
			// ‚`‚b‚o‚Ì‚n‚m‚É‚È‚Á‚½M†‚ð‹‚ß‚é
		if( nowSignal.SIGNAL3.RBIT.acpCABSW ){	// CAB On
			if( DUALPORT.mcpMode.acpCAB==0 ){	// bef CAB Off
				DUALPORT.mcpMode.acpCAB = 1;	// Now CAB ON
				DUALPORT.mcpMode.acpCABOn = 1;	// CAB Switch ON
			}
		}
		else {									// CAB Off
			if( DUALPORT.mcpMode.acpCAB ){		// bef CAB ON
				DUALPORT.mcpMode.acpCAB = 0;	// Now CAB OFF
			}
		}

		if( nowSignal.SIGNAL3.RBIT.acpSTNSW ){	// STN On
			if( DUALPORT.mcpMode.acpSTN==0 ){	// bef STN Off
				DUALPORT.mcpMode.acpSTN = 1;	// Now STN ON
				DUALPORT.mcpMode.acpSTNOn = 1;	// STN Switch ON
			}
		}
		else {									// STN Off
			if( DUALPORT.mcpMode.acpSTN ){		// bef STN ON
				DUALPORT.mcpMode.acpSTN = 0;	// Now STN OFF
			}
		}

		if( nowSignal.SIGNAL3.RBIT.acpREPSW ){	// REP On
			if( DUALPORT.mcpMode.acpREP==0 ){	// bef REP Off
				DUALPORT.mcpMode.acpREP = 1;	// Now REP ON
				DUALPORT.mcpMode.acpREPOn = 1;	// REP Switch ON
			}
		}
		else {									// REP Off
			if( DUALPORT.mcpMode.acpREP ){		// bef REP ON
				DUALPORT.mcpMode.acpREP = 0;	// Now REP OFF
			}
		}

		if( DUALPORT.mcpMode.Cab ){		// ‚b‚`‚a^‚b‚`‚aƒ‚[ƒh’†
			if( DUALPORT.mcpMode.acpCABOn ){
				DUALPORT.mcpMode.acpCABOn = 0;		// Žó‚¯•t‚¯Š®—¹
				// ‚`‚b‚o‚Ì‚b‚`‚aƒXƒCƒbƒ`‚ª‚n‚e‚e|„‚n‚m
				dvaMCPRelease();				// ‰¹ºƒoƒX‚ð–ß‚·
			}
		}
		else {							// ‚b‚`‚a^‚b‚`‚a‚Å‚È‚¢‚Æ‚«
			if( DUALPORT.mcpMode.acpCABOn ){
				DUALPORT.mcpMode.acpCABOn = 0;		// Žó‚¯•t‚¯Š®—¹
				// ‚`‚b‚o‚Ì‚b‚`‚aƒXƒCƒbƒ`‚ð‰Ÿ‚µ‚½‚Æ‚«
				dvaMCPCab();				// ‰¹ºƒoƒX‚ð‚b‚`‚aƒ‚[ƒh‚É‚·‚é
			}
			else {
				if( nowSignal.SIGNAL3.RBIT.acpPTTSW ){
					if( DUALPORT.mcpMode.MANEnable ){
						// ”ñí’Ê•ñƒ‚[ƒhˆÈŠO‚Ì‚Æ‚«Žó‚¯•t‚¯
						// ‚`‚b‚o‚Ìƒg[ƒNƒXƒCƒbƒ`‚ð‰Ÿ‚µ‚½‚Æ‚«
						DUALPORT.mcpMode.acpManual = 1;	// ‚`‚b‚o‚É‚æ‚éƒ}ƒjƒ…ƒAƒ‹ƒ‚[ƒh‚ðÝ’è
						dvaMCPManual();			// ‰¹ºƒoƒX‚ðƒ}ƒCƒN•ú‘—‚É‚·‚é
					}
				}
				else {
					// ‚`‚b‚o‚Ìƒg[ƒNƒXƒCƒbƒ`‚ð—£‚µ‚½‚Æ‚«
					if( DUALPORT.mcpMode.acpManual ){	// ‚`‚b‚o‚É‚æ‚éƒ}ƒjƒ…ƒAƒ‹ƒ‚[ƒh‚Å‚ ‚ê‚Î
						dvaMCPRelease();				// ‰¹ºƒoƒX‚ð–ß‚·
					}
				}
			}
		}

		if( DUALPORT.mcpMode.acpSTNOn ){		/* •ú‘—ŒãAŽŸ‰w‚Öi‚Þ */
			DUALPORT.mcpMode.acpSTNOn = 0;		// Žó‚¯•t‚¯Š®—¹
			// ‚`‚b‚o‚©‚ç‚r‚s‚mƒXƒCƒbƒ`‚ð‰Ÿ‚µ‚½‚Æ‚«
			if( DUALPORT.mcpMode.STNEnable ){		// ŽŸ‰w•ú‘—‚Ì‹–‰Â‚Ìê‡
				DUALPORT.mcpMode.STNOn = 1;		// ŽŸ‰w•ú‘—‚ð—v‹
			}
		}

		//
		// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
		//
		if( DUALPORT.mcpMode.Cab==0 ){		// ‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Å‚È‚¢‚Æ‚«
			if( nowSignal.SIGNAL3.RBIT.mcpPTTSW ){
				if( DUALPORT.mcpMode.MANEnable ){
					// ”ñí’Ê•ñƒ‚[ƒhˆÈŠO‚Ì‚Æ‚«Žó‚¯•t‚¯
					// ‚l‚b‚o‚Ìƒg[ƒNƒXƒCƒbƒ`‚ð‰Ÿ‚µ‚½‚Æ‚«
					DUALPORT.mcpMode.mcpManual = 1;	// ‚l‚b‚o‚É‚æ‚éƒ}ƒjƒ…ƒAƒ‹ƒ‚[ƒh‚ðÝ’è
					dvaMCPManual();			// ‰¹ºƒoƒX‚ðƒ}ƒCƒN•ú‘—‚É‚·‚é
				}
			}
			else {
				// ‚l‚b‚o‚Ìƒg[ƒNƒXƒCƒbƒ`‚ð—£‚µ‚½‚Æ‚«
				if( DUALPORT.mcpMode.mcpManual ){	// ‚l‚b‚o‚É‚æ‚éƒ}ƒjƒ…ƒAƒ‹ƒ‚[ƒh‚Å‚ ‚ê‚Î
					dvaMCPRelease();				// ‰¹ºƒoƒX‚ð–ß‚·
				}
			}
		}

		for( i=0; i<2; i++ ){
			if( DUALPORT.answerData.mcpSW[i].BIT.CAB ){
				// ‚b‚`‚aƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

				if( DUALPORT.mcpMode.Cab ){		// ‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Ìê‡
					dvaMCPRelease();				// ‰¹ºƒoƒX‚ð–ß‚·
					break;
				}
				else {							// ‚b‚`‚a^‚b‚`‚aƒ‚[ƒh‚Å‚È‚¢ê‡
					dvaMCPCab();				// ‰¹ºƒoƒX‚ð‚b‚`‚aƒ‚[ƒh‚É‚·‚é
				}
				break;
			}

			if( DUALPORT.answerData.mcpSW[i].BIT.ATC ){
				// ‚`‚s‚bƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

				if( DUALPORT.mcpMode.ATCEnable ){	// ‚`‚s‚bƒXƒCƒbƒ`‹–‰Â‚Ì‚Æ‚«
					if( DUALPORT.mcpMode.ATCOnEnable ){
						DUALPORT.mcpMode.ATCOn = 1;		// Ž©“®•ú‘——v‹
					}
					else {
						DUALPORT.mcpMode.ATCOff = 1;	// Ž©“®•ú‘—‰ðœ
					}
				}
				break;
			}

			if( DUALPORT.answerData.mcpSW[i].BIT.STN ){
				// ‚r‚s‚mƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

				if( DUALPORT.mcpMode.STNEnable ){		// ŽŸ‰w‹–‰Â‚Ìê‡
					DUALPORT.mcpMode.STNOn = 1;		// ŽŸ‰w•ú‘——v‹
				}
				break;
			}

			if( DUALPORT.answerData.mcpSW[i].BIT.RST ){	
				// ƒŠƒZƒbƒgƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

				mcp_phase = MCP_RESET;
				break;
			}

			if( DUALPORT.answerData.mcpSW[i].BIT.SEL ){	
				// ƒ‰ƒCƒ“‘I‘ð‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

				select_routeCode = backup_data.nowRouteCode;
				if( dvaMCPOpeLineDisplay( &select_routeCode, 1 )==0 ){
					mcp_phase = MCP_LINE_SELECT;
				}
				break;
			}

			if( DUALPORT.answerData.mcpSW[i].BIT.LEFT && DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){
				// Œ¾Œê‘I‘ð‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

				select_lang = config_data.lang;
				dvaMCPOpeLangDisplay( &select_lang, 0 );

				mcp_phase = MCP_LANG_SELECT;
				break;
			}
		}
}


/************************************************************************
*																		*
*	Function name	<< dvaMCPOperation >>								*
*					‚l‚b‚o•\Ž¦ˆ—										*
*	Syntax			dvaMCPOperation( void );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaMCPOperation( void )
{
		int		i;
		int		goON;

		switch( mcp_phase ){
		case MCP_RESET:
			// ƒXƒCƒbƒ`‚k‚d‚c‚È‚Ç‚Ì‰ŠúÝ’è
			DUALPORT.mcpMode.acpCABOn = 0;	// ACP CAB Switch OFF
			DUALPORT.mcpMode.acpSTNOn = 0;	// ACP STN Switch OFF
			DUALPORT.mcpMode.acpREPOn = 0;	// ACP REP Switch OFF

			DUALPORT.mcpMode.test = 0;		// ƒeƒXƒg—v‹‚Ì‰ðœ
			DUALPORT.mcpMode.EMGPOn = 0;	// ”ñí’Ê•ñŠí‚Ì‰ðœ
			DUALPORT.mcpMode.EMGPHL[0] = 0;
			DUALPORT.mcpMode.EMGPHL[1] = 0;
			DUALPORT.mcpMode.EMGPHL[2] = 0;
			DUALPORT.mcpMode.EMGPHL[3] = 0;
			DUALPORT.mcpMode.EMGPHL[4] = 0;
			DUALPORT.mcpMode.EMGPHL[5] = 0;
			DUALPORT.mcpMode.EMGPHL[6] = 0;
			DUALPORT.mcpMode.EMGPHL[7] = 0;

			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[0] = 0;		// MCP Switch OFF
			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[1] = 0;		// MCP Switch OFF
			DUALPORT.answerData.mcpSW[0].BYTE.BYTE[2] = 0;		// MCP Switch OFF
			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[0] = 0;		// MCP Switch OFF
			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[1] = 0;		// MCP Switch OFF
			DUALPORT.answerData.mcpSW[1].BYTE.BYTE[2] = 0;		// MCP Switch OFF
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BYTE = 0;	// Switch LED OFF
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BYTE = 0;	// Switch LED Blink OFF
			MCPDisplayBlank( 0 );			// Matrix LED Clear
			MCPDisplayBlank( 1 );			// Matrix LED Clear
			MCPDisplayBlank( 2 );			// Matrix LED Clear
			MCPDisplayBlank( 3 );			// Matrix LED Clear
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[0]=0x00;	// Matrix LED Blink OFF
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0x00;	// Matrix LED Blink OFF
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;	// Matrix LED Blink OFF
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0x00;	// Matrix LED Blink OFF

			mcp_phase = MCP_IDLE;
			break;

		case MCP_IDLE:
				// ‚b‚`‚a^‚b‚`‚a•ú‘—’†‚Íˆ—‚µ‚È‚¢
			if( DUALPORT.mcpMode.Cab )
				break;
				// ƒ}ƒjƒ…ƒAƒ‹•ú‘—’†‚Íˆ—‚µ‚È‚¢
			if( DUALPORT.mcpMode.acpManual )
				break;
			if( DUALPORT.mcpMode.mcpManual )
				break;
				// ƒZƒ‹ƒtƒeƒXƒg’†‚Íˆ—‚µ‚È‚¢
			if( DUALPORT.mcpMode.test )
				break;


			// ”ñí’Ê•ñŠí‚©‚ç‚ÌM†‚ª‚ ‚ê‚Î@‚l‚b‚o‚Ì‚d‚l‚f‚oƒXƒCƒbƒ`‚ð“_–Å‚³‚¹‚é
			goON = 0;
			for( i=0; i<8; i++ ){
				if( DUALPORT.answerData.rscSW[i].BYTE ){
					goON = 1;
					break;
				}
			}
			if( goON ){
				// ”ñí’Ê•ñ‚Ì‚k‚d‚c‚ð“_–Å
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=1;
			}
			else {
				// ”ñí’Ê•ñ‚Ì‚k‚d‚c‚ðÁ“”
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=0;
			}

			// ‚`‚b‚oƒXƒCƒbƒ`”»’è
			if( DUALPORT.mcpMode.acpREPOn ){
				DUALPORT.mcpMode.acpREPOn = 0;		// Žó‚¯•t‚¯Š®—¹
				// ‚q‚d‚oƒXƒCƒbƒ`‚ð‰Ÿ‚µ‚½‚Æ‚«
				if( DUALPORT.mcpMode.REPEnable ){		// Ä•ú‘—‹–‰Â‚Ìê‡
					DUALPORT.mcpMode.REPOn = 1;		// ƒŠƒs[ƒg•ú‘——v‹
				}
			}
			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.EMGP ){
						// ‚d‚l‚f|‚oƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					if( DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP ){	/*”ñí’Ê•ñŠí‚©‚ç‚Ì—v‹’†H*/
						item_skip = 0;
						dvaMCPOpeEMGPDisplay( item_skip );
						mcp_phase = MCP_EMGP1;
					}
					break;
				}

				if( DUALPORT.answerData.mcpSW[i].BIT.EMG ){
						// ”ñí•ú‘—ƒ‚[ƒhƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					item_skip = 0;
					dvaMCPOpeEMGDisplay( item_skip );
					mcp_phase = MCP_EMG1;
					break;
				}

				if( DUALPORT.answerData.mcpSW[i].BIT.SPC ){
						// ƒXƒyƒVƒƒƒ‹•ú‘—ƒ‚[ƒhƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					item_skip = 0;
					dvaMCPOpeSPCDisplay( item_skip );
					mcp_phase = MCP_SPC1;
					break;
				}

				if( DUALPORT.answerData.mcpSW[i].BIT.REP ){
						// ƒŠƒs[ƒgƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					if( DUALPORT.mcpMode.REPEnable ){		// Ä•ú‘—‹–‰Â‚Ìê‡
						DUALPORT.mcpMode.REPOn = 1;		// ƒŠƒs[ƒg•ú‘——v‹
					}
					break;
				}

				if( DUALPORT.answerData.mcpSW[i].BIT.TEST ){
						// ƒeƒXƒgƒXƒCƒbƒ`‚ª‰Ÿ‚³‚ê‚½‚Æ‚«
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					DUALPORT.mcpMode.test = 1;
					break;
				}
			}
			break;

		case MCP_EMGP1:
				// ”ñí’Ê•ñ‚Ì‚k‚d‚c‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=0;
				// ‚Qs–ÚE‚Rs–Ú‚ð“_–Å
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0xFF;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					if( item_skip )
						item_skip--;
					while( dvaMCPOpeEMGPDisplay( item_skip ) )
						item_skip--;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					item_skip++;
					while( dvaMCPOpeEMGPDisplay( item_skip ) )
						item_skip--;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.ENT ){	/* Šm’è */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					mcp_phase = MCP_EMGP2;
					break;
				}
			}
			break;

		case MCP_EMGP2:
				// ”ñí’Ê•ñ‚Ì‚k‚d‚c‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=0;
				// ‚Qs–Ú‚Í“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;
				// ‚Rs–Ú‚ð“_–Å
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0xFF;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					if( item_skip )
						item_skip--;
					while( dvaMCPOpeEMGPDisplay( item_skip ) )
						item_skip--;
					mcp_phase = MCP_EMGP1;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					item_skip++;
					while( dvaMCPOpeEMGPDisplay( item_skip ) )
						item_skip--;
					mcp_phase = MCP_EMGP1;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.EMGP ){
					// ‚d‚l‚f|‚oƒXƒCƒbƒ`
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					DUALPORT.mcpMode.EMGPOn = 1;		// ”ñí’Ê•ñ‚Ì•ú‘——v‹
						// ‘I‘ð‚³‚ê‚½”ñí’Ê•ñŠí‚ÌÝ’è
					DUALPORT.mcpMode.EMGPHL[select_rscSW_row] |= 0x0001<<select_rscSW_col;
					break;
				}
			}
			break;

		case MCP_EMG1:
				// ”ñí•ú‘—‚Ì‚k‚d‚c‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				// ‚Qs–Ú‚ð“_–Å
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					if( item_skip )
						item_skip--;
					while( dvaMCPOpeEMGDisplay( item_skip ) )
						item_skip--;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					item_skip++;
					while( dvaMCPOpeEMGDisplay( item_skip ) )
						item_skip--;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.ENT ){	/* Šm’è */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					mcp_phase = MCP_EMG2;
					break;
				}
			}
			break;

		case MCP_EMG2:
				// ”ñí•ú‘—‚Ì‚k‚d‚c‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				// ‚Qs–Ú‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					if( item_skip )
						item_skip--;
					while( dvaMCPOpeEMGDisplay( item_skip ) )
						item_skip--;
					mcp_phase = MCP_EMG1;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					item_skip++;
					while( dvaMCPOpeEMGDisplay( item_skip ) )
						item_skip--;
					mcp_phase = MCP_EMG1;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.EMG ){
					// ‚d‚l‚fƒXƒCƒbƒ`
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					DUALPORT.mcpMode.EMGCode = select_emgCode;
					DUALPORT.mcpMode.EMGOn = 1;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.REP ){
						// ƒŠƒs[ƒgƒXƒCƒbƒ`
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					DUALPORT.mcpMode.EMGCode = select_emgCode;
					DUALPORT.mcpMode.EMGOn = 1;
					break;
				}
			}
			break;

		case MCP_SPC1:
				// ƒXƒyƒVƒƒƒ‹•ú‘—‚Ì‚k‚d‚c‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				// ‚Qs–Ú‚ð“_–Å
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					if( item_skip )
						item_skip--;
					while( dvaMCPOpeSPCDisplay( item_skip ) )
						item_skip--;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					item_skip++;
					while( dvaMCPOpeSPCDisplay( item_skip ) )
						item_skip--;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.ENT ){	/* Šm’è */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					mcp_phase = MCP_SPC2;
					break;
				}
			}
			break;

		case MCP_SPC2:
				// ƒXƒyƒVƒƒƒ‹•ú‘—‚Ì‚k‚d‚c‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				// ‚Qs–Ú‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					if( item_skip )
						item_skip--;
					while( dvaMCPOpeSPCDisplay( item_skip ) )
						item_skip--;
					mcp_phase = MCP_EMG1;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;
					item_skip++;
					while( dvaMCPOpeSPCDisplay( item_skip ) )
						item_skip--;
					mcp_phase = MCP_EMG1;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.SPC ){	/* ‚r‚o‚bƒXƒCƒbƒ`‚Ì”»’è */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					DUALPORT.mcpMode.EMGCode = select_spcCode;
					DUALPORT.mcpMode.SPCOn = 1;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.REP ){		/* ƒŠƒs[ƒg•ú‘—ƒ‚[ƒh */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					DUALPORT.mcpMode.EMGCode = select_spcCode;
					DUALPORT.mcpMode.SPCOn = 1;
					break;
				}
			}
			break;

		case MCP_LINE_SELECT:
				// ‚Qs–Ú‚ð“_–Å
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					dvaMCPOpeLineDisplay( &select_routeCode, 0 );
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					dvaMCPOpeLineDisplay( &select_routeCode, 1 );
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.ENT ){	/* Šm’è */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					backup_data.nowRouteCode = select_routeCode;
					mcp_phase = MCP_LINE_SELECT2;
					break;
				}
			}
			break;

		case MCP_LINE_SELECT2:
				// ‚Qs–Ú‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					dvaMCPOpeLineDisplay( &select_routeCode, 0 );
					mcp_phase = MCP_LINE_SELECT;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					dvaMCPOpeLineDisplay( &select_routeCode, 1 );
					mcp_phase = MCP_LINE_SELECT;
					break;
				}
			}
			break;

		case MCP_LANG_SELECT:
				// ‚Qs–Ú‚ð“_–Å
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					dvaMCPOpeLangDisplay( &select_lang, -1 );
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					dvaMCPOpeLangDisplay( &select_lang, 1 );
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.ENT ){	/* Šm’è */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					config_data.lang = select_lang;
					mcp_phase = MCP_LANG_SELECT2;
					break;
				}
			}
			break;

		case MCP_LANG_SELECT2:
				// ‚Qs–Ú‚ð“_“”
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;

			// ‚l‚b‚o‚ÌƒXƒCƒbƒ`”»’è
			for( i=0; i<2; i++ ){
				if( DUALPORT.answerData.mcpSW[i].BIT.LEFT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					dvaMCPOpeLangDisplay( &select_lang, -1 );
					mcp_phase = MCP_LANG_SELECT;
					break;
				}
				if( DUALPORT.answerData.mcpSW[i].BIT.RIGHT ){	/* ‘I‘ð */
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

					dvaMCPOpeLangDisplay( &select_lang, 1 );
					mcp_phase = MCP_LANG_SELECT;
					break;
				}
			}
			break;

		}
}

