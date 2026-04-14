/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®—ñŽÔ•ú‘—§Œä‘•’uz							*
*-------------------------------------------------------------------*
*	ƒu[ƒg§Œäƒ‚ƒWƒ…[ƒ‹											*
*-------------------------------------------------------------------*
*	Author		TOA Corporation									*
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


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒ[ƒJƒ‹ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/


/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
extern	unsigned short	check_date;
extern	unsigned short	check_time;
extern	unsigned long	check_all_bytes;

extern	void	(*IRQ4Jump)(void);

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
static	unsigned char	boot_request;
static	unsigned char	boot_ack;

/*@@@E*/
void boot_int( void )
{
		long	start_adr,end_adr;
		int		len,i;
		long	adr;
		unsigned char	*p;

		start_adr = RAMTOP_ADDRESS;
		end_adr = RAMTOP_ADDRESS+BOOT_SIZE;

		boot_request = DUALBOOT2.request;
		boot_ack = DUALBOOT2.ack;

		DUALBOOT2.request = 0x0000;

			if( boot_request==0x0F ){		/* Get RAM Check Code */
				boot_request = 0;
				DUALBOOT2.check_time = check_time;
				DUALBOOT2.check_date = check_date;
				DUALBOOT2.check_all_bytes = check_all_bytes;
				DUALBOOT2.ack = 0x0F;
			}
			if( boot_request==0x0E ){		/* RAM Clear */
				boot_request = 0;
				check_time = 0;
				check_date = 0;
				check_all_bytes = 0;
				memset( (void *)RAMTOP_ADDRESS, 0, BOOT_SIZE );
				DUALBOOT2.ack = 0x0E;
			}

			if( boot_request==0x01 ){		/* Initial Set */
				boot_request = 0;
				DUALBOOT2.ack = 0x01;
			}
			if( boot_request==0x02 ){		/* Data Set */
				boot_request = 0;
				adr = DUALBOOT2.address;
				len = DUALBOOT2.len;
				for( i=0; i<len; i++,adr++ ){
					if( adr>=start_adr && adr<=end_adr ){
						p = (unsigned char *)adr;
						*p = DUALBOOT2.data[i];
					}
				}
				DUALBOOT2.ack = 0x02;
			}
			if( boot_request==0x03 ){		/* Jump to Application */
				check_time = DUALBOOT2.check_time;
				check_date = DUALBOOT2.check_date;
				check_all_bytes = DUALBOOT2.check_all_bytes;
			}
			if( boot_request==0x04 ){		/* Jump to Application */

			}

			if( boot_request==0x10 ){
				boot_request = 0;
				adr = DUALBOOT2.address;
				len = DUALBOOT2.len;
				for( i=0; i<len; i++,adr++ ){
					p = (unsigned char *)adr;
					*p = DUALBOOT2.data[i];
				}
				DUALBOOT2.ack = 0x10;
			}
			if( boot_request==0x20 ){
				boot_request = 0;
				adr = DUALBOOT2.address;
				len = DUALBOOT2.len;
				for( i=0; i<len; i++,adr++ ){
					p = (unsigned char *)adr;
					DUALBOOT2.data[i] = *p;
				}
				DUALBOOT2.ack = 0x20;
			}
}

/************************************************************************
*																		*
*	Function name	<< boot >>											*
*					‚q‚n‚lƒu[ƒgˆ—
*	Syntax			boot( void );										*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void boot( void )
{
		IRQ4Jump = boot_int;
		INTC.IER.BIT.IRQ4E = 1;		/* IRQ4 Use */

		for( ;; ){
			if( boot_request==0x03 ){		/* Jump to Application */
				DUALBOOT2.ack = 0x03;
				break;
			}
			if( boot_request==0x04 ){		/* Jump to Application */
				DUALBOOT2.ack = 0x04;
				break;
			}
		}

		memset( DUALBOOT2.data, 0, sizeof(DUALBOOT2.data) );
		DUALBOOT2.address = 0;
		DUALBOOT2.len = 0;
		DUALBOOT2.check_time = 0;
		DUALBOOT2.check_date = 0;
		DUALBOOT2.check_all_bytes = 0;

		IRQ4Jump = NULL;
		INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 Use */
}

