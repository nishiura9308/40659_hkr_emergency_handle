#include <stdio.h>

unsigned char	workbuff[0x20000];

/*************************************************************************
		改行文字の削除
*************************************************************************/
void crlf( char *p )
{
		int		len;

		while( (len=strlen(p))!=0 ){
			if( *( p+len-1 )=='\n' )
				*( p+len-1 )='\0';
			else if( *( p+len-1 )=='\r' )
				*( p+len-1 )='\0';
			else
				break;
		}
}

/*************************************************************************
		１バイト分のＨＥＸコードを変換する
*************************************************************************/
int hextobin( char *p, unsigned short *data )
{
		int		i;

		*data = 0;
		for( i=0; i<2; i++ ){
			*data *= 16;
			if( *p<'0' )	return 1;
			if( *p>'9' ){
				if( *p<'A' )	return 2;
				if( *p>'F' )	return 2;
				*data += ((*p)-'A')+0x0A;
			}
			else {
				*data += ((*p)-'0')+0x00;
			}
			p++;
		}
		return 0;
}

/*************************************************************************
		Ｓフォーマット変換

		INPUT:
			p = 文字列バッファー

		OUTPUT:
			data = データバッファー
					Address(Record Code Bytes) + Data + Sum
			len = データバイト数
					Address(Record Code Bytes) + Data + Sum

		RETURN:
			0 > 異常
			0 = S0 Record  Start Address
			1 = S1 Record  Word Address
			2 = S2 Record  3 Byte Address
			3 = S3 Record  DWord Address
			4 = S4 Record
			5 = S5 Record  Check Lines
			8 = S8 Record  End Record
*************************************************************************/
int sformtobin( char *p, unsigned char *data, int *len )
{
		unsigned short	bdata;
		int		sum,i,ret;

		if( *p!='S' )
			return( -1 );		/*Ｓフォーマットでない*/

		p++;
		ret = (int)(*p-'0')&0x0F;

		p++;
		if( hextobin( p, &bdata ) )
			return( -3 );		/*ＨＥＸコードでない*/
		p+=2;
		*len = (int)(bdata & 0x0FF);
		sum = *len;
		for( i=0; i<*len; i++ ){
			if( hextobin( p, &bdata ) )
				return( -3 );		/*ＨＥＸコードでない*/
			p+=2;
			*(data+i) = (unsigned char)bdata;
			sum += bdata;
			sum &= 0x0FF;
		}

		sum++;
		sum &= 0x0FF;

		if( sum ){
			return( -2 );
		}

		return( ret );
}

/*************************************************************************
		ＩＮＴＥＬフォーマット変換

		INPUT:
			p = 文字列バッファー

		OUTPUT:
			data = データバッファー
					Address(Record Code Bytes) + Data + Sum
			len = データバイト数
					Address(Record Code Bytes) + Data + Sum

		RETURN:
			0 > 異常
			0 = Data Record  Word Address
			1 = End Record
			2 = Segment Record  Segment Address
			3 = 
			4 = Segment Record  Segment Address
*************************************************************************/
int iformtobin( char *p, unsigned char *data, int *len )
{
		unsigned short	bdata;
		int		sum,i,ret;

		if( *p!=':' )
			return( -1 );		/*ＩＮＴＥＬフォーマットでない*/

		p++;
		if( hextobin( p, &bdata ) )
			return( -3 );		/*ＨＥＸコードでない*/
		p+=2;
		*len = (int)(bdata & 0x0FF);			/*変換バイト数*/
		sum = *len;

		if( hextobin( p, &bdata ) )
			return( -3 );		/*ＨＥＸコードでない*/
		p+=2;
		*data = (unsigned char)bdata;	/*Address High*/
		data++;
		sum += bdata;
		sum &= 0x0FF;

		if( hextobin( p, &bdata ) )
			return( -3 );		/*ＨＥＸコードでない*/
		p+=2;
		*data = (unsigned char)bdata;	/*Address Low*/
		data++;
		sum += bdata;
		sum &= 0x0FF;

		if( hextobin( p, &bdata ) )
			return( -3 );		/*ＨＥＸコードでない*/
		p+=2;
		ret = (int)(bdata & 0x0FF);		/*Hex Change Mark*/
		sum += bdata;
		sum &= 0x0FF;

		*len = *len + 2 + 1;
		for( i=0; i<*len-2; i++ ){
			if( hextobin( p, &bdata ) )
				return( -3 );		/*ＨＥＸコードでない*/
			p+=2;
			*(data+i) = (unsigned char)bdata;
			sum += bdata;
			sum &= 0x0FF;
		}

/**		sum++; **/
		sum &= 0x0FF;
		if( sum ){
			return( -2 );
		}

		return( ret );
}

/*************************************************************************
		ＲＯＭブート処理
*************************************************************************/
int boot( char *filename )
{
		FILE	*fd;
		char	buff[128];
		unsigned char data[128];
		int		len,i,j,k;
		long	adr,line;
		unsigned short	step;
		unsigned short	wdata[128];
		int		wlen;
		int		wadr;
		unsigned short	sts;
		int		first;
		int		format;
		unsigned char	*p;
		long	start_adr,end_adr;

		start_adr = 0x00000L;
		end_adr = 0x20000L;

		fd = fopen( filename, "rt" );
		if( fd==NULL ){
			return( 2 );
		}

		memset( workbuff, 0xFF, sizeof(workbuff) );

		first=1;
		line=0;

		while( fgets( buff, sizeof(buff), fd )!=NULL ){
			crlf( buff );

			if( buff[0]<0x20 )
				continue;
			format = sformtobin( buff, data, &len );
			if( format<0 ){
				if( first )
					continue;
				fclose( fd );
				return( 3 );
			}
			
			if( format==1 && len>3 ){
				line++;
				adr = (long)data[0]&0xFF;	adr<<=8;
				adr |= (long)data[1]&0xFF;
				i=2;
			}
			else if( format==2 && len>4 ){
				line++;
				adr = (long)data[0]&0xFF;	adr<<=8;
				adr |= (long)data[1]&0xFF;	adr<<=8;
				adr |= (long)data[2]&0xFF;
				i=3;
			}
			else if( format==5 && len>2 ){
				adr = (long)data[0]&0xFF;	adr<<=8;
				adr |= (long)data[1]&0xFF;
				if( adr!=line ){
					fclose( fd );
					return( 4 );
				}
			}
			else {
				continue;
			}
			if( adr>=start_adr && adr<end_adr ){
				if( first ){
					first = 0;
				}
				wlen=0;
				data[len-1]=0xFF;
				for( ; i<len-1; i++,adr++ ){
					if( adr>=start_adr && adr<=end_adr ){
						workbuff[adr] = data[i];
					}
				}
			}
		}

		if( first ){
			fclose( fd );
			return( 3 );
		}

		fclose( fd );

		return( 0 );
}


void main( int argc, char **argv )
{
		short	sumcode;
		long	i;

		if( argc<1 ){
			printf("Usage: gensum <filename>\n");
			exit(1);
		}

		if( boot( argv[1] )==0 ){
			printf("Loading Normal End\n");
		}
		else {
			printf("Illegal Format File\n");
			exit(1);
		}

		sumcode = 0;
		for( i=0; i<0x20000L; i++ ){
			sumcode = sumcode + ((short)workbuff[i] & 0x00FF);
		}

		printf("Sum Code = %04X\n", sumcode & 0x0FFFF );
}

