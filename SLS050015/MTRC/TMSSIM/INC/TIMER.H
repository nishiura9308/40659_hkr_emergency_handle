
struct TIMER_CONTROL {
	unsigned char	tm_year;
	unsigned char	tm_mon;
	unsigned char	tm_day;
	unsigned char	tm_hour;
	unsigned char	tm_min;
	unsigned char	tm_sec;
	unsigned char	tm_msec;

	short		tms_timer;
	short		tmstimeout;

	short	testExecTimer;
	short	testTimeTimer;
	short	testBroadcastTimer;

	int		menteSendTimer;		// メンテナンス連続送信ディレイ
};

