 




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
















extern unsigned short address; 


extern short nowReadIO;


static char readCount;
static short preReadIO;



void main( void )
{
int i;

extern void mcpInitialize( void );
extern void mcpSwitchIn( void );

io_output_off( 0x00, 0xFF ); 

mcpInitialize();

io_output_on( 0x00, 0x80 ); 

for( i=0; i<10; i++ ){
mcpSwitchIn();
}

address &= 0x0FFF0;
if( nowReadIO & 0x20 ){ 
address |= 1;
}
else { 
address |= 2;
}

__enable();

for( ;; ){
watch_dog_reset();
}
}

