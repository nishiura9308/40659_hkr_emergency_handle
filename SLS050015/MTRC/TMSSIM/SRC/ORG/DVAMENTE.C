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

#include <stdio.h>
#include <string.h>

#include "iodefine.h"
#include "table.h"

#include "dosaccs.h"
#include "fcntl.h"

#include	"timer.h"


/*@@@S*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒOƒ[ƒoƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	’è”’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
#define	CRLF		0x0D
#define	ESC			0x1B

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	\‘¢‘Ì’è‹`
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
	ƒe[ƒuƒ‹
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ŠO•”ŽQÆ
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
extern	void asci_1_Initialize( int baud, char data, char parity, char stop );
extern	void asci_1_tx_enable( void );
extern	void asci_1_tx_disable( char tend );
extern	void asci_1_transmit( unsigned char data );

extern	char	tms_start;
extern	union SDFORM	tms_sd_data;		// ‚s‚l‚r‚Ö‚ÌŽóMƒf[ƒ^
extern	union SDRFORM	tms_sdr_data;	// ‚s‚l‚r‚©‚ç‚Ì‘—Mƒf[ƒ^
extern	struct	TIMER_CONTROL	timedata;		// ƒ^ƒCƒ}[î•ñ

extern	int	send_abort;
extern	int	recv_abort;

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
		ƒ[ƒJƒ‹•Ï”
†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
static	char	recvbuff[512];		/*ŽóMƒoƒbƒtƒ@[*/
static	short	recvlen;			/*ŽóMƒoƒCƒg”*/
static	char	sendbuff[2048];		/*‘—Mƒoƒbƒtƒ@[*/
static	short	sendlen;			/*‘—MƒoƒCƒg”*/
static	short	sendcnt;			/*‘—MƒJƒEƒ“ƒ^[*/
static	char	recvon;

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< asci_1_send_ir >>								*
*					‘—MŠ„‚èž‚Ýˆ—									*
*	Syntax			asci_1_send_ir( void );								*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_send_ir( char tend )
{
		if( sendcnt==sendlen ){	/*‚·‚×‚Ä‘—M‚µ‚½‚Æ‚«*/
			asci_1_tx_disable( tend );		/*ŽóM‚ÖØ‚è‘Ö‚¦*/
			return;
		}

		asci_1_transmit( sendbuff[sendcnt++] );	/*‚PƒoƒCƒg‚Ì‘—M*/
}

/************************************************************************
*																		*
*	Function name	<< dvauMenteSend >>									*
*					‘—M€”õˆ—										*
*	Syntax			dvauMenteSend( void );									*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauMenteSend( void )
{
		char	data;

//		strcpy( sendbuff, menteMessage );
		strcat( sendbuff, "\r\n" );

		sendlen = strlen(sendbuff);

		sendcnt = 0;

		asci_1_tx_enable();		/*‘—MŠJŽn  RTS=ON*/

		data = sendbuff[sendcnt++];
		asci_1_transmit( data );	/*‚PƒoƒCƒg‚Ì‘—M*/
}

/****** W <ADDRESS> <DATA> ******/
int W_command(char *p)
{
	unsigned long	address;
	int 	write_data;
	char	byte_data,*byte_address;

	if ( strlen(p)>=31 ){
		strcpy( sendbuff, "\r\nWRITE ADDRESS & DATA ERROR" ) ;
		dvauMenteSend();
		return( -1 );		/*•¶Žš—ñ‚ª‘½‚·‚¬‚é*/
	}

	sscanf( p, " %lX %X", &address, &write_data );

	byte_address = (char *)address;
	byte_data = (char)write_data ;
	*byte_address = byte_data;
	write_data = (int)*byte_address;
	sprintf( sendbuff, "\r\nWRITE BYTE ADDRESS %08lX WRITE DATA %02X READ DATA %02X"
				,byte_address, byte_data & 0x00FF, write_data & 0x00FF	) ;

	dvauMenteSend();
	return( 0 );
}

/****** R <ADDRESS> <DATA> ******/
int R_command(char *p)
{
	unsigned long	address;
	char	byte_data,*byte_address;

	sscanf( p, " %lX", &address );
	byte_address = (char *)address;
	byte_data = (int)*byte_address;
	sprintf( sendbuff,"\r\nREAD BYTE ADDRESS %08lX READ DATA %02X" ,byte_address, byte_data & 0xFF ) ;
	dvauMenteSend();

	return( 0 );
}

/****** D <ADDRESS> ******/
int D_command(char *p)
{
static	unsigned long	address;
	unsigned char	byte_data,*byte_address;
	unsigned char	dump_data[16];
	int 	i,j;

	sscanf( p, " %lX", &address );
	address &= 0xFFFFFFF0L;
	p = sendbuff;
	p += sprintf( p, "\r\n                            B I N A R Y                        ASCII      " ) ;
	p += sprintf( p, "\r\n ADDRESS  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF\r\n\r\n" ) ;
	for ( i=0; i<16; i++,address+=16L ){
		memset( dump_data, '\0', sizeof ( dump_data )) ;
		for ( j=0; j<16; j++ ) {
			byte_address = (unsigned char *)(address+(unsigned long)j);
			byte_data = *byte_address;
			dump_data[j] = byte_data;
		}
		p += sprintf( p, "%08lX  " , address ) ;
		for ( j=0; j<16; j++ ) {
			p += sprintf( p, "%02X " , dump_data[j] ) ;
		}
		for ( j=0; j<16; j++ ) {
			if ( dump_data[j] < ' ' || dump_data[j] > 'z' ) dump_data[j] = ' ' ;
			p += sprintf( p, "%c" , dump_data[j] );
		}
		p += sprintf( p, "\r\n" );
	}
	dvauMenteSend();
	return( 0 );
}



extern	void	(*IRQ4Jump)(void);

static	unsigned short	boot_request;
static	unsigned short	boot_ack;
static	void boot_int( void )
{
		boot_request = DUALBOOT.request;
		boot_ack = DUALBOOT.ack;
}

/****** SD <ADDRESS> ******/
int SD_command(char *p)
{
static	unsigned long	address;
	unsigned char	dump_data[16];
	int 	i,j;
		long	lwait;

		sscanf( p, " %lX", &address );

		boot_ack = 0;
		IRQ4Jump = boot_int;
		INTC.IER.BIT.IRQ4E = 1;		/* IRQ4 Use */

		DUALBOOT.address = address;
		DUALBOOT.len = 256;
		DUALBOOT.request = 0x0200;
		for( lwait=0; lwait<100000; lwait++ ){
			if( boot_ack == 0x0200 ){
				break;
			}
			if( (lwait%100)==0 ){
				DUALBOOT.request = 0x0200;
			}
		}
		if( DUALBOOT.ack!=0x0200 ){
			IRQ4Jump = NULL;
			INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

			strcpy(sendbuff,"SUB CPU no responce\r\n");
			dvauMenteSend();
			return( 1 );
		}

		IRQ4Jump = NULL;
		INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

	p = sendbuff;
	p += sprintf( p, "\r\n                            B I N A R Y                        ASCII      " ) ;
	p += sprintf( p, "\r\n ADDRESS  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF\r\n\r\n" ) ;
	for ( i=0; i<16; i++,address+=16L ){
		memset( dump_data, '\0', sizeof ( dump_data )) ;
		for ( j=0; j<16; j++ ) {
			dump_data[j] = DUALBOOT.data[i*16+j];
		}
		p += sprintf( p, "%08lX  " , address ) ;
		for ( j=0; j<16; j++ ) {
			p += sprintf( p, "%02X " , dump_data[j] ) ;
		}
		for ( j=0; j<16; j++ ) {
			if ( dump_data[j] < ' ' || dump_data[j] > 'z' ) dump_data[j] = ' ' ;
			p += sprintf( p, "%c" , dump_data[j] );
		}
		p += sprintf( p, "\r\n" );
	}
	dvauMenteSend();

	return( 0 );
}

/****** SW <ADDRESS> <DATA> ******/
int SW_command(char *p)
{
	unsigned long	address;
	int 	write_data;
	char	byte_data,*byte_address;
		long	lwait;

	if ( strlen(p)>=31 ){
		strcpy( sendbuff, "\r\nWRITE ADDRESS & DATA ERROR \r\n" ) ;
		dvauMenteSend();
		return( -1 );		/*•¶Žš—ñ‚ª‘½‚·‚¬‚é*/
	}

	sscanf( p, " %lX %X", &address, &write_data );

		boot_ack = 0;
		IRQ4Jump = boot_int;
		INTC.IER.BIT.IRQ4E = 1;		/* IRQ4 Use */

		DUALBOOT.address = address;
		DUALBOOT.len = 1;
		DUALBOOT.data[0] = write_data;
		DUALBOOT.request = 0x0100;
		for( lwait=0; lwait<100000; lwait++ ){
			if( boot_ack == 0x0100 ){
				break;
			}
			if( (lwait%100)==0 ){
				DUALBOOT.request = 0x0100;
			}
		}
		if( DUALBOOT.ack!=0x0100 ){
			IRQ4Jump = NULL;
			INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

			strcpy(sendbuff,"SUB CPU no responce\r\n");
			dvauMenteSend();
			return( 1 );
		}

		IRQ4Jump = NULL;
		INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

	sprintf( sendbuff,"\r\nWRITE BYTE ADDRESS %08lX WRITE DATA %02X\r\n"
				,byte_address, write_data & 0x00FF	) ;
		dvauMenteSend();

		return( 0 );
}

/****** SR <ADDRESS> <DATA> ******/
int SR_command(char *p)
{
	unsigned long	address;
	char	byte_data,*byte_address;
		long	lwait;

	sscanf( p, " %lX", &address );

		boot_ack = 0;
		IRQ4Jump = boot_int;
		INTC.IER.BIT.IRQ4E = 1;		/* IRQ4 Use */

		DUALBOOT.address = address;
		DUALBOOT.len = 1;
		DUALBOOT.request = 0x0200;
		for( lwait=0; lwait<100000; lwait++ ){
			if( boot_ack == 0x0200 ){
				break;
			}
			if( (lwait%100)==0 ){
				DUALBOOT.request = 0x0200;
			}
		}
		if( DUALBOOT.ack!=0x0200 ){
			IRQ4Jump = NULL;
			INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

			strcpy(sendbuff,"SUB CPU no responce\r\n");
			dvauMenteSend();
			return( 1 );
		}

		IRQ4Jump = NULL;
		INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

		byte_data = DUALBOOT.data[0];

	sprintf( sendbuff, "\r\nREAD BYTE ADDRESS %08lX READ DATA %02X\r\n" ,byte_address, byte_data & 0xFF ) ;
	dvauMenteSend();
	return( 0 );
}

/****** DIR ******/
int DIR_command(char *p)
{
	int		fd;
	int		i,j;
	char	buff[128];
	DIR_ENTRY	dir;
	struct diskfree_t	free;
	long	adr;
	FCB		*fcb;
	BPB		*bpb;
	short	offset;
	int		dir_entry,entry2;
	unsigned short	cluster;
	long	all_bytes;
	int		flag;
	int		len;
	union {
		unsigned char	bdata[4];
		unsigned short	wdata[2];
		unsigned long	ldata;
	} data;
	unsigned char	wkdata;

			fd = new_fcb( 0 );
			fcb = get_fcb(fd);
			bpb = fcb->bpb;
			if( bpb->part==NULL ){
				strcpy(sendbuff,"Card Not Found\r\n");
				dvauMenteSend();
				return( 1 );
			}

			for( dir_entry=0; dir_entry<bpb->root_entry; dir_entry++ ){
				if( _dos_read_rootdir( fd, &dir, dir_entry ) )
					break;		/*ƒ‹[ƒgƒfƒBƒŒƒNƒgƒŠ[“Çž‚ÝƒGƒ‰[*/
				if( dir.filename[0]==0 )
					break;			/*ŒŸõI—¹*/
				memcpy( buff, dir.filename, 8 );
				buff[8]='\0';
				printf( "%s.", buff );
				memcpy( buff, &dir.filename[8], 3 );
				buff[3]='\0';
				printf( "%s  ", buff );
				if( dir.attribute & _A_SUBDIR ){
					printf( "<DIR>" );
				}
				else {
					printf( "<FILE>" );
				}
				data.ldata = dir.all_bytes;
				wkdata=data.bdata[0];
				data.bdata[0] = data.bdata[3];
				data.bdata[3] = wkdata;
				wkdata=data.bdata[1];
				data.bdata[1] = data.bdata[2];
				data.bdata[2] = wkdata;
				printf( " %8lu Bytes\r\n", data.ldata );

				if( dir.attribute & _A_SUBDIR )
					;
				else
					continue;

				data.wdata[0] = dir.start_cluster;
				wkdata=data.bdata[0];
				data.bdata[0] = data.bdata[1];
				data.bdata[1] = wkdata;
				cluster = data.wdata[0];

				do {
					if( _dos_read_cluster( fd, cluster ) )
						break;		/*ƒTƒuƒfƒBƒŒƒNƒgƒŠ[“Çž‚ÝƒGƒ‰[*/
					for( entry2=0,offset=0; offset<(short)bpb->bytes_cluster; entry2++,offset+=32 ){
						memcpy( &dir, &cluster_buffer.buffer[offset], sizeof(dir) );
						if( dir.filename[0]==0 )
							break;			/*ŒŸõI—¹*/

						if( dir.filename[0]=='.' )
							continue;
						printf( "-- " );
						memcpy( buff, dir.filename, 8 );
						buff[8]='\0';
						printf( "%s.", buff );
						memcpy( buff, &dir.filename[8], 3 );
						buff[3]='\0';
						printf( "%s  ", buff );
						if( dir.attribute & _A_SUBDIR ){
							printf( "<DIR>" );
						}
						else {
							printf( "<FILE>" );
						}
						data.ldata = dir.all_bytes;
						wkdata=data.bdata[0];
						data.bdata[0] = data.bdata[3];
						data.bdata[3] = wkdata;
						wkdata=data.bdata[1];
						data.bdata[1] = data.bdata[2];
						data.bdata[2] = wkdata;
						printf( " %8lu Bytes\r\n", data.ldata );
					}
					if( offset<(short)bpb->bytes_cluster )
						break;			/*ŒŸõI—¹*/

					cluster = (unsigned short)_dos_next_cluster( fd, cluster );
				} while( cluster >= 2 && cluster!=0xFFFF );
			}

			free_fcb( fd );

			_dos_getdiskfree( 0, &free );
			printf( "\r\n  Disk Free = %lu Bytes\r\n",
				(long)(free.total_clusters-free.avail_clusters)
				*(long)free.sectors_per_cluster * (long)free.bytes_per_sector );

		return( 0 );
}

/****** DUMP <FILENAME> ******/
int DUMP_command(char *p)
{
		char	filename[BUFSIZ];
		FILE	*fd;
		unsigned long	address;
		unsigned char	dump_data[16];
		int 	j;
		int		len;

		sprintf( filename, "A:\\%s", p );
		fd = fopen( filename, "r" );
		if( fd==NULL ){
			printf("File Not Found %s\r\n", filename );
			return( 1 );
		}
		printf( "\r\n                            B I N A R Y                        ASCII      " ) ;
		printf( "\r\n ADDRESS  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF\r\n\r\n" ) ;
		address=0;
		while( (len=fread( dump_data, 1, sizeof(dump_data), fd ))>0 ){
			printf( "%08lX  " , address ) ;
			for ( j=0; j<len; j++ ) {
				printf( "%02X " , dump_data[j] ) ;
			}
			for( ; j<16; j++ ){
				printf("   ");
			}
			for ( j=0; j<len; j++ ) {
				if ( dump_data[j] < ' ' || dump_data[j] > 'z' ) dump_data[j] = ' ' ;
				printf( "%c" , dump_data[j] );
			}
			for( ; j<16; j++ ){
				printf(" ");
			}
			printf( "\r\n" );
			address += 16;
		}
		fclose( fd );

		return( 0 );
}

/****** TYPE <FILENAME> ******/
int TYPE_command(char *p)
{
		char	filename[BUFSIZ];
		FILE	*fd;
		char	dump_data[512];
		int 	j;

		sprintf( filename, "A:\\%s", p );
		fd = fopen( filename, "r" );
		if( fd==NULL ){
			printf("File Not Found %s\r\n", filename );
			return( 1 );
		}
		while( fgets( dump_data, sizeof(dump_data), fd )!=NULL ){
			printf("%s\r\n", dump_data );
		}
		fclose( fd );

		return( 0 );
}

/************************************************************************
*																		*
*	Function name	<< dvauMenteReceive >>									*
*					ŽóMˆ—											*
*	Syntax			dvauMenteReceive( unsigned char *data );		*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaMenteReceive( char *datan )
{
		char	*p;
		int		text1,text2,text3,text4,text5,text6,text7,text8;
		char	work[64];
		int		i;

		if( memcmp( datan, "START", 5 )==0 ){
			tms_start = 1;
			
			strcpy( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}
		if( memcmp( datan, "STOP", 4 )==0 ){
			tms_start = 0;

			strcpy( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}


				// ƒeƒXƒgó‹µ
		if( memcmp( datan, "SDR1", 4 )==0 ){
			datan += 4;
			if( memcmp( datan, "EXEC", 4 )==0 ){
				tms_sdr_data.SDR.testRequest.testExecRequest=1;
			}
			else {
				tms_sdr_data.SDR.testRequest.testExecRequest=0;
			}
			datan += 4;
			if( memcmp( datan, "CAT1", 4 )==0 ){
				tms_sdr_data.SDR.testRequest.testStartRequest1=1;
			}
			else {
				tms_sdr_data.SDR.testRequest.testStartRequest1=0;
			}
			datan += 4;
			if( memcmp( datan, "CAT2", 4 )==0 ){
				tms_sdr_data.SDR.testRequest.testStartRequest2=1;
			}
			else {
				tms_sdr_data.SDR.testRequest.testStartRequest2=0;
			}
			datan += 4;
			memcpy( work, datan, 2 );
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.itemNo = text1;

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}
				// ƒeƒLƒXƒgƒAƒNƒeƒBƒuƒtƒ‰ƒOó‹µ
		if( memcmp( datan, "SDR2", 4 )==0 ){
			datan += 4;
			if( memcmp( datan, "TIME", 4 )==0 ){
				tms_sdr_data.SDR.activeText.timeDateValid =1;
			}
			else {
				tms_sdr_data.SDR.activeText.timeDateValid =0;
			}
			datan += 4;
			if( memcmp( datan, "ADJ ", 4 )==0 ){
				tms_sdr_data.SDR.activeText.timeDateAdjustment=1;
			}
			else {
				tms_sdr_data.SDR.activeText.timeDateAdjustment=1;
			}
			datan += 4;
			if( memcmp( datan, "CARNO", 5 )==0 ){
				tms_sdr_data.SDR.activeText.carNoValid=1;
			}
			else {
				tms_sdr_data.SDR.activeText.carNoValid=0;
			}
			datan += 5;
			if( memcmp( datan, "CARTYPE", 7 )==0 ){
				tms_sdr_data.SDR.activeText.carTypeValid=1;
			}
			else {
				tms_sdr_data.SDR.activeText.carTypeValid=0;
			}

			timedata.testTimeTimer = 100;		// ƒ^ƒCƒ€ƒtƒ‰ƒO‚ð‚n‚e‚e‚·‚éƒ^ƒCƒ}[

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// “ú•tŽž
		if( memcmp( datan, "SDR3", 4 )==0 ){
			datan += 4;
			tms_sdr_data.SDR.timeDate.yy10 = *datan & 0x0F;
			datan++;
			tms_sdr_data.SDR.timeDate.yy1 = *datan & 0x0F;
			datan++;
			datan++;
			tms_sdr_data.SDR.timeDate.MM10 = *datan & 0x0F;
			datan++;
			tms_sdr_data.SDR.timeDate.MM1 = *datan & 0x0F;
			datan++;
			datan++;
			tms_sdr_data.SDR.timeDate.dd10 = *datan & 0x0F;
			datan++;
			tms_sdr_data.SDR.timeDate.dd1 = *datan & 0x0F;
			datan++;
			datan++;
			tms_sdr_data.SDR.timeDate.hh10 = *datan & 0x0F;
			datan++;
			tms_sdr_data.SDR.timeDate.hh1 = *datan & 0x0F;
			datan++;
			datan++;
			tms_sdr_data.SDR.timeDate.mm10 = *datan & 0x0F;
			datan++;
			tms_sdr_data.SDR.timeDate.mm1 = *datan & 0x0F;
			datan++;
			datan++;
			tms_sdr_data.SDR.timeDate.ss10 = *datan & 0x0F;
			datan++;
			tms_sdr_data.SDR.timeDate.ss1 = *datan & 0x0F;

			timedata.tm_year = tms_sdr_data.SDR.timeDate.yy10 * 10
								+ tms_sdr_data.SDR.timeDate.yy1;
			timedata.tm_mon = tms_sdr_data.SDR.timeDate.MM10 * 10
								+ tms_sdr_data.SDR.timeDate.MM1;
			timedata.tm_day = tms_sdr_data.SDR.timeDate.dd10 * 10
								+ tms_sdr_data.SDR.timeDate.dd1;
			timedata.tm_hour = tms_sdr_data.SDR.timeDate.hh10 * 10
								+ tms_sdr_data.SDR.timeDate.hh1;
			timedata.tm_min = tms_sdr_data.SDR.timeDate.mm10 * 10
								+ tms_sdr_data.SDR.timeDate.mm1;
			timedata.tm_sec = tms_sdr_data.SDR.timeDate.ss10 * 10
								+ tms_sdr_data.SDR.timeDate.ss1;

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// ŽÔ—¼”Ô†
		if( memcmp( datan, "SDR4", 4 )==0 ){
			datan += 4;

			memcpy( work, datan, 3 );
			datan+=3;
			work[3]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.carInfo.carNo = text1;

			memcpy( work, datan, 1 );
			datan+=1;
			work[1]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.carInfo.carType = text1;

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// ˜AŒ‹ƒR[ƒh
		if( memcmp( datan, "SDR5", 4 )==0 ){
			datan += 4;

			memcpy( work, datan, 1 );
			datan+=1;
			work[1]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.couplingUnitCode = text1;

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// ƒXƒe[ƒVƒ‡ƒ“”Ô†
		if( memcmp( datan, "SDR6", 4 )==0 ){
			datan += 4;

			memcpy( work, datan, 2 );
			datan+=2;
			work[2]='\0';
			sscanf( work, "%u", &text1 );
			memcpy( work, datan, 1 );
			datan+=1;
			work[1]='\0';
			sscanf( work, "%x", &text2 );
			tms_sdr_data.SDR.stationNumber = text1 | (text2<<5);

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// NextStation
		if( memcmp( datan, "SDR7", 4 )==0 ){
			datan += 4;

			memcpy( work, datan, 2 );
			datan+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.nextStationName = text1;
			memcpy( work, datan, 2 );
			datan+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.destination = text1;
			memcpy( work, datan, 2 );
			datan+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.routeCode = text1;
			memcpy( work, datan, 2 );
			datan+=2;
			work[2]='\0';
			sscanf( work, "%x", &text1 );
			tms_sdr_data.SDR.specialCode = text1;

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// ”ñí’Ê•ñŠí
		if( memcmp( datan, "SDR8", 4 )==0 ){
			datan += 4;

			if( memcmp( datan, "A1  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[0] |= 0x01;
			}
			else {
				tms_sdr_data.SDR.talkBack[0] &= ~0x01;
			}
			datan += 4;
			if( memcmp( datan, "A2  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[0] |= 0x02;
			}
			else {
				tms_sdr_data.SDR.talkBack[0] &= ~0x02;
			}
			datan += 4;
			if( memcmp( datan, "A3  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[0] |= 0x04;
			}
			else {
				tms_sdr_data.SDR.talkBack[0] &= ~0x04;
			}
			datan += 4;
			if( memcmp( datan, "A4  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[0] |= 0x08;
			}
			else {
				tms_sdr_data.SDR.talkBack[0] &= ~0x08;
			}
			datan += 4;
			if( memcmp( datan, "A5  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[0] |= 0x10;
			}
			else {
				tms_sdr_data.SDR.talkBack[0] &= ~0x10;
			}
			datan += 4;

			if( memcmp( datan, "C1  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[1] |= 0x01;
			}
			else {
				tms_sdr_data.SDR.talkBack[1] &= ~0x01;
			}
			datan += 4;
			if( memcmp( datan, "C2  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[1] |= 0x02;
			}
			else {
				tms_sdr_data.SDR.talkBack[1] &= ~0x02;
			}
			datan += 4;
			if( memcmp( datan, "C3  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[1] |= 0x04;
			}
			else {
				tms_sdr_data.SDR.talkBack[1] &= ~0x04;
			}
			datan += 4;
			if( memcmp( datan, "C4  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[1] |= 0x08;
			}
			else {
				tms_sdr_data.SDR.talkBack[1] &= ~0x08;
			}
			datan += 4;
			if( memcmp( datan, "C5  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[1] |= 0x10;
			}
			else {
				tms_sdr_data.SDR.talkBack[1] &= ~0x10;
			}
			datan += 4;

			if( memcmp( datan, "B1  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[2] |= 0x01;
			}
			else {
				tms_sdr_data.SDR.talkBack[2] &= ~0x01;
			}
			datan += 4;
			if( memcmp( datan, "B2  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[2] |= 0x02;
			}
			else {
				tms_sdr_data.SDR.talkBack[2] &= ~0x02;
			}
			datan += 4;
			if( memcmp( datan, "B3  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[2] |= 0x04;
			}
			else {
				tms_sdr_data.SDR.talkBack[2] &= ~0x04;
			}
			datan += 4;
			if( memcmp( datan, "B4  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[2] |= 0x08;
			}
			else {
				tms_sdr_data.SDR.talkBack[2] &= ~0x08;
			}
			datan += 4;
			if( memcmp( datan, "B5  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[2] |= 0x10;
			}
			else {
				tms_sdr_data.SDR.talkBack[2] &= ~0x10;
			}
			datan += 4;

			if( memcmp( datan, "B1-1", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[3] |= 0x01;
			}
			else {
				tms_sdr_data.SDR.talkBack[3] &= ~0x01;
			}
			datan += 4;
			if( memcmp( datan, "B1-2", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[3] |= 0x02;
			}
			else {
				tms_sdr_data.SDR.talkBack[3] &= ~0x02;
			}
			datan += 4;
			if( memcmp( datan, "B1-3", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[3] |= 0x04;
			}
			else {
				tms_sdr_data.SDR.talkBack[3] &= ~0x04;
			}
			datan += 4;
			if( memcmp( datan, "B1-4", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[3] |= 0x08;
			}
			else {
				tms_sdr_data.SDR.talkBack[3] &= ~0x08;
			}
			datan += 4;
			if( memcmp( datan, "B1-5", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[3] |= 0x10;
			}
			else {
				tms_sdr_data.SDR.talkBack[3] &= ~0x10;
			}
			datan += 4;

			if( memcmp( datan, "C1-1", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[4] |= 0x01;
			}
			else {
				tms_sdr_data.SDR.talkBack[4] &= ~0x01;
			}
			datan += 4;
			if( memcmp( datan, "C1-2", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[4] |= 0x02;
			}
			else {
				tms_sdr_data.SDR.talkBack[4] &= ~0x02;
			}
			datan += 4;
			if( memcmp( datan, "C1-3", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[4] |= 0x04;
			}
			else {
				tms_sdr_data.SDR.talkBack[4] &= ~0x04;
			}
			datan += 4;
			if( memcmp( datan, "C1-4", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[4] |= 0x08;
			}
			else {
				tms_sdr_data.SDR.talkBack[4] &= ~0x08;
			}
			datan += 4;
			if( memcmp( datan, "C1-5", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[4] |= 0x10;
			}
			else {
				tms_sdr_data.SDR.talkBack[4] &= ~0x10;
			}
			datan += 4;

			if( memcmp( datan, "B1  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[5] |= 0x01;
			}
			else {
				tms_sdr_data.SDR.talkBack[5] &= ~0x01;
			}
			datan += 4;
			if( memcmp( datan, "B2  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[5] |= 0x02;
			}
			else {
				tms_sdr_data.SDR.talkBack[5] &= ~0x02;
			}
			datan += 4;
			if( memcmp( datan, "B3  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[5] |= 0x04;
			}
			else {
				tms_sdr_data.SDR.talkBack[5] &= ~0x04;
			}
			datan += 4;
			if( memcmp( datan, "B4  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[5] |= 0x08;
			}
			else {
				tms_sdr_data.SDR.talkBack[5] &= ~0x08;
			}
			datan += 4;
			if( memcmp( datan, "B5  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[5] |= 0x10;
			}
			else {
				tms_sdr_data.SDR.talkBack[5] &= ~0x10;
			}
			datan += 4;

			if( memcmp( datan, "C1  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[6] |= 0x01;
			}
			else {
				tms_sdr_data.SDR.talkBack[6] &= ~0x01;
			}
			datan += 4;
			if( memcmp( datan, "C2  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[6] |= 0x02;
			}
			else {
				tms_sdr_data.SDR.talkBack[6] &= ~0x02;
			}
			datan += 4;
			if( memcmp( datan, "C3  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[6] |= 0x04;
			}
			else {
				tms_sdr_data.SDR.talkBack[6] &= ~0x04;
			}
			datan += 4;
			if( memcmp( datan, "C4  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[6] |= 0x08;
			}
			else {
				tms_sdr_data.SDR.talkBack[6] &= ~0x08;
			}
			datan += 4;
			if( memcmp( datan, "C5  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[6] |= 0x10;
			}
			else {
				tms_sdr_data.SDR.talkBack[6] &= ~0x10;
			}
			datan += 4;

			if( memcmp( datan, "A1  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[7] |= 0x01;
			}
			else {
				tms_sdr_data.SDR.talkBack[7] &= ~0x01;
			}
			datan += 4;
			if( memcmp( datan, "A2  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[7] |= 0x02;
			}
			else {
				tms_sdr_data.SDR.talkBack[7] &= ~0x02;
			}
			datan += 4;
			if( memcmp( datan, "A3  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[7] |= 0x04;
			}
			else {
				tms_sdr_data.SDR.talkBack[7] &= ~0x04;
			}
			datan += 4;
			if( memcmp( datan, "A4  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[7] |= 0x08;
			}
			else {
				tms_sdr_data.SDR.talkBack[7] &= ~0x08;
			}
			datan += 4;
			if( memcmp( datan, "A5  ", 4 )==0 ){
				tms_sdr_data.SDR.talkBack[7] |= 0x10;
			}
			else {
				tms_sdr_data.SDR.talkBack[7] &= ~0x10;
			}

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// •ú‘—ƒtƒ‰ƒO
		if( memcmp( datan, "SDR9", 4 )==0 ){
			datan += 4;

			if( memcmp( datan, "PA", 2 )==0 ){
				tms_sdr_data.SDR.ATCpaActive.atcActive=1;
			}
			else {
				tms_sdr_data.SDR.ATCpaActive.atcActive=0;
			}
			datan += 2;
			if( memcmp( datan, "DB", 2 )==0 ){
				tms_sdr_data.SDR.ATCpaActive.dbEffective=1;
			}
			else {
				tms_sdr_data.SDR.ATCpaActive.dbEffective=0;
			}
			datan += 2;

			timedata.testBroadcastTimer = 100;		// •ú‘—ƒtƒ‰ƒO‚ð‚n‚e‚e‚·‚éƒ^ƒCƒ}[

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// ƒhƒAƒtƒ‰ƒO
		if( memcmp( datan, "SDRA", 4 )==0 ){
			datan += 4;

			if( memcmp( datan, "  ", 2 )==0 ){
				tms_sdr_data.SDR.door.doorOpen=0;
			}
			else if( memcmp( datan, " R", 2 )==0 ){
				tms_sdr_data.SDR.door.doorOpen=1;
			}
			else if( memcmp( datan, "L ", 2 )==0 ){
				tms_sdr_data.SDR.door.doorOpen=2;
			}
			else {
				tms_sdr_data.SDR.door.doorOpen=3;
			}
			datan += 2;

			if( memcmp( datan, "DOOR", 4 )==0 ){
				tms_sdr_data.SDR.door.doorOpenEffective =1;
			}
			else {
				tms_sdr_data.SDR.door.doorOpenEffective =0;
			}
			datan += 4;

			if( memcmp( datan, "OFF", 3 )==0 ){
				tms_sdr_data.SDR.door.broadCastOff=1;
			}
			else {
				tms_sdr_data.SDR.door.broadCastOff=0;
			}

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}

				// ŽÔ—¼”Ô†
		if( memcmp( datan, "SDRB", 4 )==0 ){
			datan += 4;

			for( i=0; i<8; i++ ){
				memcpy( work, datan, 3 );
				datan+=3;
				work[3]='\0';
				sscanf( work, "%x", &text1 );
				tms_sdr_data.SDR.carInfo2[i].carNo = text1;
				memcpy( work, datan, 1 );
				datan+=1;
				work[1]='\0';
				sscanf( work, "%x", &text1 );
				tms_sdr_data.SDR.carInfo2[i].carType = text1;
			}

			sprintf( sendbuff, "OK" );
			dvauMenteSend();
			return;
		}


		if( memcmp( datan, "TMSSDR", 6 )==0 ){
			p = sendbuff;

				// ƒeƒXƒgó‹µ
			p += sprintf( p, "SDR1" );
			if( tms_sdr_data.SDR.testRequest.testExecRequest ){
				p += sprintf( p, "EXEC" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.testRequest.testStartRequest1 ){
				p += sprintf( p, "CAT1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.testRequest.testStartRequest2 ){
				p += sprintf( p, "CAT2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			p += sprintf( p, "%02X", tms_sdr_data.SDR.itemNo );
			p += sprintf( p, "\r\n" );

				// ƒeƒLƒXƒgƒAƒNƒeƒBƒuƒtƒ‰ƒOó‹µ
			p += sprintf( p, "SDR2" );
			if( tms_sdr_data.SDR.activeText.timeDateValid ){
				p += sprintf( p, "TIME" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.activeText.timeDateAdjustment ){
				p += sprintf( p, "ADJ " );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.activeText.carNoValid ){
				p += sprintf( p, "CARNO" );
			}
			else {
				p += sprintf( p, "     " );
			}
			if( tms_sdr_data.SDR.activeText.carTypeValid ){
				p += sprintf( p, "CARTYPE" );
			}
			else {
				p += sprintf( p, "       " );
			}
			p += sprintf( p, "\r\n" );

				// “ú•tŽž
			p += sprintf( p, "SDR3" );
			p += sprintf( p, "%01X%01X/%01X%01X/%01X%01X %01X%01X:%01X%01X:%01X%01X\r\n",
				tms_sdr_data.SDR.timeDate.yy10,
				tms_sdr_data.SDR.timeDate.yy1,
				tms_sdr_data.SDR.timeDate.MM10,
				tms_sdr_data.SDR.timeDate.MM1,
				tms_sdr_data.SDR.timeDate.dd10,
				tms_sdr_data.SDR.timeDate.dd1,
				tms_sdr_data.SDR.timeDate.hh10,
				tms_sdr_data.SDR.timeDate.hh1,
				tms_sdr_data.SDR.timeDate.mm10,
				tms_sdr_data.SDR.timeDate.mm1,
				tms_sdr_data.SDR.timeDate.ss10,
				tms_sdr_data.SDR.timeDate.ss1 );

				// ŽÔ—¼”Ô†
			p += sprintf( p, "SDR4" );
			p += sprintf( p, "%03X", tms_sdr_data.SDR.carInfo.carNo );
			p += sprintf( p, "%1X", tms_sdr_data.SDR.carInfo.carType );
			p += sprintf( p, "\r\n" );

				// ˜AŒ‹ƒR[ƒh
			p += sprintf( p, "SDR5" );
			p += sprintf( p, "%01X\r\n", tms_sdr_data.SDR.couplingUnitCode );
				
				// ƒXƒe[ƒVƒ‡ƒ“”Ô†
			p += sprintf( p, "SDR6" );
			p += sprintf( p, "%02u", tms_sdr_data.SDR.stationNumber & 0x1F );
			p += sprintf( p, "%01u\r\n", (tms_sdr_data.SDR.stationNumber & 0x0E0)>>5 );
				
				// NextStation
			p += sprintf( p, "SDR7" );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.nextStationName );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.destination );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.routeCode );
			p += sprintf( p, "%02X", tms_sdr_data.SDR.specialCode );
			p += sprintf( p, "\r\n" );

				// ”ñí’Ê•ñŠí
			p += sprintf( p, "SDR8" );
			if( tms_sdr_data.SDR.talkBack[0] & 0x01 ){
				p+= sprintf( p, "A1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[0] & 0x02 ){
				p+= sprintf( p, "A2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[0] & 0x04 ){
				p+= sprintf( p, "A3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[0] & 0x08 ){
				p+= sprintf( p, "A4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[0] & 0x10 ){
				p+= sprintf( p, "A5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sdr_data.SDR.talkBack[1] & 0x01 ){
				p+= sprintf( p, "C1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[1] & 0x02 ){
				p+= sprintf( p, "C2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[1] & 0x04 ){
				p+= sprintf( p, "C3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[1] & 0x08 ){
				p+= sprintf( p, "C4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[1] & 0x10 ){
				p+= sprintf( p, "C5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sdr_data.SDR.talkBack[2] & 0x01 ){
				p+= sprintf( p, "B1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[2] & 0x02 ){
				p+= sprintf( p, "B2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[2] & 0x04 ){
				p+= sprintf( p, "B3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[2] & 0x08 ){
				p+= sprintf( p, "B4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[2] & 0x10 ){
				p+= sprintf( p, "B5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sdr_data.SDR.talkBack[3] & 0x01 ){
				p+= sprintf( p, "B1-1" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[3] & 0x02 ){
				p+= sprintf( p, "B1-2" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[3] & 0x04 ){
				p+= sprintf( p, "B1-3" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[3] & 0x08 ){
				p+= sprintf( p, "B1-4" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[3] & 0x10 ){
				p+= sprintf( p, "B1-5" );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sdr_data.SDR.talkBack[4] & 0x01 ){
				p+= sprintf( p, "C1-1" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[4] & 0x02 ){
				p+= sprintf( p, "C1-2" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[4] & 0x04 ){
				p+= sprintf( p, "C1-3" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[4] & 0x08 ){
				p+= sprintf( p, "C1-4" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[4] & 0x10 ){
				p+= sprintf( p, "C1-5" );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sdr_data.SDR.talkBack[5] & 0x01 ){
				p+= sprintf( p, "B1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[5] & 0x02 ){
				p+= sprintf( p, "B2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[5] & 0x04 ){
				p+= sprintf( p, "B3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[5] & 0x08 ){
				p+= sprintf( p, "B4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[5] & 0x10 ){
				p+= sprintf( p, "B5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sdr_data.SDR.talkBack[6] & 0x01 ){
				p+= sprintf( p, "C1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[6] & 0x02 ){
				p+= sprintf( p, "C2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[6] & 0x04 ){
				p+= sprintf( p, "C3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[6] & 0x08 ){
				p+= sprintf( p, "C4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[6] & 0x10 ){
				p+= sprintf( p, "C5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sdr_data.SDR.talkBack[7] & 0x01 ){
				p+= sprintf( p, "A1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[7] & 0x02 ){
				p+= sprintf( p, "A2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[7] & 0x04 ){
				p+= sprintf( p, "A3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[7] & 0x08 ){
				p+= sprintf( p, "A4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.talkBack[7] & 0x10 ){
				p+= sprintf( p, "A5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			p += sprintf( p, "\r\n" );

				// •ú‘—ƒtƒ‰ƒO
			p += sprintf( p, "SDR9" );
			if( tms_sdr_data.SDR.ATCpaActive.atcActive ){
				p+= sprintf( p, "PA" );
			}
			else {
				p+= sprintf( p, "  " );
			}
			if( tms_sdr_data.SDR.ATCpaActive.dbEffective ){
				p+= sprintf( p, "DB" );
			}
			else {
				p+= sprintf( p, "  " );
			}
			p += sprintf( p, "\r\n" );

				// ƒhƒAƒtƒ‰ƒO
			p += sprintf( p, "SDRA" );
			if( tms_sdr_data.SDR.door.doorOpen==0 ){
				p+= sprintf( p, "  " );
			}
			else if( tms_sdr_data.SDR.door.doorOpen==1 ){
				p+= sprintf( p, " R" );
			}
			else if( tms_sdr_data.SDR.door.doorOpen==2 ){
				p+= sprintf( p, "L " );
			}
			else {
				p+= sprintf( p, "LR" );
			}
			if( tms_sdr_data.SDR.door.doorOpenEffective ){
				p+= sprintf( p, "DOOR" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sdr_data.SDR.door.broadCastOff ){
				p+= sprintf( p, "OFF" );
			}
			else {
				p+= sprintf( p, "   " );
			}
			p += sprintf( p, "\r\n" );

				// ŽÔ—¼”Ô†
			p += sprintf( p, "SDRB" );
			for( i=0; i<8; i++ ){
				p += sprintf( p, "%03X", tms_sdr_data.SDR.carInfo2[i].carNo );
				p += sprintf( p, "%1X", tms_sdr_data.SDR.carInfo2[i].carType );
			}

			dvauMenteSend();
			return;
		}

		if( memcmp( datan, "TMSSD ", 6 )==0 ){
			p = sendbuff;

				// ƒeƒXƒgó‹µ
			p += sprintf( p, "SD1" );
			if( tms_sd_data.SD.testResponce.testExecutionStatus==0 ){
				p += sprintf( p, "INIT" );
			}
			if( tms_sd_data.SD.testResponce.testExecutionStatus==1 ){
				p += sprintf( p, "EXEC" );
			}
			if( tms_sd_data.SD.testResponce.testExecutionStatus==2 ){
				p += sprintf( p, "END " );
			}
			if( tms_sd_data.SD.testResponce.testExecutionStatus==3 ){
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.testResponce.testStartResponce2 ){
				p += sprintf( p, "CAT2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.testResponce.testStartResponce1 ){
				p += sprintf( p, "CAT1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			p += sprintf( p, "%04X", tms_sd_data.SD.itemNo );
			if( tms_sd_data.SD.itemResult.OK ){
				p += sprintf( p, "OK" );
			}
			else if( tms_sd_data.SD.itemResult.NG ){
				p += sprintf( p, "NG" );
			}
			else {
				p += sprintf( p, "  " );
			}
			if( tms_sd_data.SD.subItemResult & 0x01 ){
				p += sprintf( p, "NG1" );
			}
			else {
				p += sprintf( p, "   " );
			}
			if( tms_sd_data.SD.subItemResult & 0x02 ){
				p += sprintf( p, "NG2" );
			}
			else {
				p += sprintf( p, "   " );
			}
			if( tms_sd_data.SD.subItemResult & 0x04 ){
				p += sprintf( p, "NG3" );
			}
			else {
				p += sprintf( p, "   " );
			}
			if( tms_sd_data.SD.subItemResult & 0x08 ){
				p += sprintf( p, "NG4" );
			}
			else {
				p += sprintf( p, "   " );
			}
			if( tms_sd_data.SD.subItemResult & 0x10 ){
				p += sprintf( p, "NG5" );
			}
			else {
				p += sprintf( p, "   " );
			}
			if( tms_sd_data.SD.subItemResult & 0x20 ){
				p += sprintf( p, "NG6" );
			}
			else {
				p += sprintf( p, "   " );
			}
			if( tms_sd_data.SD.subItemResult & 0x40 ){
				p += sprintf( p, "NG7" );
			}
			else {
				p += sprintf( p, "   " );
			}
			if( tms_sd_data.SD.subItemResult & 0x80 ){
				p += sprintf( p, "NG8" );
			}
			else {
				p += sprintf( p, "   " );
			}
			p += sprintf( p, "\r\n" );

			// ƒo[ƒWƒ‡ƒ“
			p += sprintf( p, "SD2" );
			p += sprintf( p, "%04X", tms_sd_data.SD.version );
			p += sprintf( p, "\r\n" );

			// ”ñí’Ê•ñŠí
			p += sprintf( p, "SD3" );
			if( tms_sd_data.SD.passengerAlarm[0] & 0x01 ){
				p+= sprintf( p, "A1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[0] & 0x02 ){
				p+= sprintf( p, "A2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[0] & 0x04 ){
				p+= sprintf( p, "A3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[0] & 0x08 ){
				p+= sprintf( p, "A4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[0] & 0x10 ){
				p+= sprintf( p, "A5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sd_data.SD.passengerAlarm[1] & 0x01 ){
				p+= sprintf( p, "C1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[1] & 0x02 ){
				p+= sprintf( p, "C2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[1] & 0x04 ){
				p+= sprintf( p, "C3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[1] & 0x08 ){
				p+= sprintf( p, "C4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[1] & 0x10 ){
				p+= sprintf( p, "C5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sd_data.SD.passengerAlarm[2] & 0x01 ){
				p+= sprintf( p, "B1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[2] & 0x02 ){
				p+= sprintf( p, "B2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[2] & 0x04 ){
				p+= sprintf( p, "B3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[2] & 0x08 ){
				p+= sprintf( p, "B4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[2] & 0x10 ){
				p+= sprintf( p, "B5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sd_data.SD.passengerAlarm[3] & 0x01 ){
				p+= sprintf( p, "B1-1" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[3] & 0x02 ){
				p+= sprintf( p, "B1-2" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[3] & 0x04 ){
				p+= sprintf( p, "B1-3" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[3] & 0x08 ){
				p+= sprintf( p, "B1-4" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[3] & 0x10 ){
				p+= sprintf( p, "B1-5" );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sd_data.SD.passengerAlarm[4] & 0x01 ){
				p+= sprintf( p, "C1-1" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[4] & 0x02 ){
				p+= sprintf( p, "C1-2" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[4] & 0x04 ){
				p+= sprintf( p, "C1-3" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[4] & 0x08 ){
				p+= sprintf( p, "C1-4" );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[4] & 0x10 ){
				p+= sprintf( p, "C1-5" );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sd_data.SD.passengerAlarm[5] & 0x01 ){
				p+= sprintf( p, "B1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[5] & 0x02 ){
				p+= sprintf( p, "B2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[5] & 0x04 ){
				p+= sprintf( p, "B3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[5] & 0x08 ){
				p+= sprintf( p, "B4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[5] & 0x10 ){
				p+= sprintf( p, "B5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sd_data.SD.passengerAlarm[6] & 0x01 ){
				p+= sprintf( p, "C1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[6] & 0x02 ){
				p+= sprintf( p, "C2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[6] & 0x04 ){
				p+= sprintf( p, "C3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[6] & 0x08 ){
				p+= sprintf( p, "C4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[6] & 0x10 ){
				p+= sprintf( p, "C5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}

			if( tms_sd_data.SD.passengerAlarm[7] & 0x01 ){
				p+= sprintf( p, "A1  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[7] & 0x02 ){
				p+= sprintf( p, "A2  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[7] & 0x04 ){
				p+= sprintf( p, "A3  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[7] & 0x08 ){
				p+= sprintf( p, "A4  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			if( tms_sd_data.SD.passengerAlarm[7] & 0x10 ){
				p+= sprintf( p, "A5  " );
			}
			else {
				p+= sprintf( p, "    " );
			}
			p += sprintf( p, "\r\n" );

			// ’ÊMƒGƒ‰[
			p += sprintf( p, "SD4" );
			if( tms_sd_data.SD.dvaCommError.dvauCommError1 ){
				p += sprintf( p, "DVA1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.dvauCommError2 ){
				p += sprintf( p, "DVA2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.mcpCommError1 ){
				p += sprintf( p, "MCP1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.mcpCommError2 ){
				p += sprintf( p, "MCP2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.paaCommError1 ){
				p += sprintf( p, "PAA1" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.paaCommError2 ){
				p += sprintf( p, "PAA2" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.paaCommError3 ){
				p += sprintf( p, "PAA3" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.paaCommError4 ){
				p += sprintf( p, "PAA4" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.paaCommError5 ){
				p += sprintf( p, "PAA5" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.paaCommError6 ){
				p += sprintf( p, "PAA6" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.paaCommError7 ){
				p += sprintf( p, "PAA7" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaCommError.paaCommError8 ){
				p += sprintf( p, "PAA8" );
			}
			else {
				p += sprintf( p, "    " );
			}
			p += sprintf( p, "\r\n" );

			// ƒ‚[ƒh
			p += sprintf( p, "SD5" );
			if( tms_sd_data.SD.dvaStatus.manualMode ){
				p += sprintf( p, "MANUAL" );
			}
			else {
				p += sprintf( p, "      " );
			}
			if( tms_sd_data.SD.dvaStatus.cabMode ){
				p += sprintf( p, "CAB/CAB" );
			}
			else {
				p += sprintf( p, "       " );
			}
			if( tms_sd_data.SD.dvaStatus.emgMode ){
				p += sprintf( p, "EMERGENCY" );
			}
			else {
				p += sprintf( p, "         " );
			}
			if( tms_sd_data.SD.dvaStatus.specialMode ){
				p += sprintf( p, "SPECIAL" );
			}
			else {
				p += sprintf( p, "       " );
			}
			if( tms_sd_data.SD.dvaStatus.autoMode ){
				p += sprintf( p, "AUTO" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaStatus.doorMode ){
				p += sprintf( p, "DOOR" );
			}
			else {
				p += sprintf( p, "    " );
			}
			if( tms_sd_data.SD.dvaStatus.doorChimeMode ){
				p += sprintf( p, "CHIME" );
			}
			else {
				p += sprintf( p, "     " );
			}
			p += sprintf( p, "\r\n" );

			dvauMenteSend();
			return;

		}

		if( memcmp( datan, "HELP", 4 )==0 ){
			strcpy( sendbuff, "TMSSDR\r\n" );
			strcat( sendbuff, "TMSSD \r\n" );
			strcat( sendbuff, "SDR1-SDRB\r\n" );
			strcat( sendbuff, "\r\nOK" );

			dvauMenteSend();
			return;
		}

		if( memcmp( datan, "DIR", 3 )==0 ){
			datan+=3;
			DIR_command( datan );
			return;
		}
		if( memcmp( datan, "DUMP", 4 )==0 ){
			datan+=5;
			DUMP_command( datan );
			return;
		}
		if( memcmp( datan, "TYPE", 4 )==0 ){
			datan+=5;
			TYPE_command( datan );
			return;
		}

		if( *datan=='D' ){
			datan++;
			D_command( datan );
			return;
		}
		if( *datan=='W' ){
			datan++;
			W_command( datan );
			return;
		}
		if( *datan=='R' ){
			datan++;
			R_command( datan );
			return;
		}

		if( memcmp( datan, "SD ", 3 )==0 ){
			datan+=2;
			SD_command( datan );
			return;
		}
		if( memcmp( datan, "SW ", 3 )==0 ){
			datan+=2;
			SW_command( datan );
			return;
		}
		if( memcmp( datan, "SR ", 3 )==0 ){
			datan+=2;
			SR_command( datan );
			return;
		}

		strcpy( sendbuff, "NG" );
		dvauMenteSend();
}


/************************************************************************
*																		*
*	Function name	<< asci_1_recv_ir >>								*
*					ŽóMŠ„‚èž‚Ýˆ—									*
*	Syntax			asci_1_recv_ir( char data, char status );			*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_recv_ir( char data, char status )
{
/*.	---------------------@ˆ—Ú×à–¾  ----------------------------	*/
/*  ƒtƒF[ƒY–ˆ‚ÉŽóMƒf[ƒ^‚ðŽæ‚èž‚Þ									*/
/*. ------------------------------------------------------------------  */
status=0;

		if( data==CRLF ){
			recvbuff[recvlen]='\0';
			recvlen = 0;
			recvon = 1;
			return;
		}
		if( data==ESC ){
			recvlen = 0;
			recvbuff[recvlen]='\0';

			sendbuff[0]='\0';
			dvauMenteSend();
			return;
		}

		if( recvlen<sizeof(recvbuff) ){
			recvbuff[recvlen++] = data;
			recvbuff[recvlen]='\0';
		}
}

/************************************************************************
*																		*
*	Function name	<< dvauMENTEInitialize >>						*
*					ƒƒ“ƒeƒiƒ“ƒX’ÊM‰ŠúÝ’è									*
*	Syntax			dvauMENTEInitialize( void );							*
*	Argument		none												*
*	Author			Nissin system lab.  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauMENTEInitialize( void )
{
		asci_1_Initialize( 9600, 8, 'N', 0 );
}

void dvauMENTEMain( void )
{
		if( recvon ){
			recvon=0;
			dvaMenteReceive( recvbuff );
		}
}

