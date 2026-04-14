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

#include <string.h>

#include "dosaccs.h"
#include "fcntl.h"

/***************************************************************
	次のディレクトリー検索
			ルートディレクトリーの場合はルートセクターを順次検索
				ＢＰＢのエントリー数分順次検索する
			サブディレクトリーの場合は　クラスターを順次検索
				クラスターサイズまで検索し、次はチェインしている
				クラスターを順次読み込んで検索する

			検索はエントリーがなくなったとき
			ファイル名が一致したとき
			チェインしているクラスターがなくなったとき

	INPUT:
			fcb = ＦＣＢエリア
			fname = ファイル名＋エクステンド名
				（８＋３文字　スペース埋め込み済み）
			attribute = ファイルアトリビュート

			dir = ディレクトリーエントリー情報

	RETURN:
			0 = 正常終了
			other = 異常終了

			fcb.dir_cluster = 0
			fcb.dir_entry = 検索ディレクトリーのエントリー番号

***************************************************************/
int	next_search_directory( FCB *fcb, char *fname, unsigned char attribute, DIR_ENTRY *dir )
{
		BPB		*bpb;
		short	offset;
		int		i;
		unsigned char	*buffer;

		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/
		bpb = fcb->bpb;
		if( bpb==NULL )
			return( -1 );				/*デバイス情報無し*/

		/*前回 root_search_directory() で使用した fcb->dir_cluster, fcb->dir_entry を引き継ぐ*/

		for( ; fcb->dir_entry<1024; fcb->dir_entry++ ){

			if( fcb->dir_cluster==0 ){		/*ルートディレクトリーの場合*/
				if( fcb->dir_entry>=bpb->root_entry )
					return( -1 );				/*エントリーオーバー*/
				if( root_dir_read( bpb, fcb->dir_entry, &offset ) )
					return( -1 );				/*ルートディレクトリー読込みエラー*/

				buffer = rootdir_buffer.buffer;
			}
			else {							/*サブディレクトリーの場合*/
				offset = fcb->dir_entry * 32;				/*エントリーのオフセットを設定*/
				if( offset>=(short)bpb->bytes_cluster ){	/*クラスターオーバーのときは次のクラスター*/
					fcb->dir_cluster = next_cluster( bpb, fcb->dir_cluster );
					if( fcb->dir_cluster < 2 || fcb->dir_cluster==0xFFFF )
						return( -1 );			/*エントリーオーバー*/
					fcb->dir_entry = 0;
					offset = 0;
				}
				if( cluster_read( bpb, fcb->dir_cluster, 1 ) )
					return( -1 );		/*サブディレクトリー読込みエラー*/

				buffer = cluster_buffer.buffer;
			}

			if( buffer[offset+0]==0 )	/*エントリーの最後の場合*/
				return( 1 );			/*指定名なし*/

			i=0;
			if( buffer[offset+8+3] & attribute ){	/*属性が一致したとき*/
				for( i=0; i<8+3; i++ ){
					if( fname[i]=='?' ){		/*ワイルドカード*/
						continue;
					}
					else if( fname[i]==buffer[offset+i] ){	/*ファイル名が一致したとき*/
						continue;
					}
					else {
						break;
					}
				}
			}
			if( i==8+3 )		/*属性とファイル名が一致したとき*/
				break;			/*検索終了*/
		}

		memcpy( dir->filename, &buffer[offset+0], 8+3 );
		dir->attribute = buffer[offset+8+3];

		dir->w_time = (unsigned short)buffer[offset+22] & 0x00FF
					| ((unsigned short)buffer[offset+23]<<8) & 0xFF00;
		dir->w_date = (unsigned short)buffer[offset+24] & 0x00FF
					| ((unsigned long)buffer[offset+25]<<8) & 0xFF00;

		dir->start_cluster = (unsigned short)buffer[offset+26] & 0x00FF
					| (unsigned short)(buffer[offset+27]<<8) & 0xFF00;

		dir->all_bytes = (long)buffer[offset+28] & 0x000000FFL
					| ((long)buffer[offset+29]<<8) & 0x0000FF00L
					| ((long)buffer[offset+30]<<16) & 0x00FF0000L
					| ((long)buffer[offset+31]<<24) & 0xFF000000L;

		return( 0 );
}

/***************************************************************
	最初のディレクトリー検索
			エントリー番号を０にして検索する

	INPUT:
			fd = ＦＣＢ番号
			fname = ファイル名＋エクステンド名
				（８＋３文字　スペース埋め込み済み）
			attribute = ファイルアトリビュート

			dir = ディレクトリーエントリー情報
				fcb->dir_cluster = 0;		ルートディレクトリーを検索
				fcb->dir_cluster <> 0;		サブディレクトリーを検索

	RETURN:
			0 = 正常終了
			other = 異常終了

			fcb.dir_cluster = 検索ディレクトリーのクラスター番号(0=ROOT)
			fcb.dir_entry = 検索ディレクトリーのエントリー番号

***************************************************************/
int	search_directory( FCB *fcb, char *fname, unsigned char attribute, DIR_ENTRY *dir )
{
		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/

		fcb->dir_entry = 0;			/*最初から検索*/

		return( next_search_directory( fcb, fname, attribute, dir ) );
}


/***************************************************************
	ディレクトリー追加
			削除されたところ、未登録のところへ書込む
			サブディレクトリーの場合、エントリーがなくなったら
			新しいクラスターをチェイン追加し、最初にエントリーする

	INPUT:
			fd = ＦＣＢ番号
			fname = ファイル名＋エクステンド名
				（８＋３文字　スペース埋め込み済み）
			attribute = ファイルアトリビュート

			dir = ディレクトリーエントリー情報

	RETURN:
			0 = 正常終了
			other = 異常終了

			fcb.dir_cluster = 検索ディレクトリーのクラスター番号(0=ROOT)
			fcb.dir_entry = 検索ディレクトリーのエントリー番号

***************************************************************/
int	append_directory( FCB *fcb, DIR_ENTRY *dir )
{
		BPB		*bpb;
		short	offset;
		unsigned char *buffer;
		unsigned short	cluster;

		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/
		bpb = fcb->bpb;
		if( bpb==NULL )
			return( -1 );				/*デバイス情報無し*/

		for( fcb->dir_entry=0; fcb->dir_entry<1024; fcb->dir_entry++ ){		/*エントリーの最初から検索*/

			if( fcb->dir_cluster==0 ){		/*ルートディレクトリーの場合*/
				if( fcb->dir_entry>=bpb->root_entry )
					return( -1 );				/*エントリーオーバー*/
				if( root_dir_read( bpb, fcb->dir_entry, &offset ) )
					return( -1 );				/*ルートディレクトリー読込みエラー*/

				buffer = rootdir_buffer.buffer;
			}
			else {							/*サブディレクトリーの場合*/
				offset = fcb->dir_entry * 32;				/*エントリーのオフセットを設定*/
				if( offset>=(short)bpb->bytes_cluster ){	/*クラスターオーバーのときは次のクラスター*/
					cluster = next_cluster( bpb, fcb->dir_cluster );
					if( cluster < 2 || cluster==0xFFFF ){	/*ディレクトリーのクラスターがなくなったとき*/
						cluster = empty_cluster(bpb,fcb->dir_cluster);	/*空きクラスターを探す*/
						if( cluster < 2 || cluster==0xFFFF )	/*追加できないとき*/
							return( -1 );			/*追加エラー*/
						if(update_cluster(bpb,fcb->dir_cluster,cluster))	/*クラスターチェイン*/
							return( -1 );			/*追加エラー*/
						if(update_cluster(bpb,cluster,0xFFFF))	/*チェインエンドマーク*/
							return( -1 );			/*追加エラー*/
						if(new_cluster_read(bpb, cluster, 1 ))	/*新しいクラスターを読み込む*/
							return( -1 );			/*追加エラー*/
						memset(cluster_buffer.buffer, 0, sizeof(cluster_buffer.buffer));
						cluster_write();
					}
					fcb->dir_cluster = cluster;

					fcb->dir_entry = 0;
					offset = 0;
				}
				if( cluster_read( bpb, fcb->dir_cluster, 1 ) )
					return( -1 );		/*サブディレクトリー読込みエラー*/

				buffer = cluster_buffer.buffer;
			}

			if( buffer[offset+0]==0 )		/*エントリーの最後の場合*/
				break;		/*未登録エントリー*/

			if( buffer[offset+0]==0xE5 )	/*削除エントリーの場合*/
				break;		/*削除されたところ*/
		}

		if( buffer[offset+0]!=0 && buffer[offset+0]!=0xE5 ){
			return( 1 );			/*空なし*/
		}

		dir->w_time = now_time;
		dir->w_date = now_date;
		dir->all_bytes = 0L;
		dir->start_cluster = 0;

		memcpy( &buffer[offset+0], dir->filename, 8+3 );
		buffer[offset+8+3] = dir->attribute;

		buffer[offset+22]=(unsigned char)(dir->w_time & 0xFF);
		buffer[offset+23]=(unsigned char)((dir->w_time>>8) & 0xFF);
		buffer[offset+24]=(unsigned char)(dir->w_date & 0xFF);
		buffer[offset+25]=(unsigned char)((dir->w_date>>8) & 0xFF);

		buffer[offset+26]=(unsigned char)(dir->start_cluster & 0xFF);
		buffer[offset+27]=(unsigned char)((dir->start_cluster>>8) & 0xFF);

		buffer[offset+28]=(unsigned char)(dir->all_bytes & 0xFF);
		buffer[offset+29]=(unsigned char)((dir->all_bytes>>8) & 0xFF);
		buffer[offset+30]=(unsigned char)((dir->all_bytes>>16) & 0xFF);
		buffer[offset+31]=(unsigned char)((dir->all_bytes>>24) & 0xFF);

		if( fcb->dir_cluster==0 )
			root_dir_write();
		else
			cluster_write();

		return( 0 );
}

/***************************************************************
	ディレクトリー更新
			日付・時刻とファイルの大きさを更新する

	INPUT:
			fd = ＦＣＢ番号

			dir = ディレクトリーエントリー情報
					dir->all_bytes = ファイルの大きさ

	RETURN:
			0 = 正常終了
			other = 異常終了

			fcb.dir_cluster = 検索ディレクトリーのクラスター番号(0=ROOT)
			fcb.dir_entry = 検索ディレクトリーのエントリー番号

***************************************************************/
int	update_directory( FCB *fcb, DIR_ENTRY *dir )
{
		BPB		*bpb;
		short	offset;
		unsigned char *buffer;

		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/
		bpb = fcb->bpb;
		if( bpb==NULL )
			return( -1 );				/*デバイス情報無し*/

		if( fcb->dir_cluster==0 ){		/*ルートディレクトリーの場合*/
			if( fcb->dir_entry>=bpb->root_entry )
				return( -1 );				/*エントリーオーバー*/
			if( root_dir_read( bpb, fcb->dir_entry, &offset ) )
				return( -1 );				/*ルートディレクトリー読込みエラー*/

			buffer = rootdir_buffer.buffer;
		}
		else {							/*サブディレクトリーの場合*/
			offset = fcb->dir_entry * 32;				/*エントリーのオフセットを設定*/
			if( cluster_read( bpb, fcb->dir_cluster, 1 ) )
				return( -1 );		/*サブディレクトリー読込みエラー*/

			buffer = cluster_buffer.buffer;
		}

		memcpy( &buffer[offset+0], dir->filename, 8+3 );
		buffer[offset+8+3] = dir->attribute;
		dir->w_time = now_time;
		dir->w_date = now_date;

		buffer[offset+22]=(unsigned char)(dir->w_time & 0xFF);
		buffer[offset+23]=(unsigned char)((dir->w_time>>8) & 0xFF);
		buffer[offset+24]=(unsigned char)(dir->w_date & 0xFF);
		buffer[offset+25]=(unsigned char)((dir->w_date>>8) & 0xFF);

		buffer[offset+26]=(unsigned char)(dir->start_cluster & 0xFF);
		buffer[offset+27]=(unsigned char)((dir->start_cluster>>8) & 0xFF);

		buffer[offset+28]=(unsigned char)(dir->all_bytes & 0xFF);
		buffer[offset+29]=(unsigned char)((dir->all_bytes>>8) & 0xFF);
		buffer[offset+30]=(unsigned char)((dir->all_bytes>>16) & 0xFF);
		buffer[offset+31]=(unsigned char)((dir->all_bytes>>24) & 0xFF);

		fcb->all_bytes = dir->all_bytes;

		if( fcb->dir_cluster==0 )
			root_dir_write();
		else
			cluster_write();

		return( 0 );
}

/***************************************************************
	ディレクトリー削除
			ファイル名を検索してから行うこと

	INPUT:
			fd = ＦＣＢ番号

			dir = ディレクトリーエントリー情報
					dir->all_bytes = ファイルの大きさ

	RETURN:
			0 = 正常終了
			other = 異常終了

			fcb.dir_cluster = 0
			fcb.dir_entry = 検索ディレクトリーのエントリー番号

***************************************************************/
int	delete_directory( FCB *fcb, DIR_ENTRY *dir )
{
		BPB		*bpb;
		short	offset;
		unsigned char *buffer;
		unsigned short	cluster,cluster2;

		if( fcb==NULL )
			return( -1 );				/*ファイルコントロールブロック情報無し*/
		bpb = fcb->bpb;
		if( bpb==NULL )
			return( -1 );				/*デバイス情報無し*/

		if( fcb->dir_cluster==0 ){		/*ルートディレクトリーの場合*/
			if( fcb->dir_entry>=bpb->root_entry )
				return( -1 );				/*エントリーオーバー*/
			if( root_dir_read( bpb, fcb->dir_entry, &offset ) )
				return( -1 );				/*ルートディレクトリー読込みエラー*/

			buffer = rootdir_buffer.buffer;
		}
		else {							/*サブディレクトリーの場合*/
			offset = fcb->dir_entry * 32;				/*エントリーのオフセットを設定*/
			if( cluster_read( bpb, fcb->dir_cluster, 1 ) )
				return( -1 );		/*サブディレクトリー読込みエラー*/

			buffer = cluster_buffer.buffer;
		}

		buffer[offset+0] = 0xE5;

		if( fcb->dir_cluster==0 )
			root_dir_write();
		else
			cluster_write();

		/*クラスターチェインの削除*/
		cluster = dir->start_cluster;
		while( cluster>=2 && cluster!=0xFFFF ){
			cluster2 = next_cluster( fcb->bpb, cluster );
			update_cluster( fcb->bpb, cluster, 0 );
			cluster = cluster2;
		}

		return( 0 );
}

