 




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




















void mcpInitialize( void )
{

}

