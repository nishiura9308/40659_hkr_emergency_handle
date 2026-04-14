/************************************************************************
	ＭＳ－ＤＯＳフォーマットファイルアクセス
		ハードディスクアクセス　データ構造定義
*************************************************************************/

#define	SECTOR_BYTES	512				/*セクターのバイト数（物理レベル）*/
#define	SECTOR_CLUSTER	16				/*セクター／クラスター*/

typedef	struct	{	/*パーティション情報*/
	char			boot_flag;		/*ブートインディケーター
										b7: on ... BOOT 可能 */
	char			system_id;		/*システムインディケーター*/

	unsigned short	start_head;		/*開始ヘッド*/
	unsigned short	start_sector;	/*開始セクター*/
	unsigned short	start_cylinder;	/*開始シリンダー*/
	unsigned short	end_head;		/*終了ヘッド*/
	unsigned short	end_sector;		/*終了セクター*/
	unsigned short	end_cylinder;	/*終了シリンダー*/

	long			boot_sector;	/*ブートセクター位置*/
	long			all_sector;		/*アクセス可能セクター総数*/
	unsigned short	sector_track;	/*セクター／トラック*/
	unsigned short	head_number;	/*ヘッド数*/
	unsigned short	cylinder_vol;	/*シリンダー／ボリューム*/

	int	(*driver_read)(long sector, short num, unsigned char *buff );		/*物理読み込みアクセスルーティン*/
	int	(*driver_write)(long sector, short num, unsigned char *buff );		/*物理書き込みアクセスルーティン*/
	int	(*driver_status)( void );																	/*物理ステータス取り出しルーティン*/
} PARTITION_INFO;

typedef	struct	{	/*ブロックデバイスパラメータ*/
	PARTITION_INFO	*part;			/*パーティション情報*/

	unsigned short	bytes_sector;	/*バイト数／セクター*/
	unsigned short	sector_cluster;	/*セクター／クラスター*/
	unsigned short	reserved_sector;/*予約セクター数*/
	unsigned short	FAT_number;		/*ＦＡＴ数*/
	unsigned short	root_entry;		/*ルートディレクトリーエントリー数*/
	unsigned short	all_sector;		/*総セクター数  ==0 のときは all_sector2 */

/**	unsigned char	media_desc;		*メディアディスクリプター*/

	unsigned short	sector_FAT;		/*セクター／ＦＡＴ*/
	unsigned short	sector_track;	/*セクター／トラック*/
	unsigned short	head_number;	/*ヘッド数*/
	unsigned long	skip_sector;	/*隠しセクター*/
	unsigned long	all_sector2;	/*総セクター数*/

	unsigned short	FAT_type;		/*ＦＡＴタイプ*/

	unsigned short	bytes_cluster;	/*バイト数／クラスター*/
	unsigned short	drive;			/*ドライブ番号*/
	long			FAT_sector_s;	/*ＦＡＴ開始セクター番号*/
	long			FAT_sector_l;	/*第一ＦＡＴ占有セクター数*/
	long			DIR_sector_s;	/*ディレクトリー開始セクター番号*/
	long			DIR_sector_l;	/*ディレクトリー占有セクター数*/
	long			DATA_sector_s;	/*データ開始セクター番号*/
	long			DATA_sector_l;	/*データ占有セクター数*/
	unsigned short	DATA_cluster_l;	/*データ占有クラスター数*/
} BPB;

typedef	struct	{	/*ファイルコントロールブロック*/
	unsigned char	filename[8+3];
	unsigned short	w_time;		/*ファイル時刻*/
	unsigned short	w_date;		/*ファイル日付*/

	unsigned short	start_cluster;	/*開始クラスター番号*/

	long			all_bytes;		/*総バイト数*/
	long			cur_pos;		/*カレントファイル位置（バイト単位）*/
	unsigned short	cur_cluster;	/*カレントクラスター番号*/

	unsigned short	dir_cluster;	/*ディレクトリークラスター番号*/
	unsigned short	dir_entry;		/*ディレクトリーエントリー番号*/
	unsigned short	open_type;		/*オープンタイプ*/
	unsigned short	write_flag;		/*書き込みフラグ*/
	BPB				*bpb;			/*デバイス情報*/
} FCB;

#define	MAX_DRIVE	1
#define	MAX_FCB		5
#define	FAT16		0x04
#define	FAT12		0x01

typedef	struct	{	/*ディレクトリーエントリー情報*/
	unsigned char	filename[8+3];
	unsigned char	attribute;
	unsigned char	reserved[10];
	unsigned short	w_time;
	unsigned short	w_date;
	unsigned short	start_cluster;
	long			all_bytes;
} DIR_ENTRY;

typedef struct	{	/*ルートディレクトリー情報（１セクター毎）*/
	BPB				*bpb;				/*ＢＰＢ情報*/
	unsigned short	update;				/*更新フラグ*/
	long			sector_no;			/*セクター番号*/
	unsigned char	buffer[1024];		/*バッファー*/
} ROOTDIR_BUFFER;

typedef struct	{	/*ＦＡＴ情報（１セクターまたは２セクター毎）*/
	BPB				*bpb;				/*ＢＰＢ情報*/
	unsigned short	update;				/*更新フラグ*/
	long			sector_no;			/*セクター番号*/
	short			sector_len;			/*セクター数*/
	unsigned char	buffer[2048];		/*バッファー*/
} FAT_BUFFER;

typedef struct	{	/*クラスター情報（１クラスター毎）*/
	BPB				*bpb;				/*ＢＰＢ情報*/
	unsigned short	update;				/*更新フラグ*/
	unsigned short	subdir;				/*ｸﾗｽﾀｰ･ﾃﾞｨﾚｸﾄﾘｰ情報ﾌﾗｸﾞ*/
	unsigned short	cluster_no;			/*クラスター番号*/
	long			sector_no;			/*セクター番号*/
	unsigned char	buffer[SECTOR_BYTES*SECTOR_CLUSTER];	/*バッファー*/
} CLUSTER_BUFFER;

extern	unsigned short	now_date;		/*現在日付　ＤＯＳフォーマット*/
extern	unsigned short	now_time;		/*現在時刻　ＤＯＳフォーマット*/
extern	ROOTDIR_BUFFER	rootdir_buffer;		/*ルート・ディレクトリー情報*/
extern	FAT_BUFFER		fat_buffer;			/*ＦＡＴ情報*/
extern	CLUSTER_BUFFER	cluster_buffer;		/*クラスター情報*/

		/*論理セクター番号でアクセスする関数*/
int mount( short drive );
int dismount( short drive );
int	sector_read( BPB *bpb, long sector, short num, unsigned char *buff );
int	sector_write( BPB *bpb, long sector, short num, unsigned char *buff );
int	sector_status( BPB *bpb );

#define	SECTOR_OK		0
#define	SECTOR_NOINIT	0x8000		/*未初期化*/
#define	SECTOR_OVER		0x4000		/*セクターオーバー*/
#define	SECTOR_NG		0x2000		/*読込み書き込み異常*/
#define	SECTOR_PROTECT	0x0001		/*書き込みプロテクト*/

		/*クラスターバッファーを使用する関数*/
void		purge_cluster( void );
int 		flush_dir( void );
int 		flush_fat( void );
int 		flush_cluster( void );
int			root_dir_read( BPB *bpb, unsigned short entry, short *offset );
void		root_dir_write( void );
int			fat_read( BPB *bpb, unsigned short cluster, short *offset );
void		fat_write( void );
int			cluster_read( BPB *bpb, unsigned short cluster, unsigned short subdir );
int			new_cluster_read( BPB *bpb, unsigned short cluster, unsigned short subdir );
void		cluster_write( void );
unsigned short	next_cluster( BPB *bpb, unsigned short cluster );
unsigned short	empty_cluster( BPB *bpb, unsigned short now_cluster );
int				update_cluster( BPB *bpb, unsigned short cluster, unsigned short next_no );


		/*ＦＣＢ（ファイルコントロールブロック）を操作する関数*/
BPB	 *get_bpb( short drive );
int	 new_fcb( short drive );
FCB	 *get_fcb( int fd );
void free_fcb( int fd );

		/*ディレクトリー情報を操作する関数*/
int	next_search_directory( FCB *fcb, char *fname, unsigned char attribute, DIR_ENTRY *dir );
int	search_directory( FCB *fcb, char *fname, unsigned char attribute, DIR_ENTRY *dir );
int	append_directory( FCB *fcb, DIR_ENTRY *dir );
int	update_directory( FCB *fcb, DIR_ENTRY *dir );
int	delete_directory( FCB *fcb, DIR_ENTRY *dir );

