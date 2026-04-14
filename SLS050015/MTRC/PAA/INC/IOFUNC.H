/*@@@S*/

/**** InitA Module ****/
int		read_dipsw( void );			/* Read Jumper Switch */
void	watch_dog_reset( void );	/* Reset Watch Dog */

void	_Bankchange( short no );	/* Memory Bank Change 8000h-BFFFh */

void	__enable( void );			/* Interrupt Enable */
void	__disable( void );			/* Interrupt Disable */

int		ram_check( void );			/* ALL RAM Area check */
int		rom_check( void );			/* ALL ROM Area check */
void	HALTCPU( void );			/* HALT CPU */

/**** CommA Module ****/
void	asci_0_tx_disable( void );	/* CH.0 Disable Serial Send */
void	asci_0_tx_enable( void );	/* CH.0 Enable Serial Send */

void	asci_0_transmit( char data ); /* Ch.0 Send Data */

void	asci_0_recv_ir( char data, char status );	/* CH.0 Serial Receive Interrupt */
void	asci_0_send_ir( void );		/* CH.0 Serial Send Interrupt */

/**** PioA Module ****/
void	csio_output( char data );	/* CS I/O Send Data */
void	csio_strobe( char on );		/* CS I/O Strobe Signal */
short	switch_read( void );		/* Switch Read */
void	io_output_on1( char data1 );	/* I/O Output Signal ON */
void	io_output_on2( char data2 );	/* I/O Output Signal ON */
void	io_output_on3( char data3 );	/* I/O Output Signal ON */
void	io_output_on4( char data4 );	/* I/O Output Signal ON */
void	io_output_off1( char data1 );	/* I/O Output Signal OFF */
void	io_output_off2( char data2 );	/* I/O Output Signal OFF */
void	io_output_off3( char data3 );	/* I/O Output Signal OFF */
void	io_output_off4( char data4 );	/* I/O Output Signal OFF */

/**** TimeA Module ****/
void	timer_0_func( void );		/* Timer CH.0 Interrupt */
void	timer_1_func( void );		/* Timer CH.1 Interrupt */

extern	short	prg_version;

