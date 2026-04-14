 




int read_dipsw( void ); 
void watch_dog_reset( void ); 

void _Bankchange( short no ); 

void __enable( void ); 
void __disable( void ); 

void ram_check( void ); 
void rom_check( void ); 


void asci_0_tx_disable( void ); 
void asci_0_tx_enable( void ); 

void asci_0_transmit( char data ); 

void asci_0_recv_ir( char data, char status ); 
void asci_0_send_ir( void ); 


void csio_output( char data ); 
void csio_strobe( char on ); 
short switch_read( void ); 
void io_output_on( char data1, char data2 ); 
void io_output_off( char data1, char data2 ); 


void timer_0_func( void ); 
void timer_1_func( void ); 













struct POLLING {
unsigned short adr; 
unsigned char kind; 
unsigned char dataid; 
unsigned short carno; 
unsigned char nextstation; 
unsigned char destination; 
unsigned char route; 
unsigned char door; 
unsigned char voicebus[2]; 
unsigned char display[29]; 
unsigned char vlevel[16]; 
};




struct MCPSEND {
unsigned short adr; 
unsigned char dataid; 
unsigned char mcpsw[3]; 
unsigned short version; 
};












static const char col_pos[] =
{
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
};



extern char ledShiftData[16][10]; 



static char colCount; 




void timer_0_func( void )
{
extern void mcpSwitchIn( void );
extern void mcpDynamicScanDisplay( void );

mcpSwitchIn();

mcpDynamicScanDisplay();

}


void timer_1_func( void )
{
char *p;
int i;

p = ledShiftData[colCount];

csio_strobe( 1 );
for( i=0; i<10; i++,p++ ){
csio_output( *p );
}

io_output_off( 0x00, 0x01 ); 
csio_strobe( 0 ); 
io_output_off( 0x20, 0x00 ); 
io_output_off( 0x1F, 0x00 ); 
io_output_on( col_pos[colCount], 0x00 ); 
csio_strobe( 1 ); 
io_output_on( 0x20, 0x00 ); 
io_output_on( 0x00, 0x01 ); 

colCount++;
colCount&=0x0F; 
}

