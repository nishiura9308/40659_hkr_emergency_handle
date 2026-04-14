 




















typedef  unsigned int  size_t;
typedef  unsigned int  ptrdiff_t;






  void *memcpy(void *s1, const void *s2, size_t n);

  void *memmove(void *s1, const void *s2, size_t n);

  void *memchr(const void *s, int c, size_t n);

  void *memset(void *s, int c, size_t n);

  int memcmp(const void *s1, const void *s2, size_t n);

  char *strchr(const char *s, int c);

  int strcmp(const char *s1, const char *s2);

  int strncmp(const char *s1, const char *s2, size_t n);

  int strcoll(const char *s1, const char *s2);

  size_t strlen(const char *s);

  size_t strcspn(const char *s1, const char *s2);

  size_t strspn(const char *s1, const char *s2);

  char *strpbrk(const char *s1, const char *s2);

  char *strrchr(const char *s, int c);

  char *strstr(const char *s1, const char *s2);

  char *strcat(char *s1, const char *s2);

  char *strncat(char *s1, const char *s2, size_t n);

  char *strcpy(char *s1, const char *s2);

  char *strncpy(char *s1, const char *s2, size_t n);

  char *strerror(int errnum);

  char *strtok(char *s1, const char *s2);

  size_t strxfrm(char *s1, const char *s2, size_t n);









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






unsigned short address; 


struct MCPSEND sendarea;











extern short sw_led; 

extern char matrix_led[4][6]; 
extern char sw_blink; 
extern char matrix_blink[2]; 



static unsigned char recvbuff[64]; 
static unsigned char recvlen; 
static unsigned char recvphase; 
static unsigned char recvdle; 
static unsigned char recvbcc; 
static unsigned char sendbuff[16]; 
static unsigned char sendlen; 
static unsigned char sendcnt; 



void mcpSend( void )
{
unsigned char *p; 
unsigned char sendbcc; 
int i;

asci_0_tx_enable(); 

sendcnt = 0; 
sendlen = 0; 


sendbuff[sendlen++] =  (0x10) ;
sendbuff[sendlen++] =  (0x02) ;

sendbcc = 0; 
p = (unsigned char *)&sendarea; 
for( i=0; i<sizeof(sendarea); i++, p++ ){
sendbcc ^= *p; 
if( *p== (0x02)  || *p== (0x03)  || *p== (0x10)  ){
sendbuff[sendlen++] =  (0x10) ; 
sendbuff[sendlen++] = *p + 0x40;
}
else {
sendbuff[sendlen++] = *p;
}
}

sendbuff[sendlen++] =  (0x10) ;
sendbuff[sendlen++] =  (0x03) ;
sendbcc ^=  (0x03) ; 

if( sendbcc== (0x02)  || sendbcc== (0x03)  || sendbcc== (0x10)  ){
sendbuff[sendlen++] =  (0x10) ; 
sendbuff[sendlen++] = sendbcc + 0x40;
}
else {
sendbuff[sendlen++] = sendbcc; 
}

sendbuff[sendlen++] = 0; 

asci_0_send_ir(); 
}


char mcpReceive( unsigned char *data, char len )
{
struct POLLING *p;

p = (struct POLLING *)data;

if( len!=sizeof(*p) ){

return 0;
}


memcpy( &sw_led, &p->display[0], 2 ); 
memcpy( matrix_led, &p->display[2], 4*6 ); 
sw_blink = p->display[2+4*6]; 
memcpy( matrix_blink, &p->display[2+4*6+1], 2 ); 


if( p->kind=='S' ){
address &= 0x000F;
address |= (p->carno<<4) & 0x0FFF0;
}


if( address == p->adr ){
return 1;
}
return 0;
}



void asci_0_recv_ir( char data, char status )
{



status=0;
if( data ==  (0x10)  ){ 
recvdle = 1;
return;
}
if( data ==  (0x02)  ){ 
if( recvdle ){ 
recvdle = 0;
recvphase =  1 ;
recvlen = 0;
recvbcc = 0;
}
return;
}

switch( recvphase ){
case  0 : 
break;

case  1 :
if( recvdle ){ 
recvdle = 0;
if( data ==  (0x03)  ){ 
recvbcc ^= data;
recvphase =  2 ;
break;
}
data = data - 0x40; 
}

if( recvlen<sizeof(recvbuff) ){
recvbcc ^= data;
recvbuff[recvlen++] = data;
}
else {
recvphase =  0 ;
}
break;

case  2 :
if( recvdle ){ 
recvdle = 0;
data = data - 0x40; 
}
if( data == recvbcc ){
if( mcpReceive( recvbuff, recvlen ) ){
mcpSend();
}
}
recvphase =  0 ;
break;
}
}


void asci_0_send_ir( void )
{
if( sendcnt==sendlen ){ 
asci_0_tx_disable(); 
return;
}

asci_0_transmit( sendbuff[sendcnt++] ); 
}

