/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	ƒe[ƒuƒ‹’è‹`													*
*-------------------------------------------------------------------*
*	Author		Nissin system lab.									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

#include <stdio.h>
#include <string.h>

#include	"table.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
//
//	ƒoƒbƒNƒAƒbƒvî•ñ‚Ì’è‹`
//
#pragma section BACKUP
struct	BACKUP_DATA		backup_data;		// ƒoƒbƒNƒAƒbƒvƒf[ƒ^
struct	CONFIG_INFO		config_data;		// Ý’èî•ñ
struct	ROUTEINFO		route;				// ‰wî•ñ

#pragma section

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
#define	MAGIC_CODE	0x3145

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹ƒe[ƒuƒ‹
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
*	Function name	<< dvaGetDefaultRoute >>							*
*					‘¶Ý‚·‚éƒ‹[ƒgŒŸõˆ—										*
*	Syntax			dvaGetDefaultRoute( void );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
struct ROUTEINFO *dvaGetDefaultRoute( void )
{
		FILE	*fd;
		int		i;
		char	filename[16];

		for( i=0; i<MAX_ROUTE; i++ ){
			sprintf( filename, "A:\\ROUTE%02u.DAT", i );
			fd = fopen( filename, "rb" );
			if( fd!=NULL ){
				if( fread( &route, 1, sizeof(route), fd )==sizeof(route) ){
					fclose( fd );
					return( &route );
				}
				fclose( fd );
			}
		}

		return( NULL );
}

/************************************************************************
*																		*
*	Function name	<< dvaBackupCheck >>								*
*					ƒoƒbƒNƒAƒbƒvî•ñ‚Ìƒ`ƒFƒbƒN							*
*	Syntax			dvaBackupCheck( void );								*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
//
//	ˆ—ŠT—vF
//		ƒoƒbƒNƒAƒbƒvƒGƒŠƒA‚ðƒ`ƒFƒbƒN‚µ
//		ƒoƒbƒNƒAƒbƒvƒf[ƒ^‚ª—LŒø‚Å‚ ‚ê‚Î@ƒŠƒ^[ƒ“‚·‚é
//		—LŒø‚Å‚È‚¯‚ê‚ÎAƒoƒbƒNƒAƒbƒv‚³‚ê‚½ƒf[ƒ^‚ðƒNƒŠƒA‚µ
//		‚`‚s‚`ƒJ[ƒh‚©‚çÝ’èî•ñ‚ð“Ç‚Ýž‚Ý@ƒe[ƒuƒ‹‚ÉÝ’è‚·‚é
//		‚`‚s‚`ƒJ[ƒh‚©‚ç“Ç‚Ýž‚ß‚È‚¯‚ê‚ÎAƒGƒ‰[‚Æ‚µ‚ÄƒŠƒ^[ƒ“‚·‚é
//
int dvaBackupCheck( void )
{
		FILE	*fd;

		if( backup_data.magicCode == MAGIC_CODE ){
			// Backup OK
			return( 0 );
		}

		// Backup NG
		// Backup Area Clear
		memset( &backup_data, 0, sizeof(backup_data) );
		backup_data.magicCode = MAGIC_CODE;		// Magic Code Reset

		// Read Information Data
		fd = fopen("A:\\CONFIG.DAT", "rb" );
		if( fd==NULL ){
			return( -1 );
		}
		if( fread( &config_data, 1, sizeof(config_data), fd )!=sizeof(config_data) ){
			fclose( fd );
			return( -1 );
		}
		fclose( fd );

		// Read Default Root Data
		if( dvaGetDefaultRoute()==NULL )
			return( -1 );

		return( 0 );
}


/************************************************************************
*																		*
*	Function name	<< dvaSearchRoute >>								*
*					ƒ‹[ƒgŒŸõˆ—										*
*	Syntax			dvaSearchRoute( routeCode );						*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
struct ROUTEINFO *dvaSearchRoute( char routeCode )
{
		int		i;
		FILE	*fd;
		char	filename[16];

		if( route.routeCode==routeCode ){
			return( &route );
		}

		for( i=0; i<MAX_ROUTE; i++ ){
			sprintf( filename, "A:\\ROUTE%02u.DAT", i );
			fd = fopen( filename, "rb" );
			if( fd!=NULL ){
				if( fread( &route, 1, sizeof(route), fd )==sizeof(route) ){
					fclose( fd );
					if( route.routeCode==routeCode ){
						return( &route );
					}
				}
				else {
					fclose( fd );
				}
			}
		}

		return( NULL );
}

/************************************************************************
*																		*
*	Function name	<< dvaSearchStation >>								*
*					‰w–¼ŒŸõˆ—										*
*	Syntax			dvaSearchStation( routeCode, stationCode );			*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
struct STATIONINFO *dvaSearchStation( char routeCode, char stationCode )
{
		int		i;

		if( dvaSearchRoute( routeCode )==NULL )
			return( NULL );

		if( route.routeCode==routeCode ){
			for( i=0; i<MAX_STATION; i++ ){
				if( route.station[i].stationCode==0x00 ){
					break;
				}
				if( route.station[i].stationCode==stationCode ){
					return( &route.station[i] );
				}
			}
		}
		return( NULL );
}

/************************************************************************
*																		*
*	Function name	<< dvaSearchStationNumber >>						*
*					‰w–¼ŒŸõˆ—										*
*	Syntax			dvaSearchStationNumber( routeCode, stationNumber );	*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
struct STATIONINFO *dvaSearchStationNumber( char routeCode, char stationNumber )
{
		int		i;

		if( dvaSearchRoute( routeCode )==NULL )
			return( NULL );

		if( route.routeCode==routeCode ){
			for( i=0; i<MAX_STATION; i++ ){
				if( route.station[i].stationCode==0x00 ){
					break;
				}
				if( route.station[i].Arriving || route.station[i].Arrived ){
					if( route.station[i].stationNumber==stationNumber ){
						return( &route.station[i] );
					}
				}
			}
		}
		return( NULL );
}

/************************************************************************
*																		*
*	Function name	<< dvaSearchDestination >>							*
*					“ž’…‰w–¼ŒŸõˆ—									*
*	Syntax			dvaSearchDestination( routeCode );					*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
struct STATIONINFO *dvaSearchDestination( char routeCode )
{
		int		i;

		if( dvaSearchRoute( routeCode )==NULL )
			return( NULL );

		if( route.routeCode==routeCode ){
			for( i=0; i<MAX_STATION; i++ ){
				if( route.station[i].stationCode==0x00 ){
					if( i>0 ){
						return( &route.station[i-1] );
					}
				}
			}
		}
		return( NULL );
}

