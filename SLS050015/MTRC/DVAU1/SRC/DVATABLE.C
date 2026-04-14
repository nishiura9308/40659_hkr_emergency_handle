/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	テーブル定義
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/08/07		K.Honda
			a01. 2000/10/10		K.Honda
			a02. 2001/03/22	日付によるファイル変更
			a03. 2013/05/10 駅コードによる到着前放送検索追加
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include	"iodefine.h"

#include	"table.h"
#include	"timer.h"

#include "dosaccs.h"
#include "fcntl.h"


/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
//
//	バックアップ情報の定義
//
#pragma section BACKUP
struct	BACKUP_DATA		backup_data;		// バックアップデータ

char	dataVersionCRCL;
char	dataVersionCRCH;

struct	CONFIG_INFO		config_data;		// 設定情報
struct	ANNOUNCE_INFO	announce_data;		// 放送設定情報
#pragma section

struct	ROUTEINFO		route;				// 駅情報

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
#define	MAGIC_CODE	0x314A

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカルテーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	void dvaIOOut( unsigned long onbit, unsigned long offbit );

extern	struct	TIMER_CONTROL	timedata;		// タイマー情報

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
static	unsigned char	switchDate[8];		// yy mm dd hh mm ss
static	int				newFileFlag;		// New File Read

/*@@@E*/
							/* 5432 1098 7654 3210 */
							/* 0001 0000 0010 0001 */
#define	CRCCCITT	0x8408		/** X^16 + X^12 + X^5 + X^0 */

unsigned short	crcCCITT_table[256];
unsigned short	crcCode;

void GenerateCRC( void )
{
		int		i,j;
		unsigned short	crc;

		for( i=0; i<256; i++ ){
			crc = (unsigned short)i;
			for( j=0; j<8; j++ ){
				if( crc & 0x0001 ){
					crc >>=1;
					crc ^= CRCCCITT;
				}
				else {
					crc >>=1;
				}
			}
			crcCCITT_table[i] = crc;
		}
}

void InitializeCRC( void )
{
		crcCode = 0;
}

void CalculateCRCCCITT( unsigned char data )
{
		crcCode = crcCCITT_table[ ( crcCode ^  (unsigned short)data ) & 0xFF ] ^ ( crcCode >> 8 );
}

/************************************************************************

	Function name	<< dvaGetDefaultRoute >>
					存在するルート検索処理
	Syntax			dvaGetDefaultRoute( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	日付によるファイル変更

*************************************************************************/
struct ROUTEINFO *dvaGetDefaultRoute( void )
{
		FILE	*fd;
		int		i;
		char	filename[16];
extern	char	dosaccs_workbuff[512];		/*ストリームバッファー*/

		for( i=0; i<MAX_ROUTE; i++ ){
			if( newFileFlag ){
				sprintf( filename, "A:\\ROUTE%02X.DAT", i );
			}
			else {
				sprintf( filename, "A:\\ROUTE%02X.BEF", i );
			}
			fd = fopen( filename, "rb" );
			if( fd!=NULL ){
				setvbuf( fd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );
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

	Function name	<< dvaBackupCheck >>
					バックアップ情報のチェック
	Syntax			ret = dvaBackupCheck( void );
	Argument		int ret : o : 0= バックアップ状態ＯＫ
					      1= バックアップ状態クリア
					     -1= パラメータデータ異常
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	日付によるファイル変更

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		バックアップエリアをチェックし
		バックアップデータが有効であれば　リターンする
		有効でなければ、バックアップされたデータをクリアし
		ＡＴＡカードから設定情報を読み込み　テーブルに設定する
		ＡＴＡカードから読み込めなければ、エラーとしてリターンする

	------------------------------------------------------------------[@@@E]*/
int dvaBackupCheck( void )
{
		FILE	*fd;
		int		ret;
		unsigned char	*p;
		int		i;
extern	char	dosaccs_workbuff[512];		/*ストリームバッファー*/

#if 0
printf("Before Dump\r\n");
{
unsigned long	address;
unsigned char	byte_data,*byte_address;
unsigned char	dump_data[16];
int 	i,j;

	address = (unsigned long)&backup_data;
	address &= 0xFFFFFFF0L;
	printf( "\r\n                            B I N A R Y                        ASCII      " ) ;
	printf( "\r\n ADDRESS  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF\r\n\r\n" ) ;
	for ( i=0; i<32; i++,address+=16L ){
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
}
#endif
		ret = 0;
		if( P4.DR.BIT.B0 ){
#if 0
printf("Backup Data Check\r\n");
printf("magicCode = %X\r\n", backup_data.magicCode );
#endif
			if( backup_data.magicCode != MAGIC_CODE ){
				// Backup NG
				// Backup Area Clear
				memset( &backup_data, 0, sizeof(backup_data) );
				// Trace Data Reset
				TraceClear();

				backup_data.magicCode = MAGIC_CODE;		// Magic Code Reset
				ret = 1;
			}
		}
		else {
#if 0
printf("Backup Data Force Clear\r\n");
#endif
			// Backup NG
			// Backup Area Clear
			memset( &backup_data, 0, sizeof(backup_data) );
			// Trace Data Reset
			TraceClear();

			backup_data.magicCode = MAGIC_CODE;		// Magic Code Reset
			ret = 1;
		}

#if 0
printf("After Dump\r\n");
{
unsigned long	address;
unsigned char	byte_data,*byte_address;
unsigned char	dump_data[16];
int 	i,j;

	address = (unsigned long)&backup_data;
	address &= 0xFFFFFFF0L;
	printf( "\r\n                            B I N A R Y                        ASCII      " ) ;
	printf( "\r\n ADDRESS  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 0123456789ABCDEF\r\n\r\n" ) ;
	for ( i=0; i<32; i++,address+=16L ){
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
}
#endif

		GenerateCRC();		// ＣＲＣテーブルの作成

		// Read Information Data
		newFileFlag = 1;		// 新しいファイルを使用

		fd = fopen("A:\\SWITCH.DAT", "rb" );
		if( fd!=NULL ){
			setvbuf( fd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );
			if( fread( switchDate, 1, sizeof(switchDate), fd )!=sizeof(switchDate) ){
				fclose( fd );
				return( -1 );
			}
			fclose( fd );

			if( timedata.dateActive ){		// 日付比較ＯＫ？
				if( switchDate[0] < timedata.tm_year ){	// 新しい
					newFileFlag = 1;
				}
				else if( switchDate[0] > timedata.tm_year ){	// 古い
					newFileFlag = 0;
				}
				else if( switchDate[1] < timedata.tm_mon ){	// 新しい
					newFileFlag = 1;
				}
				else if( switchDate[1] > timedata.tm_mon ){	// 古い
					newFileFlag = 0;
				}
				else if( switchDate[2] < timedata.tm_day ){	// 新しい
					newFileFlag = 1;
				}
				else if( switchDate[2] > timedata.tm_day ){	// 古い
					newFileFlag = 0;
				}
				else if( switchDate[3] < timedata.tm_hour ){	// 新しい
					newFileFlag = 1;
				}
				else if( switchDate[3] > timedata.tm_hour ){	// 古い
					newFileFlag = 0;
				}
				else if( switchDate[4] < timedata.tm_min ){	// 新しい
					newFileFlag = 1;
				}
				else if( switchDate[4] > timedata.tm_min ){	// 古い
					newFileFlag = 0;
				}
			}
		}


		// Read Information Data
		if( newFileFlag ){
			fd = fopen("A:\\CONFIG.DAT", "rb" );
		}
		else {
			fd = fopen("A:\\CONFIG.BEF", "rb" );
		}
		if( fd==NULL ){
			return( -1 );
		}
		setvbuf( fd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );
		if( fread( &config_data, 1, sizeof(config_data), fd )!=sizeof(config_data) ){
			fclose( fd );
			return( -1 );
		}
		fclose( fd );

		InitializeCRC();
		p = (unsigned char *)&config_data;
		for( i=0; i<16; i++ ){
			CalculateCRCCCITT( (unsigned char)*p++ );
		}

		dataVersionCRCL = (unsigned char)((crcCode) & 0x0FF);
		dataVersionCRCH = (unsigned char)(((crcCode)>>8) & 0x0FF);


		if( newFileFlag ){
			fd = fopen("A:\\ANNOUNCE.DAT", "rb" );
		}
		else {
			fd = fopen("A:\\ANNOUNCE.BEF", "rb" );
		}
		if( fd==NULL ){
			return( -1 );
		}
		setvbuf( fd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );
		if( fread( &announce_data, 1, sizeof(announce_data), fd )!=sizeof(announce_data) ){
			fclose( fd );
			return( -1 );
		}
		fclose( fd );

		// Read Default Root Data
		if( dvaGetDefaultRoute()==NULL )
			return( -1 );


		return( ret );
}


/************************************************************************

	Function name	<< dvaSearchRoute >>
					ルート検索処理
	Syntax			info = dvaSearchRoute( routeCode );
	Argument		unsigned char routeCode : i : ルートコード
				struct ROUTEINFO *info  : o : ルート情報アドレス
							       NULL = ルート情報なし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	日付によるファイル変更

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		ルートコードからＡＴＡカード上のルート情報を読み出す

	------------------------------------------------------------------[@@@E]*/
struct ROUTEINFO *dvaSearchRoute( unsigned char routeCode )
{
		FILE	*fd;
		char	filename[16];
		int		len;
extern	char	dosaccs_workbuff[512];		/*ストリームバッファー*/
extern	BPB	root_bpb[MAX_DRIVE];		/* ＢＰＢ管理エリア */

		if( route.routeCode==routeCode ){
			return( &route );
		}

		if( newFileFlag ){
			sprintf( filename, "A:\\ROUTE%02X.DAT", routeCode );
		}
		else {
			sprintf( filename, "A:\\ROUTE%02X.BEF", routeCode );
		}
		fd = fopen( filename, "rb" );
		if( fd!=NULL ){
			setvbuf( fd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );
			if( (len=fread( &route, 1, sizeof(route), fd ))==sizeof(route) ){
				fclose( fd );
				if( route.routeCode==routeCode ){
					return( &route );
				}
			}
			else {
				fclose( fd );
			}
		}

		// Card Check
		fd = fopen("A:\\CONFIG.DAT", "rb" );
		if( fd==NULL ){
			dvaIOOut( OUTSIGNAL_LED7|OUTSIGNAL_LED6, 0xFFFF );
			Halt();
		}
		fclose( fd );

//		root_bpb[0].part = NULL;
//		INTC.IER.BIT.IRQ0E = 0;		/* IRQ0 No Use */

		return( NULL );
}

/************************************************************************

	Function name	<< dvaSearchStation >>
					駅名検索処理
	Syntax			info = dvaSearchStation( routeCode, stationCode );
	Argument		unsigned char routeCode   : i : ルートコード
				unsigned char stationCode : i : 駅コード
				struct STATIONINFO *info  : o : 駅情報アドレス
							       NULL = 駅情報なし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2000/11/13  stationNumber 用データを検索しない
				a02 2001/03/22	日付によるファイル変更

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		ルートコードからＡＴＡカード上のルート情報を読み出す
		駅コードからルート情報中の駅情報を取り出す

	------------------------------------------------------------------[@@@E]*/
struct STATIONINFO *dvaSearchStation( unsigned char routeCode, unsigned char stationCode )
{
		int		i;

		if( dvaSearchRoute( routeCode )==NULL )
			return( NULL );

		if( route.routeCode==routeCode ){
			for( i=0; i<MAX_STATION; i++ ){
				if( route.station[i].stationCode==0x00 ){
					break;
				}
				if( route.station[i].Arriving==0 && route.station[i].Arrived==0 ){
					if( route.station[i].stationCode==stationCode ){
						backup_data.nowStationNo = i;
						return( &route.station[i] );
					}
				}
			}
		}
		return( NULL );
}

/************************************************************************

	Function name	<< dvaSearchStationNumber >>
					stationNumber検索処理
	Syntax			info = dvaSearchStationNumber( routeCode, stationNumber );
	Argument		unsigned char routeCode     : i : ルートコード
				unsigned char stationNumber : i : 駅コード
				struct STATIONINFO *info    : o : 駅情報アドレス
							       NULL = 駅情報なし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/09/07
				a01 2001/03/22	日付によるファイル変更

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		ルートコードからＡＴＡカード上のルート情報を読み出す
		駅コードからルート情報中のstaionNumber情報を取り出す

	------------------------------------------------------------------[@@@E]*/
struct STATIONINFO *dvaSearchStationNumber( unsigned char routeCode, unsigned char stationNumber )
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
						backup_data.nowStationNo = i;
						return( &route.station[i] );
					}
				}
			}
		}
		return( NULL );
}

/************************************************************************

	Function name	<< dvaSearchStationNumberExt >>
					stationNumber検索処理
	Syntax			info = dvaSearchStationNumberExt( routeCode, stationCode );
	Argument		unsigned char routeCode     : i : ルートコード
				unsigned char stationCode : i : 駅コード
				struct STATIONINFO *info    : o : 駅情報アドレス
							       NULL = 駅情報なし
	Author			TOA Corporation  H.nishiura
		Revision		a00 2013/05/10

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		ルートコードからＡＴＡカード上のルート情報を読み出す
		駅コードからルート情報中のstaionNumber情報を取り出す

	------------------------------------------------------------------[@@@E]*/
struct STATIONINFO *dvaSearchStationNumberExt( unsigned char routeCode, unsigned char stationCode )
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
					if( route.station[i].stationCode==stationCode ){
						backup_data.nowStationNo = i;
						return( &route.station[i] );
					}
				}
			}
		}
		return( NULL );
}

/************************************************************************

	Function name	<< dvaSearchDestination >>
					到着駅検索処理
	Syntax			info = dvaSearchDestination( routeCode );
	Argument		unsigned char routeCode     : i : ルートコード
				struct STATIONINFO *info    : o : 駅情報アドレス
							       NULL = 駅情報なし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	日付によるファイル変更

*************************************************************************/
/*. --------------------------【機能詳細】----------------------------[@@@S]

	処理概要：
		ルートコードからＡＴＡカード上のルート情報を読み出す
		ルート情報中の最終駅の情報を取り出す

	------------------------------------------------------------------[@@@E]*/
struct STATIONINFO *dvaSearchDestination( unsigned char routeCode )
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

/************************************************************************

	Function name	<< dvaBackupSet >>
					バックアップ情報の格納
	Syntax			ret = dvaBackup( void );
	Argument		int ret : o : 0= バックアップＯＫ
					     -1= パラメータデータ異常
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/07
				a01 2001/03/22	日付によるファイル変更

*************************************************************************/
int dvaBackup( void )
{
		FILE	*fd;
extern	char	dosaccs_workbuff[512];		/*ストリームバッファー*/

		// Write Information Data
		if( newFileFlag ){
			fd = fopen("A:\\CONFIG.DAT", "w" );
		}
		else {
			fd = fopen("A:\\CONFIG.BEF", "w" );
		}
		if( fd==NULL ){
			return( -1 );
		}
		setvbuf( fd, dosaccs_workbuff, _IOFBF, sizeof(dosaccs_workbuff) );
		if( fwrite( &config_data, 1, sizeof(config_data), fd )!=sizeof(config_data) ){
			fclose( fd );
			return( -1 );
		}
		fclose( fd );

		return( 0 );
}

