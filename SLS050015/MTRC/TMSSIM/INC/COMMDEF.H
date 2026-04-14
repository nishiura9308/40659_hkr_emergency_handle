/********************************************************************
*	y`’n‰º“SŒü‚¯Ž©“®•ú‘—‘•’uz								*
*-------------------------------------------------------------------*
*	’ÊM•”@’è‹`ƒtƒ@ƒCƒ‹											*
*-------------------------------------------------------------------*
*	Author		TOA Corporation									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

/*@@@S*/

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
/*	ŽóMƒoƒbƒtƒ@\‘¢’è‹`											*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
struct	POLLING {
	unsigned short	adr;		/* Polling Address  */
	unsigned char	kind;		/* Kind of Data
									'S' = ŽÔ—¼”Ô†“o˜^ƒ|[ƒŠƒ“ƒO
									'N' = ŽÔ—¼”Ô†–³Ž‹’Êíƒ|[ƒŠƒ“ƒO
									'M' = ŽÔ—¼”Ô†•t‚«’Êíƒ|[ƒŠƒ“ƒO
									'C' = ‚g‚b‚q‚É‚æ‚éƒAƒNƒeƒBƒu•ÏXƒ|[ƒŠƒ“ƒO
											”ñƒAƒNƒeƒBƒu‚c‚u‚`‚t‚É‘Î‚µ‚ÄƒAƒNƒeƒBƒu‚É‚È‚é‚æ‚¤—v‹
									'I' = ƒCƒjƒVƒƒƒ‹ƒeƒXƒg‚É‚æ‚éƒAƒNƒeƒBƒu•ÏXƒ|[ƒŠƒ“ƒO
									'T' = ƒZƒ‹ƒtƒeƒXƒgE‚s‚l‚rƒeƒXƒg‚É‚æ‚éƒAƒNƒeƒBƒu•ÏXƒ|[ƒŠƒ“ƒO
									'R' = ƒŠƒZƒbƒg—v‹ƒ|[ƒŠƒ“ƒO
								*/
	unsigned char	dataid;		/* Data ID
									00 = Test Abort
									01 = Odd Car DVAU Test
									02 = Odd Car MCP Test
									03 = Odd Car ACP Test
									04 = Even Car DVAU Test
									05 = Even Car MCP Test
									06 = Even Car ACP Test
									1x = AMP1 Test
									2x = AMP2 Test
									3x = AMP3 Test
									4x = AMP4 Test
									5x = AMP5 Test
									6x = AMP6 Test
									7x = AMP7 Test
									8x = AMP8 Test
									x0 = AMP
									x1 = SP1
									x2 = SP2
									x3 = SP3
									x4 = SP4
									x5 = SP5
								*/
	unsigned short	carno;		/* Car No. */
	unsigned char	nextstation;/* Next Station Code */
	unsigned char	destination;/* Destination Station Code */
	unsigned char	route;		/* Route Code */
	unsigned char	door;		/* Door Information */
	unsigned char	voicebus[9];/* Voice Bus Information */
	unsigned char	display[32];/* MCP Display Data */
	unsigned char	vlevel[8];	/* Voice Level */
};

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
/*	‘—Mƒoƒbƒtƒ@\‘¢’è‹`											*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
struct	MCPSEND {
	unsigned short	adr;		/* Car No. + Polling Address  */
	unsigned char	dataid;		/* Data ID */
	unsigned char	mcpsw[3];	/* MCP Switch Information */
	unsigned short	version;	/* Software Version */
};

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
/*	‘—Mƒoƒbƒtƒ@\‘¢’è‹`											*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
struct	PAASEND {
	unsigned short	adr;		/* Car No. + Polling Address  */
	unsigned char	dataid;		/* Data ID */
	unsigned char	rscsw;		/* RSC Switch Information */
	unsigned char	trouble;	/* Trouble Data */
	unsigned short	version;	/* Software Version */
};

/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
/*	‘—Mƒoƒbƒtƒ@\‘¢’è‹`											*/
/*†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£*/
struct	DVAUSEND {
	unsigned short	adr;		/* Car No. + Polling Address  */
	unsigned char	dataid;		/* Data ID */
	unsigned char	signal1;	/* InputSignal */
	unsigned char	signal2;	/* InputSignal */
	unsigned char	signal3;	/* InputSignal */
	unsigned char	trouble;	/* Trouble Info */
	unsigned short	version;	/* Software Version */
	unsigned short	carNo[8];	/* Car No. PAA x 8 */
};

struct	DOWNLOAD {
	unsigned short	adr;		/* Polling Address  */
	unsigned char	kind;		/* Kind of Data
									'D' = Download
									'E' = End
								*/
	unsigned char	seq;		/* Sequence Number */

	unsigned char	filename[16];	/* Download Filename */
	unsigned short	filedate;		/* Download File Date */
	unsigned short	filetime;		/* Download File Time */
	unsigned short	datasize;		/* Download Data Size */
	unsigned char	data[64];		/* Download Data */
};

struct	DOWNLOAD_ACK {
	unsigned short	adr;		/* Polling Address  */
	unsigned char	kind;		/* Kind of Data
									'D' = Download
									'E' = Error
								*/
	unsigned char	seq;		/* Sequence Number */

};

