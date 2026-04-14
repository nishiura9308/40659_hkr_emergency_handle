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

#include "dosaccs.h"
#include "fcntl.h"

#define IOSTREAM MAX_FCB

#pragma section DOSACCS
	char	dosaccs_workbuff[512];		/*ストリームバッファー*/

	ROOTDIR_BUFFER	rootdir_buffer;		/*ルート・ディレクトリー情報*/
	FAT_BUFFER		fat_buffer;			/*ＦＡＴ情報*/
	CLUSTER_BUFFER	cluster_buffer;		/*クラスター情報*/

	// ブートチェックエリア
	unsigned short	check_date;
	unsigned short	check_time;
	unsigned long	check_all_bytes;
#pragma section DOSACCS2
	unsigned short	ide_error;

	struct _iobuf _iob[IOSTREAM];

	unsigned short	now_date;		/*現在日付　ＤＯＳフォーマット*/
	unsigned short	now_time;		/*現在時刻　ＤＯＳフォーマット*/

	PARTITION_INFO	root_part;

	BPB	root_bpb[MAX_DRIVE];
	FCB	root_fcb[MAX_FCB];

	int		dosaccs_function;
	int		dosaccs_fd_result;
	char	*dosaccs_filename_buffer;
	int		dosaccs_access_len_pos;
	int		dosaccs_file_attr_seek;
	struct find_t *dosaccs_result;
	unsigned dosaccs_drive;
	struct _diskfree_t *dosaccs_param;
	char	*dosaccs_filename2;

	short	dosaccs_target_run;
#pragma section

const int _nfiles = IOSTREAM;

extern	void dosaccs_trap( void );

/****************************************************************************/
/* _INIT_IOLIB                                                              */
/****************************************************************************/
void _INIT_IOLIB(void)
{
FILE *fp;

	dosaccs_target_run = 1;			// アプリケーション用

	memset( (char *)root_bpb, 0, sizeof(root_bpb) );
	memset( (char *)root_fcb, 0, sizeof(root_fcb) );

	memset( &rootdir_buffer, 0, sizeof(rootdir_buffer) );
	memset( &fat_buffer, 0, sizeof(fat_buffer) );
	memset( &cluster_buffer, 0, sizeof(cluster_buffer) );

	for( fp = _iob; fp < _iob + _NFILE; fp++ )
	 {
		fp->_bufptr = NULL;
		fp->_bufcnt = 0;
		fp->_buflen = 0;
		fp->_bufbase = NULL;
		fp->_ioflag1 = 0;
		fp->_ioflag2 = 0;
		fp->_iofd = 0;
	 }

	freopen( "stdin", "r", stdin );
	stdin->_ioflag1 |= _IOUNBUF;
	freopen( "stdout", "w", stdout );
	stdout->_ioflag1 |= _IOUNBUF;
	freopen( "stderr", "w", stderr );
	stderr->_ioflag1 |= _IOUNBUF;
}

/****************************************************************************/
/* _CLOSEALL                                                                */
/****************************************************************************/

void _CLOSEALL(void)
{
int i;

	for( i=0; i < _NFILE; i++ )
		if( _iob[i]._ioflag1 & (_IOREAD | _IOWRITE | _IORW ) )
			fclose( & _iob[i] );
}

/***************************************************************
	ＢＰＢ（デバイス情報）の決定

	INPUT:
		drive = ドライブ番号

	RETURN:
		ＢＰＢアドレス
		NULL = ＢＰＢ無し
***************************************************************/
BPB	 *get_bpb( short drive )
{
		if( drive<0 || drive>=MAX_DRIVE )
			return( NULL );

		if( root_bpb[drive].part==NULL ){
			dosaccs_drive = drive;
			dosaccs_function = 15;
			dosaccs_trap();

			if( dosaccs_fd_result ){
				return NULL;
			}
		}

		return( &root_bpb[drive] );
}

/***************************************************************
	新規ＦＣＢの決定

	INPUT:
		drive = ドライブ番号

	RETURN:
			ファイルＩＤ
			-1 = ＦＣＢ無し
***************************************************************/
int	new_fcb( short drive )
{
		int		i;

		for( i=0; i<MAX_FCB; i++ ){
			if( root_fcb[i].bpb == NULL )
				break;
		}
		if( i==MAX_FCB )
			return( -1 );

		root_fcb[i].bpb = get_bpb( drive );

		return( i );
}

/***************************************************************
	ＦＣＢアドレスの取り出し

	INPUT:
			fd = ファイルＩＤ
	RETURN:
			ＦＣＢエリアアドレス
			NULL = ＦＣＢ無し
			ファイルＩＤ
			-1 = ＦＣＢ無し
***************************************************************/
FCB	*get_fcb( int fd )
{
		if( fd<0 || fd>=MAX_FCB )
			return( NULL );

		return( &root_fcb[fd] );
}

/***************************************************************
	ＦＣＢの開放

	INPUT:
			fd = ファイルＩＤ
***************************************************************/
void	free_fcb( int fd )
{
		if( fd<0 || fd>=MAX_FCB )
			return;
		root_fcb[fd].bpb=NULL;
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
int	open( char *filename, int access, int file_attr )
{
		dosaccs_function = 0;
		dosaccs_filename_buffer = filename;
		dosaccs_access_len_pos = access;
		dosaccs_file_attr_seek = file_attr;
		dosaccs_trap();

		return dosaccs_fd_result;
}

/***************************************************************
	ファイルクローズ処理

	INPUT:
			fd = ファイルＩＤ

	RETURN:
			0 = 正常終了
			-1 = エラー終了
***************************************************************/
int	close( int fd )
{
		dosaccs_function = 1;
		dosaccs_fd_result = fd;
		dosaccs_trap();
		return dosaccs_fd_result;
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
int	read( int fd, unsigned char *buff, int len )
{
		dosaccs_function = 2;
		dosaccs_fd_result = fd;
		dosaccs_filename_buffer = (char *)buff;
		dosaccs_access_len_pos = len;
		dosaccs_trap();
		return dosaccs_fd_result;
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
int	write( int fd, unsigned char *buff, int len )
{
		dosaccs_function = 3;
		dosaccs_fd_result = fd;
		dosaccs_filename_buffer = (char *)buff;
		dosaccs_access_len_pos = len;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/***************************************************************
	ファイル位置変更処理

	INPUT:
			fd = ファイルＩＤ
			pos = 位置
			seek = 基点
				SEEK_CUR  1 = 現在位置
				SEEK_END  2 = 最後
				SEEK_SET  0 = 最初

	RETURN:
			実読込みサイズ
***************************************************************/
long	lseek( int fd, long pos, int seek )
{
		dosaccs_function = 4;
		dosaccs_fd_result = fd;
		dosaccs_access_len_pos = pos;
		dosaccs_file_attr_seek = seek;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/***************************************************************
	ファイル削除処理

	INPUT:
			filename = ファイル名
						"C:\\SUB\\FILE.EXT"
	RETURN:
			-1 = エラー終了
***************************************************************/
int	unlink( char *filename )
{
		dosaccs_function = 5;
		dosaccs_filename_buffer = filename;
		dosaccs_trap();
		return dosaccs_fd_result;
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
int _dos_getdiskfree(unsigned drive, struct _diskfree_t *param)
{
		dosaccs_function = 6;
		dosaccs_drive = drive;
		dosaccs_param = param;
		dosaccs_trap();
		return dosaccs_fd_result;
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
int	mkdir( char *filename )
{
		dosaccs_function = 7;
		dosaccs_filename_buffer = filename;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/***************************************************************
	ディレクトリー削除処理

	INPUT:
			filename = ディレクトリー名
						"C:\\SUB\\SUB1"
	RETURN:
			-1 = エラー終了
***************************************************************/
int	rmdir( char *filename )
{
		dosaccs_function = 8;
		dosaccs_filename_buffer = filename;
		dosaccs_trap();
		return dosaccs_fd_result;
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
int _dos_findfirst( char *filename, unsigned attrib, struct find_t *result )
{
		dosaccs_function = 9;
		dosaccs_filename_buffer = filename;
		dosaccs_file_attr_seek = attrib;
		dosaccs_result = result;
		dosaccs_trap();
		return dosaccs_fd_result;
}

int _dos_findnext( struct find_t *result )
{
		dosaccs_function = 10;
		dosaccs_result = result;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/*************************************************************
		下位ＣＰＵへブートプログラムを出力する
**************************************************************/

int boot_cpu2( char *filename )
{
		dosaccs_filename_buffer = filename;
		dosaccs_function = 11;
		dosaccs_trap();
		return dosaccs_fd_result;
}

int _dos_read_rootdir( int fd, DIR_ENTRY *buff, unsigned short entry )
{
		dosaccs_fd_result = fd;
		dosaccs_filename_buffer = (char *)buff;
		dosaccs_access_len_pos = entry;
		dosaccs_function = 12;
		dosaccs_trap();
		return dosaccs_fd_result;
}

int _dos_read_cluster( int fd, unsigned short cluster )
{
		dosaccs_fd_result = fd;
		dosaccs_access_len_pos = cluster;
		dosaccs_function = 13;
		dosaccs_trap();
		return dosaccs_fd_result;
}

int _dos_next_cluster( int fd, unsigned short cluster )
{
		dosaccs_fd_result = fd;
		dosaccs_access_len_pos = cluster;
		dosaccs_function = 14;
		dosaccs_trap();
		return dosaccs_fd_result;
}

int	_dos_card_ok( void )
{
		if( root_bpb[0].part==NULL ){
			return( 0 );
		}
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
int	rename( char *srcname, char *destname )
{
		dosaccs_filename_buffer = (char *)srcname;
		dosaccs_filename2 = (char *)destname;
		dosaccs_function = 16;
		dosaccs_trap();
		return dosaccs_fd_result;
}

