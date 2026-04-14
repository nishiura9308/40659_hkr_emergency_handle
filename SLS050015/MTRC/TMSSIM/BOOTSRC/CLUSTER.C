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

#include "dosaccs.h"
#include "fcntl.h"

/***************************************************************
		ＦＡＴおよび、クラスター情報の初期設定
			更新情報をクリアする
***************************************************************/
void purge_cluster( void )
{
		rootdir_buffer.bpb = NULL;
		rootdir_buffer.update = 0;
		fat_buffer.bpb = NULL;
		fat_buffer.update = 0;
		cluster_buffer.bpb = NULL;
		cluster_buffer.update = 0;
}

/***************************************************************
	ルートディレクトリー情報計算
		エントリー番号からセクター番号を求める

	INPUT:
		bpb = デバイス情報
		entry = ルートディレクトリーエントリー番号

	OUTPUT:
		sector = セクター番号
		offset = バッファー上の位置

	RETURN:
		0 = クラスター正常
		other = 読み込みエラーステータス
***************************************************************/
static	int	root_dir_calc( BPB *bpb, unsigned short entry, long *sector, short *offset )
{
		if( bpb==NULL )
			return( -1 );			/*デバイス情報無し*/

		if( entry>=bpb->root_entry )
			return( -1 );			/*エントリー番号オーバー*/

		*sector = 0;
		*offset = 0;
		while( entry-- ){
			*offset += 32;
			if( *offset>=(short)bpb->bytes_sector ){
				*offset = 0;
				*sector = *sector + 1;
			}
		}
		if( *sector>bpb->DATA_sector_l )
			return( -1 );			/*エントリー番号エラー*/

		*sector += bpb->DIR_sector_s;

		return( 0 );
}

/***************************************************************
	ＦＡＴ情報計算（クラスター番号でのＦＡＴ上の位置）

	INPUT:
		bpb = デバイス情報
		cluster = 2...	クラスター番号

	OUTPUT:
		sector = セクター番号
		sector_l = 読込みセクター数
		offset = バッファー上の位置

	RETURN:
		0 = クラスター正常
		other = 読み込みエラーステータス
***************************************************************/
static	int	fat_calc( BPB *bpb, unsigned short cluster, long *sector, short *sector_l, short *offset )
{
		long	entry;

		if( bpb==NULL )
			return( -1 );			/*デバイス情報無し*/
		if( cluster<2 || cluster==0xFFFF )
			return( -1 );			/*クラスター番号エラー*/

		if( cluster>bpb->DATA_cluster_l+1 )
			return( -1 );			/*クラスター番号エラー*/

		if( bpb->FAT_type & FAT16 ){
			entry = (long)cluster * 2L;
			*sector = entry / bpb->bytes_sector;
			*offset = (short)(entry % bpb->bytes_sector);
			*sector_l = 1;
		}
		else {
			entry = (long)cluster + (long)(cluster>>1);
			*sector = entry / bpb->bytes_sector;
			*offset = (short)(entry % bpb->bytes_sector);
			if( *offset==(short)bpb->bytes_sector-1 )
				*sector_l = 2;
			else
				*sector_l = 1;
		}

		*sector += bpb->FAT_sector_s;

		return( 0 );
}


/***************************************************************
	論理クラスターからセクター番号を計算する

	INPUT:
		bpb = デバイス情報
		cluster = 2...	クラスター番号

	OUTPUT:
		sector = セクター番号

	RETURN:
		0 = 読み込み正常
		other = 読み込みエラーステータス

***************************************************************/
static	int	cluster_calc( BPB *bpb, unsigned short cluster, long *sector )
{
		if( bpb==NULL )
			return( -1 );			/*デバイス情報無し*/
		if( cluster<2 || cluster==0xFFFF )
			return( -1 );			/*クラスター番号エラー*/

		if( cluster>bpb->DATA_cluster_l )
			return( -1 );			/*クラスター番号エラー*/

		*sector = bpb->sector_cluster * ( cluster - 2 );

		*sector += bpb->DATA_sector_s;

		return( 0 );
}

/**************************************************************************
	ルートディレクトリーの更新情報を実際に更新する
***************************************************************************/
int flush_dir( void )
{
		int		ret;

		if( rootdir_buffer.bpb!=NULL && rootdir_buffer.update ){

			ret = sector_write( rootdir_buffer.bpb, rootdir_buffer.sector_no, 1, rootdir_buffer.buffer );
			if( ret )
				return( -2 );

		}

		rootdir_buffer.update = 0;

		return( 0 );
}

/**************************************************************************
	ＦＡＴの更新情報を実際に更新する
***************************************************************************/
int flush_fat( void )
{
		long	li;
		int		ret;

		if( fat_buffer.bpb!=NULL && fat_buffer.update ){
			for( li=0; li<(long)fat_buffer.bpb->FAT_number; li++ ){
				ret = sector_write( fat_buffer.bpb, fat_buffer.sector_no+fat_buffer.bpb->FAT_sector_l*li, fat_buffer.sector_len, fat_buffer.buffer );
				if( ret )
					break;
			}
			if( ret )
				return( -2 );
		}

		fat_buffer.update = 0;

		return( 0 );
}

/**************************************************************************
	クラスターの更新情報を実際に更新する
***************************************************************************/
int flush_cluster( void )
{
		int		ret;

		if( cluster_buffer.bpb!=NULL && cluster_buffer.update ){
			ret = sector_write( cluster_buffer.bpb, cluster_buffer.sector_no, cluster_buffer.bpb->sector_cluster, cluster_buffer.buffer );
			if( ret )
				return( -2 );
		}

		cluster_buffer.update = 0;

		return( 0 );
}

/***************************************************************
	ルートディレクトリー情報読み込み
		エントリー番号指定で、１セクターずつ読み込む

	INPUT:
		bpb = デバイス情報
		entry = ルートディレクトリーエントリー番号

	OUTPUT:
		offset = バッファー上の開始位置
		cluster_buffer[] = ディレクトリー情報

	RETURN:
		0 = 読み込み正常
		other = 読み込みエラーステータス

***************************************************************/
int	root_dir_read( BPB *bpb, unsigned short entry, short *offset )
{
		long	sector;

		if( root_dir_calc( bpb, entry, &sector, offset ) )
			return( -1 );

		if( rootdir_buffer.bpb!=bpb || rootdir_buffer.sector_no!=sector ){
			/** 前回読み込んだ情報が更新されているとき **/
			if( flush_dir() )
				return( -2 );		/*書き込みエラー*/

			if( sector_read( bpb, sector, 1, rootdir_buffer.buffer ) )
				return( -1 );			/*読込み不可*/

			rootdir_buffer.bpb = bpb;
			rootdir_buffer.sector_no = sector;
		}

		return( 0 );
}

/***************************************************************
	ディレクトリー情報書き込み
		ディレクトリー情報を書き換えたとき呼び出す
		実際に書込むのは別のエリアを読み込んだとき

***************************************************************/
void	root_dir_write( void )
{
		rootdir_buffer.update = 1;
}

/***************************************************************
	ＦＡＴ情報読み込み（クラスター番号で読み込む）
		クラスター番号に相当するＦＡＴ情報エリアを読込み
		バッファー上の開始位置を返す


	INPUT:
		bpb = デバイス情報
		cluster = 2...	クラスター番号

	OUTPUT:
		offset = バッファー上の開始位置
		cluster_buffer[] = ＦＡＴ情報

	RETURN:
		0 = 読み込み正常
		other = 読み込みエラーステータス

***************************************************************/
int	fat_read( BPB *bpb, unsigned short cluster, short *offset )
{
		long	sector;
		short	sector_l;

		if( fat_calc( bpb, cluster, &sector, &sector_l, offset ) )
			return( -1 );

		if( fat_buffer.bpb!=bpb || fat_buffer.sector_no!=sector || fat_buffer.sector_len!=sector_l ){
			/** 前回読み込んだ情報が更新されているとき **/
			if( flush_fat() )
				return( -2 );		/*書き込みエラー*/

			if( sector_read( bpb, sector, sector_l, fat_buffer.buffer ) )
				return( -1 );			/*読込み不可*/

			fat_buffer.bpb = bpb;
			fat_buffer.sector_no = sector;
			fat_buffer.sector_len = sector_l;
		}

		return( 0 );
}

/***************************************************************
	ＦＡＴ情報書き込み
		ＦＡＴ情報を書き換えたとき呼び出す
		実際に書込むのは別のエリアを読み込んだとき

***************************************************************/
void	fat_write( void )
{
		fat_buffer.update = 1;
}

/***************************************************************
	論理クラスター読み込み（クラスター番号で読み込む）

	INPUT:
		bpb = デバイス情報
		cluster = 2...	クラスター番号

	OUTPUT:
		cluster_buffer[] = クラスター情報

	RETURN:
		0 = 読み込み正常
		other = 読み込みエラーステータス

***************************************************************/
int	cluster_read( BPB *bpb, unsigned short cluster, unsigned short subdir )
{
		long	sector;

		if( cluster_buffer.bpb!=bpb || cluster_buffer.cluster_no!=cluster ){
			/** 前回読み込んだ情報が更新されているとき **/
			if( flush_cluster() )
				return( -2 );		/*書き込みエラー*/

			if( cluster_calc( bpb, cluster, &sector ) )
				return( -1 );

			if( sector_read( bpb, sector, bpb->sector_cluster, cluster_buffer.buffer ) ){
				return( -1 );
			}

			cluster_buffer.bpb = bpb;
			cluster_buffer.cluster_no = cluster;
			cluster_buffer.sector_no = sector;
			cluster_buffer.subdir = subdir;
		}
		return( 0 );
}

/***************************************************************
	新規論理クラスター読み込み（物理的に読み込まない）

	INPUT:
		bpb = デバイス情報
		cluster = 2...	クラスター番号

	OUTPUT:
		cluster_buffer[] = クラスター情報

	RETURN:
		0 = 読み込み正常
		other = 読み込みエラーステータス

***************************************************************/
int	new_cluster_read( BPB *bpb, unsigned short cluster, unsigned short subdir )
{
		long	sector;

		if( cluster_buffer.bpb!=bpb || cluster_buffer.cluster_no!=cluster ){
			/** 前回読み込んだ情報が更新されているとき **/
			if( flush_cluster() )
				return( -2 );		/*書き込みエラー*/

			if( cluster_calc( bpb, cluster, &sector ) )
				return( -1 );

			cluster_buffer.bpb = bpb;
			cluster_buffer.cluster_no = cluster;
			cluster_buffer.sector_no = sector;
			cluster_buffer.subdir = subdir;
		}
		return( 0 );
}

/***************************************************************
	論理クラスター書き込み
		クラスターを書き換えたとき呼び出す
		実際に書込むのは別のエリアを読み込んだとき
***************************************************************/
void	cluster_write( void )
{
		cluster_buffer.update = 1;
}



/***************************************************************
	次のクラスター番号検索

	INPUT:
		bpb = デバイス情報
		cluster = 2...	検索開始クラスター番号

	RETURN:
		クラスター番号
***************************************************************/
unsigned short	next_cluster( BPB *bpb, unsigned short cluster )
{
		short			offset;
		unsigned short	next_data;

		next_data = cluster;		// Ｈ８のコンパイラー不具合
		/*ＦＡＴ情報読込み*/
		if( fat_read( bpb, cluster, &offset ) ){
			return( 0xFFFF );			/*クラスター番号エラー*/
		}

		if( bpb->FAT_type & FAT16 ){
			next_data = ((unsigned short)fat_buffer.buffer[offset] & 0xFF)
					| ((unsigned short)(fat_buffer.buffer[offset+1]<<8)&0xFF00);
			return( next_data );
		}

		if( (next_data % 2)==0 ){
			next_data = ((unsigned short)fat_buffer.buffer[offset] & 0xFF)
					| ((unsigned short)(fat_buffer.buffer[offset+1]<<8) & 0xF00);
			if( next_data==0x0FFF )
				next_data = 0xFFFF;
			return( next_data );
		}

		next_data = (((unsigned short)fat_buffer.buffer[offset]>>4) & 0x0F)
				| ((unsigned short)(fat_buffer.buffer[offset+1]<<4) & 0xFF0);
		if( next_data==0x0FFF )
			next_data = 0xFFFF;
		return( next_data );
}

/***************************************************************
	空のクラスター番号検索

	INPUT:
		bpb = デバイス情報

	RETURN:
		クラスター番号
		-1 = 空クラスター番号無し
***************************************************************/
unsigned short	empty_cluster( BPB *bpb, unsigned short now_cluster )
{
		unsigned short	cluster;

		if( bpb==NULL )
			return( 0xFFFF );			/*デバイス情報無し*/

		for( cluster=now_cluster; cluster<bpb->DATA_cluster_l+2; cluster++ ){
			if( next_cluster( bpb, cluster )==0 )
				break;
		}
		if( cluster<bpb->DATA_cluster_l+2 )
			return( cluster );

		for( cluster=2; cluster<bpb->DATA_cluster_l+2; cluster++ ){
			if( next_cluster( bpb, cluster )==0 )
				break;
		}
		if( cluster<bpb->DATA_cluster_l+2 )
			return( cluster );

		return( 0xFFFF );
}

/***************************************************************
	クラスター番号登録

	INPUT:
		bpb = デバイス情報
		cluster = 2...	登録するクラスター番号
		next_no = 2... 登録する内容

	RETURN:
		0 = 読み込み正常
		other = 読み込みエラーステータス
***************************************************************/
int	update_cluster( BPB *bpb, unsigned short cluster, unsigned short next_no )
{
		short			offset;

		/*ＦＡＴ情報読込み*/
		if( fat_read( bpb, cluster, &offset ) ){
			return( -1 );			/*クラスター番号エラー*/
		}

		if( bpb->FAT_type & FAT16 ){
			fat_buffer.buffer[offset] = ((unsigned char)next_no & 0xFF);
			fat_buffer.buffer[offset+1] = ((unsigned char)(next_no>>8) & 0xFF);
		}
		else if( cluster % 2 ){
			fat_buffer.buffer[offset] = ((unsigned char)(next_no<<4) & 0xF0)
								| ((unsigned char)fat_buffer.buffer[offset] & 0x0F);
			fat_buffer.buffer[offset+1] = (unsigned char)(next_no>>4) & 0xFF;
		}
		else {
			fat_buffer.buffer[offset] = ((unsigned char)next_no & 0xFF);
			fat_buffer.buffer[offset+1] = ((unsigned char)(next_no>>8) & 0x0F)
								| ((unsigned char)fat_buffer.buffer[offset+1] & 0xF0);
		}

		fat_write();

		return( 0 );
}
