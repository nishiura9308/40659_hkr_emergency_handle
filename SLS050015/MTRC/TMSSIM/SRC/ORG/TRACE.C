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
#include    "timer.h"
#include    "trace.h"

/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

#define	MAX_TRACE	2000

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
extern	struct	TIMER_CONTROL	timedata;


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

static	struct TRACE traceData[MAX_TRACE];
static	int			trace_top;
static	int			trace_end;
static	int			trace_get;

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< TraceClear >>	
*					ƒgƒŒ[ƒX‰ŠúÝ’è
*	Syntax			TraceClear( void );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int TraceClear( void )
{
		trace_top = 0;
		trace_end = 0;
		trace_get = 0;
		memset( traceData, 0, sizeof(traceData) );
}

/************************************************************************
*																		*
*	Function name	<< TraceAdd >>	
*					ƒgƒŒ[ƒX’Ç‰ÁÝ’è
*	Syntax			TraceAdd( event );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
int TraceAdd( char event )
{
		set_imask_ccr(1);

		traceData[trace_top].tm_year = timedata.tm_year;
		traceData[trace_top].tm_mon = timedata.tm_mon;
		traceData[trace_top].tm_day = timedata.tm_day;
		traceData[trace_top].tm_hour = timedata.tm_hour;
		traceData[trace_top].tm_min = timedata.tm_min;
		traceData[trace_top].tm_sec = timedata.tm_sec;
		traceData[trace_top].tm_100msec = timedata.tm_msec/10;

		traceData[trace_top].eventID[0] = event;

		trace_top++;
		if( trace_top>=MAX_TRACE ){
			trace_top=0;
		}
		if( trace_top==trace_end ){
			trace_end++;
			if( trace_end>=MAX_TRACE ){
				trace_end=0;
			}
		}
		if( trace_top==trace_get ){
			trace_get++;
			if( trace_get>=MAX_TRACE ){
				trace_get=0;
			}
		}
		set_imask_ccr(0);
}

/************************************************************************
*																		*
*	Function name	<< TraceGet >>	
*					ƒgƒŒ[ƒXŽæ‚èo‚µ
*	Syntax			TraceGet( event );
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
struct TRACE *TraceGet( int first, char event )
{
		int		getno;
		int		trace_on;

		trace_on = 1;
		set_imask_ccr(1);
		if( first ){
			trace_get = trace_end;
			getno = trace_get;
		}
		else {
			getno = trace_get;
			getno++;
		}
		if( getno>=MAX_TRACE ){
			getno = 0;
		}
		if( getno==trace_top )
			trace_on = 0;

		if( traceData[getno].eventID[0]==0 )
			trace_on = 0;

		if( trace_on ){
			trace_get = getno;
		}
		set_imask_ccr(0);

		if( trace_on ){
			return &traceData[getno];
		}

		return( NULL );
}

