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
void	asci_0_tx_enable( char disInt );	/* CH.0 Enable Serial Send */
void	asci_0_rx_disable( void );	/* CH.0 Disable Serial Recv */
void	asci_0_rx_enable( void );	/* CH.0 Enable Serial Recv */

void	asci_0_transmit( char data ); /* Ch.0 Send Data */

void	asci_0_recv_ir( char data, char status );	/* CH.0 Serial Receive Interrupt */
void	asci_0_send_ir( void );		/* CH.0 Serial Send Interrupt */

/**** PioA Module ****/
void	csio_output( char data );	/* CS I/O Send Data */
void	csio_strobe( char on );		/* CS I/O Strobe Signal */
short	switch_read( void );		/* Switch Read */
void	io_output_on( char data1, char data2 );	/* I/O Output Signal ON */
void	io_output_off( char data1, char data2 );	/* I/O Output Signal OFF */

void io_disp_set( char data );

/**** TimeA Module ****/
void	timer_0_func( void );		/* Timer CH.0 Interrupt */
void	timer_1_func( void );		/* Timer CH.1 Interrupt */

extern	short	prg_version;

