
struct	TRACE {
	unsigned short	tm_mon:4;
	unsigned short	tm_day:5;
	unsigned short	tm_hour:5;
	unsigned short	tm_min:6;
	unsigned short	tm_sec:6;
	unsigned short	tm_msec:4;
	unsigned char	eventID[8];
};

#define	EVENT_INIT_TEST		1
#define	EVENT_INIT_TEST2	2
#define	EVENT_SELF_TEST		3
#define	EVENT_TEST			4
#define	EVENT_STANDBY		5
#define	EVENT_IDLE			6
#define	EVENT_AUTO			7
#define	EVENT_TRS			8
#define	EVENT_EMGMA			9
#define	EVENT_SPCMA			10
#define	EVENT_EMGHL			11

#define	EVENT_REPEAT		12
#define	EVENT_EMG			13
#define	EVENT_SPC			14
#define	EVENT_STN			15
#define	EVENT_CAB			16
#define	EVENT_MAN			17
#define	EVENT_REL			18

#define	EVENT_TMS_NEXT		20
#define	EVENT_TMS_SPC		21
#define	EVENT_TMS_STN		22

#define	EVENT_DOOR_OPENACHIME		30
#define	EVENT_DOOR_CLOSEACHIME		31
#define	EVENT_DOOR_OPENAANNOUNCE	32
#define	EVENT_DOOR_CLOSEAANNOUNCE	33
#define	EVENT_DOOR_OPENBCHIME		34
#define	EVENT_DOOR_CLOSEBCHIME		35
#define	EVENT_DOOR_OPENBANNOUNCE	36
#define	EVENT_DOOR_CLOSEBANNOUNCE	37

#define	EVENT_DOWNLOAD_CONFIG		40
#define	EVENT_DOWNLOAD_ROUTE		41
#define	EVENT_DOWNLOAD_PROG			42
#define	EVENT_DOWNLOAD_RECONF		43
#define	EVENT_DOWNLOAD_REBOOT		44

#define	EVENT_VOICE					80
#define	EVENT_VOICE_OK				81
#define	EVENT_VOICE_NG				82
#define	EVENT_VOICE_ABORT			83
#define	EVENT_EVCOMM_NG				84

#define	EVENT_TROUBLE_DVAU				0x80
#define	EVENT_TROUBLE_PAA				0x90
#define	EVENT_TROUBLE_MCP				0xA0
#define	EVENT_TROUBLE_COMMPAA			0xB0
#define	EVENT_TROUBLE_COMMMCP			0xC0





int TraceClear( void );
int TraceAdd( char event );
int TraceTrouble( char event, unsigned char troubleBit );
struct TRACE *TraceGet( int first );
void TraceText( struct TRACE *getp, char *buffer );

