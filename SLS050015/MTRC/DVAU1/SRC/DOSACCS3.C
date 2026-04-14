/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	ファイル制御モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/07/27		K.Honda
																	*
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
	unsigned short	check_date;		/* ブート済み日付 */
	unsigned short	check_time;		/* ブート済み時刻 */
	unsigned long	check_all_bytes;	/* ブート済みサイズ */
#pragma section DOSACCS2
	unsigned short	ide_error;		/* ＰＣカードアクセスエラーフラグ */

	struct _iobuf _iob[IOSTREAM];		/* 標準関数管理エリア */

	unsigned short	now_date;		/*現在日付　ＤＯＳフォーマット*/
	unsigned short	now_time;		/*現在時刻　ＤＯＳフォーマット*/

	PARTITION_INFO	root_part;		/* パーティション情報 */

	BPB	root_bpb[MAX_DRIVE];		/* ＢＰＢ管理エリア */
	FCB	root_fcb[MAX_FCB];		/* ＦＣＢ管理エリア */

	int		dosaccs_function;	/* 呼び出し関数番号 */
	int		dosaccs_fd_result;	/* 結果 */
	char	*dosaccs_filename_buffer;	/* ファイル名・バッファーアドレス */
	int		dosaccs_access_len_pos;	/* バイト数、ポジションデータ */
	int		dosaccs_file_attr_seek;	/* 属性・シーク位置 */
	struct find_t *dosaccs_result;		/* ファイル検索データ */
	unsigned dosaccs_drive;			/* ドライブデータ */
	struct _diskfree_t *dosaccs_param;	/* 空きエリア検索データ */
	char	*dosaccs_filename2;		/* ファイル名変更用 */

	short	dosaccs_target_run;		/* ターゲット実行中フラグ */
#pragma section

const int _nfiles = IOSTREAM;			/* 同時オープン数 */

extern	void dosaccs_trap( void );		/* トラップ関数 */

/************************************************************************

	Function name	<< _INIT_IOLIB >>
					Ｉ／Ｏ関数初期設定
	Syntax			_INIT_IOLIB( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void _INIT_IOLIB(void)
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・標準関数の初期設定
　・stdin,stdout,stderr を設定する
	------------------------------------------------------------------[@@@E]*/
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

/************************************************************************

	Function name	<< _CLOSEALL >>
					Ｉ／Ｏ関数終了
	Syntax			_CLOSEALL( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void _CLOSEALL(void)
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・標準関数の終了設定
　・オープン中のすべてのファイルをクローズする
	------------------------------------------------------------------[@@@E]*/
int i;

	for( i=0; i < _NFILE; i++ )
		if( _iob[i]._ioflag1 & (_IOREAD | _IOWRITE | _IORW ) )
			fclose( & _iob[i] );
}

/************************************************************************

	Function name	<< get_bpb >>
					ＢＰＢ情報の取り出し
	Syntax			bpb = get_bpb( drive );
	Argument		short drive : i : ドライブ番号
				BPB bpb     : o : ＢＰＢ情報のアドレス
							NULL のとき情報なし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
BPB	 *get_bpb( short drive )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・ドライブごとのBPB 情報の取り出しを行う
　・ただし、ドライブ番号は０のみ
　・カードの情報が入っていなければ、mountを呼び出し情報を設定する
	------------------------------------------------------------------[@@@E]*/
		if( drive<0 || drive>=MAX_DRIVE )
			return( NULL );			/* ドライブ番号エラー */

		if( root_bpb[drive].part==NULL ){	/* 未初期化時 */
			dosaccs_drive = drive;
			dosaccs_function = 15;
			dosaccs_trap();			/*カード初期設定*/

			if( dosaccs_fd_result ){
				return NULL;	/*カード初期化エラー時*/
			}
		}

		return( &root_bpb[drive] );
}

/************************************************************************

	Function name	<< new_fcb >>
					新規ＦＣＢの設定
	Syntax			ret = new_fcb( drive );
	Argument		short drive : i : ドライブ番号
				int ret     : o : ＦＣＢ情報の番号
							-1 のとき情報なし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	new_fcb( short drive )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・ドライブごとのＦＣＢ 情報の設定を行う
　・ただし、ドライブ番号は０のみ
	------------------------------------------------------------------[@@@E]*/
		int		i;

		for( i=0; i<MAX_FCB; i++ ){		//空きＦＣＢエリアを検索する
			if( root_fcb[i].bpb == NULL )
				break;
		}
		if( i==MAX_FCB )
			return( -1 );	//空きＦＣＢなし

		root_fcb[i].bpb = get_bpb( drive );	//ＢＰＢ情報を設定

		return( i );
}

/************************************************************************

	Function name	<< get_fcb >>
					ＦＣＢの取り出し
	Syntax			fcb = get_fcb( fd );
	Argument		int fd      : i : fd番号
				FCB fcb     : o : ＦＣＢ情報のアドレス
							NULL のとき情報なし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
FCB	*get_fcb( int fd )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・オープンしているファイルのＦＣＢ 情報の取り出しを行う
	------------------------------------------------------------------[@@@E]*/
		if( fd<0 || fd>=MAX_FCB )
			return( NULL );		// ＦＣＢなし

		return( &root_fcb[fd] );	
}

/************************************************************************

	Function name	<< free_fcb >>
					ＦＣＢの開放
	Syntax			free_fcb( fd );
	Argument		int fd      : i : fd番号
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void	free_fcb( int fd )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・オープンしているファイルのＦＣＢ 情報を開放する
	------------------------------------------------------------------[@@@E]*/
		if( fd<0 || fd>=MAX_FCB )
			return;
		root_fcb[fd].bpb=NULL;	// ＢＰＢ情報と切り離す
}

/************************************************************************

	Function name	<< open >>
					ファイルのオープン
	Syntax			fd = open( filename, access, attr );
	Argument		char *filename : i : ファイル名
				int access     : i : オープンモード
				int attr       : i : ファイル属性
				int fd         : o : オープンしたfd番号
							-1 のときオープンエラー
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	open( char *filename, int access, int file_attr )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・カードのファイルをオープンし、fd番号を返す
　・access は
		O_RDONLY	0 = READ ONLY
		O_WRONLY	1 = WRITE ONLY
		O_RDWR		2 = READ/WRITE mode
		O_APPEND	8 = APPEND mode
		O_CREAT		0x100 = Create open
		O_TRUNC		0x200 = open truncate
		O_EXCL		0x400 = open not already file
		O_TEXT		0x4000 = TEXT mode
		O_BINARY	0x8000 = BINARY mode
　・file_attr は　ダミー（未使用）
	------------------------------------------------------------------[@@@E]*/
		dosaccs_function = 0;
		dosaccs_filename_buffer = filename;
		dosaccs_access_len_pos = access;
		dosaccs_file_attr_seek = file_attr;
		dosaccs_trap();

		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< close >>
					ファイルのクローズ
	Syntax			ret= close( fd );
	Argument		int fd  : i : オープンしたfd番号
				int ret : o : 終了コード
						0 = 正常終了
						-1 = 異常終了
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	close( int fd )
{
		dosaccs_function = 1;
		dosaccs_fd_result = fd;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< read >>
					ファイルのリード
	Syntax			readed= read( fd, buff, len );
	Argument		int fd     : i : オープンしたfd番号
				char *buff : i : 読み込みエリア
				int len    : i : 読み込むバイト数
				int readed : o : 読み込まれたバイト数
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	read( int fd, unsigned char *buff, int len )
{
		dosaccs_function = 2;
		dosaccs_fd_result = fd;
		dosaccs_filename_buffer = (char *)buff;
		dosaccs_access_len_pos = len;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< write >>
					ファイルのライト
	Syntax			writed= writed( fd, buff, len );
	Argument		int fd     : i : オープンしたfd番号
				char *buff : i : 書き込むエリア
				int len    : i : 書き込むバイト数
				int writed : o : 書き込まれたバイト数
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	write( int fd, unsigned char *buff, int len )
{
		dosaccs_function = 3;
		dosaccs_fd_result = fd;
		dosaccs_filename_buffer = (char *)buff;
		dosaccs_access_len_pos = len;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< lseek >>
					ファイル位置設定
	Syntax			ret= lseek( fd, pos, seek );
	Argument		int fd     : i : オープンしたfd番号
				long pos   : i : 移動するバイト数
				int seek   : i : 基点
				long ret   : o : 移動した位置
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
long	lseek( int fd, long pos, int seek )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・ファイルのＲ／Ｗする位置を変更する
　・seek は
	SEEK_CUR  1 = 現在位置
	SEEK_END  2 = 最後
	SEEK_SET  0 = 最初
	------------------------------------------------------------------[@@@E]*/
		dosaccs_function = 4;
		dosaccs_fd_result = fd;
		dosaccs_access_len_pos = pos;
		dosaccs_file_attr_seek = seek;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< unlink >>
					ファイル削除
	Syntax			ret= unlink( filename );
	Argument		char *filename : i : 削除するファイル名
				int ret        : o : 処理結果
							0=正常終了
							-1 = 異常終了
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	unlink( char *filename )
{
		dosaccs_function = 5;
		dosaccs_filename_buffer = filename;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< _dos_getdiskfree >>
					ディスク空き容量
	Syntax			ret= _dos_getdiskfree( drive, param  );
	Argument		int drive                  : i : 取り出すドライブ番号(0のみ）
				struct _disk_free_t param  : o : 処理結果
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int _dos_getdiskfree(unsigned drive, struct _diskfree_t *param)
{
		dosaccs_function = 6;
		dosaccs_drive = drive;
		dosaccs_param = param;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< mkdir >>
					ディレクトリー作成
	Syntax			ret= mkdir( pathname  );
	Argument		char *pathname  : i : 作成するディレクトリー名（フルパス）
				int ret         : o : 処理結果
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	mkdir( char *filename )
{
		dosaccs_function = 7;
		dosaccs_filename_buffer = filename;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< rmdir >>
					ディレクトリー削除
	Syntax			ret= rmdir( pathname  );
	Argument		char *pathname  : i : 削除するディレクトリー名（フルパス）
				int ret         : o : 処理結果
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	rmdir( char *filename )
{
		dosaccs_function = 8;
		dosaccs_filename_buffer = filename;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< _dos_findfirst >>
					最初のファイル検索
	Syntax			ret= _dos_findfirst( filename, attrib, result  );
	Argument		char *filename  : i : 検索するワイルドカード
				unsigned attrib : i : ファイル属性
				struct find_t result : o : 検索結果
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int _dos_findfirst( char *filename, unsigned attrib, struct find_t *result )
{
		dosaccs_function = 9;
		dosaccs_filename_buffer = filename;
		dosaccs_file_attr_seek = attrib;
		dosaccs_result = result;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< _dos_findnext >>
					次のファイル検索
	Syntax			ret= _dos_findnext( result  );
	Argument		struct find_t result : i/o : 検索結果
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int _dos_findnext( struct find_t *result )
{
		dosaccs_function = 10;
		dosaccs_result = result;
		dosaccs_trap();
		return dosaccs_fd_result;
}

/************************************************************************

	Function name	<< boot_cpu2 >>
					サブＣＰＵのブート処理
	Syntax			ret= boot_cpu2( filename );
	Argument		char *filename  : i : ブートするプログラムファイル名
				int ret         : o : 処理結果
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
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

/************************************************************************

	Function name	<< rename >>
					ファイル名変更
	Syntax			ret= rename( oldname, newname );
	Argument		char *oldname  : i : 変更前のファイル名
				char *newname  : i : 変更後のファイル名
				int ret         : o : 処理結果
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
int	rename( char *srcname, char *destname )
{
		dosaccs_filename_buffer = (char *)srcname;
		dosaccs_filename2 = (char *)destname;
		dosaccs_function = 16;
		dosaccs_trap();
		return dosaccs_fd_result;
}

