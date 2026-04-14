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

/***************************************************************
			実アクセスルーティン
***************************************************************/

#include "dosaccs.h"
#include "fcntl.h"

extern	BPB	root_bpb[MAX_DRIVE];
extern	FCB	root_fcb[MAX_FCB];

/***************************************************************
	物理セクター読み込み（セクター番号で読み込む）
		セクター０をブートセクターとした読み込み

	INPUT:
		bpb = 			デバイス情報
		sector = 0...	読み込みセクター
		num = 1...		読み込みセクター数

		buffer = 		読み込みバッファーアドレス

	RETURN:
		0 = 読み込み正常
		other = 読み込みエラーステータス
***************************************************************/
int	sector_read( BPB *bpb, long sector, short num, unsigned char *buff )
{
		int		ret;
		long	cylinder,head,sector_n;
		PARTITION_INFO	*part;

		part = bpb->part;
		if( part==NULL )
			return( SECTOR_NOINIT );		/*初期設定エラー*/
		if( (void*)part->driver_read==NULL )
			return( SECTOR_NOINIT );		/*初期設定エラー*/

		sector_n = sector + part->boot_sector;
		if( sector_n >= part->all_sector )
			return( SECTOR_OVER );		/*セクターオーバー*/

//			/*シリンダー番号を求める*/
//		cylinder = sector_n / ( part->sector_track * part->head_number );
//		sector_n = sector_n % ( part->sector_track * part->head_number );
//
//			/*ヘッド番号を求める*/
//		head     = sector_n / part->sector_track;
//			/*セクター番号を求める*/
//		sector_n = sector_n % part->sector_track;
//
//			/*物理セクターの読み込み*/
		ret = part->driver_read( sector_n, num, buff );

		return( ret );
}

/***************************************************************
	物理セクター書き込み（セクター番号で読み込む）
		セクター０をブートセクターとした書き込み

	INPUT:
		bpb = 			デバイス情報
		sector = 0...	書き込みセクター
		num = 1...		書き込みセクター数

		buffer = 		書き込みバッファーアドレス

	RETURN:
		0 = 書き込み正常
		other = 書き込みエラーステータス
***************************************************************/
int	sector_write( BPB *bpb, long sector, short num, unsigned char *buff )
{
		int		ret;
		long	cylinder,head,sector_n;
		PARTITION_INFO	*part;

		part = bpb->part;
		if( part==NULL )
			return( SECTOR_NOINIT );		/*初期設定エラー*/
		if( (void*)part->driver_write==NULL )
			return( SECTOR_NOINIT );		/*初期設定エラー*/

		sector_n = sector + part->boot_sector;
		if( sector_n >= part->all_sector )
			return( SECTOR_OVER );		/*セクターオーバー*/

//			/*シリンダー番号を求める*/
//		cylinder = sector_n / ( part->sector_track * part->head_number );
//		sector_n = sector_n % ( part->sector_track * part->head_number );
//
//			/*ヘッド番号を求める*/
//		head     = sector_n / part->sector_track;
//			/*セクター番号を求める*/
//		sector_n = sector_n % part->sector_track;
//
			/*物理セクターの書き込み*/
		ret = part->driver_write( sector_n, num, buff );

		return( ret );
}

/***************************************************************
	物理ドライブ情報の読込み

	INPUT:
		bpb = 			デバイス情報

	RETURN:
		ドライブステータス
			SECTOR_OK
			SECTOR_PROTECT
			SECTOR_NOINIT
***************************************************************/
int	sector_status( BPB *bpb )
{
		PARTITION_INFO	*part;

		part = bpb->part;
		if( part==NULL )
			return( SECTOR_NOINIT );		/*初期設定エラー*/
		if( (void*)part->driver_status==NULL )
			return( SECTOR_NOINIT );		/*初期設定エラー*/

			/*物理セクターの書き込み*/
		return( part->driver_status( ) );
}

