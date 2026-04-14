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

#include    <machine.h>
#include    "vect.h"
#include    "iodefine.h"
#include    "atacard.h"

#include "dosaccs.h"
#include "fcntl.h"

extern	ROOTDIR_BUFFER	rootdir_buffer;		/*ルート・ディレクトリー情報*/
extern	FAT_BUFFER		fat_buffer;			/*ＦＡＴ情報*/
extern	CLUSTER_BUFFER	cluster_buffer;		/*クラスター情報*/

extern	BPB	root_bpb[MAX_DRIVE];
extern	FCB	root_fcb[MAX_FCB];

#define	MAX_RETRY	400000L

/*******************************************************************
	このファイルには物理アクセスルーティンを
	いれている
	論理アクセス部は、パーティション情報を元にアクセスする
*******************************************************************/

/***************************************************************************

	CE2#	CE1#	A0		Offset	Data Bus
*	 L		 L		L		0H,8H	D15-D0		Word Data
*	 L		 L		H		1H,9H	D15-D0		Word Data
	 H		 L		L		0H,8H	D7-D0		Even Byte Data
	 H		 L		H		9H		D7-D0		Odd Byte Data
*	 L		 H		X		8H,9H	D15-D8		Odd Byte Data
	 H		 L		H		1H,0Dh	D7-D0		Error/Feature 
*	 L		 H		X		0H,1H	D15-D8		Error/Feature
*	 L		 L		X		0CH,0DH	D15-D8		Error/Feature

			* = This Board is Not used

	I/O Mapped Addressing

				Primary	Secondary	Contiguous	IORD#=0			IOWR#=0
		REG#	A9::0	A9::0			A3::0
		 L		1F0h	170h			0h		Even Read Data	Even Write Data
		 L		1F1h	171h			1h		Error Register	Feature
		 L		1F2h	172h			2h		Sector Count	Sector Count
		 L		1F3h	173h			3h		Sector Number	Sector Number
		 L		1F4h	174h			4h		Cylinder Low	Cylinder Low
		 L		1F5h	175h			5h		Cylinder High	Cylinder High
		 L		1F6h	176h			6h		Drive/Head		Drive/Head
		 L		1F7h	177h			7h		Status			Command
		 L		---		---				8h		Duplicate		Duplicate
		 										Even Read Data	Even Write Data
		 L		---		---				9h		Duplicate		Duplicate
		 										Odd Read Data	Odd Write Data
		 L		---		---				0Dh		Duplicate Error	Duplicate Feature
		 L		3F6h	376h			0Eh		Alternate Status Device Control
		 L		3F7h	377h			0Fh		Drive Address	Reserved


	Memory Mapped
		REG#	A10	A9::4	A3	A2	A1	A0		OE#=0			WE#=0
		 H		 L	   X	L	L	L	L		Read Data		Write Data
		 H		 L	   X	L	L	L	H		Error			Feature
		 H		 L	   X	L	L	H	L		Sector Count	Sector Count
		 H		 L	   X	L	L	H	H		Sector Number	Sector Number
		 H		 L	   X	L	H	L	L		Cylinder Low	Cylynder Low
		 H		 L	   X	L	H	L	H		Cylinder High	Cylynder High
		 H		 L	   X	L	H	H	L		Drive/Head		Drive/Head
		 H		 L	   X	L	H	H	H		Status			Command
		 H		 L	   X	H	L	L	L		Duplicate Even	Duplicate Even
		 										Read Data		Write Data
		 H		 L	   X	H	L	L	H		Duplicate Odd	Duplicate Odd
		 										Read Data		Write Data
		 H		 L	   X	H	L	H	L		
		 H		 L	   X	H	L	H	H		
		 H		 L	   X	H	H	L	L
		 H		 L	   X	H	H	L	H
		 H		 L	   X	H	H	H	L		Alt Status		Device Ctl
		 H		 L	   X	H	H	H	H		Drive Address	Reserved

		 H		 H	   X	X	X	X	L		Even Read Data	Even Write Data
		 H		 H	   X	X	X	X	H		Odd Read Data	Odd Write Data



	Command Block Cylinder-Head-Sector Addressing

		+1	Feautre
		+2	Sector Count
		+3	Sector Number
		+4	Cylinder Low
		+5	Cylinder High
		+6	101<Drv><Head>
		+7	Command

	Command Block Logical Block Addressing
	
		+1	Feature
		+2	Sector Count
		+3	LBN A7-A0
		+4	LBN A15-A8
		+5	LBN	A23-A16
		+6	111<Drv><LBN A27-A24>
		+7	Command
		
		LBA = ( C*HpC + H ) * SpH + S-1		: Logical Block Address
		C = LBA/( HpC * SpH )				: Cylinder Number
		H = (LBA/SpH) mod HpC				: Head Number
		S = (LBA mod SpH)+1					: Sector Number
									HpC	: Heads per Cylinder
									SpH	: Sectors per Head(Track)


	Drive/Head Register
		b7   b6   b5   b4   b3   b2   b1   b0
		 1   LBA   1   DRV  --- HEAD/LBA ----
		 		LBA = 1..LBA mode, 0..CHS mode

	Status And Alternate Status
		b7   b6   b5   b4   b3   b2   b1   b0
		BSY  DRDY DWF  DSC  DRQ  CORR IDX  ERR

			IDX = ANSI ATA Standard
			DWF = Drive Write Failute

	Device Control Register
		b7   b6   b5   b4   b3   b2   b1   b0
		--   --   --   --   --  SRST nlEN  --

			nlEN = I/O interface mode only, negated unless enable
			SRST = Software Reset Bit

	Drive Address Register
		b7   b6   b5   b4   b3   b2   b1   b0
		--  nWTG nHS3 nHS2 nHS1 nHS0 nDS1 nDS0

			nWTG = 0... While a write operation is in progress.


*************************************************************************/

/************************************************************************
	DISK Data structure

		offset	size		description
		000h	446			Boot code
		1BEh	16			Partition Entry
		1CEh	16			Partition Entry
		1DEh	16			Partition Entry
		1EEh	16			Partition Entry
		1FEh	2			Signature 0x55AA


	Paritition Entry

		offset	size		description
		00h		1			X86 default boot parition
							00h=Not Default boot partition
							80h = Default boot partition
		01h		1			StartHead Zero-based head number
		02h		1			Start Sector bits0...5 = one-based sector number
										 bits6...7 = zero based cylinder number
		03h		1			Start Cylinder zero-based cylinder number
		04h		1			Partition type
								00h:Unknown or delted if num sector is zero
								01h:MS-DOS 12 bit BPB/FAT < 16MB
								04h:MS-DOS 16 bit BPB/FAT < 32MB
								05h:Extended MS-DOS Partition
								06h:MS-DOS 16 bit BPB/FAT >= 32MB
		05h		1			End Head Zero-based head number
		06h		1			End Sector   bits0...5 = one-based sector number
											 6...7 = zero-based cylinder number
		07h		1			End Cylinder zero-based cylinder number
		08h		4			Start Sector(relative to beginning of media or extended MS-DOS Partition )
		0Ch		4			NumSectors


	Cluster Code in FAT
		(0)000h				Available or unallocated cluster
		(0)001h				Reserved, do not use this
		(0)002h..(F)FF6h	Next cluster 
		(F)FF7h				Bad Cluster
		(F)FF8h..(F)FFFh	last cluster

	BPB Record

		offset	size		description
		000h	3			JMP Instruction
		003h	8			OEM Name and version
		00Bh	25			BIOS Parameter Block
		024h	1			Drive Number (00h=Floppy, 80h=Fixed)
		025h	1			Reserved 
		026h	1			ExtBootSignature 0x29
		027h	4			Volume ID or Serial Number
		02Bh	11			Volume Label  Ascii Characters(Padded Space)
		036h	8			File Sys Type (Padded Space)
								'FAT12   '		12bit FAT File Allocation
								'FAT16   '		16bit FAT File Allocation
		03Eh	448			Boot Code
		1FEh	2			Signature 0x55AA


	BIOS Parameter Block

		offset	size		description
		000h	2			Bytes Per Sector
		002h	1			Sectors Per Cluster
		003h	2			Reserved Sectors
		005h	1			NumFATs
		006h	2			RootDirEntries
		008h	2			TotalSectors
								0 is Huge Total Sectors field
		00Ah	1			MediaID Byte
								0F0h	Various types of media
								0F8h	Hard Disk
								0F9h	720K 3.5' or 1.2M 5.25'
								0FAh	320K 5.25'
								0FBh	640K 3.5'
								0FCh	180K 5.25'
								0FDh	360K 5.25'
								0FEh	160K 5.25'
								0FFh	320K 5.25'
		00Bh	2			NumFATSectors
		00Dh	2			Sectors per Track
		00Fh	2			NumHeads
		011h	4			Hidden Sectors
		015h	4			Huge Sectors 

***************************************************************************/

extern	unsigned short	ide_error;
extern	short	dosaccs_target_run;

/***************************************************************
		割り込み処理
			IDE_STATUS
				b7 : BUSY
				b6 : Drive Ready
				b5 : Drive Write Fault
				b4 : Drive Seek Complete
				b3 : Data Request
				b2 : Corrected Data(Recovered Error)
				b1 : 0
				b0 : Error
			IDE_ERROR
				b7 : Bad Block Detected
				b6 : Uncorrectable Data Error
				b5 : 0
				b4 : ID Not Found
				b3 : 0
				b2 : Aborted Command
				b1 : 0
				b0 : Address Mark Not Found
****************************************************************/
void PC_intr( void )
{
		INTC.IER.BIT.IRQ0E = 0;		/* IRQ0 Disable */
		ide_error = 0x40;
}

/***************************************************************
	ハードディスクの物理セクター読み込み

	INPUT:
		cyliner = 0...	読み込みシリンダー番号
		head = 0...		読み込みヘッド番号
		sector = 0...	読み込みセクター
		num = 1...		読み込みセクター数

		buffer = 		読み込みバッファーアドレス

	RETURN:
		0 = 読み込み正常
		other = 読み込みエラーステータス
***************************************************************/
int	ide_sector_read( long sector, short num, unsigned char *buff )
{
		int		i;
		long	li;
		ATA_STATUS	status;
		union {
			long	ldata;
			char	cdata[4];
		} data;
		

		if( ATA_CARD.READ.Status.ata_stat.DSC==0 ){
			return( SECTOR_NG );		// Card Not Found
		}
		if( ATA_CARD.READ.Status.ata_stat.RDY==0 ){
			return( SECTOR_NOINIT );	/*ドライブの準備ができていない*/
		}

		// Busy Check
		for( li=0; li<MAX_RETRY; li++ ){
			if( dosaccs_target_run ){
				WatchDogReset();
			}

			if( ATA_CARD.READ.Status.ata_stat.BUSY==0 )
				break;
		}
		if( ATA_CARD.READ.Status.ata_stat.BUSY )
			return( SECTOR_NG );

		/* 割り込み処理用バッファーを設定する */
		ide_error = 0;

		data.ldata = sector;

		/* コマンド発行 */
				/* ダミービットセット */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.dummy1 = 1;
				/* LBAモード選択 */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.LBA = 1;
				/* ダミービットセット */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.dummy2 = 1;
				/* ATAドライブ番号セット(0固定) */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.DRV = 0;
				/* LBA27-LBA24 */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.HS = data.cdata[0];
				/* LBA23-LBA16 */
		ATA_CARD.WRITE.CylinderHigh = data.cdata[1];
				/* LBA15-8 */
		ATA_CARD.WRITE.CylinderLow = data.cdata[2];
				/* LBA7-0 */
		ATA_CARD.WRITE.SectorNumber = data.cdata[3];
				/* リードセクタカウント数セット */
		ATA_CARD.WRITE.SectorCount = num;
				/* リードセクタコマンドセット */
		ATA_CARD.WRITE.Command = PC_ATA_CMD_RDSEC;

		while( num ){
			INTC.IER.BIT.IRQ0E = 1;		/* IRQ0 Enable */
			/* 割り込み処理待ち */
			for( li=0; li<MAX_RETRY; li++ ){
				if( dosaccs_target_run ){
					WatchDogReset();
				}

				if( ide_error )
					break;			// 割り込み有り
				if( ATA_CARD.READ.Status.ata_stat.DRQ )
					break;			// データ転送準備完了
				if( ATA_CARD.READ.Status.ata_stat.ERR ){	/*エラー発生*/
					ide_error = ATA_CARD.READ.Dup_Error.Code;
					break;
				}
			}
			INTC.IER.BIT.IRQ0E = 0;		/* IRQ0 Disable */

			if( li==MAX_RETRY ){
				ide_error = 0x02;
				break;
			}

			status.Code = ATA_CARD.READ.Status.Code;	/*ＨＤＤコントローラー割り込み要因クリア*/
			if( status.ata_stat.ERR ){	/*エラー発生*/
				ide_error = ATA_CARD.READ.Dup_Error.Code;
				break;
			}
			if( status.ata_stat.BUSY ){		/*まだ、ＢＵＳＹのとき*/
				continue;
			}
			if( status.ata_stat.DRQ==0 ){	/* DRQ off */
				continue;
			}

			for( i=0; i<SECTOR_BYTES; i+=2 ){
				*buff++ = ATA_CARD.READ.Dup_evenRD;	/* DATA READ */
				*buff++ = ATA_CARD.READ.Dup_oddRD;	/* DATA READ */
			}
			num--;
		}
		INTC.IER.BIT.IRQ0E = 0;		/* IRQ0 Disable */

		if( ide_error==0 ){
			return( SECTOR_OK );
		}
		return( SECTOR_NG );
}


/***************************************************************
	ハードディスクの物理セクター書き込み

	INPUT:
		cyliner = 0...	書き込みシリンダー番号
		head = 0...		書き込みヘッド番号
		sector = 0...	書き込みセクター
		num = 1...		書き込みセクター数

		buffer = 		書き込みバッファーアドレス

	RETURN:
		0 = 書き込み正常
		other = 書き込みエラーステータス
***************************************************************/
int	ide_sector_write( long sector, short num, unsigned char *buff )
{
		int		i;
		long	li;
		ATA_STATUS	status;
		union {
			long	ldata;
			char	cdata[4];
		} data;

		if( PB.DR.BIT.B5 ){		/* Write protected */
			return( SECTOR_NG );
		}
		if( ATA_CARD.READ.Status.ata_stat.DSC==0 ){
			return( SECTOR_NG );		// Card Not Found
		}
		if( ATA_CARD.READ.Status.ata_stat.RDY==0 ){
			return( SECTOR_NOINIT );	/*ドライブの準備ができていない*/
		}

		// Busy Check
		for( li=0; li<MAX_RETRY; li++ ){
			if( dosaccs_target_run ){
				WatchDogReset();
			}

			if( ATA_CARD.READ.Status.ata_stat.BUSY==0 )
				break;
		}
		if( ATA_CARD.READ.Status.ata_stat.BUSY )
			return( SECTOR_NG );

		/* 割り込み処理用バッファーを設定する */
		ide_error = 0;

		data.ldata = sector;

		/* コマンド発行 */
				/* ダミービットセット */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.dummy1 = 1;
				/* LBAモード選択 */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.LBA = 1;
				/* ダミービットセット */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.dummy2 = 1;
				/* ATAドライブ番号セット(0固定) */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.DRV = 0;
				/* LBA27-LBA24 */
		ATA_CARD.WRITE.SelectCard_head.ata_drv_hd.HS = data.cdata[0];
				/* LBA23-LBA16 */
		ATA_CARD.WRITE.CylinderHigh = data.cdata[1];
				/* LBA15-8 */
		ATA_CARD.WRITE.CylinderLow = data.cdata[2];
				/* LBA7-0 */
		ATA_CARD.WRITE.SectorNumber = data.cdata[3];
				/* ライトセクタカウント数セット */
		ATA_CARD.WRITE.SectorCount = num;
				/* ライトセクタコマンドセット */
		ATA_CARD.WRITE.Command = PC_ATA_CMD_WRSEC;

		while( num ){
			status.Code = ATA_CARD.READ.Status.Code;	/*ＨＤＤコントローラー割り込み要因クリア*/
			if( status.ata_stat.ERR ){	/*エラー発生*/
				ide_error = ATA_CARD.READ.Dup_Error.Code;
				break;
			}
			if( status.ata_stat.BUSY ){		/*まだ、ＢＵＳＹのとき*/
				continue;
			}
			if( status.ata_stat.DRQ==0 ){	/* DRQ off */
				continue;
			}
			for( i=0; i<SECTOR_BYTES; i+=2 ){
				ATA_CARD.WRITE.Dup_evenWR = *buff++;	/* DATA WRITE */
				ATA_CARD.WRITE.Dup_oddWR = *buff++;	/* DATA WRITE */
			}

			INTC.IER.BIT.IRQ0E = 1;		/* IRQ0 Enable */
			/* 割り込み処理待ち */
			for( li=0; li<MAX_RETRY; li++ ){
				if( dosaccs_target_run ){
					WatchDogReset();
				}

				if( ide_error )
					break;			// 割り込み有り
				if( ATA_CARD.READ.Status.ata_stat.DRQ )
					break;			// データ転送準備完了
				if( ATA_CARD.READ.Status.ata_stat.ERR ){	/*エラー発生*/
					ide_error = ATA_CARD.READ.Dup_Error.Code;
					break;
				}
				if( ATA_CARD.READ.Status.ata_stat.BUSY==0 )
					break;
			}
			INTC.IER.BIT.IRQ0E = 0;		/* IRQ0 Disable */
			if( li==MAX_RETRY ){
				ide_error = 0x02;
				break;
			}
			if( ATA_CARD.READ.Status.ata_stat.ERR ){	/*エラー発生*/
				ide_error = ATA_CARD.READ.Dup_Error.Code;
				break;
			}

			num--;
		}
		INTC.IER.BIT.IRQ0E = 0;		/* IRQ0 Disable */

		if( ide_error==0 ){
			return( SECTOR_OK );
		}
		return( SECTOR_NG );
}

int ide_status( void )
{
		return( ide_error );
}

extern	void	(*IRQ0Jump)(void);



/*******************************************************************
	ディスクの接続処理
*******************************************************************/
extern	PARTITION_INFO	root_part;

int mount( short drive )
{
		int		no;
		long	li;

drive = 0;
		for( no=0; no<2; no++ ){
			if( root_bpb[drive].part==NULL ){
				PB.DR.BIT.B6=0;			// SLOT-CS is Selected
				PB.DR.BIT.B4=1;			// IORDA = not Active
				PB.DR.BIT.B3=1;			// IOWRA = not Active
	
				PB.DR.BIT.B0=0;
				PB.DR.BIT.B0=1;
				PB.DR.BIT.B0=0;
	
				PB.DR.BIT.B1=0;			// Card I/F Reset
				for( li=0; li<100000L; li++ ){
					PB.DR.BIT.B1=1;
					if( dosaccs_target_run ){
						WatchDogReset();
					}
				}
				PB.DR.BIT.B1=0;
			}

//			ATA_CARD.WRITE.DeviceCtl.ata_dev_cntl.SW_Rst = 1;
//			ATA_CARD.WRITE.DeviceCtl.ata_dev_cntl.SW_Rst = 0;
//
//			ATA_CARD.WRITE.DeviceCtl.ata_dev_cntl.IEn = 1;
//

			PB.DR.BIT.B2 = 1;		// REG = H

			IRQ0Jump = PC_intr;

			ide_error = 0;
			INTC.IER.BIT.IRQ0E = 0;		/* IRQ0 Disable */
			if( root_bpb[drive].part==NULL ){
				for( li=0; li<MAX_RETRY; li++ ){
					if( dosaccs_target_run ){
						WatchDogReset();
					}

					if( ATA_CARD.READ.Status.ata_stat.DSC )
						break;
				}
			}

			if( ATA_CARD.READ.Status.ata_stat.DSC )
				break;
			root_bpb[drive].part=NULL;
		}

			/*パーティションテーブルの読み込み*/
		memset( cluster_buffer.buffer, 0, sizeof(cluster_buffer.buffer));
		if( ide_sector_read( 0, 1, cluster_buffer.buffer )!=SECTOR_OK ){
			return( -1 );
		}

			/*パーティションＩＤチェック*/
		if( cluster_buffer.buffer[0x1fe]!=0x55 )
			return( -1 );
		if( cluster_buffer.buffer[0x1ff]!=0xAA )
			return( -1 );

			/*パーティションの先頭を求める*/
		if( cluster_buffer.buffer[0x1ee+4]!=0 ){
			no = 0x1ee;
		}
		else if( cluster_buffer.buffer[0x1de+4]!=0 ){
			no = 0x1de;
		}
		else if( cluster_buffer.buffer[0x1ce+4]!=0 ){
			no = 0x1ce;
		}
		else if( cluster_buffer.buffer[0x1be+4]!=0 ){
			no = 0x1be;
		}
		else {
			return( -1 );
		}

		root_part.boot_flag = cluster_buffer.buffer[no];
		root_part.system_id = cluster_buffer.buffer[no+4];
		root_part.start_head = cluster_buffer.buffer[no+1];
		root_part.start_sector = cluster_buffer.buffer[no+2];
		root_part.start_cylinder = cluster_buffer.buffer[no+3];
		root_part.end_head = cluster_buffer.buffer[no+5]&0x3F;
		root_part.end_sector = (unsigned short)(cluster_buffer.buffer[no+6]&0x3F);
		root_part.end_cylinder = ((unsigned short)(cluster_buffer.buffer[no+6]&0xC0)<<2)
							   | (unsigned short)(cluster_buffer.buffer[no+7]);

		root_part.boot_sector = ((long)cluster_buffer.buffer[no+8]&0x000000FFL)
							  | (((long)cluster_buffer.buffer[no+9]<<8)&0x0000FF00L)
							  | (((long)cluster_buffer.buffer[no+10]<<16)&0x00FF0000L)
							  | (((long)cluster_buffer.buffer[no+11]<<24)&0xFF000000L);
		root_part.all_sector  = ((long)cluster_buffer.buffer[no+12]&0x000000FFL)
							  | (((long)cluster_buffer.buffer[no+13]<<8)&0x0000FF00L)
							  | (((long)cluster_buffer.buffer[no+14]<<16)&0x00FF0000L)
							  | (((long)cluster_buffer.buffer[no+15]<<24)&0xFF000000L);
		root_part.sector_track = 63;
		root_part.head_number = 16;
		root_part.cylinder_vol = 0;

		root_part.driver_read = ide_sector_read;
		root_part.driver_write = ide_sector_write;
		root_part.driver_status = ide_status;


		root_bpb[drive].part = &root_part;

		purge_cluster();

		if( sector_read( &root_bpb[drive], 0, 1, cluster_buffer.buffer ) )
			return( -1 );

		root_bpb[drive].bytes_sector = (unsigned short)cluster_buffer.buffer[0x0B]
						  | ((unsigned short)cluster_buffer.buffer[0x0C]<<8);

		root_bpb[drive].sector_cluster = (unsigned short)cluster_buffer.buffer[0x0D];
		if( root_bpb[drive].sector_cluster > SECTOR_CLUSTER )	return( -2 );

		root_bpb[drive].reserved_sector = (unsigned short)cluster_buffer.buffer[0x0E]
							 | ((unsigned short)cluster_buffer.buffer[0x0F]<<8);

		root_bpb[drive].FAT_number = (unsigned short)cluster_buffer.buffer[0x10];

		root_bpb[drive].root_entry = (unsigned short)cluster_buffer.buffer[0x11]
						| ((unsigned short)cluster_buffer.buffer[0x12]<<8);

		root_bpb[drive].all_sector = (unsigned short)cluster_buffer.buffer[0x13]
						| ((unsigned short)cluster_buffer.buffer[0x14]<<8);

		root_bpb[drive].sector_FAT = (unsigned short)cluster_buffer.buffer[0x16]
						| ((unsigned short)cluster_buffer.buffer[0x17]<<8);

		root_bpb[drive].sector_track = (unsigned short)cluster_buffer.buffer[0x18]
						  | ((unsigned short)cluster_buffer.buffer[0x19]<<8);

		root_bpb[drive].head_number = (unsigned short)cluster_buffer.buffer[0x1A]
						 | ((unsigned short)cluster_buffer.buffer[0x1B]<<8);

		root_bpb[drive].skip_sector = (unsigned long)cluster_buffer.buffer[0x1C]
						 | ((unsigned long)cluster_buffer.buffer[0x1D]<<8)
						 | ((unsigned long)cluster_buffer.buffer[0x1E]<<16)
						 | ((unsigned long)cluster_buffer.buffer[0x1F]<<24);

		root_bpb[drive].all_sector2 = (unsigned long)cluster_buffer.buffer[0x20]
						 | ((unsigned long)cluster_buffer.buffer[0x21]<<8)
						 | ((unsigned long)cluster_buffer.buffer[0x22]<<16)
						 | ((unsigned long)cluster_buffer.buffer[0x23]<<24);

		if( root_bpb[drive].all_sector2==0 ){
			root_bpb[drive].all_sector2 = root_part.all_sector;
			root_bpb[drive].FAT_type = FAT12;
		}
		else {
			root_bpb[drive].FAT_type = FAT16;
		}

		root_bpb[drive].bytes_cluster = root_bpb[drive].bytes_sector * root_bpb[drive].sector_cluster;
		root_bpb[drive].drive = drive;

		root_bpb[drive].FAT_sector_s = root_bpb[drive].reserved_sector;
		root_bpb[drive].FAT_sector_l = root_bpb[drive].sector_FAT;
		root_bpb[drive].DIR_sector_s = root_bpb[drive].FAT_sector_s
									+ root_bpb[drive].FAT_sector_l * root_bpb[drive].FAT_number;
		root_bpb[drive].DIR_sector_l = root_bpb[drive].root_entry * 32
									/ root_bpb[drive].bytes_sector;

		root_bpb[drive].DATA_sector_s = root_bpb[drive].DIR_sector_s
									+ root_bpb[drive].DIR_sector_l;

		root_bpb[drive].DATA_sector_l = root_bpb[drive].all_sector2
									- root_bpb[drive].DATA_sector_s;

		root_bpb[drive].DATA_cluster_l = (unsigned short)(root_bpb[drive].DATA_sector_l
									/ root_bpb[drive].sector_cluster);

		return( 0 );
}

/*******************************************************************
	ディスクの切断処理
*******************************************************************/
int dismount( short drive )
{
drive = 0;
		flush_cluster();
		flush_fat();
		flush_dir();

		root_bpb[drive].part = NULL;

		INTC.IER.BIT.IRQ0E = 0;		/* IRQ0 No Use */
}

