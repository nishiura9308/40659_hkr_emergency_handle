/*
/ ９８からフォントをぶっこ抜くソフト
/ ９８エミュレター用に開発
/ 出力は2048*2048,1bit/pixel のＤＩＢファイル
/ 2000/01/29
/ (c)長沢紫炎@JDC-SOFTWARE 1996,1999
*/
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>


void GetFontIO( unsigned int code , unsigned char *p);
void GetFontIO2( unsigned int code , unsigned char _far *p);

unsigned char	_far font[32];	/* フォントバッファ */

void main(int argv, char *argc[])
{
	FILE	*fp;		/* ファイルポインタ */

	unsigned int i,j,k,l,x,y;		/* カウンタ */

	fp = fopen("ANK.TXT","wt");

	/* ANKぶっこぬき */
	printf("ANK文字処理中\n");
	for(j=0;j<=0xff;j++)	/* キャラループ */
	{
printf("Code=%X\n",j);
		GetFontIO2(j+0x2000,font); 
		/* ループ展開 x:15~0
		// *(p+j+256*x)=font[(15-x)*2]
		// フォントをビットマップに変換 */
		fprintf(fp,"   CODE = %02X\n", j );
		for( k=0; k<16; k++ ){
			for( l=0; l<8; l++ ){
				if( font[k+2] & (0x80>>l) )
					fprintf(fp,"*");
				else
					fprintf(fp," ");
			}
			fprintf(fp,"\n");
		}
	}
	fclose(fp);					/*ファイルクローズ */
}

/*
// I/O からFont吸い取り
// code : JIS漢字コード
// p    : フォント保存領域(32Byte必要)
*/
union REGS inregs, outregs;
struct SREGS segregs;

void GetFontIO2( unsigned int code , unsigned char _far *p)
{
	code -= 0x2000;

    inregs.h.ah = 0x14;
	inregs.x.cx = FP_OFF( p );
	inregs.x.bx = FP_SEG( p );
	inregs.x.dx = code;
    int86( 0x18, &inregs, &outregs );

}

/*
// I/O からFont吸い取り
// code : JIS漢字コード
// p    : フォント保存領域(32Byte必要)
*/
void GetFontIO( unsigned int code , unsigned char *p)
{
	int rc, lr;

	_disable();

	code -= 0x2000;

	/* ドットアクセスモード */
	outp( 0x68, 0x0b);

	/* 下位コード指定 */
	while( (inp( 0x60 ) & (1<<5)) ) ;
	outp( 0xa1, (unsigned char)(code & 0x00ff));

	/* 上位コード指定 */
	while( (inp( 0x60 ) & (1<<5)) ) ;
	outp( 0xa3, (unsigned char)((code & 0xff00) >> 8));

	for( rc=0; rc<16; rc++ )
	{
		for( lr=0; lr<2; lr++ )
		{
			while( (inp( 0x60 ) & (1<<5)) ) ;
			outp( 0xa5, rc+(lr<<5) );
			while( (inp( 0x60 ) & (1<<5)) ) ;
			p[rc*2+(1-lr)] = (unsigned char)~inp( 0xa9 );
		}
	}
	/* コードアクセスモード */
	outp( 0x68, 0x0a);

	_enable();
}

