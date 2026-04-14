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

#include    <machine.h>
#include    "iodefine.h"
#include	"table.h"

#include	"timer.h"


/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

#pragma section BACKUP
struct	TIMER_CONTROL	timedata;		// ƒ^ƒCƒ}[î•ñ
#pragma section

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
extern	unsigned short	now_date;		/*Œ»Ý“ú•t@‚c‚n‚rƒtƒH[ƒ}ƒbƒg*/
extern	unsigned short	now_time;		/*Œ»ÝŽž@‚c‚n‚rƒtƒH[ƒ}ƒbƒg*/

extern	void	(*IMIA0Jump)(void);

extern	union SDFORM	tms_sd_data;		// ‚s‚l‚r‚Ö‚ÌŽóMƒf[ƒ^
extern	union SDRFORM	tms_sdr_data;	// ‚s‚l‚r‚©‚ç‚Ì‘—Mƒf[ƒ^

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

static	char	signalCheckTime;

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< timer_0_Intr >>	
*					ƒ^ƒCƒ}[Š„‚èž‚Ý
*	Syntax			timer_0_Intr( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void    timer_0_Intr(void)
{
		char	tmstimeactive;

		tmstimeactive=0;
		// “ú•tEŽž‚ÌƒJƒEƒ“ƒgˆ—
		timedata.tm_msec++;
		if( timedata.tm_msec>99 ){
			timedata.tm_msec=0;
			timedata.tm_sec++;
		}
		if( timedata.tm_sec>59 ){
			timedata.tm_sec=0;
			timedata.tm_min++;

			tmstimeactive = 1;
		}
		if( timedata.tm_min>59 ){
			timedata.tm_min=0;
			timedata.tm_hour++;
		}
		if( timedata.tm_hour>59 ){
			timedata.tm_hour=0;
			timedata.tm_day++;
		}

		if( timedata.tm_day==0 ){
			timedata.tm_day=1;
		}
		if( timedata.tm_mon==0 ){
			timedata.tm_mon=1;
		}
		switch( timedata.tm_mon ){
		case 2:
			if( (timedata.tm_year%4)==0 ){
				if( timedata.tm_day>29 ){
					timedata.tm_day=1;
					timedata.tm_mon++;
				}
			}
			else {
				if( timedata.tm_day>28 ){
					timedata.tm_day=1;
					timedata.tm_mon++;
				}
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if( timedata.tm_day>30 ){
				timedata.tm_day = 1;
				timedata.tm_mon++;
			}
			break;
		default:
			if( timedata.tm_day>31 ){
				timedata.tm_day = 1;
				timedata.tm_mon++;
			}
			break;
		}

		if( timedata.tm_mon>12 ){
			timedata.tm_mon=1;
			timedata.tm_year++;
		}

		now_date = (((unsigned short)timedata.tm_year+20)<<9)		/* 2000-1980 */
				| (((unsigned short)timedata.tm_mon)<<5)
				| (((unsigned short)timedata.tm_day));
		now_time = (((unsigned short)timedata.tm_hour)<<11)
				| (((unsigned short)timedata.tm_min)<<5)
				| (((unsigned short)timedata.tm_sec)>>1);


		if( tms_sdr_data.SDR.activeText.timeDateValid ){
			tms_sdr_data.SDR.timeDate.yy10 = timedata.tm_year/10;
			tms_sdr_data.SDR.timeDate.yy1 = timedata.tm_year%10;
			tms_sdr_data.SDR.timeDate.MM10 = timedata.tm_mon/10;
			tms_sdr_data.SDR.timeDate.MM1 = timedata.tm_mon%10;
			tms_sdr_data.SDR.timeDate.dd10 = timedata.tm_day/10;
			tms_sdr_data.SDR.timeDate.dd1 = timedata.tm_day%10;
			tms_sdr_data.SDR.timeDate.hh10 = timedata.tm_hour/10;
			tms_sdr_data.SDR.timeDate.hh1 = timedata.tm_hour%10;
			tms_sdr_data.SDR.timeDate.mm10 = timedata.tm_min/10;
			tms_sdr_data.SDR.timeDate.mm1 = timedata.tm_min%10;
			tms_sdr_data.SDR.timeDate.ss10 = timedata.tm_sec/10;
			tms_sdr_data.SDR.timeDate.ss1 = timedata.tm_sec%10;
		}
		if( tmstimeactive ){
			tms_sdr_data.SDR.activeText.timeDateAdjustment = 1;
			timedata.testTimeTimer = 100;
		}

		or_ccr(0xC0);						/* —DæŠ„‚èž‚Ý‹ÖŽ~ */
		if( timedata.tms_timer ){
			timedata.tms_timer--;
		}
		if( timedata.tmstimeout ){
			timedata.tmstimeout--;
		}
		and_ccr(0xBF);						/* —DæŠ„‚èž‚Ý‹–‰Â */

		if( timedata.testTimeTimer ){
			timedata.testTimeTimer--;
			if( timedata.testTimeTimer==0 ){
				tms_sdr_data.SDR.activeText.timeDateAdjustment=0;
			}
		}
		if( timedata.menteSendTimer ){
			timedata.menteSendTimer--;
		}
}

/************************************************************************
*																		*
*	Function name	<< timer_0_Initialize >>	
*					‰ŠúÝ’è
*	Syntax			timer_0_Initialize( void );
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void timer_0_Initialize( void )
{
		/******* ƒ^ƒCƒ}[‰ŠúÝ’è *******/

		timedata.tms_timer = 0;
		timedata.tmstimeout = 0;
		timedata.testExecTimer = 0;
		timedata.testTimeTimer = 0;
		timedata.testBroadcastTimer = 0;

		/*** ƒ^ƒCƒ}[‚O‚Ì‚ÝŽg—p ***/

		IMIA0Jump = timer_0_Intr;

		ITU0.TCR.BYTE = 0;
		ITU0.TCR.BIT.CCLR = 1;		/* ‚f‚q‚`ƒJƒEƒ“ƒg‚ÅƒNƒŠƒA */
		ITU0.TCR.BIT.CKEG = 0;		/* ƒNƒƒbƒN‚ÌÙ‚¿ã‚ª‚è‚ÅƒJƒEƒ“ƒg */
		ITU0.TCR.BIT.TPSC = 2;		/* ƒ³^‚SƒNƒƒbƒN */
		ITU0.TIOR.BYTE = 0;			/* ƒRƒ“ƒyƒAƒ}ƒbƒ`‚É‚æ‚éo—Í‹ÖŽ~ */
		ITU0.TIER.BYTE = 0;
		ITU0.TIER.BIT.IMIEA = 1;	/* ‚h‚l‚e‚`ƒtƒ‰ƒO‚É‚æ‚éŠ„‚èž‚Ý */
		ITU0.TSR.BYTE = 0;			/* ƒ^ƒCƒ}[ƒXƒe[ƒ^ƒXƒNƒŠƒA */

		ITU0.TCNT = 0;				/* ƒJƒEƒ“ƒ^[ƒNƒŠƒA */
										/* 14.7456MHz -> ƒ³
										ƒ³^‚S = 3.6864MHz
										274.243uS
										10mS / 247.243uS = 0x9000
									*/
		ITU0.GRA = 0x9000;

		/** ƒ^ƒCƒ}[ƒXƒ^[ƒg **/
		ITU.TSTR.BYTE = 0;
		ITU.TSTR.BIT.STR0 = 1;
		ITU.TSNC.BYTE = 0;			/*ƒ^ƒCƒ}[“¯Šú‚È‚µ */
		ITU.TSNC.BIT.SYNC4 = 0;
		ITU.TSNC.BIT.SYNC3 = 0;
		ITU.TSNC.BIT.SYNC2 = 0;
		ITU.TSNC.BIT.SYNC1 = 0;
		ITU.TSNC.BIT.SYNC0 = 0;
		ITU.TMDR.BYTE = 0;			/*ˆÊ‘ŠŒv”‚È‚µ*/
		ITU.TFCR.BYTE = 0;
		ITU.TOER.BYTE = 0;
		ITU.TOCR.BYTE = 0;

}


/*@@@E*/
