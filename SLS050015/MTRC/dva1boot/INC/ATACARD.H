
typedef union {
	unsigned char Code;				/* ｴﾗｰﾚｼﾞｽﾀ */
	struct {
		unsigned char BBK	:1;		/* ｾｸﾀのID部で欠陥ﾌﾞﾛｯｸが発見された */
		unsigned char UNC	:1;		/* 修復不可能なｴﾗｰが発生 */
		unsigned char		:1;		/* 常に0 */
		unsigned char IDNF	:1;		/* 要求ｾｸﾀのIDが見つからない */
		unsigned char		:1;		/* 常に0 */
		unsigned char ABRT	:1;		/* 要求ｺﾏﾝﾄﾞ失敗 */
		unsigned char		:1;		/* 常に0 */
		unsigned char AMNF	:1;		/* ﾃﾞｰﾀｱﾄﾞﾚｽﾏｰｸがない */
	} ata_err;
} ATA_ERROR;

typedef union {
    unsigned char Code;             /* ﾄﾞﾗｲﾌﾞ/ﾍｯﾄﾞﾚｼﾞｽﾀ */
    struct {
        unsigned char dummy1:1;     /* 常に1 */
        unsigned char LBA	:1;		/* LBA/CHSﾓｰﾄﾞ選択ﾋﾞｯﾄ */
        unsigned char dummy2:1;     /* 常に1 */
        unsigned char DRV	:1;		/* ﾄﾞﾗｲﾌﾞ番号 */
        unsigned char HS	:4;		/* ﾍｯﾀﾞｰ番号 または LBAの27-24ﾋﾞｯﾄ */
    } ata_drv_hd;
} ATA_DRIVE_HEAD;

typedef union {
    unsigned char Code;             /* ｽﾃｰﾀｽﾚｼﾞｽﾀ */
    struct {
        unsigned char BUSY	:1;		/* ﾋﾞｼﾞｰ状態 */
        unsigned char RDY	:1;		/* ｺﾏﾝﾄﾞ応答可/不可状態 */
        unsigned char DWF	:1;		/* ﾗｲﾄﾌｫｰﾙﾄ状態 */
        unsigned char DSC	:1;		/* ｶｰﾄﾞﾚﾃﾞｨｰ状態 */
        unsigned char DRQ	:1;		/* ｶｰﾄﾞ、ﾎｽﾄ間転送準備状態 */
        unsigned char CORR	:1;		/* ｺﾚｸﾃｯﾄﾞﾃﾞｰﾀｴﾗｰ状態 */
        unsigned char IDX	:1;		/* 常に0 */
        unsigned char ERR	:1;		/* 前ｺﾏﾝﾄﾞ実行時ｴﾗｰ */
    } ata_stat;
} ATA_STATUS;

typedef union {
    unsigned char Code;             /* ﾃﾞﾊﾞｲｽｺﾝﾄﾛｰﾙﾚｼﾞｽﾀ */
    struct {
        unsigned char dummy	:1;		/* don't care */
        unsigned char		:1;		/* don't care */
        unsigned char		:1;		/* don't care */
        unsigned char		:1;		/* don't care */
        unsigned char		:1;		/* 無視される */
        unsigned char SW_Rst:1;		/* ｿﾌﾄｳｪｱﾘｾｯﾄ */
        unsigned char IEn	:1;		/* 割込み許可 (Active Low) */
        unsigned char		:1;		/* 無視される */
    } ata_dev_cntl;
} ATA_DEVICE_CONTROL;

typedef union {
    unsigned char Code;             /* ｶｰﾄﾞ（ﾄﾞﾗｲﾌﾞ）ｱﾄﾞﾚｽﾚｼﾞｽﾀ */
    struct {
        unsigned char dummy	:1;		/* 不定 */
        unsigned char WTG	:1;		/* ﾗｲﾄ動作中0 */
        unsigned char HS	:4;		/* ﾍｯﾄﾞ番号の反転値 */
        unsigned char nDS1	:1;		/* ﾄﾞﾗｲﾌﾞ1選択 (active Low) */
        unsigned char nDS0	:1;		/* ﾄﾞﾗｲﾌﾞ0選択 (active Low) */
    } ata_cd_adr;
} ATA_CARD_ADDRESS;

typedef union {
    struct {								/* ﾒﾓﾘﾏｯﾌﾟﾓｰﾄﾞ */
        unsigned char      dummy;           /* ﾊｰﾄﾞ構造上ﾀﾞﾐｰとなる(evenRD) */
        ATA_ERROR          Error;           /* Dup側を使うべき */
        unsigned char      SectorCount;     /* ｾｸﾀｶｳﾝﾀ */
        unsigned char      SectorNumber;    /* ｾｸﾀ番号 */
        unsigned char      CylinderLow;     /* ｼﾘﾝﾀﾞｱﾄﾞﾚｽ(Low側) */
        unsigned char      CylinderHigh;    /* ｼﾘﾝﾀﾞｱﾄﾞﾚｽ(High側) */
        ATA_DRIVE_HEAD     SelectCard_head; /* ｾﾚｸﾄｶｰﾄﾞ/ﾍｯﾄﾞ */
        ATA_STATUS         Status;          /*  */
        unsigned char      Dup_evenRD;      /* 読込ﾃﾞｰﾀ(偶数ｱﾄﾞﾚｽ側:複製) */
        unsigned char      Dup_oddRD;       /* 読込ﾃﾞｰﾀ(奇数ｱﾄﾞﾚｽ側:複製) */
        unsigned char      filler[3];
        ATA_ERROR          Dup_Error;       /* ｴﾗｰ情報(複製) */
        ATA_STATUS         AltStatus;       /*  */
        ATA_CARD_ADDRESS   DriveAddress;    /*  */
    } READ;
    struct {
        unsigned char      dummy;           /* ﾊｰﾄﾞ構造上ﾀﾞﾐｰとなる(evenWR) */
        unsigned char      Features;        /* Dup側を使うべき */
        unsigned char      SectorCount;     /* ｾｸﾀｶｳﾝﾀ */
        unsigned char      SectorNumber;    /* ｾｸﾀ番号 */
        unsigned char      CylinderLow;     /* ｼﾘﾝﾀﾞｱﾄﾞﾚｽ(Low側) */
        unsigned char      CylinderHigh;    /* ｼﾘﾝﾀﾞｱﾄﾞﾚｽ(High側) */
        ATA_DRIVE_HEAD     SelectCard_head; /* ｾﾚｸﾄｶｰﾄﾞ/ﾍｯﾄﾞ */
        unsigned char      Command;         /* ATA ｺﾏﾝﾄﾞ */
        unsigned char      Dup_evenWR;      /* 書込ﾃﾞｰﾀ(偶数ｱﾄﾞﾚｽ側:複製) */
        unsigned char      Dup_oddWR;       /* 書込ﾃﾞｰﾀ(奇数ｱﾄﾞﾚｽ側:複製) */
        unsigned char      filler[3];
        unsigned char      Dup_Features;
        ATA_DEVICE_CONTROL DeviceCtl;
        unsigned char      Reserved;
    } WRITE;
} ATA_REGISTER;


/****************************************************************************/
/*		ﾌｫｰﾏｯﾄ情報(Identify Drive)											*/	
/****************************************************************************/
typedef struct {
    unsigned short  cylinder;           /* ｼﾘﾝﾀﾞ数 */
    unsigned short  dummy;				/* ﾘｻﾞｰﾌﾞ */
    unsigned short  header;             /* ﾍｯﾄﾞ数 */
    unsigned short  byte_per_track;     /* 1ﾄﾗｯｸの容量(bytes:当てにならない) */
    unsigned short  byte_per_sector;    /* 1ｾｸﾀの容量(bytes) */
    unsigned short  sector_per_track;   /* ﾄﾗｯｸ毎のｾｸﾀ数 */
    unsigned long   sector_per_card;    /* ｶｰﾄﾞ毎のｾｸﾀ数(ﾍﾞﾝﾀﾞｰﾕﾆｰｸ...使用しないこと) */
} PC_ATACARD_FORMAT;


union st_ataregister {
    struct {								/* ﾒﾓﾘﾏｯﾌﾟﾓｰﾄﾞ */
        unsigned char      dummy;           /* ﾊｰﾄﾞ構造上ﾀﾞﾐｰとなる(evenRD) */
        ATA_ERROR          Error;           /* Dup側を使うべき */
        unsigned char      SectorCount;     /* ｾｸﾀｶｳﾝﾀ */
        unsigned char      SectorNumber;    /* ｾｸﾀ番号 */
        unsigned char      CylinderLow;     /* ｼﾘﾝﾀﾞｱﾄﾞﾚｽ(Low側) */
        unsigned char      CylinderHigh;    /* ｼﾘﾝﾀﾞｱﾄﾞﾚｽ(High側) */
        ATA_DRIVE_HEAD     SelectCard_head; /* ｾﾚｸﾄｶｰﾄﾞ/ﾍｯﾄﾞ */
        ATA_STATUS         Status;          /*  */
        unsigned char      Dup_evenRD;      /* 読込ﾃﾞｰﾀ(偶数ｱﾄﾞﾚｽ側:複製) */
        unsigned char      Dup_oddRD;       /* 読込ﾃﾞｰﾀ(奇数ｱﾄﾞﾚｽ側:複製) */
        unsigned char      filler[3];
        ATA_ERROR          Dup_Error;       /* ｴﾗｰ情報(複製) */
        ATA_STATUS         AltStatus;       /*  */
        ATA_CARD_ADDRESS   DriveAddress;    /*  */
    } READ;
    struct {
        unsigned char      dummy;           /* ﾊｰﾄﾞ構造上ﾀﾞﾐｰとなる(evenWR) */
        unsigned char      Features;        /* Dup側を使うべき */
        unsigned char      SectorCount;     /* ｾｸﾀｶｳﾝﾀ */
        unsigned char      SectorNumber;    /* ｾｸﾀ番号 */
        unsigned char      CylinderLow;     /* ｼﾘﾝﾀﾞｱﾄﾞﾚｽ(Low側) */
        unsigned char      CylinderHigh;    /* ｼﾘﾝﾀﾞｱﾄﾞﾚｽ(High側) */
        ATA_DRIVE_HEAD     SelectCard_head; /* ｾﾚｸﾄｶｰﾄﾞ/ﾍｯﾄﾞ */
        unsigned char      Command;         /* ATA ｺﾏﾝﾄﾞ */
        unsigned char      Dup_evenWR;      /* 書込ﾃﾞｰﾀ(偶数ｱﾄﾞﾚｽ側:複製) */
        unsigned char      Dup_oddWR;       /* 書込ﾃﾞｰﾀ(奇数ｱﾄﾞﾚｽ側:複製) */
        unsigned char      filler[3];
        unsigned char      Dup_Features;
        ATA_DEVICE_CONTROL DeviceCtl;
        unsigned char      Reserved;
    } WRITE;
};

#define ATA_CARD   (*(volatile union st_ataregister *)0x200000) /* ATA Card Address*/

/* PCｶｰﾄﾞ ｺﾓﾝ/ｱﾄﾘﾋﾞｭｰﾄ切替値 */
#define PCCD_ATR		0				 /* PCｶｰﾄﾞをｱﾄﾘﾋﾞｭｰﾄﾒﾓﾘｱｸｾｽ状態 */
#define PCCD_COM		1				 /* PCｶｰﾄﾞをｺﾓﾝﾒﾓﾘｱｸｾｽ状態 */

/* ATA (Flash-ROM) ｺﾏﾝﾄﾞ */
#define PC_ATA_CMD_RDSEC            ((unsigned char )0x20)   /* read sector */
#define PC_ATA_CMD_WRSEC            ((unsigned char )0x30)   /* write sector */
#define PC_ATA_CMD_FORMAT           ((unsigned char )0x50)   /* format track */
#define PC_ATA_CMD_IDENTIFY         ((unsigned char )0xec)   /* identify drive */

/* ATAｶｰﾄﾞCONFIGRATIONｱﾄﾞﾚｽ(attribute memory) */
#define PC_ATA_CNFGREG_OPTION   ((volatile unsigned char *)0x200)
#define PC_ATA_CNFGREG_STATUS   ((volatile unsigned char *)0x202)
#define PC_ATA_CNFGREG_REPLACE  ((volatile unsigned char *)0x204)
#define PC_ATA_CNFGREG_SOKET    ((volatile unsigned char *)0x206)

/* TUPLE ｺｰﾄﾞ */
#define PC_TUPLE_DEVICE_INFO ((unsigned short )0x0001) /* CISTPL_DEVICE */
#define PC_TUPLE_JEDEC       ((unsigned short )0x0018) /* CISTPL_JEDEC_C */
#define PC_TUPLE_INTERFACE   ((unsigned short )0x001b) /* CISTPL_CE */
#define PC_TUPLE_CARD_CLASS  ((unsigned short )0x00d0) /* DxH is I/O-DEVICE */
#define PC_TUPLE_SEARCH_END  0x1FFUL                /* search tuple last offset */
#define PC_TUPLE_LINK_END    ((unsigned char )0xff)		/* ATA tuple link end sign */
#define PC_TPL_JEDEC_ATA1    ((unsigned short )0x00df) /* ATA sapport code */
#define PC_TPL_JEDEC_ATA2    ((unsigned short )0x0045) /* ATA sapport code */
#define PC_TPL_JEDEC_NOVPP   ((unsigned short )0x0001) /* NO Vpp added */
#define PC_TPL_JEDEC_ADVPP   ((unsigned short )0x0002) /* Vpp added */
#define PC_TPL_ATAIF_MEMMAP  ((unsigned short )0x00c0) /* Memory Mapped I/F */
#define PC_TPL_ATAIF_IOMAP16 ((unsigned short )0x00c1) /* I/O Mapped 16 Reg I/F */
#define PC_TPL_ATAIF_IOAPMAP ((unsigned short )0x00c2) /* AT Fixed Disk Primary I/O adr. Mapped I/F */
#define PC_TPL_ATAIF_IO2MAP  ((unsigned short )0x00c3) /* AT Fixed Disk Secondry I/O adr. Mapped I/F */
#define PC_TPL_ATAIF_UNKNOWN ((unsigned short )0x00ff) /* ATA I/F unknowned */

/* ｶｰﾄﾞ種別(上位ﾆﾌﾞﾙのみ有効) */
#define PC_CDCLSS_UNKNOWN   ((unsigned char )0x00)  /* non-card or other-type card */
#define PC_CDCLSS_MASK_ROM  ((unsigned char )0x10)  /* MASK ROM */
#define PC_CDCLSS_OTPROM    ((unsigned char )0x20)  /* OTPROM */
#define PC_CDCLSS_EPROM     ((unsigned char )0x30)  /* EPROM */
#define PC_CDCLSS_ESQRPROM  ((unsigned char )0x40)  /* EEPROM */
#define PC_CDCLSS_F_EPROM   ((unsigned char )0x50)  /* Flash-EPROM */
#define PC_CDCLSS_RAMWB     ((unsigned char )0x60)  /* RAM with Battery backup */
#define PC_CDCLSS_IO_DEVICE ((unsigned char )0xd0)  /* ATA type CARD */

