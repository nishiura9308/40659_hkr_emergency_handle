
struct TIMER_CONTROL {
	int		dateActive;			// 日時有効フラグ

	unsigned char	tm_year;
	unsigned char	tm_mon;
	unsigned char	tm_day;
	unsigned char	tm_hour;
	unsigned char	tm_min;
	unsigned char	tm_sec;
	unsigned char	tm_msec;

	int		checkTimer;			// プロセス用タイマー
	int		tmsActiveTimer;		// ＴＭＳデータ有効タイマー
	int		ev5000timer;		// ＥＶ５０００用タイマー
	int		commGateTimer;		// 連結通信チェックタイマー

	int		testExecTimer;		// テスト時間

	int		testDispTimer;		// テストエラー表示間隔

	int		subActiveTimer;		// サブＣＰＵチェック間隔

	int		acpBlink;			// ＡＣＰ　ＬＥＤブリンク間隔

	int		menteSendTimer;		// メンテナンス連続送信ディレイ
};

#define	TMS_OFFLINE_TIME	500		// ＴＭＳとの通信が異常となるまでの時間

