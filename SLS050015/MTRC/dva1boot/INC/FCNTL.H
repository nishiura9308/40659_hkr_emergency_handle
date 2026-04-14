
#ifndef	NULL
#define	NULL		((void *)0)
#endif

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define O_RDONLY	0x0001	/* open for reading only */
#define O_WRONLY	0x0002	/* open for writing only */
#define O_RDWR		0x0004	/* open for reading and writing */
#define O_APPEND	0x0020	/* writes done at eof */

#define O_CREAT 	0x0008	/* create and open file */
#define O_TRUNC 	0x0010	/* open and truncate */
#define O_EXCL		0x0000	/* open only if file doesn't already exist */

#define O_TEXT		0x0000	/* file mode is text (translated) */
#define O_BINARY	0x0080	/* file mode is binary (untranslated) */

#define _A_NORMAL	0x00	/* Normal file - No read/write restrictions */
#define _A_RDONLY	0x01	/* Read only file */
#define _A_HIDDEN	0x02	/* Hidden file */
#define _A_SYSTEM	0x04	/* System file */
#define _A_VOLID	0x08	/* Volume ID file */
#define _A_SUBDIR	0x10	/* Subdirectory */
#define _A_ARCH 	0x20	/* Archive file */

/*******************************************************************
	ディスクの接続処理
*******************************************************************/
int mount( short drive );

/*******************************************************************
	ディスクの切断処理
*******************************************************************/
int dismount( short drive );

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
long	lseek( int fd, long pos, int flag );

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
int	open( char *filename, int access, int file_attr );

/***************************************************************
	ファイルリード処理

	INPUT:
			fd = ファイルＩＤ
			buff = 読込みバッファー
			len = 読込みサイズ

	RETURN:
			実読込みサイズ
***************************************************************/
int	read( int fd, unsigned char *buff, int len );

/***************************************************************
	ファイルライト処理

	INPUT:
			fd = ファイルＩＤ
			buff = 書き込みバッファー
			len = 書き込みサイズ

	RETURN:
			実書き込みサイズ
***************************************************************/
int	write( int fd, unsigned char *buff, int len );

/***************************************************************
	ファイルクローズ処理

	INPUT:
			fd = ファイルＩＤ

	RETURN:
			0 = 正常終了
			-1 = エラー終了
***************************************************************/
int	close( int fd );

/***************************************************************
	ファイル削除処理

	INPUT:
			filename = ファイル名
						"C:\\SUB\\FILE.EXT"

	RETURN:
			-1 = エラー終了
***************************************************************/
int	unlink( char *filename );

struct _diskfree_t {
	unsigned int total_clusters;			/*全クラスター数*/
	unsigned int avail_clusters;			/*使用中クラスター数*/
	unsigned short sectors_per_cluster;		/*１クラスター中のセクター数*/
	unsigned short bytes_per_sector;		/*Ⅰセクターのバイト数*/
};

#define diskfree_t _diskfree_t

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
int _dos_getdiskfree(unsigned drive, struct _diskfree_t *param);

/***************************************************************
	ディレクトリー作成処理

	INPUT:
			filename = ディレクトリー名
						"C:\\SUB\\SUB1"

	RETURN:
			ファイルＩＤ
			-1 = エラー終了
***************************************************************/
int	mkdir( char *filename );

/***************************************************************
	ディレクトリー削除処理

	INPUT:
			filename = ディレクトリー名
						"C:\\SUB\\SUB1"
	RETURN:
			-1 = エラー終了
***************************************************************/
int	rmdir( char *filename );

struct find_t {
	char reserved[21];
	char attrib;
	unsigned wr_time;
	unsigned wr_date;
	long size;
	char name[13];
	};

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
int _dos_findfirst( char *filename, unsigned attrib, struct find_t *result );

int _dos_findnext( struct find_t *result );

int boot_cpu2( char *filename );
int _dos_read_rootdir( int fd, DIR_ENTRY *buff, unsigned short entry );
int _dos_read_cluster( int fd, unsigned short cluster );
int _dos_next_cluster( int fd, unsigned short cluster );

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
int	rename( char *srcname, char *destname );


