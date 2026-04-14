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

#include <stdio.h>
#include <string.h>

#include "iodefine.h"
#include "table.h"

#include "dosaccs.h"
#include "fcntl.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#define	CRLF		0x0D
#define	ESC			0x1B

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	テーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	void asci_1_Initialize( int baud, char data, char parity, char stop );
extern	void asci_1_tx_enable( void );
extern	void asci_1_tx_disable( char tend );
extern	void asci_1_transmit( unsigned char data );

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	char	recvbuff[512];		/*受信バッファー*/
static	short	recvlen;			/*受信バイト数*/
static	char	sendbuff[512];		/*送信バッファー*/
static	short	sendlen;			/*送信バイト数*/
static	short	sendcnt;			/*送信カウンター*/
static	char	recvon;

/*@@@E*/
/************************************************************************
*																		*
*	Function name	<< asci_1_send_ir >>								*
*					送信割り込み処理									*
*	Syntax			asci_1_send_ir( void );								*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_send_ir( char tend )
{
		if( sendcnt==sendlen ){	/*すべて送信したとき*/
			asci_1_tx_disable( tend );		/*受信へ切り替え*/
			return;
		}

		asci_1_transmit( sendbuff[sendcnt++] );	/*１バイトの送信*/
}

/************************************************************************
*																		*
*	Function name	<< dvauMenteSend >>									*
*					送信準備処理										*
*	Syntax			dvauMenteSend( void );									*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauMenteSend( void )
{
		char	data;

		strcat( sendbuff, "\r\n" );
		sendlen = strlen(sendbuff);

		sendcnt = 0;

		asci_1_tx_enable();		/*送信開始  RTS=ON*/

		data = sendbuff[sendcnt++];
		asci_1_transmit( data );	/*１バイトの送信*/
}

/****** W <ADDRESS> <DATA> ******/
int W_command(char *p)
{
	unsigned long	address;
	int 	write_data;
	char	byte_data,*byte_address;

	if ( strlen(p)>=31 ){
		printf( "\r\nWRITE ADDRESS & DATA ERROR \r\n" ) ;
		return( -1 );		/*文字列が多すぎる*/
	}

	sscanf( p, " %lX %X", &address, &write_data );
/*
		if ( address > 0xFFFFFF ){
			printf( "\nWRITE ADDRESS & DATA ERROR \n" ) ;
			return( -1 );
		}
*/
	byte_address = (char *)address;
	byte_data = (char)write_data ;
	*byte_address = byte_data;
	write_data = (int)*byte_address;
	printf( "\r\nWRITE BYTE ADDRESS %08lX WRITE DATA %02X READ DATA %02X\r\n"
				,byte_address, byte_data & 0x00FF, write_data & 0x00FF	) ;
	return( 0 );
}

/****** R <ADDRESS> <DATA> ******/
int R_command(char *p)
{
	unsigned long	address;
	char	byte_data,*byte_address;

	sscanf( p, " %lX", &address );
/*
		if ( address > 0xFFFFFF ){
			printf( "\r\nWRITE ADDRESS & DATA ERROR \r\n" ) ;
			return( -1 );
		}
*/
	byte_address = (char *)address;
	byte_data = (int)*byte_address;
	printf( "\r\nREAD BYTE ADDRESS %08lX READ DATA %02X\r\n" ,byte_address, byte_data & 0xFF ) ;
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
/*
		if ( address+16*16-1 > 0xFFFFFF ){
			printf( "\r\nWRITE ADDRESS & DATA ERROR \r\n" ) ;
			return( -1 );
		}
*/
	address &= 0xFFFFFFF0L;
	printf( "\r\n                            B I N A R Y                        ASCII      " ) ;
	printf( "\r\n ADDRESS  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF\r\n\r\n" ) ;
	for ( i=0; i<16; i++,address+=16L ){
		memset( dump_data, '\0', sizeof ( dump_data )) ;
		for ( j=0; j<16; j++ ) {
			byte_address = (unsigned char *)(address+(unsigned long)j);
			byte_data = *byte_address;
			dump_data[j] = byte_data;
		}
		printf( "%08lX  " , address ) ;
		for ( j=0; j<16; j++ ) {
			printf( "%02X " , dump_data[j] ) ;
		}
		for ( j=0; j<16; j++ ) {
			if ( dump_data[j] < ' ' || dump_data[j] > 'z' ) dump_data[j] = ' ' ;
			printf( "%c" , dump_data[j] );
		}
		printf( "\r\n" );
	}
	return( 0 );
}



extern	void	(*IRQ4Jump)(void);

static	unsigned char	boot_request;
static	unsigned char	boot_ack;
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
		DUALBOOT.request = 0x20;
		for( lwait=0; lwait<100000; lwait++ ){
			if( boot_ack == 0x20 ){
				break;
			}
			if( (lwait%100)==0 ){
				DUALBOOT.request = 0x20;
			}
		}
		if( DUALBOOT.ack!=0x20 ){
			IRQ4Jump = NULL;
			INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

			printf("SUB CPU no responce\r\n");
			return( 1 );
		}

		IRQ4Jump = NULL;
		INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

	printf( "\r\n                            B I N A R Y                        ASCII      " ) ;
	printf( "\r\n ADDRESS  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF\r\n\r\n" ) ;
	for ( i=0; i<16; i++,address+=16L ){
		memset( dump_data, '\0', sizeof ( dump_data )) ;
		for ( j=0; j<16; j++ ) {
			dump_data[j] = DUALBOOT.data[i*16+j];
		}
		printf( "%08lX  " , address ) ;
		for ( j=0; j<16; j++ ) {
			printf( "%02X " , dump_data[j] ) ;
		}
		for ( j=0; j<16; j++ ) {
			if ( dump_data[j] < ' ' || dump_data[j] > 'z' ) dump_data[j] = ' ' ;
			printf( "%c" , dump_data[j] );
		}
		printf( "\r\n" );
	}

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
		printf( "\r\nWRITE ADDRESS & DATA ERROR \r\n" ) ;
		return( -1 );		/*文字列が多すぎる*/
	}

	sscanf( p, " %lX %X", &address, &write_data );

		boot_ack = 0;
		IRQ4Jump = boot_int;
		INTC.IER.BIT.IRQ4E = 1;		/* IRQ4 Use */

		DUALBOOT.address = address;
		DUALBOOT.len = 1;
		DUALBOOT.data[0] = write_data;
		DUALBOOT.request = 0x10;
		for( lwait=0; lwait<100000; lwait++ ){
			if( boot_ack == 0x10 ){
				break;
			}
			if( (lwait%100)==0 ){
				DUALBOOT.request = 0x10;
			}
		}
		if( DUALBOOT.ack!=0x10 ){
			IRQ4Jump = NULL;
			INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

			printf("SUB CPU no responce\r\n");
			return( 1 );
		}

		IRQ4Jump = NULL;
		INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

	printf( "\r\nWRITE BYTE ADDRESS %08lX WRITE DATA %02X\r\n"
				,byte_address, write_data & 0x00FF	) ;

		return( 0 );
}

/****** SR <ADDRESS> <DATA> ******/
int SR_command(char *p)
{
	unsigned long	address;
	char	byte_data,*byte_address;
		long	lwait;

	sscanf( p, " %lX", &address );
/*
		if ( address > 0xFFFFFF ){
			printf( "\r\nWRITE ADDRESS & DATA ERROR \r\n" ) ;
			return( -1 );
		}
*/
		boot_ack = 0;
		IRQ4Jump = boot_int;
		INTC.IER.BIT.IRQ4E = 1;		/* IRQ4 Use */

		DUALBOOT.address = address;
		DUALBOOT.len = 1;
		DUALBOOT.request = 0x20;
		for( lwait=0; lwait<100000; lwait++ ){
			if( boot_ack == 0x20 ){
				break;
			}
			if( (lwait%100)==0 ){
				DUALBOOT.request = 0x20;
			}
		}
		if( DUALBOOT.ack!=0x20 ){
			IRQ4Jump = NULL;
			INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

			printf("SUB CPU no responce\r\n");
			return( 1 );
		}

		IRQ4Jump = NULL;
		INTC.IER.BIT.IRQ4E = 0;		/* IRQ4 NoUse */

		byte_data = DUALBOOT.data[0];

	printf( "\r\nREAD BYTE ADDRESS %08lX READ DATA %02X\r\n" ,byte_address, byte_data & 0xFF ) ;
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
				printf("Card Not Found\r\n");
				return( 1 );
			}

			for( dir_entry=0; dir_entry<bpb->root_entry; dir_entry++ ){
				if( _dos_read_rootdir( fd, &dir, dir_entry ) )
					break;		/*ルートディレクトリー読込みエラー*/
				if( dir.filename[0]==0 )
					break;			/*検索終了*/
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
						break;		/*サブディレクトリー読込みエラー*/
					for( entry2=0,offset=0; offset<(short)bpb->bytes_cluster; entry2++,offset+=32 ){
						memcpy( &dir, &cluster_buffer.buffer[offset], sizeof(dir) );
						if( dir.filename[0]==0 )
							break;			/*検索終了*/

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
						break;			/*検索終了*/

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
*					受信処理											*
*	Syntax			dvauMenteReceive( unsigned char *data );		*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvaMenteReceive( char *datan )
{
		char	*p;
		int		text1,text2,text3,text4,text5,text6,text7,text8;
		int		i;
		char	fname[32];

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

		if( memcmp( datan, "SD", 2 )==0 ){
			datan+=2;
			SD_command( datan );
			return;
		}
		if( memcmp( datan, "SW", 2 )==0 ){
			datan+=2;
			SW_command( datan );
			return;
		}
		if( memcmp( datan, "SR", 2 )==0 ){
			datan+=2;
			SR_command( datan );
			return;
		}
}


/************************************************************************
*																		*
*	Function name	<< asci_1_recv_ir >>								*
*					受信割り込み処理									*
*	Syntax			asci_1_recv_ir( char data, char status );			*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void asci_1_recv_ir( char data, char status )
{
/*.	---------------------　処理詳細説明  ----------------------------	*/
/*  フェーズ毎に受信データを取り込む									*/
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
*					メンテナンス通信初期設定									*
*	Syntax			dvauMENTEInitialize( void );							*
*	Argument		none												*
*	Author			TOA Corporation  K.Honda							*
*	Revision		a00 2000/06/27										*
*																		*
*************************************************************************/
void dvauMENTEInitialize( void )
{
		asci_1_Initialize( 9600, 8, 'N', 0 );

		recvon=0;
}


void dvauMENTEMain( void )
{
		if( recvon ){
			recvon=0;
			dvaMenteReceive( recvbuff );
		}
}

