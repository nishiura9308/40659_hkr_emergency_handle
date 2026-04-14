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

#include <machine.h>
#include <string.h>

#include    "vect.h"
#include "dosaccs.h"
#include "fcntl.h"

#define STDIN  0
#define STDOUT 1
#define STDERR 2

extern	int		dosaccs_function;
extern	int		dosaccs_fd_result;
extern	char	*dosaccs_filename_buffer;
extern	int		dosaccs_access_len_pos;
extern	int		dosaccs_file_attr_seek;
extern	struct find_t *dosaccs_result;
extern	unsigned dosaccs_drive;
extern	struct _diskfree_t *dosaccs_param;
extern	char	*dosaccs_filename2;

extern	short	dosaccs_target_run;

extern	FCB	root_fcb[];

extern	void charput( char data );

/***************************************************************
	共通ファイル検索処理
		処理対象のファイル名までサブディレクトリーから
		順に検索してゆく

	INPUT:
			filename = 検索指定文字列（ワイルドカード可）
					サブディレクトリー名などにワイルドカードを付けると
					最初に一致するサブディレクトリー名となる
						"C:\SUB\FILE.EXT"
						"C:\SUB\F???.EXT"
						"C:\SUB\F*.EXT"
			fcb = 確保したＦＣＢエリア

			dir = 検索結果のディレクトリー情報
			fname = 検索したファイル名

	RETURN:
			 0 ... 正常終了
			 1 ... ワイルドカードのファイル名
			-1 ... サブディレクトリー名無し
			-2 ... ファイル名無し

***************************************************************/
static	int	search_filename( char *filename, FCB *fcb, DIR_ENTRY *dir, char *fname )
{
		int		len;
		int		wild;

		memset( fname, ' ', 8+3);		/*検索名のクリア*/
		len = 0;
		wild = 0;
		fcb->dir_cluster = 0;			/*ルートディレクトリーから検索*/

		/*サブディレクトリー検索*/
		while( *filename ){
			if( *filename=='\\' ){			/*ディレクトリーセパレーター？*/
				if( fname[0]!=' ' ){			/*検索名が入っている場合*/
					if( search_directory( fcb, fname, _A_SUBDIR, dir ) ){
						return( -1 );			/*ディレクトリーがない*/
					}
					fcb->dir_cluster = dir->start_cluster;	/*開始クラスター番号が検索結果のクラスター*/
				}

				memset( fname, ' ', 8+3 );		/*検索名のクリア*/
				len = 0;
				wild = 0;
			}
			else if( *filename=='.' ){			/*エクステンドセパレーター？*/
				len = 8;						/*次からはエクステンド名*/
			}
			else {
				if( *filename=='*' ){		/*ワイルドカードの場合*/
					while( len<8 )					/*次からはすべて検索対象文字列*/
						fname[len++] = '?';
					wild = 1;
				}
				else {
					fname[len++] = *filename;
					if( *filename=='?' )
						wild = 1;
				}
			}
			filename++;
		}

		if( fname[0]==' ' ){
			return( -2 );			/*検索名（ファイル名）がない*/
		}

		if( wild ){
			return( 1 );			/*ワイルドカードのファイル名*/
		}
		return( 0 );				/*通常ファイル名*/
}

/***************************************************************
	ファイル位置変更処理

	INPUT:
			fd = ファイルＩＤ
			pos = 位置
			flag = 基点
				SEEK_CUR  1 = 現在位置
				SEEK_END  2 = 最後
				SEEK_SET  0 = 最初

	RETURN:
			実読込みサイズ
***************************************************************/
long	dosaccs_seek( int fd, long pos, int flag )
{
		FCB		*fcb;
		BPB		*bpb;
		unsigned short	cluster;

		fcb = get_fcb(fd);
		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/
		bpb = fcb->bpb;
		if( bpb==NULL )
			return( -1 );				/*デバイス情報無し*/
		if( bpb==(BPB *)-1 )
			return( -1 );				/*ファイル入出力でない*/

		switch( flag ){
			case SEEK_CUR:
				pos = pos + fcb->cur_pos;
				break;
			case SEEK_END:
				pos = fcb->all_bytes - pos;
				break;
		}

		if( pos < 0L )
			pos = 0L;

		if( pos!=fcb->cur_pos ){
			cluster = fcb->start_cluster;
			fcb->cur_cluster = cluster;
			fcb->cur_pos = 0L;
			while( pos >= (long)bpb->bytes_cluster ){
				cluster = next_cluster( bpb, cluster );
				pos -= bpb->bytes_cluster;
				fcb->cur_pos += bpb->bytes_cluster;
				if( cluster<2 ){
					pos = 0;
					break;
				}
				fcb->cur_cluster = cluster;
			}

			fcb->cur_pos += pos;
		}

		if( fcb->cur_pos > fcb->all_bytes ){
			fcb->cur_pos = fcb->all_bytes;
		}

		return( fcb->cur_pos );
}

/***************************************************************
	ファイルオープン処理

	INPUT:
			filename = ファイル名
						"C:\\SUB\\FILE.EXT"
			mode = オープンモード
						O_RDONLY	0 = READ ONLY
						O_WRONLY	1 = WRITE ONLY
						O_RDWR		2 = READ/WRITE mode
						O_APPEND	8 = APPEND mode
						O_CREAT		0x100 = Create open
						O_TRUNC		0x200 = open truncate
						O_EXCL		0x400 = open not already file
						O_TEXT		0x4000 = TEXT mode
						O_BINARY	0x8000 = BINARY mode
			pmode = アクセスモード
						（ダミー）

	RETURN:
			ファイルＩＤ
			-1 = エラー終了
***************************************************************/
int	dosaccs_open( char *filename, int access, int file_attr )
{
		short	drive;				/*ドライブ番号*/
		short	skip;
		int		fd;					/*ファイルディスクリプター*/
		char	fname[16];			/*ファイル名*/
		DIR_ENTRY	dir;
		FCB		*fcb;

	    if(strcmp(filename,"stdin")==0){
			root_fcb[STDIN].bpb = (BPB *)-1;
			return( STDIN );
		}
	    if(strcmp(filename,"stdout")==0){
			root_fcb[STDOUT].bpb = (BPB *)-1;
			return( STDOUT );
		}
	    if(strcmp(filename,"stderr")==0){
			root_fcb[STDERR].bpb = (BPB *)-1;
			return( STDERR );
		}

		skip = 0;
		drive = 2;
		if( filename[1]==':' ){
			if( filename[0]>='A' && filename[0]<='Z' )
				drive = (short)(filename[0]-'A');
			if( filename[0]>='a' && filename[0]<='z' )
				drive = (short)(filename[0]-'A');
			skip = 2;
		}

			/*管理エリアの確保*/
		fd = new_fcb( drive );
		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );
		if( fcb->bpb->part==NULL ){
			return -1;
		}

		fcb->write_flag = 0;

		/*ルートからサブディレクトリーまでの検索*/
		if( search_filename( &filename[skip], fcb, &dir, fname )!=0 ){
			free_fcb( fd );
			return( -1 );			/*サブディレクトリー名が無いなど*/
		}

		/*ファイル名検索*/
		if( search_directory( fcb, fname, _A_ARCH, &dir ) ){
				/*ファイルが存在しないときの処理*/
			if( access & (O_CREAT | O_TRUNC | O_EXCL ) ){		/*ファイルの作成*/
				memcpy( dir.filename, fname, 8+3 );
				dir.attribute = _A_ARCH;
				if( append_directory( fcb, &dir ) ){
					free_fcb( fd );
					return( -1 );	/*ファイル作成不可*/
				}
				fcb->write_flag = 1;
			}
			else {
				free_fcb( fd );
				return( -1 );			/*ファイル未存在*/
			}
		}
		else {
				/*ファイルが存在するときの処理*/
			if( access & O_EXCL ){
				free_fcb( fd );
				return( -1 );			/*ファイルが存在する*/
			}
		}

			/*ＦＣＢエリアの設定*/
		memcpy( fcb->filename, dir.filename, 8+3 );

		fcb->w_time = dir.w_time;
		fcb->w_date = dir.w_date;

		fcb->start_cluster = dir.start_cluster;
		fcb->cur_cluster = dir.start_cluster;

		fcb->all_bytes = dir.all_bytes;

		fcb->cur_pos = 0L;

		fcb->open_type = access;

		if( access & O_APPEND ){
			dosaccs_seek( fd, 0L, SEEK_END );
		}
		else {
			dosaccs_seek( fd, 0L, SEEK_SET );
		}

		return( fd );
}

/***************************************************************
	ファイルクローズ処理

	INPUT:
			fd = ファイルＩＤ

	RETURN:
			0 = 正常終了
			-1 = エラー終了
***************************************************************/
int	dosaccs_close( int fd )
{
		FCB		*fcb;
		BPB		*bpb;
		DIR_ENTRY	dir;

		fcb = get_fcb(fd);
		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/
		bpb = fcb->bpb;
		if( bpb==NULL )
			return( -1 );				/*デバイス情報無し*/
		if( bpb==(BPB *)-1 )
			return( -1 );				/*ファイル入出力でない*/

		if( fcb->open_type & O_RDONLY ){
			free_fcb( fd );

			return( 0 );			/*読込み専用のときはなにもしない*/
		}

		if( fcb->write_flag==0 ){
			free_fcb( fd );

			return( 0 );
		}

		memcpy( dir.filename, fcb->filename, 8+3 );
		dir.attribute = _A_ARCH;

		dir.all_bytes = fcb->all_bytes;
		dir.w_time = 0;
		dir.w_date = 0;
		dir.start_cluster = fcb->start_cluster;

		/*ディレクトリーの更新*/
		update_directory( fcb, &dir );

		free_fcb( fd );

		if( flush_cluster() ){
			return( -1 );
		}
		if( flush_fat() ){
			return( -1 );
		}
		if( flush_dir() ){
			return( -1 );
		}

		return( 0 );
}

/***************************************************************
	ファイルリード処理

	INPUT:
			fd = ファイルＩＤ
			buff = 読込みバッファー
			len = 読込みサイズ

	RETURN:
			実読込みサイズ
***************************************************************/
int	dosaccs_read( int fd, unsigned char *buff, int len )
{
		FCB				*fcb;
		BPB				*bpb;
		int				answer_len;
		unsigned short	cluster;
		int				read_len;
		int				copy_pos,remain_bytes;

		fcb = get_fcb(fd);
		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/
		bpb = fcb->bpb;
		if( bpb==NULL )
			return( -1 );				/*デバイス情報無し*/
		if( bpb==(BPB *)-1 )
			return( -1 );				/*ファイル入出力でない*/

		if( fcb->open_type & O_WRONLY ){
			return( -1 );			/*書き込み専用のときはエラー*/
		}
		if( fcb->open_type & O_APPEND ){
			return( -1 );			/*追記のときはエラー*/
		}

		if( fcb->cur_pos >= fcb->all_bytes ){
			return( 0 );			/*ファイルエンプティー*/
		}

		answer_len = 0;
		while( len ){
			if( fcb->cur_pos+len>=fcb->all_bytes ){		/*コピーできるバイト数の計算*/
				len = (int)(fcb->all_bytes - fcb->cur_pos);
			}
			if( len==0 )
				break;

			copy_pos = (int)(fcb->cur_pos % bpb->bytes_cluster);	/*現在のバッファーのコピーするポジション*/
			remain_bytes = (int)(bpb->bytes_cluster - copy_pos);	/*現在のバッファーにコピーできるバイト数*/

			if( copy_pos==0 && fcb->cur_pos!=0 ){	/*バッファーの先頭の場合,すでに使い果たしている場合*/
				cluster = next_cluster( bpb, fcb->cur_cluster );
				if( cluster<2 ){		/*クラスターが存在しない*/
					break;
				}
				/*最後でないクラスターは,次の読み込みのため保存しておく*/
				if( cluster!=0xFFFF )
					fcb->cur_cluster = cluster;
			}

				/*現在のクラスターデータを読み込む*/
			if( cluster_read( bpb, fcb->cur_cluster, 0 ) ){
				return( -1 );
			}

				/*コピーできるバイト数を計算*/
			if( len>remain_bytes )
				read_len = remain_bytes;
			else
				read_len = len;
			memcpy( buff, &cluster_buffer.buffer[copy_pos], read_len );

			buff+=read_len;
			len-=read_len;
			answer_len+=read_len;
			fcb->cur_pos+=read_len;
		}

		return( answer_len );
}

/***************************************************************
	ファイルライト処理

	INPUT:
			fd = ファイルＩＤ
			buff = 書き込みバッファー
			len = 書き込みサイズ

	RETURN:
			実書き込みサイズ
***************************************************************/
int	dosaccs_write( int fd, unsigned char *buff, int len )
{
		FCB				*fcb;
		BPB				*bpb;
		int				answer_len;
		unsigned short	cluster;
		int				write_len;
		int				copy_pos,remain_bytes;

		fcb = get_fcb(fd);
		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/
		bpb = fcb->bpb;
		if( bpb==NULL )
			return( -1 );				/*デバイス情報無し*/
		if( bpb==(BPB *)-1 ){
			for(write_len=0; write_len<len; write_len++){
				charput( *buff++ );
			}
			return len;
		}


		if( fcb->open_type & O_RDONLY ){
			return( -1 );			/*読込み専用のときはエラー*/
		}

		fcb->write_flag=1;

			/*クラスター内の書き込み*/
		answer_len = 0;
		while( len ){
			copy_pos = (int)(fcb->cur_pos % bpb->bytes_cluster);	/*現在のバッファーのコピーするポジション*/
			remain_bytes = (int)(bpb->bytes_cluster - copy_pos);	/*現在のバッファーにコピーできるバイト数*/
				/*コピーできるバイト数を計算*/
			if( len>remain_bytes )
				write_len = remain_bytes;
			else
				write_len = len;
			if( copy_pos==0 ){		/*バッファーの先頭の場合,すでに使い果たしている場合*/
				if( fcb->start_cluster==0 ){	/*最初の書き込み*/
					cluster = empty_cluster( bpb, 2 );
					if( cluster<2 || cluster==0xFFFF ){
						return( -1 );
					}
					if( update_cluster( bpb, cluster, 0xFFFF ) ){
						return( -3 );
					}
					fcb->start_cluster = cluster;
					fcb->cur_cluster = cluster;
					if( new_cluster_read( bpb, cluster, 0 ) ){
						return( answer_len );
					}
				}
				else if( fcb->cur_pos ){
					cluster = next_cluster( bpb, fcb->cur_cluster );	/*新しいクラスターの追加*/
					if( cluster==0xFFFF ){			/*現在最後のクラスターのときは,追加する*/
						cluster = empty_cluster( bpb, fcb->cur_cluster );
						if( cluster<2 || cluster==0xFFFF ){
							return( -1 );
						}
						if( update_cluster( bpb, fcb->cur_cluster, cluster ) ){
							return( -2 );
						}
						if( update_cluster( bpb, cluster, 0xFFFF ) ){
							return( -3 );
						}
						if( new_cluster_read( bpb, cluster, 0 ) ){
							return( answer_len );
						}
					}
					else {
						if( write_len>=(int)bpb->bytes_cluster ){
							if( new_cluster_read( bpb, cluster, 0 ) ){
								return( answer_len );
							}
						}
					}
					fcb->cur_cluster = cluster;
				}
				else {
					fcb->cur_cluster = fcb->start_cluster;
							/*すでにあるﾌｧｲﾙの場合１回目は読み飛ばし*/
				}
			}

				/*現在のクラスターデータを読み込む*/
			if( cluster_read( bpb, fcb->cur_cluster, 0 ) ){
				return( answer_len );
			}

			memcpy( &cluster_buffer.buffer[copy_pos], buff, write_len );

				/*現在のクラスターデータを書込む*/
			cluster_write();

			buff+=write_len;
			len-=write_len;
			answer_len+=write_len;
			fcb->cur_pos+=write_len;
		}

		if( fcb->cur_pos > fcb->all_bytes ){
			fcb->all_bytes = fcb->cur_pos;
		}

		return( answer_len );
}

/***************************************************************
	ファイル削除処理

	INPUT:
			filename = ファイル名
						"C:\\SUB\\FILE.EXT"
	RETURN:
			-1 = エラー終了
***************************************************************/
int	dosaccs_unlink( char *filename )
{
		short			drive;				/*ドライブ番号*/
		short			skip;
		int				fd;				/*ファイルディスクリプター*/
		char			fname[16];			/*ファイル名*/
		DIR_ENTRY		dir;
		FCB				*fcb;

		skip = 0;
		drive = 2;
		if( filename[1]==':' ){
			if( filename[0]>='A' && filename[0]<='Z' )
				drive = (short)(filename[0]-'A');
			if( filename[0]>='a' && filename[0]<='z' )
				drive = (short)(filename[0]-'A');
			skip = 2;
		}

			/*管理エリアの確保*/
		fd = new_fcb( drive );
		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );

		fcb->write_flag = 0;

		/*サブディレクトリーまでの検索*/
		if( search_filename( &filename[skip], fcb, &dir, fname )!=0 ){
			free_fcb( fd );
			return( -1 );			/*サブディレクトリー名が無いなど*/
		}

		/*ファイル名検索*/
		if( search_directory( fcb, fname, _A_ARCH, &dir ) ){
				/*ファイルが存在しないときの処理*/
			free_fcb( fd );
			return( -1 );			/*ファイル未存在*/
		}

		/*ディレクトリー情報の削除*/
		delete_directory( fcb, &dir );

		free_fcb( fd );

		if( flush_cluster() )
			return( -1 );
		if( flush_fat() )
			return( -1 );
		if( flush_dir() )
			return( -1 );

		return( 0 );
}

/***************************************************************
	ディスク残容量

	INPUT:
			drive = 0:A drive
					1:B drive
					2:C drive

	OUTPUT:
			param = ディスク残容量パラメータ

	RETURN:
			-1 = エラー終了
***************************************************************/
int dosaccs_getdiskfree(unsigned drive, struct _diskfree_t *param)
{
		BPB		*bpb;
		unsigned short	cluster;

		bpb = get_bpb( drive );
		if( bpb==NULL ){
			return( -1 );
		}
		if( bpb->part==NULL ){
			return( -1 );
		}

		param->total_clusters = bpb->DATA_cluster_l;
		param->sectors_per_cluster = bpb->sector_cluster;
		param->bytes_per_sector = bpb->bytes_sector;

		param->avail_clusters = 0;
		for( cluster=2; cluster<bpb->DATA_cluster_l+2; cluster++ ){
			if(	next_cluster( bpb, cluster ) )
				param->avail_clusters++;
		}

		return( 0 );
}


/***************************************************************
	ディレクトリー作成処理

	INPUT:
			filename = ディレクトリー名
						"C:\\SUB\\SUB1"

	RETURN:
			ファイルＩＤ
			-1 = エラー終了
***************************************************************/
int	dosaccs_mkdir( char *filename )
{
		short	drive;				/*ドライブ番号*/
		short	skip;
		int		fd;					/*ファイルディスクリプター*/
		char	fname[16];			/*ファイル名*/
		DIR_ENTRY	dir;
		FCB		*fcb;
		unsigned int	cluster,parent_cluster;
		short			offset;

		skip = 0;
		drive = 2;
		if( filename[1]==':' ){
			if( filename[0]>='A' && filename[0]<='Z' )
				drive = (short)(filename[0]-'A');
			if( filename[0]>='a' && filename[0]<='z' )
				drive = (short)(filename[0]-'A');
			skip = 2;
		}

			/*管理エリアの確保*/
		fd = new_fcb( drive );
		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );

		fcb->write_flag = 0;

		/*サブディレクトリーまでの検索*/
		if( search_filename( &filename[skip], fcb, &dir, fname )!=0 ){
			free_fcb( fd );
			return( -1 );			/*サブディレクトリー名が無いなど*/
		}

		/*ファイル名検索*/
		if( fcb->dir_cluster==0 ){
			parent_cluster = 0;		/*親ディレクトリーはルートディレクトリー*/
		}
		else {
			parent_cluster = dir.start_cluster;	/*親ディレクトリーをおぼえておく*/
		}

		if( search_directory( fcb, fname, _A_SUBDIR|_A_ARCH, &dir ) ){
			/*ファイルが存在しないときの処理*/
			memcpy( dir.filename, fname, 8+3 );
			dir.attribute = _A_SUBDIR|_A_ARCH;
			if( append_directory( fcb, &dir ) ){
				free_fcb( fd );
				return( -1 );	/*ファイル作成不可*/
			}
			fcb->write_flag = 1;
		}
		else {
				/*ファイルが存在するときの処理*/
			free_fcb( fd );
			return( -1 );			/*ファイルが存在する*/
		}

		cluster = empty_cluster( fcb->bpb, 2 );
		if( cluster<2 || cluster==0xFFFF ){
			free_fcb( fd );
			return( -1 );
		}
		if( update_cluster( fcb->bpb, cluster, 0xFFFF ) ){
			free_fcb( fd );
			return( -3 );
		}

		if( new_cluster_read( fcb->bpb, cluster, 1 ) ){
			free_fcb( fd );
			return( -4 );
		}

		dir.start_cluster = cluster;

		memset( cluster_buffer.buffer, 0, sizeof( cluster_buffer.buffer ) );

		/*カレントディレクトリーの作成*/
		offset = 0;
		memcpy( &cluster_buffer.buffer[offset], ".          ", 8+3 );
		cluster_buffer.buffer[offset+8+3] = _A_SUBDIR;

		cluster_buffer.buffer[offset+22]=(unsigned char)(dir.w_time & 0xFF);
		cluster_buffer.buffer[offset+23]=(unsigned char)((dir.w_time>>8) & 0xFF);
		cluster_buffer.buffer[offset+24]=(unsigned char)(dir.w_date & 0xFF);
		cluster_buffer.buffer[offset+25]=(unsigned char)((dir.w_date>>8) & 0xFF);

		cluster_buffer.buffer[offset+26]=(unsigned char)(dir.start_cluster & 0xFF);
		cluster_buffer.buffer[offset+27]=(unsigned char)((dir.start_cluster>>8) & 0xFF);

		/*親ディレクトリーの作成*/
		offset = 32;
		memcpy( &cluster_buffer.buffer[offset], "..         ", 8+3 );
		cluster_buffer.buffer[offset+8+3] = _A_SUBDIR;

		cluster_buffer.buffer[offset+22]=(unsigned char)(dir.w_time & 0xFF);
		cluster_buffer.buffer[offset+23]=(unsigned char)((dir.w_time>>8) & 0xFF);
		cluster_buffer.buffer[offset+24]=(unsigned char)(dir.w_date & 0xFF);
		cluster_buffer.buffer[offset+25]=(unsigned char)((dir.w_date>>8) & 0xFF);

		cluster_buffer.buffer[offset+26]=(unsigned char)(parent_cluster & 0xFF);
		cluster_buffer.buffer[offset+27]=(unsigned char)((parent_cluster>>8) & 0xFF);

		cluster_write();

		/*ディレクトリーの更新*/
		update_directory( fcb, &dir );

		free_fcb( fd );

		if( flush_cluster() )
			return( -1 );
		if( flush_fat() )
			return( -1 );
		if( flush_dir() )
			return( -1 );

		return( 0 );
}

/***************************************************************
	ディレクトリー削除処理

	INPUT:
			filename = ディレクトリー名
						"C:\\SUB\\SUB1"
	RETURN:
			-1 = エラー終了
***************************************************************/
int	dosaccs_rmdir( char *filename )
{
		short			drive;				/*ドライブ番号*/
		short			skip;
		int				fd;				/*ファイルディスクリプター*/
		unsigned short	cluster;
		short			offset;
		char			fname[16];			/*ファイル名*/
		DIR_ENTRY		dir;
		FCB				*fcb;

		if( filename[0]=='/' ){
			return(0);
		}

		skip = 0;
		drive = 2;
		if( filename[1]==':' ){
			if( filename[0]>='A' && filename[0]<='Z' )
				drive = (short)(filename[0]-'A');
			if( filename[0]>='a' && filename[0]<='z' )
				drive = (short)(filename[0]-'A');
			skip = 2;
		}

			/*管理エリアの確保*/
		fd = new_fcb( drive );
		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );			/*管理エリアが確保できない*/

		fcb->write_flag = 0;

		/*サブディレクトリーまでの検索*/
		if( search_filename( &filename[skip], fcb, &dir, fname )!=0 ){
			free_fcb( fd );
			return( -1 );			/*サブディレクトリー名が無いなど*/
		}

		/*ファイル名検索*/
		if( search_directory( fcb, fname, _A_SUBDIR, &dir ) ){
				/*ファイルが存在しないときの処理*/
			free_fcb( fd );
			return( -1 );			/*ファイル未存在*/
		}

		/*ディレクトリーに存在するファイルのチェック*/
		offset = 64;
		cluster = dir.start_cluster;
		do {
			if( cluster_read( fcb->bpb, cluster, 1 ) ){
				free_fcb( fd );
				return( -1 );		/*ルートディレクトリー読込みエラー*/
			}
			for( ; offset<(short)fcb->bpb->bytes_cluster; offset+=32 ){
				if( cluster_buffer.buffer[offset+0]!=0 && cluster_buffer.buffer[offset+0]!=0xE5 )
					break;			/*検索終了*/
			}
			if( offset<(short)fcb->bpb->bytes_cluster )
				break;			/*検索終了*/
			offset = 0;
			cluster = next_cluster( fcb->bpb, cluster );
		} while( cluster >= 2 && cluster!=0xFFFF );

		if( (cluster>=2 && cluster!=0xFFFF ) && cluster_buffer.buffer[offset+0]!=0 && cluster_buffer.buffer[offset+0]!=0xE5 ){
			free_fcb( fd );
			return( -1 );			/*ファイルが存在するため削除不可*/
		}

		/*ディレクトリー情報の削除*/
		delete_directory( fcb, &dir );

		free_fcb( fd );

		if( flush_cluster() )
			return( -1 );
		if( flush_fat() )
			return( -1 );
		if( flush_dir() )
			return( -1 );

		return( 0 );
}

/***************************************************************
	ファイル検索処理
		最初に_dos_findfirst() を実行し、
		次からは _dos_findnext() を実行する

	INPUT:
			filename = ディレクトリーとワイルドカード名
						"C:\\SUB\\*.DAT"
			attrib = ファイル属性（通常 0xFF）
						_A_NORMAL
						_A_RDONLY
						_A_HIDDEN
						_A_SYSTEM
						_A_VOLID
						_A_SUBDIR
						_A_ARCH
			result = 検索結果

	RETURN:
			-1 = エラー終了
***************************************************************/
struct Find_RESERVE {	/* reserved の中身 */
	short	drive;				/*ドライブ番号*/
	unsigned short attrib;		/*検索属性*/
	char	filename[8+3+1];	/*検索ファイル名*/
	unsigned short	dir_cluster; /*検索中クラスター番号*/
	unsigned short	dir_entry;	/*検索中エントリー番号*/
};

int dosaccs_findfirst( char *filename, unsigned attrib, struct find_t *result )
{
		short			drive;				/*ドライブ番号*/
		short			skip;
		int				fd;				/*ファイルディスクリプター*/
		char			fname[16];			/*ファイル名*/
		DIR_ENTRY		dir;
		FCB				*fcb;
		struct	Find_RESERVE	*reserve;

		reserve = (struct Find_RESERVE *)result->reserved;
		reserve->drive = -1;		/*検索無効フラグの設定*/

		skip = 0;
		drive = 2;
		if( filename[1]==':' ){
			if( filename[0]>='A' && filename[0]<='Z' )
				drive = (short)(filename[0]-'A');
			if( filename[0]>='a' && filename[0]<='z' )
				drive = (short)(filename[0]-'A');
			skip = 2;
		}

			/*管理エリアの確保*/
		fd = new_fcb( drive );
		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );

		/*サブディレクトリーまでの検索*/
		if( search_filename( &filename[skip], fcb, &dir, fname )!=0 ){
			free_fcb( fd );
			return( -1 );			/*サブディレクトリー名が無いなど*/
		}

		/*ファイル名検索*/
		if( search_directory( fcb, fname, (unsigned char)attrib, &dir ) ){
			free_fcb( fd );
			return( -1 );			/*ファイル未存在*/
		}

		/*検索結果情報の設定*/
		memcpy( result->name, dir.filename, 8 );			/*結果ファイル名*/
		result->name[8]='.';
		memcpy( &result->name[9], &dir.filename[8], 3 );	/*結果エクステンド名*/
		result->attrib = (char)dir.attribute;				/*結果アトリビュート名*/
		result->wr_time = dir.w_time;						/*結果ファイル時刻*/
		result->wr_date = dir.w_date;						/*結果ファイル日付*/
		result->size = dir.all_bytes;						/*結果バイト数*/

		/*次回検索用データの設定*/
		reserve->drive = drive;			/*検索ドライブ番号*/
		reserve->attrib = attrib;		/*検索属性*/
		memcpy( reserve->filename, fname, 8+3 );	/*検索名*/
		reserve->dir_cluster = fcb->dir_cluster;	/*検索中クラスター番号*/
		reserve->dir_entry = fcb->dir_entry;		/*検索中エントリー番号*/

		free_fcb( fd );

		return( fd );
}

int dosaccs_findnext( struct find_t *result )
{
		short			drive;				/*ドライブ番号*/
		int				fd;				/*ファイルディスクリプター*/
		char			fname[16];			/*ファイル名*/
		DIR_ENTRY		dir;
		FCB				*fcb;
		struct	Find_RESERVE	*reserve;
		unsigned		attrib;

		reserve = (struct Find_RESERVE *)result->reserved;
		/*今回検索用データの設定*/
		drive = reserve->drive;		/*検索ドライブ番号*/

			/*管理エリアの確保*/
		fd = new_fcb( drive );
		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );

		/*今回検索用データの設定*/
		attrib = (unsigned)reserve->attrib;			/*検索属性*/
		memcpy( fname, reserve->filename, 8+3 );	/*検索名*/
		fcb->dir_cluster = reserve->dir_cluster;	/*検索中クラスター番号*/
		fcb->dir_entry = reserve->dir_entry;		/*検索中エントリー番号*/

		fcb->dir_entry++;		/*次のエントリーから検索*/
		if( next_search_directory( fcb, fname, (unsigned char)attrib, &dir ) ){
			reserve->drive = -1;
			free_fcb( fd );
			return( -1 );		/*ファイル名無しエラー*/
		}

		/*検索結果情報の設定*/
		memcpy( result->name, dir.filename, 8 );			/*結果ファイル名*/
		result->name[8]='.';
		memcpy( &result->name[9], &dir.filename[8], 3 );	/*結果エクステンド名*/
		result->attrib = (char)dir.attribute;				/*結果アトリビュート名*/
		result->wr_time = dir.w_time;						/*結果ファイル時刻*/
		result->wr_date = dir.w_date;						/*結果ファイル日付*/
		result->size = dir.all_bytes;						/*結果バイト数*/

		/*次回検索用データの設定*/
		reserve->drive = drive;			/*検索ドライブ番号*/
		reserve->attrib = attrib;		/*検索属性*/
		memcpy( reserve->filename, fname, 8+3 );	/*検索名*/
		reserve->dir_cluster = fcb->dir_cluster;	/*検索中クラスター番号*/
		reserve->dir_entry = fcb->dir_entry;		/*検索中エントリー番号*/

		free_fcb( fd );

		return( fd );
}

int dosaccs_dir_read( int fd, char *buff, unsigned short entry )
{
		FCB		*fcb;
		short	offset;

		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );

		if( root_dir_read( fcb->bpb, entry, &offset ) )
			return( -1 );

		memcpy( buff, &rootdir_buffer.buffer[offset], sizeof(DIR_ENTRY) );
		return( 0 );
}

int dosaccs_read_cluster( int fd, unsigned short cluster )
{
		FCB				*fcb;

		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );

		if( cluster_read( fcb->bpb, cluster, 1 ) )
			return( -1 );
		return( 0 );
}

int dosaccs_next_cluster( int fd, unsigned short cluster )
{
		FCB				*fcb;

		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );

		return( next_cluster( fcb->bpb, cluster ) );
}

/***************************************************************
	ファイル名変更処理

	INPUT:
			srcname = 変更前ファイル名
						"C:\\SUB\\FILE.EXT"
			destname = 変更後ファイル名
						"C:\\SUB\\FILE.EXT"
			
	RETURN:
			0 = 正常終了
			-1 = エラー終了
***************************************************************/
int	dosaccs_rename( char *srcname, char *destname )
{
		short	drive;				/*ドライブ番号*/
		short	skip;
		int		fd;				/*ファイルディスクリプター*/
		char	fname_src[16],fname_dest[16];	/*ファイル名*/
		DIR_ENTRY	dir;
		FCB		*fcb;

		/** 変更後のファイルが存在しないことをチェックする **/
		skip = 0;
		drive = 2;
		if( destname[1]==':' ){
			if( destname[0]>='A' && destname[0]<='Z' )
				drive = (short)(destname[0]-'A');
			if( destname[0]>='a' && destname[0]<='z' )
				drive = (short)(destname[0]-'A');
			skip = 2;
		}

			/*管理エリアの確保*/
		fd = new_fcb( drive );
		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );
		if( fcb->bpb->part==NULL ){
			return -1;
		}

		/*ルートからサブディレクトリーまでの検索*/
		if( search_filename( &destname[skip], fcb, &dir, fname_dest )!=0 ){
			free_fcb( fd );
			return( -1 );			/*サブディレクトリー名が無いなど*/
		}

		/*ファイル名検索*/
		if( search_directory( fcb, fname_dest, _A_ARCH, &dir )==0 ){
				/*ファイルが存在するときの処理*/
			free_fcb( fd );
			return( -1 );			/*ファイルが存在する*/
		}
		free_fcb( fd );

		/** 変更前のファイルが存在することをチェックする **/
		skip = 0;
		drive = 2;
		if( srcname[1]==':' ){
			if( srcname[0]>='A' && srcname[0]<='Z' )
				drive = (short)(srcname[0]-'A');
			if( srcname[0]>='a' && srcname[0]<='z' )
				drive = (short)(srcname[0]-'A');
			skip = 2;
		}

			/*管理エリアの確保*/
		fd = new_fcb( drive );
		fcb = get_fcb( fd );
		if( fcb==NULL )
			return( -1 );			/*管理エリアが確保できない*/
		if( fcb->bpb==NULL )
			return( -1 );
		if( fcb->bpb->part==NULL ){
			return -1;
		}

		/*ルートからサブディレクトリーまでの検索*/
		if( search_filename( &srcname[skip], fcb, &dir, fname_src )!=0 ){
			free_fcb( fd );
			return( -1 );			/*サブディレクトリー名が無いなど*/
		}

		/*ファイル名検索*/
		if( search_directory( fcb, fname_src, _A_ARCH, &dir )!=0 ){
				/*ファイルが存在しないときの処理*/
			free_fcb( fd );
			return( -1 );			/*ファイルが存在しない*/
		}

		/* ファイル名を変更 */
		memcpy( dir.filename, fname_dest, 8+3 );

		/*ディレクトリーの更新*/
		update_directory( fcb, &dir );

		free_fcb( fd );

		if( flush_cluster() ){
			return( -1 );
		}
		if( flush_fat() ){
			return( -1 );
		}
		if( flush_dir() ){
			return( -1 );
		}

		return( 0 );
}


//  vector 10 TRAP
void    INT_TRAP3(void) {

		if( dosaccs_target_run ){
			WatchDogReset();
		}

		set_imask_ccr(0);
		switch( dosaccs_function ){
		case 0:		/* Open */
			dosaccs_fd_result = dosaccs_open( dosaccs_filename_buffer, dosaccs_access_len_pos, dosaccs_file_attr_seek );
			break;
		case 1:		/* Close */
			dosaccs_fd_result = dosaccs_close( dosaccs_fd_result );
			break;
		case 2:		/* Read */
			dosaccs_fd_result = dosaccs_read( dosaccs_fd_result, (unsigned char *)dosaccs_filename_buffer, dosaccs_access_len_pos );
			break;
		case 3:		/* Write */
			dosaccs_fd_result = dosaccs_write( dosaccs_fd_result, (unsigned char *)dosaccs_filename_buffer, dosaccs_access_len_pos );
			break;
		case 4:		/* Seek */
			dosaccs_fd_result = dosaccs_seek( dosaccs_fd_result, dosaccs_access_len_pos, dosaccs_file_attr_seek );
			break;
		case 5:		/* Unlink */
			dosaccs_fd_result = dosaccs_unlink( dosaccs_filename_buffer );
			break;
		case 6:		/* diskfree */
			dosaccs_fd_result = dosaccs_getdiskfree( dosaccs_drive, dosaccs_param );
			break;
		case 7:		/* mkdir */
			dosaccs_fd_result = mkdir( dosaccs_filename_buffer );
			break;
		case 8:		/* rmdir */
			dosaccs_fd_result = rmdir( dosaccs_filename_buffer );
			break;
		case 9:		/* find first */
			dosaccs_fd_result = dosaccs_findfirst( dosaccs_filename_buffer, dosaccs_file_attr_seek, dosaccs_result );
			break;
		case 10:	/* find next */
			dosaccs_fd_result = dosaccs_findnext( dosaccs_result );
			break;
		case 11:	/* boot cpu2 */
			WatchDogDisable();
			dosaccs_fd_result = boot2( dosaccs_filename_buffer );
			break;

		case 12:	/* Root Directory Read */
			dosaccs_fd_result = dosaccs_dir_read( dosaccs_fd_result, dosaccs_filename_buffer, dosaccs_access_len_pos );
			break;
		case 13:	/* Cluster Read */
			dosaccs_fd_result = dosaccs_read_cluster( dosaccs_fd_result, dosaccs_access_len_pos );
			break;
		case 14:	/* Next Cluster */
			dosaccs_fd_result = dosaccs_next_cluster( dosaccs_fd_result, dosaccs_access_len_pos );
			break;

		case 15:
			dosaccs_fd_result = mount( dosaccs_drive );
			break;

		case 16:
			dosaccs_fd_result = dosaccs_rename( dosaccs_filename_buffer, dosaccs_filename2 );
			break;
		}
}

