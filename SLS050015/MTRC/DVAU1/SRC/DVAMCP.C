/********************************************************************
	【香港地下鉄向け自動列車放送制御装置】
*-------------------------------------------------------------------*
	ＭＣＰ操作モジュール
*-------------------------------------------------------------------*
	Author		TOA Corporation

	Revision	a00. 2000/07/27		K.Honda

				a04. 2003/04/01		h.nisiura   スペシャル放送文コードが０のものも選択から外すように修正
												放送分選択をサイクリックに変更
				a05. 2005/07/12		h.nisiura   非常ハンドルの
												表示文字をＰＥＡＨ→ＰＡＤへ変更。
												非常ハンドルの物理位置と表示位置を
												ＰＡＡアドレスにより反転する処理を追加。

*********************************************************************/

#include	<stdio.h>
#include    <machine.h>

#include    "iodefine.h"
#include    "table.h"
#include    "trace.h"
#include    "timer.h"

/*@@@S*/
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	グローバル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル定数定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
	// キー操作画面定義
#define	MCP_RESET		0		// ＭＣＰ表示リセット
#define	MCP_IDLE		1		// ＭＣＰアイドル状態
#define	MCP_LINE_SELECT		2		// ライン選択中
#define	MCP_STATION_SELECT		3	// 駅名選択中
#define	MCP_LANG_SELECT		4		// 言語選択中１
#define	MCP_LANG_SELECT2	5		// 言語選択中２
#define	MCP_EMGP1		6		// 非常ハンドル選択中１
#define	MCP_EMGP2		7		// 非常ハンドル選択中２
#define	MCP_EMG1		8		// 非常放送選択中１
#define	MCP_EMG2		9		// 非常放送選択中２
#define	MCP_SPC1		10		// スペシャル放送選択中１
#define	MCP_SPC2		11		// スペシャル放送選択中２
#define	MCP_TEST_SELECT	12			// セルフテスト選択中１
#define	MCP_TEST_SELECT2	13		// セルフテスト選択中２

	// キーコード
#define	MCPSW_TALK		1
#define	MCPSW_STN		2
#define	MCPSW_ENT		3
#define	MCPSW_RST		4
#define	MCPSW_SEL		5
#define	MCPSW_RIGHT		6
#define	MCPSW_LEFT		7
#define	MCPSW_REP		8
#define	MCPSW_SPC		9
#define	MCPSW_EMG		10
#define	MCPSW_HL		11
#define	MCPSW_ATC		12
#define	MCPSW_TEST		13
#define	MCPSW_RB		14
#define	MCPSW_CAB		15
#define	MCPSW_EMGP		16
#define	MCPSW_LANG		17

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカル構造体定義
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
	ローカルテーブル
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/


/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		外部参照
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/
extern	void dvaIOOut( unsigned long onbit, unsigned long offbit );


// ルート検索
extern	struct ROUTEINFO *dvaSearchRoute( unsigned char routeCode );
// 駅名検索
extern	struct STATIONINFO *dvaSearchStation( unsigned char routeCode, unsigned char stationCode );
// 最終駅検索
extern	struct STATIONINFO *dvaSearchDestination( unsigned char routeCode );

extern	struct	BACKUP_DATA		backup_data;		// バックアップデータ
extern	struct	CONFIG_INFO		config_data;		// 設定情報
extern	struct	ANNOUNCE_INFO	announce_data;		// 放送設定情報
extern	struct	ROUTEINFO		route;				// 駅情報

extern	struct st_ReadSIGNAL onSignal;			// ＯＮになった信号
extern	struct st_ReadSIGNAL nowSignal;			// 最新読みだし値
extern	struct	TIMER_CONTROL	timedata;		// タイマー情報

extern	int		ReturnAutoMode;		// 2001/07/02 自動放送モードへ戻るフラグ
/*・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・
		ローカル変数
・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・・*/

static	int		mcp_phase;				// 処理フェーズ
static	unsigned char	acceptSW;		// 受け付けたキーコード

static	unsigned char	now_rscSW_row[16*5];
static	unsigned char	now_rscSW_col[16*5];
static	unsigned char	select_rscSW_row;		// 非常通報器番号（ＰＡＡ番号）
static	unsigned char	select_rscSW_col;		// 非常通報器番号（ハンドル番号）
static	unsigned char	select_emgCode;			// 選択中非常放送コード
static	unsigned char	select_spcCode;			// 選択中スペシャル放送コード
static	unsigned char	select_routeCode;		// 選択中ルートコード
static	struct ROUTEINFO	*select_routeData;	// 選択中ルートデータ
static	unsigned char	select_station;			// 選択中ステーション番目
static	struct SELECT_LANG	select_lang;		// 選択中言語
static	int		item_skip;						// 選択番号

static	unsigned char	select_rscSW_row2;		// 非常通報器番号（ＰＡＡ番号）
static	unsigned char	select_rscSW_col2;		// 非常通報器番号（ハンドル番号）

/*@@@E*/
/************************************************************************

	Function name	<< dvaMCPInitial >>
					ＭＣＰ表示処理の初期設定
	Syntax			dvaMCPInitial( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/07/27

*************************************************************************/
void dvaMCPInitial( void )
{
		acceptSW = 0;			// 処理中キーコードなし
		mcp_phase = MCP_RESET;		// 画面初期設定
		DUALPORT.mcpMode.mcpCABOn=0;	// ＣＡＢ／ＣＡＢ受付中ＯＦＦ
		DUALPORT.mcpMode.Cab = 0;
		DUALPORT.mcpMode.mcpManual = 0;
}

/************************************************************************

	Function name	<< dvaMCPDisplaySet >>
					ＭＣＰへ表示情報を設定
	Syntax			dvaMCPDisplaySet( line, strings );
	Argument		char line     : i : 表示行番号(0-3)
				char *strings : i : 表示文字（６文字固定）
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
	Revision		a01 2000/09/07   Compiler Bugfix

*************************************************************************/
void dvaMCPDisplaySet( char line, char *strings )
{
		int		i;

		for( i=0; i<6; i++ ){
			DUALPORT.pollingData.mcpDisplay.BIT.lineDisplay[line][i] = *strings++;
		}
//		memcpy( DUALPORT.pollingData.mcpDisplay.BIT.lineDisplay[line], strings, 6 );
}

/************************************************************************

	Function name	<< MCPDisplayBlank >>
					ＭＣＰへブランクを設定
	Syntax			MCPDisplayBlank( line );
	Argument		char line : i : ブランクにする行番号(0-3)
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09

*************************************************************************/
static	void MCPDisplayBlank( char line )
{
		dvaMCPDisplaySet( line, "      " );
}

/************************************************************************

	Function name	<< dvaMCPGetEMGP >>
					非常ハンドルの順序取り込み
	Syntax			ret = dvaMCPGetEMGP( void );
	Argument		int ret : o : 1 = 新規発生
					      0 = 変化なし
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09

*************************************************************************/
int dvaMCPGetEMGP( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非常ハンドル番号を取り込む
　・非常ハンドルの発生順に now_rscSW_row[] と now_rscSW_col[] へ設定する
	------------------------------------------------------------------[@@@E]*/
		int		i,j,k,m;
		int		first;

		first = 0;
		for( i=0; i<16; i++ ){
			for( j=0; j<5; j++ ){
				if( DUALPORT.answerData.rscSW[i].BYTE & (0x0001<<j) ){	// 非常ハンドルＯＮ？
					for( k=0; k<16*5; k++ ){
						if( now_rscSW_row[k]==i+1 && now_rscSW_col[k]==j+1 ){
							break;		// すでに登録済み
						}
						if( now_rscSW_row[k]==0 || now_rscSW_col[k]==0 ){	// 未登録エリア？
							now_rscSW_row[k]=i+1;		// 非常ハンドル番号を登録する
							now_rscSW_col[k]=j+1;
							first = 1;	// 新規登録フラグＯＮ
							break;
						}
					}
				}
				else {		// 非常ハンドルＯＦＦ
					for( k=0; k<16*5; k++ ){
						if( now_rscSW_row[k]==i+1 && now_rscSW_col[k]==j+1 ){	// 登録済みのときは
							for( m=k+1; m<16*5; m++ ){		// 前に詰める
								now_rscSW_row[m-1] = now_rscSW_row[m];
								now_rscSW_col[m-1] = now_rscSW_col[m];
								if( now_rscSW_row[m]==0 || now_rscSW_col[m]==0 ){
									now_rscSW_row[m] = 0;
									now_rscSW_col[m] = 0;
								}
							}
							first = 1;	// 新規解除フラグＯＮ
							break;
						}
					}
				}
			}
		}

		return( first );
}

/************************************************************************

	Function name	<< dvaMCPOpeEMGPDisplay >>
					ＭＣＰへ非常通報器を表示
	Syntax			ret = dvaMCPOpeEMGPDisplay( skip );
	Argument		char skip   : i : 表示を飛ばす行数
				int  ret    : o : 表示できなかった行数
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
					a01 2005/07/12 h.nisiura 表示文字をＰＥＡＨ→ＰＡＤへ変更。
											 非常ハンドルの物理位置と表示位置を
											 ＰＡＡアドレスにより反転する処理を追加。
*************************************************************************/
extern	const char	*TalkBackTable[][5];
int dvaMCPOpeEMGPDisplay( char skip )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非常ハンドル番号を表示する
　・非常ハンドルは dvaMCPGetEMGPで受け付けたデータを使用する
	------------------------------------------------------------------[@@@E]*/
		int		i;
		int		j;
		int		k;
		int		ttl;
		char	work[16];
		char	*p;
		int		tmpCarSel;

		select_rscSW_row2 = 0;	// 選択中ＰＡＡ番号
		select_rscSW_col2 = 0;	// 選択中非常ハンドル番号
		k=0;
		ttl = 0;
		for( i=0; i<16*5; i++ ){
			if( now_rscSW_row[i]!=0 || now_rscSW_col[i]!=0 ){	// 非常ハンドルあり
				ttl++;
				if( skip==0 ){
					if( k==0 ){
						select_rscSW_row = now_rscSW_row[i];	// 最初の非常ハンドル
						select_rscSW_col = now_rscSW_col[i];
					}
					if( k<2 ){
						select_rscSW_row2 = now_rscSW_row[i];	// 選択している非常ハンドル
						select_rscSW_col2 = now_rscSW_col[i];

						// ＰＡＡと非常ハンドル番号からハンドル表示を設定
						p = work;
						if( now_rscSW_row[i]<9 ){
							*p++ = *TalkBackTable[now_rscSW_row[i]-1][0];
							p += sprintf( p, "%03X", backup_data.carInfo[now_rscSW_row[i]-1].carNo );
							tmpCarSel = now_rscSW_row[i]-1;
						}
						else {
							*p++ = *TalkBackTable[now_rscSW_row[i]-9][0];
							p += sprintf( p, "%03X", DUALPORT.answerData.carInfo[now_rscSW_row[i]-9].carNo );
							tmpCarSel = now_rscSW_row[i]-9;
						}
						*p++ = '*';
						
						switch( tmpCarSel )			// 非常ハンドルの物理位置と表示位置を
						{					 		// ＰＡＡアドレスにより反転する
						case 0:
						case 1:
						case 3:
						case 5:
							p += sprintf( p, "%u", now_rscSW_col[i] );
							break;
						case 2:
						case 4:
						case 6:
						case 7:
							if( now_rscSW_col[i] < 6 )
							{
								p += sprintf( p, "%u", 6-now_rscSW_col[i] );
							}
							else
							{
								p += sprintf( p, "%u", now_rscSW_col[i] );
							}
							break;
						}

						dvaMCPDisplaySet( k+2, work );
						k++;
					}
				}
				if( skip )
					skip--;
			}
		}

		if( k<2 ){
			MCPDisplayBlank( k+2 );		// ２行目以降がなければブランクで埋める
		}

		sprintf( work, "PAD%2u ", ttl );	// 非常ハンドル数を表示
		dvaMCPDisplaySet( 0, work );
		DUALPORT.mcpMode.INUSE = 1;		// １行目使用中

		if( k==0 )
			return( -1 );

		return( skip );
}

/************************************************************************

	Function name	<< allspace >>
					非常放送・スペシャル放送の放送分未定義判定
	Syntax			ret = allspace( mcpChar );
	Argument		char mcpChar : i : 表示データ
				int  ret     : o : 1 = すべて空白
						　 0 = 表示文字あり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/09/23

*************************************************************************/
static	int allspace( char mcpChar[6] )
{
		int		i;

		for( i=0; i<6; i++ ){
			if( mcpChar[i]!='\0' && mcpChar[i]!=' ' )
				return 0;		// 表示文字がある
		}
		return 1;	// 表示文字がない
}

/************************************************************************

	Function name	<< allempty >>
					非常放送・スペシャル放送の放送分未定義判定
	Syntax			ret = allempty( sequence );
	Argument		union EV5000_SEQUENCE sequence : i : 放送定義
				int  ret     : o : 1 = 放送言語なし
						　 0 = 放送言語あり
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/11/13

*************************************************************************/
static	int allempty( union EV5000_SEQUENCE	sequence[MAX_SEQUENCE] )
{
		int		i;

		for( i=0; i<MAX_SEQUENCE; i++ ){
			if( sequence[i].sendRequest.en )
				return 0;		// 英語放送あり
			if( sequence[i].sendRequest.cn )
				return 0;		// 広東語放送あり
			if( sequence[i].sendRequest.ma )
				return 0;		// マンダリン放送あり
		}
		return 1;	// 放送文なし
}

/************************************************************************

	Function name	<< allscodempty >>
					非常放送・スペシャル放送の放送分コード未定義判定
	Syntax			ret = allscodeempty( sequence );
	Argument		union EV5000_SEQUENCE sequence : i : 放送定義
				int  ret     : o : 1 = 放送言語なし
						　 0 = 放送言語あり
	Author			TOA Corporation  h.nishiura
	Revision		a00 2002/12/10

*************************************************************************/
static	int allscodeempty( union EV5000_SEQUENCE	sequence[MAX_SEQUENCE] )
{
		int		i;

		for( i=0; i<MAX_SEQUENCE; i++ ){
			if( sequence[i].sendRequest.sendCode )
				return 0;
		}
		return 1;	// 放送文なし
}

/************************************************************************

	Function name	<< dvaMCPOpeEMGDisplay >>
					ＭＣＰへ非常放送選択を表示
	Syntax			ret = dvaMCPOpeEMGDisplay( skip );
	Argument		char skip : i : 表示を飛ばす行数
				int ret   : o : 0 固定
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/09/07
	Revision		a01 2000/11/13	放送文がないときは選択できない
					a02 2002/12/10  放送文コードが０のものも選択から外すように修正
									放送分選択をサイクリックに変更
*************************************************************************/
int dvaMCPOpeEMGDisplay( char skip )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・非常放送を表示する
	------------------------------------------------------------------[@@@E]*/
		int		i,j;

		if( skip==0 ){		// 最初の表示は、選択中非常放送文の表示
			dvaMCPDisplaySet( 1, announce_data.emgma[select_emgCode].mcpChar2 );
			dvaMCPDisplaySet( 2, announce_data.emgma[select_emgCode].mcpChar3 );
		}
		if( skip>0 ){		// 次の放送文を選択表示
			if( select_emgCode+1 == MAX_EMGMA )		i=0;
			else									i=select_emgCode+1;
			for( j=0 ; j<MAX_EMGMA-1; j++ ){
				if( (allspace( announce_data.emgma[i].mcpChar2 )==0 
					|| allspace( announce_data.emgma[i].mcpChar3 )==0)
					&& allempty( announce_data.emgma[i].sequence )==0 
					&& allscodeempty( announce_data.emgma[i].sequence )==0 ){	// 表示と放送が定義されていれば選択対象
					select_emgCode = i;
					dvaMCPDisplaySet( 1, announce_data.emgma[select_emgCode].mcpChar2 );
					dvaMCPDisplaySet( 2, announce_data.emgma[select_emgCode].mcpChar3 );
					break;
				}
				i++;
				if( i==MAX_EMGMA ) i=0;
			}
		}
		if( skip<0 ){		// 前の放送文を選択表示
			if( select_emgCode == 0 )	i=MAX_EMGMA-1;
			else						i=select_emgCode-1;
			for( j=0 ; j<MAX_EMGMA-1; j++ ){
				if( (allspace( announce_data.emgma[i].mcpChar2 )==0 
					|| allspace( announce_data.emgma[i].mcpChar3 )==0)
					&& allempty( announce_data.emgma[i].sequence )==0 
					&& allscodeempty( announce_data.emgma[i].sequence )==0 ){	// 表示と放送が定義されていれば選択対象
					select_emgCode = i;
					dvaMCPDisplaySet( 1, announce_data.emgma[select_emgCode].mcpChar2 );
					dvaMCPDisplaySet( 2, announce_data.emgma[select_emgCode].mcpChar3 );
					break;
				}
				i--;
				if( i<0 ) i=MAX_EMGMA-1;
			}
		}

		dvaMCPDisplaySet( 0, "EMG-MA" );
		DUALPORT.mcpMode.INUSE = 1;	// １行目は使用中

		return( 0 );
}

/************************************************************************

	Function name	<< dvaMCPOpeSPCDisplay >>
					ＭＣＰへスペシャル放送選択を表示
	Syntax			ret = dvaMCPOpeSPCDisplay( skip );
	Argument		char skip : i : 表示を飛ばす行数
				int ret   : o : 0 固定
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/09/07
	Revision		a01 2000/11/13	放送文がないときは選択できない
					a02 2001/03/22  最終コードを超えて選択していた
					a03 2002/12/11  放送文コードが０のものも選択から外すように修正
									放送分選択をサイクリックに変更
*************************************************************************/
int dvaMCPOpeSPCDisplay( char skip )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・スペシャル放送を表示する
	------------------------------------------------------------------[@@@E]*/
		int		i,j;

		if( skip==0 ){		// 最初の表示は、選択中スペシャル放送文の表示
			dvaMCPDisplaySet( 1, announce_data.spcma[select_spcCode].mcpChar2 );
			dvaMCPDisplaySet( 2, announce_data.spcma[select_spcCode].mcpChar3 );
		}
		if( skip>0 ){		// 次の放送文を表示
			if( select_spcCode+1 == MAX_SPCMA-1 )	i=0;
			else									i=select_spcCode+1;
			for( j=0 ; j<MAX_SPCMA-2; j++ ){
				if( (allspace( announce_data.spcma[i].mcpChar2 )==0 
					|| allspace( announce_data.spcma[i].mcpChar3 )==0)
					&& allempty( announce_data.spcma[i].sequence )==0 
					&& allscodeempty( announce_data.spcma[i].sequence )==0 ){	// 表示と放送が定義されていれば選択対象
					select_spcCode = i;
					dvaMCPDisplaySet( 1, announce_data.spcma[select_spcCode].mcpChar2 );
					dvaMCPDisplaySet( 2, announce_data.spcma[select_spcCode].mcpChar3 );
					break;
				}
				i++;
				if( i==MAX_SPCMA-1 ) i=0;
			}
		}
		if( skip<0 ){		// 前の放送文を表示
			if( select_spcCode == 0 )	i=MAX_SPCMA-2;
			else						i=select_spcCode-1;
			for( j=0 ; j<MAX_SPCMA-2; j++ ){
				if( (allspace( announce_data.spcma[i].mcpChar2 )==0 
					|| allspace( announce_data.spcma[i].mcpChar3 )==0)
					&& allempty( announce_data.spcma[i].sequence )==0 
					&& allscodeempty( announce_data.spcma[i].sequence )==0 ){	// 表示と放送が定義されていれば選択対象
					select_spcCode = i;
					dvaMCPDisplaySet( 1, announce_data.spcma[select_spcCode].mcpChar2 );
					dvaMCPDisplaySet( 2, announce_data.spcma[select_spcCode].mcpChar3 );
					break;
				}
				i--;
				if( i<0 ) i=MAX_SPCMA-2;
			}
		}

		dvaMCPDisplaySet( 0, "SPC-MA" );
		DUALPORT.mcpMode.INUSE = 1;	// １行目を使用中

		return( 0 );
}

/************************************************************************

	Function name	<< dvaMCPOpeLineDisplay >>
					ＭＣＰへライン選択を表示
	Syntax			dvaMCPOpeLineDisplay( routeCode, stationCode, skip );
	Argument		unsigned char *routeCode  : i/o : 選択中ルートコード
				unsigned char stationCode : i   : 選択中駅名順番
				char skip                 : i   : 表示を飛ばす行数
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09

*************************************************************************/
int dvaMCPOpeLineDisplay( unsigned char *routeCode, unsigned char stationCode, char skip )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・ラインの選択表示する
	------------------------------------------------------------------[@@@E]*/
		int		i;
		char	direct;
		unsigned char	route;
		struct ROUTEINFO	*routeData;

		// 有効なルートを探す
		direct = skip;
		route = *routeCode;
		for( i=0; i<MAX_ROUTE; i++ ){
			routeData = dvaSearchRoute( route );	// ルート検索
			if( routeData!=NULL ){			// ルートあり
				if( skip==0 )			// このルートが表示対象
					break;
				if( skip>0 ){			// 次のルートを検索
					skip--;
				}
				if( skip<0 ){			// 前のルートを検索
					skip++;
				}
			}
			if( direct>=0 ){
				route++;
				stationCode = 0;	// ルートが変更されたので表示する駅は最初となる
			}
			else {
				route--;
				stationCode = 0;	// ルートが変更されたので表示する駅は最初となる
			}
			if( route==0 ){			// ルートが最初になれば
				route = 1;		// ルートを０１とし
				stationCode = 0;	// 駅を最初にする
			}
		}

		if( routeData!=NULL ){		// ルートがあれば、表示する
			select_routeData = routeData;
			*routeCode = route;
			dvaMCPDisplaySet( 0, (char *)routeData->mcpRouteChar );
			dvaMCPDisplaySet( 1, (char *)routeData->station[stationCode].mcpStationChar );
			DUALPORT.mcpMode.INUSE = 1;	// １行目使用中
			return( 0 );		// ルートあり
		}

		return( 1 );
}

/************************************************************************

	Function name	<< dvaMCPOpeLangDisplay >>
					ＭＣＰへ言語選択を表示
	Syntax			ret = dvaMCPOpeLangDisplay( lang, adv );
	Argument		struct SELECT_LANG *lang : i/o : 放送言語ビット
				int adv                  : i : 選択方向
				int ret                  : o : 常に１
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09

*************************************************************************/
int dvaMCPOpeLangDisplay( struct SELECT_LANG *lang, int adv )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・放送言語の選択表示する
	------------------------------------------------------------------[@@@E]*/
		int		i;
		int		j;
		int		k;
		int		mode;

		// 現在の状況をチェック
		if( lang->en ){
			if( lang->cn ){
				if( lang->ma ){
					mode = 3;		// E-C-M
				}
				else {
					mode = 1;		// EN-CN
				}
			}
			else if( lang->ma ){
				mode = 2;			// EN-MA
			}
			else {
				mode = 0;			// CN-MA
			}
		}
		else {
			mode = 0;				// CN-MA
		}

		// 次の選択を決定
		mode += adv;
		if( mode>3 )
			mode=3;
		if( mode<0 )
			mode=0;

		// 選択後の表示
		switch( mode ){
		case 0:		/* CN-MA */
			lang->en = 0;
			lang->cn = 1;
			lang->ma = 1;
			dvaMCPDisplaySet( 2, "CN-MA " );
			break;
		case 1:		/* EN-CN */
			lang->en = 1;
			lang->cn = 1;
			lang->ma = 0;
			dvaMCPDisplaySet( 2, "EN-CN " );
			break;
		case 2:		/* EN-MA */
			lang->en = 1;
			lang->cn = 0;
			lang->ma = 1;
			dvaMCPDisplaySet( 2, "EN-MA " );
			break;
		default:
			lang->en = 1;
			lang->cn = 1;
			lang->ma = 1;
			dvaMCPDisplaySet( 2, "E-C-M " );
			break;
		}

		dvaMCPDisplaySet( 0, "LANG  " );
		DUALPORT.mcpMode.INUSE = 1;

		return( 1 );
}

/************************************************************************

	Function name	<< dvaMCPCab >>
					ＭＣＰによるＣＡＢモード処理
	Syntax			dvaMCPCab( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
					a01 2001/02/15 音声バスモードを変更

*************************************************************************/
void dvaMCPCab( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・運転席間連絡の設定を行う
	------------------------------------------------------------------[@@@E]*/
		int		i,j;

		dvaIOOut( 
			OUTSIGNAL_audioBusDisable		// 車内放送禁止
			| OUTSIGNAL_trsDisable			// 列車無線起動停止
			,
			OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
			| OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
			);

		// 音声バス使用状況は　ＣＡＢ／ＣＡＢ
		DUALPORT.pollingData.voiceBus.BIT.mode = 0x00;		// Active MCP

		// ＣＡＢスイッチＬＥＤを点灯
		for( i=0; i<4; i++ ){
			for( j=0; j<3; j++ ){
				DUALPORT.answerData.mcpSW[i].BYTE.BYTE[j] = 0;		// MCP Switch OFF
			}
		}
		DUALPORT.pollingData.mcpDisplay.BIT.swLED.BYTE = 0;	// Switch LED OFF
		DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BYTE = 0;	// Switch LED Blink OFF
		for( i=0; i<4; i++ ){
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[i]=0x00;	// Matrix LED Blink OFF
		}

		DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.CAB=1;
		DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.CAB=0;
			// ＭＣＰへＣＡＢ／ＣＡＢ放送中を表示
		dvaMCPDisplaySet( 0, "CA/CA " );

			// 自動放送をキャンセル要求
		DUALPORT.ev5000_request.Abort = 1;

		// 元の状態への復帰要求の解除
		DUALPORT.mcpMode.Release = 0;	
			// ＣＡＢ／ＣＡＢモードの設定
		DUALPORT.mcpMode.Cab = 1;

		DUALPORT.tms_answer.dvaStatus.BYTE = 0;
		DUALPORT.tms_answer.dvaStatus.BIT.manualMode = 0;
		DUALPORT.tms_answer.dvaStatus.BIT.cabMode = 1;

		TraceAdd( EVENT_CAB );

		mcp_phase = MCP_IDLE;
}

/************************************************************************

	Function name	<< dvaMCPManual >>
					ＭＣＰ／ＡＣＰによるマニュアルモード処理
	Syntax			dvaMCPManual( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
				a01 2001/03/22	スピーカー制御
				a02 2001/07/04　ＥＩＤＳへのリセット信号の出力

*************************************************************************/
void dvaMCPManual( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・マイク放送の設定を行う
	------------------------------------------------------------------[@@@E]*/
		int		i,j;

		dvaIOOut( 
			OUTSIGNAL_trsDisable			// 列車無線起動停止
			,
			OUTSIGNAL_audioBusDisable		// 車内放送中
			| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
			| OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
			| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＦＦ
			| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＦＦ
			);

			// 音声バス使用状況は　マイク放送
		DUALPORT.pollingData.voiceBus.BIT.mode = 1;		
			// ＭＣＰへマイク放送中を表示
			for( i=0; i<4; i++ ){
				for( j=0; j<3; j++ ){
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[j] = 0;		// MCP Switch OFF
				}
			}
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BYTE = 0;	// Switch LED OFF
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BYTE = 0;	// Switch LED Blink OFF
			for( i=0; i<4; i++ ){
				DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[i]=0x00;	// Matrix LED Blink OFF
			}

		dvaMCPDisplaySet( 0, "MANUAL" );

		// 自動放送をキャンセル要求
		DUALPORT.ev5000_request.Abort = 1;

		// 元の状態への復帰要求の解除
		DUALPORT.mcpMode.Release = 0;	

		DUALPORT.tms_answer.dvaStatus.BYTE = 0;
		DUALPORT.tms_answer.dvaStatus.BIT.manualMode = 1;
		DUALPORT.tms_answer.dvaStatus.BIT.cabMode = 0;

		DUALPORT.eids.Reset = 1;			// リセット信号アクティブ 2001/07/04

		TraceAdd( EVENT_MAN );

		mcp_phase = MCP_IDLE;
}

/************************************************************************

	Function name	<< dvaMCPRelease >>
					ＭＣＰ／ＡＣＰによる
					ＣＡＢ／ＣＡＢマニュアルモード解除処理
	Syntax			dvaMCPRelease( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
				a01 2001/03/22	スピーカー制御

*************************************************************************/
//	車内放送・ＡＣＰ／ＭＣＰ音声不可
void dvaMCPRelease( void )
{
/*. --------------------------【機能詳細】----------------------------[@@@S]
　・運転席間放送または、マイク放送を元に戻す設定を行う
	------------------------------------------------------------------[@@@E]*/
		dvaIOOut( 
			0
			,
			OUTSIGNAL_audioBusDisable		// 車内放送中
			| OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
			| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
			);

			// ＣＡＢスイッチＬＥＤを消灯
		DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.CAB=0;
		DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.CAB=0;

			// 自動放送をキャンセル要求を解除
		DUALPORT.ev5000_request.Abort = 0;

			// ＣＡＢ／ＣＡＢモードの解除
		DUALPORT.mcpMode.Cab = 0;
			// ＭＣＰによるマニュアルモードを解除
		DUALPORT.mcpMode.mcpManual = 0;	
			// 元の状態への復帰要求
		DUALPORT.mcpMode.Release = 1;

		DUALPORT.tms_answer.dvaStatus.BIT.manualMode = 0;
		DUALPORT.tms_answer.dvaStatus.BIT.cabMode = 0;

		mcp_phase = MCP_RESET;

		TraceAdd( EVENT_REL );
}

/************************************************************************

	Function name	<< getMCPSW >>
					ＭＣＰスイッチの受け付け処理
	Syntax			union MCPSW getMCPSW( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/09/23
				a01 2001/03/22	スピーカー制御

*************************************************************************/
void getMCPSW( void )
{
		int		i;
		union MCPSW	swdata;
		union st_READSignal3 signal3;
		struct ROUTEINFO	*routeData;

		acceptSW = 0;

		signal3.BYTE = nowSignal.SIGNAL3.BYTE;			// 自分のＡＣＰスイッチ
		if( nowSignal.SIGNAL2.RBIT.hcrON ){
			signal3.BYTE |= DUALPORT.answerData.dvau3 & 0x22;	// 相手のＡＣＰスイッチ（ＣＡＢと音声信号）
			signal3.BYTE |= DUALPORT.answerData.otherCar[0].dvau3 & 0x22;	// 別車両のＡＣＰスイッチ（ＣＡＢと音声信号）
			signal3.BYTE |= DUALPORT.answerData.otherCar[1].dvau3 & 0x22;	// 別車両のＡＣＰスイッチ（ＣＡＢと音声信号）
		}
		else {
			if( DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError1==0
				&& DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError2==0 ){
				signal3.BYTE |= DUALPORT.answerData.dvau3 & 0x22;		// 相手のＡＣＰスイッチ
				signal3.BYTE |= DUALPORT.answerData.otherCar[0].dvau3 & 0x22;	// 別車両のＡＣＰスイッチ（ＣＡＢと音声信号）
				signal3.BYTE |= DUALPORT.answerData.otherCar[1].dvau3 & 0x22;	// 別車両のＡＣＰスイッチ（ＣＡＢと音声信号）
			}
			else if( DUALPORT.tms_answer.dvaCommError.BIT.mcpCommError1!=0 || DUALPORT.tms_answer.dvaCommError.BIT.mcpCommError2!=0 ){
					// ＭＣＰがどちらか異常の場合は、自分がアクティブとなる（１台とみなす）
					;		// 入力信号は、自分自信のみ
			}
			else {
				if( DUALPORT.pollingData.voiceBus.BIT.mode==0 ){
					dvaIOOut( 
						OUTSIGNAL_audioBusDisable		// 車内放送禁止
						,
						OUTSIGNAL_acpDisable 			// ＡＣＰ音声許可
						| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声許可
						| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＦＦ
						| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＦＦ
					);
				}
				else if( DUALPORT.ev5000_request.Complete ){	// 放送終了
					dvaIOOut( 
						OUTSIGNAL_audioBusDisable		// 車内放送禁止
						| OUTSIGNAL_acpDisable 			// ＡＣＰ音声禁止
						| OUTSIGNAL_mcpDisable 			// ＭＣＰ音声禁止
						| OUTSIGNAL_acpMONSPON			// ＡＣＰスピーカーＯＮ
						| OUTSIGNAL_mcpMONSPON			// ＭＣＰスピーカーＯＮ
						,
						0
					);
				}
//	'01/03/21 アクティブＤＶＡＵなので、すべてを受け付ける
				signal3.BYTE &= 0x22;		// 自分のＣＡＢスイッチと音声信号のみ許可
//
			}
		}

		if( DUALPORT.mcpMode.Cab==0 ){		// ＣＡＢ／ＣＡＢモードでないとき
			if( signal3.RBIT.acpPTTSW || signal3.RBIT.mcpPTTSW ){
				if( DUALPORT.mcpMode.MANEnable ){
					// 非常通報モード以外のとき受け付け
					// ＡＣＰのトークスイッチを押したとき
					if( DUALPORT.mcpMode.mcpManual==0 ){
						DUALPORT.mcpMode.mcpManual = 1;	// ＡＣＰによるマニュアルモードを設定
						dvaMCPManual();			// 音声バスをマイク放送にする
					}
					return;
				}
			}
			else {
				// ＡＣＰのトークスイッチを離したとき
				if( DUALPORT.mcpMode.mcpManual ){	// ＡＣＰによるマニュアルモードであれば
					dvaMCPRelease();				// 音声バスを戻す
					return;
				}
			}
		}
		else {			// CAB mode のときは、マイクスイッチによってスピーカーを制御
			if( nowSignal.SIGNAL3.RBIT.mcpPTTSW ){
				dvaIOOut( 
					0
					,
					OUTSIGNAL_mcpMONSPON		// ＭＣＰスピーカーＯＦＦ
				);
			}
			else {
				dvaIOOut( 
					OUTSIGNAL_mcpMONSPON		// ＭＣＰスピーカーＯＮ
					,
					0
				);
			}
			if( nowSignal.SIGNAL3.RBIT.acpPTTSW ){
				dvaIOOut( 
					0
					,
					OUTSIGNAL_acpMONSPON		// ＡＣＰスピーカーＯＦＦ
				);
			}
			else {
				dvaIOOut( 
					OUTSIGNAL_acpMONSPON		// ＡＣＰスピーカーＯＮ
					,
					0
				);
			}
		}


		if( DUALPORT.mcpMode.mcpCABOn  ){
			if( DUALPORT.ev5000_request.Complete ){
				DUALPORT.mcpMode.mcpCABOn = 0;
				if( signal3.RBIT.mcpPTTSW==0 && signal3.RBIT.acpPTTSW==0 ){
					// ＣＡＢスイッチが押されたとき
					if( DUALPORT.mcpMode.Cab ){		// ＣＡＢ／ＣＡＢモードの場合
						dvaMCPRelease();				// 音声バスを戻す
						return;
					}
					else {							// ＣＡＢ／ＣＡＢモードでない場合
						dvaMCPCab();				// 音声バスをＣＡＢモードにする
						return;
					}
				}
			}
			else {
				// 自動放送をキャンセル要求
				DUALPORT.ev5000_request.Abort = 1;
			}
			return;
		}


		//
		// ＡＣＰスイッチ判定
		//
			// ＡＣＰのＯＮになった信号を求める
		if( signal3.RBIT.acpSTNSW ){	// STN On
			if( DUALPORT.mcpMode.acpSTN==0 ){	// bef STN Off
				DUALPORT.mcpMode.acpSTN = 1;	// Now STN ON
				if( DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN ){
					acceptSW = MCPSW_STN;
					return;
				}
			}
		}
		else {									// STN Off
			if( DUALPORT.mcpMode.acpSTN ){		// bef STN ON
				DUALPORT.mcpMode.acpSTN = 0;	// Now STN OFF
			}
		}

		if( signal3.RBIT.acpREPSW ){	// REP On
			if( DUALPORT.mcpMode.acpREP==0 ){	// bef REP Off
				DUALPORT.mcpMode.acpREP = 1;	// Now REP ON
				if( DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP ){
					acceptSW = MCPSW_REP;
					return;
				}
			}
		}
		else {									// REP Off
			if( DUALPORT.mcpMode.acpREP ){		// bef REP ON
				DUALPORT.mcpMode.acpREP = 0;	// Now REP OFF
			}
		}

		if( signal3.RBIT.acpCABSW ){	// CAB On
			if( DUALPORT.mcpMode.acpCAB==0 ){
				DUALPORT.mcpMode.acpCAB = 1;
				acceptSW = MCPSW_CAB;
				return;
			}
		}
		else {
			if( DUALPORT.mcpMode.acpCAB ){
				DUALPORT.mcpMode.acpCAB = 0;
			}
		}

		// ＭＣＰのスイッチを受付ける
		for( i=0; i<4; i++ ){
			swdata.BYTE = DUALPORT.answerData.mcpSW[i].BYTE;
			DUALPORT.answerData.mcpSW[i].BYTE.BYTE[0] = 0;
			DUALPORT.answerData.mcpSW[i].BYTE.BYTE[1] = 0;
			DUALPORT.answerData.mcpSW[i].BYTE.BYTE[2] = 0;

			if( swdata.BIT.CAB ){		// ＣＡＢスイッチは常に受け付ける
				acceptSW = MCPSW_CAB;
				return;
			}

			DUALPORT.pollingData.doorInfo.BIT.mcp1on = 1;	// 奇数アドレスＭＣＰの表示ＯＮ
			DUALPORT.pollingData.doorInfo.BIT.mcp2on = 1;	// 偶数アドレスＭＣＰの表示ＯＦＦ

			if( i>=2 )	continue;

			if( DUALPORT.tms_answer.dvaCommError.BIT.mcpCommError1==0 && DUALPORT.tms_answer.dvaCommError.BIT.mcpCommError2==0 ){
					// アクティブ側のＭＣＰスイッチのみ受け付ける処理
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){	// 奇数側が
					if( nowSignal.SIGNAL2.RBIT.hcrON==0 ){		// バックアップ中であれば
						if( DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError2 ){
							if( i==0 )	continue;		// アクティブ側がエラーなので偶数側のＭＣＰが有効
						}
						else {
							if( i==1 )	continue;		// 奇数側のＭＣＰが有効
						}
					}
					else {						// アクティブであれば
						if( i==1 )	continue;			// 奇数側のＭＣＰが有効
					}
				}
				else {						// 偶数側が
					if( nowSignal.SIGNAL2.RBIT.hcrON==0 ){		// バックアップ中であれば
						if( DUALPORT.tms_answer.dvaCommError.BIT.dvauCommError1 ){
							if( i==1 )	continue;		// アクティブ側がエラーなので奇数側のＭＣＰが有効
						}
							else {
							if( i==0 )	continue;		// 偶数側のＭＣＰが有効
						}
					}
					else {						// アクティブであれば
						if( i==0 )	continue;			// 偶数側のＭＣＰが有効
					}
				}
			}

				// ＭＣＰからのキースイッチ情報をキーコードに変換
//			if( swdata.BIT.Talk ){
//				acceptSW = MCPSW_TALK;
//				return;
//			}
			if( swdata.BIT.STN && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN ){
				acceptSW = MCPSW_STN;
				return;
			}
			if( swdata.BIT.ENT && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT ){
				acceptSW = MCPSW_ENT;
				return;
			}
			if( swdata.BIT.RST && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST ){
				acceptSW = MCPSW_RST;
				return;
			}
			if( swdata.BIT.SEL && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL ){
				acceptSW = MCPSW_SEL;
				return;
			}
			if( swdata.BIT.RIGHT && swdata.BIT.LEFT && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT ){
				acceptSW = MCPSW_LANG;
				return;
			}
			if( swdata.BIT.RIGHT && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT ){
				acceptSW = MCPSW_RIGHT;
				return;
			}
			if( swdata.BIT.LEFT && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT ){
				acceptSW = MCPSW_LEFT;
				return;
			}
			if( swdata.BIT.REP && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP ){
				acceptSW = MCPSW_REP;
				return;
			}
			if( swdata.BIT.SPC && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC ){
				acceptSW = MCPSW_SPC;
				return;
			}
			if( swdata.BIT.EMG && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG ){
				acceptSW = MCPSW_EMG;
				return;
			}
			if( swdata.BIT.HL && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.HL ){
				acceptSW = MCPSW_HL;
				return;
			}
			if( swdata.BIT.ATC ){
				acceptSW = MCPSW_ATC;
				return;
			}
			if( swdata.BIT.TEST && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST ){
				acceptSW = MCPSW_TEST;
				return;
			}
			if( swdata.BIT.RB && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB ){
				acceptSW = MCPSW_RB;
				return;
			}
			if( swdata.BIT.EMGP && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP ){
				acceptSW = MCPSW_EMGP;
				return;
			}
		}
}

/************************************************************************

	Function name	<< dvaMCPSWIn >>
					ＭＣＰスイッチ処理（常に受け付けるもの）
	Syntax			dvaMCPSWIn( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
				a01 2000/10/10	表示リセットを追加
				a02 2001/07/02  自動放送モードへ戻る処理の追加
				a03 2001/07/10  放送中は、ＥＩＤＳへリセット信号を出力する

*************************************************************************/
void dvaMCPSWIn( void )
{
		int		i,goON;

		if( acceptSW==MCPSW_CAB ){		// CAB switch
			DUALPORT.mcpMode.mcpCABOn = 1;	// ＣＡＢスイッチＯＮ中
		}

		if( acceptSW==MCPSW_RST ){

			if( mcp_phase == MCP_EMGP1 )
			{
				goON = 0;
				for( i=0; i<16; i++ ){
					if( DUALPORT.answerData.rscSW[i].BYTE ){
						goON = 1;
						break;
					}
				}
				if( goON ){	// 非常ハンドルが入ってきている場合、RSTスイッチは非常ハンドルのPadResetの役割とし、全体リセットの処理には入らない。
					return;
				}
			}
			// リセットスイッチが押されたとき
			if( backup_data.opeMode==OPEMODE_INITIAL_TEST
				|| backup_data.opeMode==OPEMODE_SELF_TEST
				|| backup_data.opeMode==OPEMODE_TEST ){
				DUALPORT.mcpMode.testRelease = 1;	// テスト中のときはテストを止める
			}
			else {
				if( ReturnAutoMode ){		// 自動放送モードへ戻る場合
					backup_data.opeMode = OPEMODE_AUTO;
				}
				else {						// ＩＤＬＥモードへ戻る場合
					backup_data.opeMode = OPEMODE_IDLE;
				}
				backup_data.rootOpeMode = 0;
				DUALPORT.mcpMode.EMGRepeat = 0;
				DUALPORT.tms_answer.dvaStatus.BIT.emgMode = 0;

//				if( DUALPORT.ev5000_request.Complete ==0 ){	// 放送中のときは、リセット出力
					DUALPORT.eids.Reset = 1;
//				}
			}
			mcp_phase = MCP_RESET;		// ＭＣＰ表示のリセット

			return;
		}

}


static	const	char troublePAA[][6] = {
	"FTAMP1",
	"FTAMP2",
	"FT-SP1",
	"FT-SP2",
	"FT-SP3",
	"FT-SP4",
	"FT-SP5",
	"FT-SP6"
};

static	unsigned char	testdispno;
/************************************************************************

	Function name	<< dvaMCPErrorDisplay >>
					テスト中のエラー表示処理
	Syntax			dvaMCPErrorDisplay( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
				a01 2000/09/08	テストモードのエラー表示

*************************************************************************/
dvaMCPErrorDisplay( void )
{
		int		i,j;
		char	buff[BUFSIZ];
		int		skip;
		int		ttl;

		dvaMCPDisplaySet( 0, " TEST " );

		if( timedata.testExecTimer ){
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.TEST=1;
		}
		else {				// テストが終了したら　TEST を点灯する
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.TEST=0;
		}

		if( timedata.testDispTimer )	// 表示切替タイミング待ち
			return;

		ttl=0;
		skip = testdispno;
		if( DUALPORT.nowTrouble & 0x01 ){	// 自ＤＶＡＵへ音声24V が入ってこない場合
			ttl++;
			if( skip==0 ){
				dvaMCPDisplaySet( 2, "24VSig" );
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					dvaMCPDisplaySet( 3, "DVAU1" );
				}
				else {
					dvaMCPDisplaySet( 3, "DVAU2" );
				}
			}
			skip--;
		}
		if( DUALPORT.nowTrouble & 0x02 ){	// 自ＤＶＡＵへ音声 が入ってこない場合
			ttl++;
			if( skip==0 ){
				dvaMCPDisplaySet( 2, "Signal" );
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					dvaMCPDisplaySet( 3, "DVAU1" );
				}
				else {
					dvaMCPDisplaySet( 3, "DVAU2" );
				}
			}
			skip--;
		}
		if( DUALPORT.nowTrouble & 0x04 ){	// 自ＤＶＡＵのＥＶ５０００が通信エラーとなった場合
			ttl++;
			if( skip==0 ){
				dvaMCPDisplaySet( 2, "FT-EV " );
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					dvaMCPDisplaySet( 3, "DVAU1" );
				}
				else {
					dvaMCPDisplaySet( 3, "DVAU2" );
				}
			}
			skip--;
		}
		if( DUALPORT.answerData.dvau_trouble & 0x01 ){	// 相手ＤＶＡＵへ音声24V が入ってこない場合
			ttl++;
			if( skip==0 ){
				dvaMCPDisplaySet( 2, "24VSig" );
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					dvaMCPDisplaySet( 3, "DVAU2" );
				}
				else {
					dvaMCPDisplaySet( 3, "DVAU1" );
				}
			}
			skip--;
		}
		if( DUALPORT.answerData.dvau_trouble & 0x02 ){	// 相手ＤＶＡＵへ音声 が入ってこない場合
			ttl++;
			if( skip==0 ){
				dvaMCPDisplaySet( 2, "Signal" );
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					dvaMCPDisplaySet( 3, "DVAU2" );
				}
				else {
					dvaMCPDisplaySet( 3, "DVAU1" );
				}
			}
			skip--;
		}
		if( DUALPORT.answerData.dvau_trouble & 0x04 ){	// 相手ＤＶＡＵのＥＶ５０００がエラーになった場合
			ttl++;
			if( skip==0 ){
				dvaMCPDisplaySet( 2, "FT-EV " );
				if( nowSignal.SIGNAL2.RBIT.addressOdd ){
					dvaMCPDisplaySet( 3, "DVAU2" );
				}
				else {
					dvaMCPDisplaySet( 3, "DVAU1" );
				}
			}
			skip--;
		}
				
		for( i=0; i<8; i++ ){	// ＰＡＡからのトラブルが通知された場合
			if( DUALPORT.answerData.trouble[i].BYTE ){
				for( j=0; j<8; j++ ){
					if( DUALPORT.answerData.trouble[i].BYTE & (0x001<<j) ){	// トラブルあり？
						ttl++;
						if( skip==0 ){
							dvaMCPDisplaySet( 2, troublePAA[j] );	// トラブルを表示する
							sprintf( buff, "  %02u  ", i+1 );
							dvaMCPDisplaySet( 3, buff );
						}
						skip--;
					}
				}
			}
		}
		for( i=0; i<12; i++ ){		// 通信エラーが発生している場合
			if( DUALPORT.tms_answer.dvaCommError.WORD & (0x01<<i) ){
				ttl++;
				if( skip==0 ){
					if( i<8 ){
						dvaMCPDisplaySet( 2, "FT-PAA" );	// ＰＡＡの通信エラーを表示
						sprintf( buff, "  %02u  ", i+1 );
						dvaMCPDisplaySet( 3, buff );
					}
					if( i==8 || i==9 ){
						dvaMCPDisplaySet( 2, "F-DVAU" );	// ＤＶＡＵの通信エラーを表示
					}
					if( i==10 || i==11 ){
						dvaMCPDisplaySet( 2, "FT-MCP" );	// ＭＣＰの通信エラーを表示
					}
					if( i==8 || i==10 ){
						dvaMCPDisplaySet( 3, "FRONT " );	// ＤＶＡＵ／ＭＣＰが奇数アドレスの場合表示
					}
					if( i==9 || i==11 ){
						dvaMCPDisplaySet( 3, " REAR " );	// ＤＶＡＵ／ＭＣＰが偶数アドレスの場合表示
					}
				}
				skip--;
			}
		}

		if( skip>0 ){			// 表示順序が該当しなかったとき
			testdispno = 0;		// 最初から表示しなおす
		}
		else {
			testdispno++;		// 表示順序を次へ進める
			if( testdispno > ttl )
				testdispno = 0;	// 発生している行数を超えた場合、最初から表示しなおす
			timedata.testDispTimer = 100;	// 次の表示タイミングの設定(x10mS)
		}

		if( ttl==0 ){
			if( timedata.testExecTimer==0 ){
				dvaMCPDisplaySet( 2, "      " );
				dvaMCPDisplaySet( 3, "ALL OK" );
			}
		}
}

/************************************************************************

	Function name	<< dvaMCPOperation >>
					ＭＣＰ表示処理
	Syntax			dvaMCPOperation( void );
	Argument		none
	Author			TOA Corporation  K.Honda
	Revision		a00 2000/08/09
				a01 2000/10/10	操作流れを変更
					a02 2001/07/02  自動放送モードへ戻る処理を追加
									列車無線と非常ハンドルは、マニュアルモードと自動モードは同じ処理とした
					a03 2001/07/12  非常ハンドルがＯＮ－＞ＯＦＦのときに
									１行目を強制的に表示するように変更

					a04 2025/09/12 PADリモートリセット・ドライ接点制御の仕様変更
*************************************************************************/
void dvaMCPOperation( void )
{
		int		i,j;
		int		goON;
		struct ROUTEINFO	*routeData;
		struct STATIONINFO *stationData;

		getMCPSW();			// ＭＣＰキーの受け付け

		dvaMCPSWIn();		// 常に受け付けるキーの処理

		if( mcp_phase!=MCP_IDLE && mcp_phase!=MCP_EMGP1 && mcp_phase!=MCP_EMGP2 ){
			goON = 0;	
			for( i=0; i<16; i++ ){
				if( DUALPORT.answerData.rscSW[i].BYTE ){
					goON = 1;
					break;
				}
			}
			if( goON ){	// 非常ハンドルが入ってきている場合、EMGP を点滅させる
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=1;
			}
			else {		// 非常ハンドルがない場合、EMGP を消灯させる
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=0;
			}
		}

		switch( mcp_phase ){
		case MCP_RESET:
			// スイッチＬＥＤなどの初期設定
			DUALPORT.mcpMode.INUSE = 0;
			DUALPORT.mcpMode.test = 0;		// テスト要求の解除
			DUALPORT.mcpMode.EMGPOn = 0;	// 非常通報器の解除
			for( i=0; i<16; i++ ){
				DUALPORT.mcpMode.EMGPHL[i] = 0;
			}

			for( i=0; i<4; i++ ){
				for( j=0; j<3; j++ ){
					DUALPORT.answerData.mcpSW[i].BYTE.BYTE[j] = 0;		// MCP Switch OFF
				}
			}
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BYTE = 0;	// Switch LED OFF
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BYTE = 0;	// Switch LED Blink OFF
			for( i=0; i<4; i++ ){
				MCPDisplayBlank( i );			// Matrix LED Clear

				DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[i]=0x00;	// Matrix LED Blink OFF
			}

			DUALPORT.ev5000_request.Abort = 1;

			if( backup_data.opeMode == OPEMODE_EMGMA ){
				if( DUALPORT.mcpMode.EMGRepeat ){
					mcp_phase = MCP_EMG2;
					break;
				}
				if( ReturnAutoMode ){		// 自動放送モードへ戻る処理
					backup_data.opeMode = OPEMODE_AUTO;
				}
				else {
					backup_data.opeMode = OPEMODE_IDLE;
				}
			}
			if( backup_data.opeMode == OPEMODE_SPCMA ){
				if( ReturnAutoMode ){		// 自動放送モードへ戻る処理
					backup_data.opeMode = OPEMODE_AUTO;
				}
				else {
					backup_data.opeMode = OPEMODE_IDLE;
				}
			}
			if( backup_data.opeMode == OPEMODE_EMGHL ){
				if( ReturnAutoMode ){		// 自動放送モードへ戻る処理
					backup_data.opeMode = OPEMODE_AUTO;
				}
				else {
					backup_data.opeMode = OPEMODE_IDLE;
				}
			}

			for( i=0; i<16; i++ ){
				backup_data.nowEMGHL[i] = 0;
				DUALPORT.nowEMGHL[i] = 0;
			}

			memset( now_rscSW_row, 0, sizeof(now_rscSW_row) );
			memset( now_rscSW_col, 0, sizeof(now_rscSW_col) );

			for( i=0; i<8; i++ ){
				DUALPORT.tms_request.talkBack[i] = 0;
			}

				// 駅名を表示
			routeData = dvaSearchRoute( backup_data.nowRouteCode );
			if( routeData!=NULL ){
				if( DUALPORT.mcpMode.INUSE==0 ){
					dvaMCPDisplaySet( 0, (char *)routeData->mcpRouteChar );
				}

				stationData = &routeData->station[backup_data.nowStationNo];
				if( backup_data.opeMode == OPEMODE_IDLE ){
					DUALPORT.mcpMode.REPEnable = 1;
					DUALPORT.mcpMode.STNEnable = 1;		// ＳＴＮキー受け付け許可
				}

				// データがあれば、ＭＣＰとＥＩＤＳ表示、
				dvaMCPDisplaySet( 1, (char *)stationData->mcpStationChar );
			}

			DUALPORT.ev5000_request.Repeat = 0;

			acceptSW = 0;
			mcp_phase = MCP_IDLE;
			break;

		case MCP_IDLE:
			// 非常通報器からの信号があれば　ＭＣＰのＥＭＧＰスイッチを点滅させる
			goON = 0;
			for( i=0; i<16; i++ ){
				if( DUALPORT.answerData.rscSW[i].BYTE ){
					goON = 1;
					break;
				}
			}
			if( goON ){
				// ２行目・３行目を点滅
				if( backup_data.nowEMGHL[select_rscSW_row-1] & (0x0001<<(select_rscSW_col-1)) ){
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;
				}
				else {
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;
				}
				if( select_rscSW_row2!=0 ){
					if( backup_data.nowEMGHL[select_rscSW_row2-1] & (0x0001<<(select_rscSW_col2-1)) ){
						DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0x00;
						DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=1;
						DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=0;
					}
					else {
						DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0xFF;
						DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=1;
						DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=1;
					}
				}
				else {
					// 非常通報のＬＥＤを点滅
					DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=1;
					DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=1;
				}


				if( dvaMCPGetEMGP() ){
					item_skip = 0;
					dvaMCPOpeEMGPDisplay( item_skip );
				}
				// ＣＡＢ／ＣＡＢ放送中は処理しない
				if( DUALPORT.mcpMode.Cab ){
					break;
				}
				// マニュアル放送中は処理しない
				if( DUALPORT.mcpMode.mcpManual ){
					break;
				}
			}
			else {
				if( DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP ){
					for( i=0; i<4; i++ ){
						MCPDisplayBlank( i );			// Matrix LED Clear

						DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[i]=0x00;	// Matrix LED Blink OFF
					}

					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;	// Matrix LED Blink OFF
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0x00;	// Matrix LED Blink OFF

					for( i=0; i<16; i++ ){
						backup_data.nowEMGHL[i] = 0;
					}

					memset( now_rscSW_row, 0, sizeof(now_rscSW_row) );
					memset( now_rscSW_col, 0, sizeof(now_rscSW_col) );

					for( i=0; i<8; i++ ){
						DUALPORT.tms_request.talkBack[i] = 0;
					}

						// 駅名を表示
					routeData = dvaSearchRoute( backup_data.nowRouteCode );
					if( routeData!=NULL ){

						switch( backup_data.opeMode ){	// １行目は、現在のモードの表示を行う
						case OPEMODE_INITIAL_TEST:
						case OPEMODE_SELF_TEST:
							dvaMCPDisplaySet( 0, " TEST " );
							break;
						case OPEMODE_EMGMA:
							dvaMCPDisplaySet( 0, "EMG-MA" );
							break;
						case OPEMODE_SPCMA:
							dvaMCPDisplaySet( 0, "SPC-MA" );
							break;
						default:			// 該当モードがなければ　ルート表示を行う
							dvaMCPDisplaySet( 0, (char *)routeData->mcpRouteChar );
							break;
						}

						stationData = &routeData->station[backup_data.nowStationNo];
						if( backup_data.opeMode == OPEMODE_IDLE ){
							DUALPORT.mcpMode.REPEnable = 1;
							DUALPORT.mcpMode.STNEnable = 1;		// ＳＴＮキー受け付け許可
						}

						// データがあれば、ＭＣＰとＥＩＤＳ表示、
						dvaMCPDisplaySet( 1, (char *)stationData->mcpStationChar );
					}
				}
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMGP=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=0;
				// 非常通報のＬＥＤを消灯
				memset( now_rscSW_row, 0, sizeof(now_rscSW_row) );
				memset( now_rscSW_col, 0, sizeof(now_rscSW_col) );

				// ＣＡＢ／ＣＡＢ放送中は処理しない
				if( DUALPORT.mcpMode.Cab ){
					mcp_phase = MCP_IDLE;
					break;
				}
					// マニュアル放送中は処理しない
				if( DUALPORT.mcpMode.mcpManual ){
					mcp_phase = MCP_IDLE;
					break;
				}
			}

				// セルフテスト中はエラー表示をおこなう
			if( backup_data.opeMode == OPEMODE_INITIAL_TEST
				|| backup_data.opeMode == OPEMODE_SELF_TEST
				|| backup_data.opeMode == OPEMODE_TEST ){
				dvaMCPErrorDisplay();

				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
				return;
			}

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;
			if( backup_data.opeMode==OPEMODE_IDLE ){
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
				if( DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RB ){
					DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				}
				else {
					DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=1;
				}
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.TEST=0;
			}
			else if( backup_data.opeMode==OPEMODE_TRS ){
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.TEST=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=0;
			}
			else if( backup_data.opeMode==OPEMODE_EMGHL ){
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.TEST=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=0;
			}
			else if( backup_data.opeMode==OPEMODE_AUTO ){
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.TEST=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=0;
			}
			else {
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.TEST=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=0;
			}
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;

			if( DUALPORT.mcpMode.REPEnable ){
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=1;
				if( DUALPORT.ev5000_request.Complete ){
					DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
				}
			}
			else {
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			}

#if 0
{
static int stnen;
static int stnled;
static int stnblink;
static int evcomp;

if( evcomp==0 && DUALPORT.ev5000_request.Complete ){
evcomp=1;
printf("EV5000 Active\r\n");
}
if( evcomp==1 && DUALPORT.ev5000_request.Complete==0 ){
evcomp=0;
printf("EV5000 End\r\n");
}
if( stnen==0 && DUALPORT.mcpMode.STNEnable ){
stnen=1;
printf("STN Enabled\r\n");
}
if( stnen!=0 && DUALPORT.mcpMode.STNEnable==0 ){
stnen=0;
printf("STN Disabled\r\n");
}
if( stnled==0 && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN ){
stnled=0;
printf("STN LED ON\r\n");
}
if( stnled!=0 && DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN==0 ){
stnled=1;
printf("STN LED OFF\r\n");
}
if( stnblink==0 && DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN ){
stnblink=1;
printf("STN LED BLINK ON\r\n");
}
if( stnblink!=0 && DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN==0 ){
stnblink=0;
printf("STN LED BLINK OFF\r\n");
}
}
#endif

			if( DUALPORT.mcpMode.STNEnable ){
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=1;
				if( DUALPORT.ev5000_request.Complete ){
					DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;
				}
			}
			else {
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;
			}


			if( acceptSW==MCPSW_ATC ){
				// ＡＴＣスイッチが押されたとき
				if( DUALPORT.mcpMode.ATCEnable ){	// ＡＴＣスイッチ許可のとき
					if( DUALPORT.mcpMode.ATCOnEnable ){
						DUALPORT.mcpMode.ATCOff = 1;	// 自動放送解除
					}
					else {
						DUALPORT.mcpMode.ATCOn = 1;		// 自動放送要求
					}
				}
				break;
			}


			if( acceptSW==MCPSW_EMG ){
				// 非常放送モードスイッチが押されたとき
				select_emgCode = backup_data.nowEMGCode;
				dvaMCPOpeEMGDisplay( 0 );
				mcp_phase = MCP_EMG1;

				backup_data.rootOpeMode = OPEMODE_IDLE;
				backup_data.opeMode = OPEMODE_EMGMA;
				DUALPORT.ev5000_request.Abort = 1;
				break;
			}

			if( acceptSW==MCPSW_SPC ){
				// スペシャル放送モードスイッチが押されたとき
				select_spcCode = backup_data.nowSPCCode;
				dvaMCPOpeSPCDisplay( 0 );
				mcp_phase = MCP_SPC1;

				backup_data.rootOpeMode = OPEMODE_IDLE;
				backup_data.opeMode = OPEMODE_SPCMA;
				DUALPORT.ev5000_request.Abort = 1;
				break;
			}

			if( DUALPORT.ev5000_request.Complete ){

				// ＭＣＰのスイッチ判定
				if( acceptSW==MCPSW_STN ){
					// ＳＴＮスイッチが押されたとき
					if( DUALPORT.mcpMode.STNEnable ){		// 次駅許可の場合
						DUALPORT.mcpMode.STNOn = 1;		// 次駅放送要求
						DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=1;
					}
					break;
				}
				if( acceptSW==MCPSW_REP ){
					// リピートスイッチが押されたとき
					if( DUALPORT.mcpMode.REPEnable ){		// 再放送許可の場合
						DUALPORT.mcpMode.REPOn = 1;		// リピート放送要求
						DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=1;
					}
					break;
				}


				if( acceptSW==MCPSW_TEST ){
					// テストスイッチが押されたとき
					mcp_phase = MCP_TEST_SELECT;
					break;
				}

				if( acceptSW==MCPSW_SEL ){
					// ライン選択が押されたとき
					select_routeCode = backup_data.nowRouteCode;
					routeData = dvaSearchRoute( select_routeCode );
					if( routeData==NULL ){
						select_station=0;
					}
					else {
						select_station=backup_data.nowStationNo;
					}
					if( dvaMCPOpeLineDisplay( &select_routeCode, select_station, 0 )==0 ){
						mcp_phase = MCP_LINE_SELECT;
					}
					break;
				}

				if( acceptSW==MCPSW_LANG ){
					// 言語選択が押されたとき
					select_lang = config_data.lang;
					dvaMCPOpeLangDisplay( &select_lang, 0 );
	
					mcp_phase = MCP_LANG_SELECT;
					break;
				}
			}

			// 2001/04/26 優先順位のため、放送中も ＥＭＧＰとトークバックは受け付ける
			if( acceptSW==MCPSW_EMGP ){	
				// ＥＭＧ－Ｐスイッチが押されたとき
				item_skip = 0;
				dvaMCPOpeEMGPDisplay( item_skip );
				mcp_phase = MCP_EMGP1;
				break;
			}

			if( DUALPORT.tms_request.Active ){			// ＴＭＳ有効
				for( i=0; i<8; i++ ){
					if( DUALPORT.tms_request.talkBack[i] )
						break;
				}
				if( i<8 ){	// ＴＭＳから非常通報を要求したとき
					mcp_phase = MCP_EMGP1;
					break;
				}
			}
			break;

		case MCP_EMGP1:
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.TEST=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;


			goON = 0;
			for( i=0; i<16; i++ ){
				if( DUALPORT.answerData.rscSW[i].BYTE ){
					goON = 1;
					break;
				}
			}
			if( goON ){
				dvaMCPGetEMGP();
				dvaMCPOpeEMGPDisplay( item_skip );
			}
			else {
				mcp_phase = MCP_RESET;
				break;
			}

			DUALPORT.mcpMode.EMGPOn = 1;		// 非常通報の放送要求
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=0;

				// ２行目・３行目を点滅
			if( backup_data.nowEMGHL[select_rscSW_row-1] & (0x0001<<(select_rscSW_col-1)) ){
				DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;
			}
			else {
				DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;
			}
			if( select_rscSW_row2!=0 ){
				if( backup_data.nowEMGHL[select_rscSW_row2-1] & (0x0001<<(select_rscSW_col2-1)) ){
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0x00;
				}
				else {
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0xFF;
				}
			}

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;

			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				if( item_skip )
					item_skip--;
				while( dvaMCPOpeEMGPDisplay( item_skip ) )
					item_skip--;
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				item_skip++;
				while( dvaMCPOpeEMGPDisplay( item_skip ) )
					item_skip--;
				break;
			}
			if( acceptSW==MCPSW_ENT ){		/* 確定 */
				DUALPORT.mcpMode.EMGPHL[select_rscSW_row-1] ^= 0x0001<<(select_rscSW_col-1);
				break;
			}

			if( acceptSW==MCPSW_RST ){		/* リセットスイッチが押されたとき */
				for( i=0; i<16; i++ ){
					DUALPORT.mcpMode.PadReset[i] = DUALPORT.mcpMode.EMGPHL[i];
					DUALPORT.mcpMode.EMGPHL[i] = 0;
				}
				break;
			}

			
			if( acceptSW==MCPSW_EMG ){
				// 非常放送モードスイッチが押されたとき
				select_emgCode = backup_data.nowEMGCode;
				dvaMCPOpeEMGDisplay( 0 );
				mcp_phase = MCP_EMG1;

				backup_data.rootOpeMode = OPEMODE_IDLE;
				backup_data.opeMode = OPEMODE_EMGMA;
				break;
			}

			if( DUALPORT.tms_request.Active ){			// ＴＭＳ有効
				for( i=0; i<8; i++ ){
					if( DUALPORT.tms_request.talkBack[i] )
						break;
				}
				if( i<8 ){	// ＴＭＳから非常通報を要求したとき
					for( i=0; i<16; i++ ){
						DUALPORT.mcpMode.EMGPHL[i] = 0;
					}
					// ＥＭＧ－Ｐスイッチが押されたときと同じ処理をおこなう
					for( i=0; i<16*6; i++ ){
						if( dvaMCPOpeEMGPDisplay( i )!=0 ){
							dvaMCPOpeEMGPDisplay( item_skip );
							break;
						}

						if( DUALPORT.tms_request.talkBack[select_rscSW_row-1] & (0x0001<<(select_rscSW_col-1)) ){
							item_skip = i;
							// ＴＭＳからの非常通報要求
							DUALPORT.mcpMode.EMGPHL[select_rscSW_row-1] = DUALPORT.tms_request.talkBack[select_rscSW_row-1];
							DUALPORT.tms_request.talkBack[select_rscSW_row-1] = 0;
							break;
						}
					}
					for( i=0; i<8; i++ ){
						DUALPORT.tms_request.talkBack[i] = 0;
					}
				}
			}
			break;

		case MCP_EMGP2:
			goON = 0;
			for( i=0; i<16; i++ ){
				if( DUALPORT.answerData.rscSW[i].BYTE ){
					goON = 1;
					break;
				}
			}
			if( goON ){
				dvaMCPGetEMGP();
				dvaMCPOpeEMGPDisplay( item_skip );
			}
			else {
				mcp_phase = MCP_RESET;
				break;
			}

			if( backup_data.nowEMGHL[select_rscSW_row-1] & (0x0001<<(select_rscSW_col-1)) ){
				DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;
			}
			else {
				DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;
			}
			if( select_rscSW_row2!=0 ){
				if( backup_data.nowEMGHL[select_rscSW_row2-1] & (0x0001<<(select_rscSW_col2-1)) ){
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0x00;
				}
				else {
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[3]=0xFF;
				}
			}

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;


			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				if( item_skip )
					item_skip--;
				while( dvaMCPOpeEMGPDisplay( item_skip ) )
					item_skip--;
				mcp_phase = MCP_EMGP1;
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				item_skip++;
				while( dvaMCPOpeEMGPDisplay( item_skip ) )
					item_skip--;
				mcp_phase = MCP_EMGP1;
				break;
			}
			if( acceptSW==MCPSW_ENT ){		/* 確定 */
				DUALPORT.mcpMode.EMGPOn = 1;		// 非常通報の放送要求
					// 選択された非常通報器の設定
				DUALPORT.mcpMode.EMGPHL[select_rscSW_row-1] ^= 0x0001<<(select_rscSW_col-1);
				break;
			}

			if( acceptSW==MCPSW_EMG ){
				// 非常放送モードスイッチが押されたとき
				select_emgCode = backup_data.nowEMGCode;
				dvaMCPOpeEMGDisplay( 0 );
				mcp_phase = MCP_EMG1;

				backup_data.rootOpeMode = OPEMODE_IDLE;
				backup_data.opeMode = OPEMODE_EMGMA;
				break;
			}
			break;

		case MCP_EMG1:
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				// 非常放送のＬＥＤを点灯
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				// ２行目・３行目を点滅
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0xFF;
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMGP=1;

			if( DUALPORT.mcpMode.INUSE == 0 ){
				dvaMCPDisplaySet( 0, "EMG-MA" );
			}

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;

			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				dvaMCPOpeEMGDisplay( -1 );
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				dvaMCPOpeEMGDisplay( 1 );
				break;
			}
			if( acceptSW==MCPSW_ENT ){		/* 確定 */
				mcp_phase = MCP_EMG2;
				break;
			}
			break;

		case MCP_EMG2:
				// 非常放送のＬＥＤを点灯
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=1;
//			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.EMG=0;
				// ２行目を点灯
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0x00;
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;
			MCPDisplayBlank( 3 );

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;
			if( DUALPORT.mcpMode.EMGRepeat==0 ){
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
			}
			else {
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=0;
			}

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;

			if( DUALPORT.mcpMode.INUSE == 0 ){
				dvaMCPDisplaySet( 0, "EMG-MA" );
			}

			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				dvaMCPOpeEMGDisplay( -1 );
				mcp_phase = MCP_EMG1;
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				dvaMCPOpeEMGDisplay( 1 );
				mcp_phase = MCP_EMG1;
				break;
			}
			if( acceptSW==MCPSW_EMG ){		/* 放送 */
				DUALPORT.mcpMode.EMGCode = select_emgCode;
				DUALPORT.mcpMode.EMGOn = 1;
				break;
			}
			break;

		case MCP_SPC1:
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				// スペシャル放送のＬＥＤを点灯
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				// ２行目・３行目を点滅
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0xFF;
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;

			if( DUALPORT.mcpMode.INUSE == 0 ){
				dvaMCPDisplaySet( 0, "SPC-MA" );
			}

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;

			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				dvaMCPOpeSPCDisplay( -1 );
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				dvaMCPOpeSPCDisplay( 1 );
				break;
			}
			if( acceptSW==MCPSW_ENT ){		/* 確定 */
				mcp_phase = MCP_SPC2;
				break;
			}

			// 2001/07/13 優先順位のため、放送中も ＥＭＧＰとトークバックは受け付ける
			if( acceptSW==MCPSW_EMGP ){	
				// ＥＭＧ－Ｐスイッチが押されたとき
				item_skip = 0;
				dvaMCPOpeEMGPDisplay( item_skip );
				mcp_phase = MCP_EMGP1;
				break;
			}
			break;

		case MCP_SPC2:
				// スペシャル放送のＬＥＤを点灯
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=1;
//			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC=0;
				// ２行目を点灯
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0x00;
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;
			MCPDisplayBlank( 3 );

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=0;
			if( DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SPC==0 ){
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
			}
			else {
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=0;
				DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=0;
			}
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;

			if( DUALPORT.mcpMode.INUSE == 0 ){
				dvaMCPDisplaySet( 0, "SPC-MA" );
			}

			// ＭＣＰのスイッチ判定
			if( DUALPORT.ev5000_request.Complete ){
				if( acceptSW==MCPSW_LEFT ){		/* 選択 */
					dvaMCPOpeSPCDisplay( -1 );
					mcp_phase = MCP_SPC1;
					break;
				}
				if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
					dvaMCPOpeSPCDisplay( 1 );
					mcp_phase = MCP_SPC1;
					break;
				}
			}
			if( acceptSW==MCPSW_SPC ){		/* 放送 */
				backup_data.nowSPCCode = select_spcCode;
				DUALPORT.mcpMode.SPCOn = 1;
				break;
			}
			// 2001/07/13 優先順位のため、放送中も ＥＭＧＰとトークバックは受け付ける
			if( acceptSW==MCPSW_EMGP ){	
				// ＥＭＧ－Ｐスイッチが押されたとき
				item_skip = 0;
				dvaMCPOpeEMGPDisplay( item_skip );
				mcp_phase = MCP_EMGP1;
				break;
			}

			if( backup_data.opeMode != OPEMODE_SPCMA ){
				mcp_phase = MCP_RESET;
			}
			break;

		case MCP_LINE_SELECT:
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				// １行目を点滅
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[0]=0xFF;
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0xFF;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;

			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				dvaMCPOpeLineDisplay( &select_routeCode, 0, -1 );
				select_station = 0;
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				dvaMCPOpeLineDisplay( &select_routeCode, 0, 1 );
				select_station = 0;
				break;
			}
			if( acceptSW==MCPSW_STN ){		/* 駅選択 */
				dvaMCPDisplaySet( 1, (char *)select_routeData->station[select_station].mcpStationChar );
				mcp_phase = MCP_STATION_SELECT;
				break;
			}
			if( acceptSW==MCPSW_ENT ){		/* 確定 */
				select_routeData = dvaSearchRoute( select_routeCode );
				if( select_routeData!=NULL ){
					backup_data.nowRouteCode = select_routeData->routeCode;
					backup_data.nowStationName = select_routeData->station[select_station].stationCode;
					DUALPORT.mcpMode.STNEnable = 1;
					backup_data.nowStationNo = select_station;
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[0]=0x00;
					DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0x00;
					mcp_phase = MCP_IDLE;
				}
				break;
			}
			break;

		case MCP_STATION_SELECT:
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				// １行目を点灯
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[0]=0x00;
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0xFF;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RST=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.RST=0;

			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				if( select_station>0 ){
					select_station--;
					dvaMCPDisplaySet( 1, (char *)select_routeData->station[select_station].mcpStationChar );
				}
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				if( select_station+1<MAX_STATION ){
					if( select_routeData->station[select_station+1].stationCode!=0 ){
						select_station++;
						dvaMCPDisplaySet( 1, (char *)select_routeData->station[select_station].mcpStationChar );
					}
				}
				break;
			}
			if( acceptSW==MCPSW_ENT ){		/* 確定 */
				backup_data.nowRouteCode = select_routeData->routeCode;
				backup_data.nowStationName = select_routeData->station[select_station].stationCode;
				DUALPORT.mcpMode.STNEnable = 1;
				backup_data.nowStationNo = select_station;
				DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[0]=0x00;
				DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[1]=0x00;
				mcp_phase = MCP_IDLE;
				break;
			}
			break;

		case MCP_LANG_SELECT:
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.TEST=0;
				// ２行目を点滅
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0xFF;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;

			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				dvaMCPOpeLangDisplay( &select_lang, -1 );
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				dvaMCPOpeLangDisplay( &select_lang, 1 );
				break;
			}
			if( acceptSW==MCPSW_ENT ){		/* 確定 */
				config_data.lang = select_lang;
				mcp_phase = MCP_LANG_SELECT2;
				break;
			}
			break;

		case MCP_LANG_SELECT2:
				// ２行目を点灯
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[2]=0x00;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=1;

			// ＭＣＰのスイッチ判定
			if( acceptSW==MCPSW_LEFT ){		/* 選択 */
				dvaMCPOpeLangDisplay( &select_lang, -1 );
				mcp_phase = MCP_LANG_SELECT;
				break;
			}
			if( acceptSW==MCPSW_RIGHT ){	/* 選択 */
				dvaMCPOpeLangDisplay( &select_lang, 1 );
				mcp_phase = MCP_LANG_SELECT;
				break;
			}
			break;

		case MCP_TEST_SELECT:
			dvaMCPDisplaySet( 0, " TEST " );
				// １行目を点滅
			DUALPORT.pollingData.mcpDisplay.BIT.lineBlink[0]=0xFF;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.ENT=1;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.ENT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.LEFT=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.RIGHT=0;

			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.REP=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.STN=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.EMG=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SPC=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swLED.BIT.SEL=0;
			DUALPORT.pollingData.mcpDisplay.BIT.swBLINK.BIT.SEL=0;

			if( acceptSW==MCPSW_ENT ){		/* 確定 */
				DUALPORT.mcpMode.test = 1;
				mcp_phase = MCP_RESET;
				break;
			}
			break;

		}
}

