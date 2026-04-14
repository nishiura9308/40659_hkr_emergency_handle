#ifndef DEFINE_TABLE_DATA
#define	DEFINE_TABLE_DATA	1
/********************************************************************
*	【香港地下鉄向け自動列車放送制御装置】							*
*-------------------------------------------------------------------*
*	テーブルフォーマット定義													*
*-------------------------------------------------------------------*
*	Author		TOA Corporation									*
*																	*
*	Revision	a00. 2000/06/27		K.Honda							*
*																	*
*********************************************************************/

//
//	ＴＭＳ通信フォーマット
//
/*** from TMS Command ***/
union SDRFORM {
	unsigned char	text[105];
	struct {
		unsigned char	lowadr;			// 下位アドレス
		unsigned char	highadr;		// 上位アドレス
		unsigned char	control;		// コントロールコード

		unsigned char	code;			// 情報コード
		struct {
			unsigned char	b7:1;
			unsigned char	testExecRequest:1;		// テスト要求
			unsigned char	b5:1;
			unsigned char	b4:1;
			unsigned char	b3:1;
			unsigned char	b2:1;
			unsigned char	testStartRequest2:1;	// カテゴリー２テスト準備要求
			unsigned char	testStartRequest1:1;	// カテゴリー１テスト準備要求
		} testRequest;
		unsigned char	itemNo;						// テスト要求番号
													//   TestStartRequest1
													//	01h = Odd DVAU
													//		  Odd MCP
													//		  Odd ACP
													//		  Even DVAU
													//		  Even MCP
													//		  Even ACP
													//	TestStartRequest2
													//	01h = PAA1 Self
													//		  SP1
													//		  SP2
													//		  SP3
													//		  SP4
													//		  SP5
													//	02h = PAA2
													//	03h = PAA3
													//	04h = PAA4
													//	05h = PAA5
													//	06h = PAA6
													//	07h = PAA7
													//	08h = PAA8
		unsigned char	text3;
		unsigned char	text4;
		struct {
			unsigned char	b7:1;
			unsigned char	b6:1;
			unsigned char	b5:1;
			unsigned char	b4:1;
			unsigned char	carTypeValid:1;			// 車両タイプ有効フラグ
			unsigned char	carNoValid:1;			// 車両番号有効フラグ
			unsigned char	timeDateAdjustment:1;	// 時計設定フラグ
			unsigned char	timeDateValid:1;		// 時計情報有効フラグ
		} activeText;
		struct {						// 時計情報（ＢＣＤ）
			unsigned char	yy10:4;
			unsigned char	yy1:4;
			unsigned char	MM10:4;
			unsigned char	MM1:4;
			unsigned char	dd10:4;
			unsigned char	dd1:4;
			unsigned char	hh10:4;
			unsigned char	hh1:4;
			unsigned char	mm10:4;
			unsigned char	mm1:4;
			unsigned char	ss10:4;
			unsigned char	ss1:4;
		} timeDate;
		struct {
			unsigned short	carNo:12;		// 車両番号（ＢＣＤ）
			unsigned short	carType:4;		// 車両タイプ
		} carInfo;
		unsigned char	couplingUnitCode;	// 連結コード
		unsigned char	text15;
		unsigned char	stationNumber;		// 到着駅案内用駅番号（ライン番号＋駅番号）
				// 上位３ビットがライン番号
				// 下位５ビットが駅番号
		unsigned char	nextStationName;	// 次駅コード
		unsigned char	destination;		// 到着駅コード
		unsigned char	routeCode;			// ルートコード
				// Chnage Code Timing ---> Next Station Broadcast

		unsigned char	talkBack[8];		// 非常通報器接続要求
		unsigned char	specialCode;		// スペシャルメッセージコード 70-7F
				// Change Code Timing ---> Special Broadcast
		struct {
			unsigned char	dbEffective:1;		// データ有効フラグ
				// nextStationName, destination, routeCode を取り込む
			unsigned char	atcActive:1;		// 放送開始フラグ
				// atcActive
				//		0->1 ... stationNumber を取り込み、放送する
			unsigned char	b5:1;
			unsigned char	b4:1;
			unsigned char	b3:1;
			unsigned char	b2:1;
			unsigned char	b1:1;
			unsigned char	b0:1;
		} ATCpaActive;
		struct {
			unsigned char	b7:1;
			unsigned char	b6:1;
			unsigned char	b5:1;
			unsigned char	b4:1;
			unsigned char	broadCastOff:1;			// 放送禁止
			unsigned char	doorOpenEffective:1;	// ドア情報有効
			unsigned char	doorOpen:2;				// ドア情報
													//   00 = Both Closed
													//   01 = Right Open
													//   10 = Left Open
													//   11 = Both Open
		} door;
		struct {
			unsigned char	b7:1;
			unsigned char	b6:1;
			unsigned char	b5:1;
			unsigned char	b4:1;
			unsigned char	b3:1;
			unsigned char	b2:1;
			unsigned char	b1:1;
			unsigned char	doorClosed:1;		// 2001/07/04 ドアステータス追加
		} doorStatus;
		struct {					// 車両毎の車両タイプと番号
			unsigned short	carNo:12;
			unsigned short	carType:4;
		} carInfo2[8];
		unsigned char	text49;
		unsigned char   PadReset[8];  // 非常通報器接続要求解除
	} SDR;
	struct {
		unsigned char	lowadr;			// 下位アドレス
		unsigned char	highadr;		// 上位アドレス
		unsigned char	control;		// コントロールコード

		unsigned char	code;			// 情報コード

		unsigned char	traceNo;		// 要求トレースデータ番号
		unsigned char	blockNo;		// 要求ブロック番号
	} TDR;
	struct {
		unsigned char	lowadr;			// 下位アドレス
		unsigned char	highadr;		// 上位アドレス
		unsigned char	control;		// コントロールコード

		unsigned char	code;			// 情報コード

		unsigned char	blockNo;		// ブロック番号
		unsigned char	data[101];
	} LDR;
};


/*** to TMS Answer ***/
union SDFORM {
	unsigned char	text[106];
	struct {
		unsigned char	lowadr;				// 下位アドレス
		unsigned char	highadr;			// 上位アドレス
		unsigned char	control;			// コントロールコード

		unsigned char	code;		// 情報コード
		struct TEST_RESPONCE {
			unsigned char	testExecutionStatus:2;	// テスト中
			unsigned char	b5:1;
			unsigned char	b4:1;
			unsigned char	b3:1;
			unsigned char	b2:1;
			unsigned char	testStartResponce2:1;	// テスト準備完了
			unsigned char	testStartResponce1:1;	// テスト準備完了
		} testResponce;
		unsigned char	itemNo;						// テスト中番号
													//   TestStartRequest1
													//	01h = Odd DVAU
													//		  Odd MCP
													//		  Odd ACP
													//		  Even DVAU
													//		  Even MCP
													//		  Even ACP
													//	TestStartRequest2
													//	01h = PAA1 Self
													//		  SP1
													//		  SP2
													//		  SP3
													//		  SP4
													//		  SP5
													//	02h = PAA2
													//	03h = PAA3
													//	04h = PAA4
													//	05h = PAA5
													//	06h = PAA6
													//	07h = PAA7
													//	08h = PAA8
		struct TEST_OKNG {
			unsigned char	b7:1;
			unsigned char	b6:1;
			unsigned char	b5:1;
			unsigned char	b4:1;
			unsigned char	b3:1;
			unsigned char	b2:1;
			unsigned char	OK:1;				// テスト正常
			unsigned char	NG:1;				// テスト異常
		} itemResult;
		unsigned char	subItemResult;			// テストサブ項目フラグ(Fault)
		unsigned char	subItemMeasurement[8];
		unsigned char	text13;
		unsigned short	version;				// バージョン情報
		unsigned char	crcL;
		unsigned char	crcH;
		unsigned char	text18;
		unsigned char	passengerAlarm[8];		// 非常通報器状態
		union TMS_SD_COMMERR {
			unsigned char	BYTE[2];
			unsigned short	WORD;
			struct {
				unsigned char	b7:1;
				unsigned char	b6:1;
				unsigned char	b5:1;
				unsigned char	b4:1;
				unsigned char	mcpCommError2:1;	// MCP2 Error
				unsigned char	mcpCommError1:1;	// MCP1 Error
				unsigned char	dvauCommError2:1;	// DVAU2 Error
				unsigned char	dvauCommError1:1;	// DVAU1 Error
				unsigned char	paaCommError8:1;	// PAA8 Error
				unsigned char	paaCommError7:1;	// PAA7 Error
				unsigned char	paaCommError6:1;	// PAA6 Error
				unsigned char	paaCommError5:1;	// PAA5 Error
				unsigned char	paaCommError4:1;	// PAA4 Error
				unsigned char	paaCommError3:1;	// PAA3 Error
				unsigned char	paaCommError2:1;	// PAA2 Error
				unsigned char	paaCommError1:1;	// PAA1 Error
			} BIT;
		} dvaCommError;
		union TMS_SD_STATUS {
			unsigned char	BYTE;
			struct {
				unsigned char	b7:1;
				unsigned char	doorChimeMode:1;	// Door Chime 
				unsigned char	doorMode:1;			// Door Announce
				unsigned char	autoMode:1;			// Auto Announce
				unsigned char	specialMode:1;		// Special Message
				unsigned char	emgMode:1;			// Emergency MEssage
				unsigned char	cabMode:1;			// CAB/CAB Mode
				unsigned char	manualMode:1;		// Manual Mode
			} BIT;
		} dvaStatus;
		unsigned char	text30;
		unsigned char	text31;
		unsigned char	text32;
		unsigned char	text33;
		unsigned char	text34;
		unsigned char	text35;
		unsigned char	text36;
		unsigned char	text37;
		unsigned char	text38;
		unsigned char	text39;
	} SD;

	struct {
		unsigned char	lowadr;				// 下位アドレス
		unsigned char	highadr;			// 上位アドレス
		unsigned char	control;			// コントロールコード

		unsigned char	code;		// 情報コード

		unsigned char	blockNo;		// ブロック番号
		unsigned char	traceNo;		// トレースデータ番号

		unsigned char	data[100];
	} TD;

	struct {
		unsigned char	lowadr;				// 下位アドレス
		unsigned char	highadr;			// 上位アドレス
		unsigned char	control;			// コントロールコード

		unsigned char	code;		// 情報コード

		unsigned char	blockNo;		// 要求ブロック番号
		unsigned char	text2;
	} LD;
};

//
//	ＥＩＤＳフォーマット
//
struct	EIDS_FORMAT {
	unsigned char	stx;
	unsigned char	Messages;
	union {
		unsigned char	BYTE;
		struct {
			unsigned char	b7:1;
			unsigned char	b6:1;
			unsigned char	All1:1;
			unsigned char	reset:1;
			unsigned char	doorstatus:1;
			unsigned char	b2:1;
			unsigned char	b1:1;
			unsigned char	b0:1;
		} BIT;
	} status;
	unsigned char	nullCode;
	unsigned char	Route;
	unsigned char	Station;
	unsigned char	etx;
};



//
//	放送文構造定義
//

#define	MAX_SEQUENCE	6			// 最大シーケンス
union EV5000_SEQUENCE {
	struct {
		unsigned short	Code[2];
	} BYTE;
	struct {
		unsigned short	timewait;		// 放送文待ち時間 (x100mS)
		unsigned short	en:1;			// 英語放送文
		unsigned short	cn:1;			// 広東語放送文
		unsigned short	ma:1;			// 北京語放送文
		unsigned short	b12:1;
		unsigned short	b11:1;
		unsigned short	b10:1;
		unsigned short	b9:1;
		unsigned short	sendCode:9;		// 放送文コード
	} sendRequest;
};


#define	MAX_DOOR	4				// 最大ドア文章
#define	DOOR_OPEN_A				0		// ドアＡ開放送文
#define	DOOR_CLOSE_A			1		// ドアＡ閉放送文
#define	DOOR_OPEN_B				2		// ドアＢ開放送文
#define	DOOR_CLOSE_B			3		// ドアＢ閉放送文

struct DOORINFO {		// ドア開閉放送文
	union EV5000_SEQUENCE	announce_sequence[MAX_SEQUENCE];	// アナウンス放送
};

#define	MAX_EMGMA	32
struct EMGMAINFO {		// 非常放送文
	unsigned char	eidsChar;		// ＥＩＤＳ送信コード

	char	mcpChar2[6];	// ＭＣＰ表示文字
	char	mcpChar3[6];	// ＭＣＰ表示文字

	union EV5000_SEQUENCE	sequence[MAX_SEQUENCE];	// 放送文
};

#define	MAX_SPCMA	40
struct SPCINFO {		// スペシャル放送文
	unsigned char	eidsChar;		// ＥＩＤＳ送信コード

	char	mcpChar2[6];	// ＭＣＰ表示文字
	char	mcpChar3[6];	// ＭＣＰ表示文字

	union EV5000_SEQUENCE	sequence[MAX_SEQUENCE];	// 放送文
};

#define	MAX_STATION		64			// １ルートあたりの最大駅数
struct STATIONINFO {	// １ルートあたりの駅情報
	unsigned char	stationCode;		// 駅コード
	unsigned char	eidsStationChar;	// ＥＩＤＳ送信コード
	unsigned char	mcpStationChar[6];	// ＭＣＰ表示文字
	unsigned char	stationNumber;		// ライン番号とステーション番号
	unsigned char	Arriving:1;			// 到着案内フラグ(arriving)
	unsigned char	Arrived:1;			// 到着案内フラグ(arrived)
	unsigned char	b5:1;
	unsigned char	b4:1;
	unsigned char	b3:1;
	unsigned char	b2:1;
	unsigned char	b1:1;
	unsigned char	b0:1;

	union EV5000_SEQUENCE	sequence[MAX_SEQUENCE];	// 放送文
};

#define	MAX_ROUTE		32			// 最大ルート数
struct ROUTEINFO {	// ルート情報
	unsigned char	routeCode;			// ルートコード (00h=End)
	unsigned char	eidsRouteChar;		// ＥＩＤＳ送信文字 (00h=Nothing)
	unsigned char	mcpRouteChar[6];	// ＭＣＰ表示文字 (00h=Nothing)
	struct STATIONINFO	station[MAX_STATION];	// 駅情報
};

#define	MAX_PAA		8
struct	PAAINFO {	// ＰＡＡ基本音量
	unsigned char	level;
};

#define	MAX_MCP		2


struct SELECT_LANG {
	unsigned char	en:1;			// 英語放送
	unsigned char	cn:1;			// 広東語放送
	unsigned char	ma:1;			// マンダリン放送
	unsigned char	b4:1;
	unsigned char	b3:1;
	unsigned char	b2:1;
	unsigned char	b1:1;
	unsigned char	b0:1;
	unsigned char	dummy;			// ワード境界にするため
};


struct	CONFIG_INFO {
	struct	SELECT_LANG		lang;				// 言語情報
	union EV5000_SEQUENCE	chime_sequence;		// チャイム放送
	unsigned short			version;			// 機器のバージョン情報
	struct	PAAINFO			paa[MAX_PAA];		// ＰＡＡ基本情報
};

struct	ANNOUNCE_INFO {
	struct	DOORINFO		door[MAX_DOOR];		// ドア情報
	struct	EMGMAINFO		emgma[MAX_EMGMA];	// 非常放送情報
	struct	SPCINFO			spcma[MAX_SPCMA];	// スペシャル放送情報
	unsigned short			dvau_version;		// 各機器のバージョン情報
	unsigned short			mcp_version;
	unsigned short			paa_version;
};

//
//	バックアップ情報
//

struct BACKUP_DATA {		// バックアップの情報
		unsigned short	magicCode;		// バックアップ有効フラグ

		unsigned char	opeMode;		// 実行中モード
		unsigned char	rootOpeMode;	// 基本モード
		unsigned char	nowRouteCode;	// 現在処理中ルートコード
		unsigned char	nowStationName;	// 現在処理中駅名コード
		unsigned char	nowStationNo;	// 現在処理中駅名テーブルの番号
		struct STATIONINFO *Station;	// 現在のステーション情報
		unsigned char	nowEMGHL[8];	// 選択要求中非常通報器
		unsigned char	nowEMGCode;		// 選択中非常放送コード
		unsigned char	nowSPCCode;		// 選択中スペシャル放送コード

		struct {
			unsigned short	carNo:12;			// 車両番号
			unsigned char	carType:4;			// 車両タイプ
		} carInfo[8];
		unsigned char	couplingUnitCode;	// 連結種別
		unsigned char	stationNumber;		// 到着駅コード
		unsigned char	nextStationName;	// 次駅コード
		unsigned char	destination;		// 最終駅コード
		unsigned char	routeCode;			// ルートコード
		unsigned char	specialCode;		// スペシャルメッセージコード

		union {
			unsigned short	sendcode;
			unsigned char	bsend[2];
		} ev5000code;
};

//	操作モード
#define	OPEMODE_INITIAL_TEST	1
#define	OPEMODE_INITIAL_TEST2	2
#define	OPEMODE_STANDBY			3
#define	OPEMODE_IDLE			4
#define	OPEMODE_TEST			5
#define	OPEMODE_TRS				6
#define	OPEMODE_AUTO			7
#define	OPEMODE_EMGHL			8
#define	OPEMODE_EMGMA			9
#define	OPEMODE_SPCMA			10
#define	OPEMODE_SELF_TEST		11



//
//	２ポートメモリー
//

/** プログラムブート時のマップ **/
struct st_dualboot{
		unsigned long	address;		/* Boot Address */
		unsigned long	len;			/* Boot length */
		unsigned char	data[2030];		/* Boot Data */
		unsigned short	check_time;
		unsigned short	check_date;
		unsigned long	check_all_bytes;
		unsigned char	ack;			/*  0=Boot Request, 1=Initial Ack, 2=Data Ack, 3=Reboot Ack */
		unsigned char	request;		/*  0=None, 1=Initial, 2=Data, 3=Reboot */
};
#define DUALBOOT   (*(volatile struct st_dualboot *)0x0C0000) /* Dual Port Address*/

/** アプリケーション時のマップ **/
struct st_dualport{
		unsigned char	activeDVAU;		// +0 アクティブＤＶＡＵ
											// 0: エラーもしくは、起動中
											// 1: アクティブＤＶＡＵ
											// 2: 非アクティブＤＶＡＵ

		unsigned char	changeDVAU;		// +1 下位からのアクティブ要求
											// 1: Initial Test Mode
											// 2: IDLE Mode
											// 3: Test Mode
		unsigned char	activeDVAUrequest;	// +2 下位へのアクティブ切り替え要求
											// 1: Change Active
		unsigned char	couplingCheck;	// +3 連結チェックフラグ
											// 1: Check Start

		unsigned char	nowSignal1;		// +4 最新読みだし値(Signal1)
		unsigned char	nowSignal2;		// +4 最新読みだし値(Signal2)
		unsigned char	nowSignal3;		// +4 最新読みだし値(Signal3)
		unsigned char	nowTrouble;

		struct TEST_RESPONCE testResponce;		// +5 テスト状況
		unsigned char	testItemNo;				// +6 テスト中番号
													//   TestStartRequest1
													//	01h = Odd DVAU
													//		  Odd MCP
													//		  Odd ACP
													//		  Even DVAU
													//		  Even MCP
													//		  Even ACP
													//	TestStartRequest2
													//	01h = PAA1 Self
													//		  SP1
													//		  SP2
													//		  SP3
													//		  SP4
													//		  SP5
													//	02h = PAA2
													//	03h = PAA3
													//	04h = PAA4
													//	05h = PAA5
													//	06h = PAA6
													//	07h = PAA7
													//	08h = PAA8
		struct TEST_OKNG  testItemResult;		// +7
		unsigned char	testSubItemResult;			// +8 テストサブ項目フラグ(Fault)

		struct {		// 下位への通信情報
			unsigned char	dataID;			// +9 テスト開始・終了種別コード
			unsigned short	carNo;			// +10 車両番号
			unsigned char	nextStation;	// +11 次駅コード
			unsigned char	destination;	// +12 到着駅コード
			unsigned char	routeCode;		// +13 ルートコード
			union {		// ドア情報
				unsigned char	BYTE;
				struct {
					unsigned char	mcp2on:1;
					unsigned char	mcp1on:1;
					unsigned char	b5:1;
					unsigned char	b4:1;
					unsigned char	b3:1;
					unsigned char	Running:1;		// 走行中
					unsigned char	RightSide:1;	// 右ドア
					unsigned char	LeftSide:1;		// 左ドア
				} BIT;
			} doorInfo;							// +14 ドア情報
			union {		// 音声バス使用状況
				struct {
					unsigned char	BYTE[9];
				} BYTE;
				struct {
					unsigned char	mode;		// 音声モード
												//   0=CAB/CAB
												//   1=Microphone
												//   2=非常通報モード
												//   3=列車無線モード
												//   4=自動放送
					struct {
						unsigned char	b7:1;
						unsigned char	b6:1;
						unsigned char	b5:1;
						unsigned char	SP5:1;		// 非常通報器５
						unsigned char	SP4:1;		// 非常通報器４
						unsigned char	SP3:1;		// 非常通報器３
						unsigned char	SP2:1;		// 非常通報器２
						unsigned char	SP1:1;		// 非常通報器１
					} paa[8];
				} BIT;
			} voiceBus;							// +15,16,17,18,19,20,21,22,23 音声バス使用状況
			union {		// ＭＣＰ表示灯データ
				struct {
					unsigned char	BYTE[32];
				} BYTE;
				struct {
					union {
						unsigned short	BYTE;
						struct {		// スイッチＬＥＤ
							unsigned short	SPC:1;
							unsigned short	EMG:1;
							unsigned short	HL:1;
							unsigned short	ATC:1;
							unsigned short	TEST:1;
							unsigned short	RB:1;
							unsigned short	CAB:1;
							unsigned short	EMGP:1;

							unsigned short	SPC2:1;
							unsigned short	STN:1;
							unsigned short	ENT:1;
							unsigned short	RST:1;
							unsigned short	SEL:1;
							unsigned short	LEFT:1;
							unsigned short	RIGHT:1;
							unsigned short	REP:1;
						} BIT;
					} swLED;							// +24 
					unsigned char	lineDisplay[4][6];	// +25-+49 マトリックスデータ
					union {
						unsigned short	BYTE;
						struct {		// スイッチＬＥＤ点滅情報
							unsigned short	SPC:1;
							unsigned short	EMG:1;
							unsigned short	HL:1;
							unsigned short	ATC:1;
							unsigned short	TEST:1;
							unsigned short	RB:1;
							unsigned short	CAB:1;
							unsigned short	EMGP:1;

							unsigned short	SPC2:1;
							unsigned short	STN:1;
							unsigned short	ENT:1;
							unsigned short	RST:1;
							unsigned short	SEL:1;
							unsigned short	LEFT:1;
							unsigned short	RIGHT:1;
							unsigned short	REP:1;
						} BIT;
					} swBLINK;							//+50
					unsigned char	lineBlink[4];		// +51-+54 マトリックス点滅データ
				} BIT;
			} mcpDisplay;
			unsigned char	voiceLevel[8];			// +55-+63 基本音量データ
		} pollingData;		// 下位への通信データ

		struct {		// 下位からの通信データ
			unsigned short	version[16];		// 各機器のバージョン情報
			unsigned char	dataID[16];			// 各機器のテスト状況

			unsigned char	dvau1;		// 非アクティブＤＶＡＵ信号
			unsigned char	dvau2;		// 非アクティブＤＶＡＵ信号
			unsigned char	dvau3;		// 非アクティブＤＶＡＵ信号
			unsigned char	dvau_trouble;

			union MCPSW {			// ＭＣＰからのスイッチ情報
				struct {
					unsigned char	BYTE[3];
				} BYTE;
				struct {
					unsigned char	b23:1;
					unsigned char	b22:1;
					unsigned char	ADO:1;
					unsigned char	b20:1;
					unsigned char	b19:1;
					unsigned char	DIPSW2:1;
					unsigned char	DIPSW1:1;
					unsigned char	DIPSW0:1;

					unsigned char	Talk:1;
					unsigned char	STN:1;
					unsigned char	ENT:1;
					unsigned char	RST:1;
					unsigned char	SEL:1;
					unsigned char	RIGHT:1;
					unsigned char	LEFT:1;
					unsigned char	REP:1;

					unsigned char	SPC:1;
					unsigned char	EMG:1;
					unsigned char	HL:1;
					unsigned char	ATC:1;
					unsigned char	TEST:1;
					unsigned char	RB:1;
					unsigned char	CAB:1;
					unsigned char	EMGP:1;
				} BIT;
			} mcpSW[2];		// ＭＣＰからのスイッチ情報

			union {				// ＰＡＡからの非常通報の情報
				unsigned char	BYTE;
				struct {
					unsigned char	b7:1;
					unsigned char	b6:1;
					unsigned char	b5:1;
					unsigned char	RSC5:1;		// 非常通報器５
					unsigned char	RSC4:1;		// 非常通報器４
					unsigned char	RSC3:1;		// 非常通報器３
					unsigned char	RSC2:1;		// 非常通報器２
					unsigned char	RSC1:1;		// 非常通報器１
				} BIT;
			} rscSW[8];			// PAA RSC Switch (Now)
			union {				// ＰＡＡからのトラブル情報
				unsigned char	BYTE;
				struct {
					unsigned char	b7:1;
					unsigned char	SP5:1;		// スピーカー５故障
					unsigned char	SP4:1;		// スピーカー４故障
					unsigned char	SP3:1;		// スピーカー３故障
					unsigned char	SP2:1;		// スピーカー２故障
					unsigned char	SP1:1;		// スピーカー１故障
					unsigned char	AMP2:1;		// アンプＣＨ．２故障
					unsigned char	AMP1:1;		// アンプＣＨ．１故障
				} BIT;
			} trouble[8];		// PAA Trouble (Now) 
		} answerData;		// 下位からの通信情報


		struct {		// ＴＭＳからの情報
			unsigned char	Active:1;				// ＴＭＳデータ有効フラグ

			unsigned char	testExecRequest:1;		// テスト実行要求
			unsigned char	testStartRequest1:1;	// カテゴリー１テスト準備要求
			unsigned char	testStartRequest2:1;	// カテゴリー２テスト準備要求

			unsigned char	change_dbEffective:1;
			unsigned char	change_nextStationName:1;
			unsigned char	change_destination:1;
			unsigned char	change_routeCode:1;
			unsigned char	change_specialCode:1;
			unsigned char	change_atcActive:1;
			unsigned char	change_broadCastOff:1;
			unsigned char	change_doorOpen:1;

			unsigned char	itemNo;				// テスト番号

			unsigned short	carNo:12;			// 車両番号
			unsigned char	carType:4;			// 車両タイプ
			unsigned char	couplingUnitCode;	// 連結種別
			unsigned char	stationNumber;		// 駅名コード
			unsigned char	nextStationName;	// 次駅コード
			unsigned char	destination;		// 最終駅コード
			unsigned char	routeCode;			// ルートコード
			unsigned char	talkBack[8];		// 非常通報器選択
			unsigned char	specialCode;		// スペシャルメッセージコード

			unsigned char	dbEffective:1;		// データ有効
			unsigned char	atcActive:1;		// 自動放送起動要求
			unsigned char	broadCastOff:1;		// 自動放送禁止
			unsigned char	doorOpenEffective:1;	// ドア状態変化
			unsigned char	doorOpen:2;			// ドアオープン状態
		} tms_request;		// ＴＭＳからの受信情報

		struct {		// ＴＭＳへの情報
			unsigned char	passengerAlarm[8];
			union TMS_SD_COMMERR dvaCommError;
			union TMS_SD_STATUS dvaStatus;
		} tms_answer;		// ＴＭＳへの送信情報
			
		struct {		// ＥＶ５０００への情報
			unsigned char	Initial:1;		// 初期設定要求
			unsigned char	Request:1;		// 放送要求
			unsigned char	Abort:1;		// 放送キャンセル要求
			unsigned char	Complete:1;		// 放送終了フラグ
			unsigned char	Aborted:1;		// 放送キャンセル終了
			unsigned char	Repeat:1;		// リピート応答中
			unsigned char	ev5000_error;	// エラーコード
											//   0:Normal, 1:Timeout, 2:Error Responce
			union EV5000_SEQUENCE *Sequence;	// 放送シーケンスコード
		} ev5000_request;

		struct {	// ＥＩＤＳ送信情報
			unsigned char	Message;		// メッセージコード
			unsigned char	Door;			// ドア状態コード
			unsigned char	Route;			// ルートコード
			unsigned char	Station;		// 駅名コード
		} eids;		// ＥＩＤＳ送信情報

		struct {	// ＭＣＰ／ＡＣＰコントロール状態
			unsigned char	mcpManual:1;	// ＭＣＰによるマニュアルモード中
			unsigned char	mcpCABOn:1;		// ＭＣＰのＣＡＢ要求中
			unsigned char	Cab:1;			// ＣＡＢ／ＣＡＢモード中
			unsigned char	acpCAB:1;		// ＡＣＰのＣＡＢスイッチの状態
			unsigned char	acpSTN:1;		// ＡＣＰのＳＴＮスイッチの状態
			unsigned char	acpREP:1;		// ＡＣＰのＲＥＰスイッチの状態
			unsigned char	Release:1;		// マニュアルモード・ＣＡＢ／ＣＡＢモードからの復帰
			unsigned char	MANEnable:1;	// マニュアルモード・ＣＡＢ／ＣＡＢモード受付許可
			unsigned char	ATCEnable:1;	// ＡＴＣスイッチ許可中（自動放送許可中）
			unsigned char	ATCOnEnable:1;	// ＡＴＣ放送開始要求
			unsigned char	ATCOn:1;		// 自動放送要求
			unsigned char	ATCOff:1;		// 自動放送解除
			unsigned char	STNEnable:1;	// 次駅スイッチ許可中
			unsigned char	STNOn:1;		// 次駅放送要求
			unsigned char	REPEnable:1;	// リピート放送許可中
			unsigned char	REPOn:1;		// リピート放送要求
			unsigned char	EMGPOn:1;		// 非常通報の要求
			unsigned char	INUSE:1;		// １行目を使用中
			unsigned char	EMGPHL[8];		// 非常通報器の選択番号
			unsigned char	EMGOn:1;		// 非常放送の放送要求
			unsigned char	SPCOn:1;		// スペシャル放送の放送要求
			unsigned char	EMGCode;		// 非常放送・スペシャル放送コード
			unsigned char	EMGRepeat:1;	// ＥＭＧリピート放送中
			unsigned char	test:1;			// セルフテスト要求
			unsigned char	trsBUSY:1;		// 列車無線放送中
			unsigned char	testRelease:1;	// テスト終了要求
		} mcpMode;

		unsigned char	download_active;
		unsigned char	download_seq;
		unsigned char	download_filename[16];	// ダウンロードファイル名
		unsigned short	download_date;			// 日時情報
		unsigned short	download_time;
		unsigned short	download_size;			// ダウンロードデータサイズ
		unsigned char	download_data[512];		// ダウンロードデータ

		unsigned char	upload_active;
		unsigned char	upload_seq;
		unsigned char	upload_filename[16];	// アップロードファイル名
		unsigned short	upload_date;			// 日時情報
		unsigned short	upload_time;
		unsigned short	upload_size;			// アップロードデータサイズ
		unsigned char	upload_data[512];		// アップロードデータ

		unsigned char	dummy[774];

		unsigned char	subActive;		// サブＣＰＵ起動中
		unsigned char	mainActive;		// メインＣＰＵ起動中

		unsigned char	ack;
		unsigned char	request;
};


#define DUALPORT   (*(volatile struct st_dualport *)0x0C0000) /* Dual Port Address*/

#endif
