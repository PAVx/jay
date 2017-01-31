// softuart.c
// AVR-port of the generic software uart written in C
//
// Generic code from
// Colin Gittins, Software Engineer, Halliburton Energy Services
// (has been available from iar.com web-site -> application notes)
//
// Generic software uart written in C, requiring a timer set to 3 times
// the baud rate, and two software read/write pins for the receive and
// transmit functions.
//
// Modified/Expanded by Niraj Raniga Jan 19, 2017
//
// * Received characters are buffered
// * putchar(), getchar(), kbhit() and flush_input_buffer() are available
// * There is a facility for background processing while waiting for input
// The baud rate can be configured by changing the BAUD_RATE macro as
// follows:
//
// #define BAUD_RATE  19200.0
//
// The function init_uart() must be called before any comms can take place
//
// Interface routines required:
// 1. _get_rx_pin_status()
//    Returns 0 or 1 dependent on whether the receive pin is high or low.
// 2. _set_tx_pin_high()
//    Sets the transmit pin to the high state.
// 3. _set_tx_pin_low()
//    Sets the transmit pin to the low state.
// 4. _idle()
//    Background functions to execute while waiting for input.
// 5. _timer_set( BAUD_RATE )
//    Sets the timer to 3 times the baud rate.
// 6. _set_timer_interrupt( timer_isr )
//    Enables the timer interrupt.
//
// Functions provided:
// 1. void _flush_input_buffer( void )
//    Clears the contents of the input buffer.
// 2. char _kbhit( void )
//    Tests whether an input character has been received.
// 3. char _getchar( void )
//    Reads a character from the input buffer, waiting if necessary.
// 4. void _turn_rx_on( void )
//    Turns on the receive function.
// 5. void _turn_rx_off( void )
//    Turns off the receive function.
// 6. void _putchar( char )
//    Writes a character to the serial port.
//
// ---------------------------------------------------------------------

/*
	TODO:
		-	Fix init (I don't think all the members of the struct are
			being initialized).
		-	Test :(
		-	ISR is broken, not too sure what's going on.
		-	The #defined values are the actual registers. When I put them in a struct,
			they are no longer refering to a register.
		-	Once the porgram frreezes, the interrupt doesn't get called again.
*/

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart_driver.h"
#include "softuart.h"

#define SU_TRUE    1
#define SU_FALSE   0

// startbit and stopbit parsed internally (see ISR)
#define RX_NUM_OF_BITS (8)
// static char           inbuf[SOFTUART_IN_BUF_SIZE]; 	//CHANGE: have this in the struct
// static unsigned char  qin;													//CHANGE: have this in the struct
// static unsigned char           qout;								//CHANGE: have this in the struct
// static unsigned char  flag_rx_off;									//CHANGE: have this in the struct
// static unsigned char  flag_rx_ready;								//CHANGE: have this in the struct

// 1 Startbit, 8 Databits, 1 Stopbit = 10 Bits/Frame
#define TX_NUM_OF_BITS (10)
// static unsigned char  flag_tx_busy;									//CHANGE: have this in the struct
// static unsigned char  timer_tx_ctr;									//CHANGE: have this in the struct
// static unsigned char  bits_left_in_tx;							//CHANGE: have this in the struct
// static unsigned short internal_tx_buffer; /* ! mt: was type uchar - this was wrong */		//CHANGE: have this in the struct

//extern softUART softUART_array[SOFTUART_CHANNELS];
static softUART softUART_array[SOFTUART_CHANNELS];

/* HELPER FUNCTION*/
void isr_routine(softUART *softUART);

void _set_tx_pin_high(uint8_t i){

	#ifdef SOFTUART_TXPORT_1
	if(i == 1) {
		( SOFTUART_TXPORT_1 |=  ( 1 << SOFTUART_TXBIT_1 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_2
	else if(i == 2){
		( SOFTUART_TXPORT_2 |=  ( 1 << SOFTUART_TXBIT_2 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_3
	else if(i == 3){
		( SOFTUART_TXPORT_3 |=  ( 1 << SOFTUART_TXBIT_3 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_4
	else if(i == 4){
		( SOFTUART_TXPORT_4 |=  ( 1 << SOFTUART_TXBIT_4 ) );
	}
	#endif
}

void _set_tx_pin_low(uint8_t i){

	#ifdef SOFTUART_TXPORT_1
	if(i == 1) {
		( SOFTUART_TXPORT_1 &= ~( 1 << SOFTUART_TXBIT_1 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_2
	else if(i == 2){
		( SOFTUART_TXPORT_2 &= ~( 1 << SOFTUART_TXBIT_2 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_3
	else if(i == 3){
		( SOFTUART_TXPORT_3 &= ~( 1 << SOFTUART_TXBIT_3 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_4
	else if(i == 4){
		( SOFTUART_TXPORT_4 &= ~( 1 << SOFTUART_TXBIT_4 ) );
	}
	#endif
}

uint8_t _get_rx_pin_status(uint8_t i){

	if(i == 1) {
		return ( SOFTUART_RXPIN_1  &   ( 1 << SOFTUART_RXBIT_1 ) );
	}

	#ifdef SOFTUART_RXPIN_2
	else if(i == 2){
		return ( SOFTUART_RXPIN_2  &   ( 1 << SOFTUART_RXBIT_2 ) );
	}
	#endif

	#ifdef SOFTUART_RXPIN_3
	else if(i == 3){
		return ( SOFTUART_RXPIN_3  &   ( 1 << SOFTUART_RXBIT_3 ) );
	}
	#endif

	#ifdef SOFTUART_RXPIN_4
	else if(i == 4){
		return ( SOFTUART_RXPIN_4  &   ( 1 << SOFTUART_RXBIT_4 ) );
	}
	#endif

	return 0;
}

ISR(SOFTUART_T_COMP_LABEL)
{
	int i = 0;
	for(i = 0; i < SOFTUART_CHANNELS; i++)
	{
		isr_routine(&(softUART_array[i]));
	}
	//PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => onma
}

static void _io_init(void)
{
	/****************************************************************************/
	// These are the initialization for Software UART Channel 1
	#if SOFTUART_CHANNELS > 0
	// TX-Pin as output
	SOFTUART_TXDDR_1 |= ( 1 << SOFTUART_TXBIT_1 );
	// RX-Pin as input
	SOFTUART_RXDDR_1 &= ~( 1 << SOFTUART_RXBIT_1 );
	#endif

	/****************************************************************************/
	// These are the initialization for Software UART Channel 2
	#if SOFTUART_CHANNELS > 1
	// TX-Pin as output
	SOFTUART_TXDDR_2 |= ( 1 << SOFTUART_TXBIT_2 );
	// RX-Pin as input
	SOFTUART_RXDDR_2 &= ~( 1 << SOFTUART_RXBIT_2 );
	#endif

	/****************************************************************************/
	// These are the initialization for Software UART Channel 3
	#if SOFTUART_CHANNELS > 2
	// TX-Pin as output
	SOFTUART_TXDDR_3 |= ( 1 << SOFTUART_TXBIT_3 );
	// RX-Pin as input
	SOFTUART_RXDDR_3 &= ~( 1 << SOFTUART_RXBIT_3 );
	#endif

	/****************************************************************************/
	// These are the initialization for Software UART Channel 4
	#if SOFTUART_CHANNELS > 3
	// TX-Pin as output
	SOFTUART_TXDDR_4 |= ( 1 << SOFTUART_TXBIT_4 );
	// RX-Pin as input
	SOFTUART_RXDDR_4 &= ~( 1 << SOFTUART_RXBIT_4 );
	#endif

}

static void _timer_init(void)
{
	unsigned char sreg_tmp;

	sreg_tmp = SREG;
	//_uart_driver_SendByte(0x32);
	cli();
	//_uart_driver_SendByte(0x33);
	SOFTUART_T_COMP_REG = SOFTUART_TIMERTOP;     /* set top */

	SOFTUART_T_CONTR_REGA = SOFTUART_CTC_MASKA | SOFTUART_PRESC_MASKA;
	SOFTUART_T_CONTR_REGB = SOFTUART_CTC_MASKB | SOFTUART_PRESC_MASKB;

	SOFTUART_T_INTCTL_REG |= SOFTUART_CMPINT_EN_MASK;

	SOFTUART_T_CNT_REG = 0; /* reset counter */

	SREG = sreg_tmp;
}

void _softuart_init( void )
{
	int i = 0;
	for(i = 0; i < SOFTUART_CHANNELS; i++){
		softUART_array[i].tx.flag_tx_busy  = SU_FALSE;
		softUART_array[i].rx.flag_rx_ready = SU_FALSE;
		softUART_array[i].rx.flag_rx_off   = SU_FALSE;
		softUART_array[i].id = i + 1;
		_set_tx_pin_high(i + 1); /* mt: set to high to avoid garbage on init */
	}

	_io_init();
	_timer_init();
}

static void _idle(void)
{
	// timeout handling goes here
	// - but there is a "softuart_kbhit" in this code...
	// add watchdog-reset here if needed
}

void _softuart_turn_rx_on( uint8_t id )
{
	softUART_array[id].rx.flag_rx_off = SU_FALSE;
}

void _softuart_turn_rx_off( uint8_t id )
{
	softUART_array[id].rx.flag_rx_off = SU_TRUE;
}

char _softuart_getchar( uint8_t id )
{
	char ch;

	while ( softUART_array[id].rx.qout == softUART_array[id].rx.qin ) {
		_idle();
	}
	ch = softUART_array[id].rx.inbuf[softUART_array[id].rx.qout];
	if ( ++(softUART_array[id].rx.qout) >= SOFTUART_IN_BUF_SIZE ) {
		softUART_array[id].rx.qout = 0;
	}

	return( ch );
}

unsigned char _softuart_kbhit( uint8_t id )
{
	return( softUART_array[id].rx.qin != softUART_array[id].rx.qout );
}

void _softuart_flush_input_buffer( uint8_t id )
{
	softUART_array[id].rx.qin  = 0;
	softUART_array[id].rx.qout = 0;
}

unsigned char _softuart_transmit_busy( uint8_t id )
{
	return ( softUART_array[id].tx.flag_tx_busy == SU_TRUE ) ? 1 : 0;
}

void _softuart_putchar( const char ch, uint8_t id )
{
	while ( softUART_array[id].tx.flag_tx_busy == SU_TRUE ) {
		PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => on; // wait for transmitter ready
		  // add watchdog-reset here if needed;
	}
	PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off
	// invoke_UART_transmit
	softUART_array[id].tx.timer_tx_ctr       = 3;
	softUART_array[id].tx.bits_left_in_tx    = TX_NUM_OF_BITS;
	softUART_array[id].tx.internal_tx_buffer = ( ch << 1 ) | 0x200;
	softUART_array[id].tx.flag_tx_busy       = SU_TRUE;
}

void _softuart_puts( const char *s, uint8_t id )
{
	while ( *s ) {
		_softuart_putchar( *s, id );
		s++;
		_delay_us(113);
	}
}

void _softuart_puts_p( const char *prg_s, uint8_t id )
{
	char c;

	while ( ( c = pgm_read_byte( prg_s++ ) ) ) {
		_softuart_putchar(c, id);
	}
}

void isr_routine(softUART *softUART){
	PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off
	static unsigned char flag_rx_waiting_for_stop_bit[4];
	memset(flag_rx_waiting_for_stop_bit, SU_FALSE, 4 * sizeof(char));

	static unsigned char rx_mask[4];

	static unsigned char timer_rx_ctr[4];
	static unsigned char bits_left_in_rx[4];
	static unsigned char internal_rx_buffer[4];

	unsigned char start_bit, flag_in;
	unsigned char tmp;

	// softUART->isr.flag_rx_waiting_for_stop_bit = SU_FALSE;
	// softUART->isr.start_bit = 0;
	// softUART->isr.flag_in = 0;
	// softUART->isr.tmp = 0;

	// Transmitter Section
	if ( softUART->tx.flag_tx_busy == SU_TRUE ) {
		tmp = softUART->tx.timer_tx_ctr;
		if ( --(tmp) == 0 ) { // if ( --timer_tx_ctr <= 0 )
			if ( softUART->tx.internal_tx_buffer & 0x01 ) {
				_set_tx_pin_high(softUART->id);
			}
			else {
				_set_tx_pin_low(softUART->id);
			}
			softUART->tx.internal_tx_buffer >>= 1;
			tmp = 3; // timer_tx_ctr = 3;
			if ( --(softUART->tx.bits_left_in_tx) == 0 ) {
				softUART->tx.flag_tx_busy = SU_FALSE;
			}
		}
		softUART->tx.timer_tx_ctr = tmp;
	}

	// Receiver Section
	if ( softUART->rx.flag_rx_off == SU_FALSE ) {
		if ( flag_rx_waiting_for_stop_bit[(softUART->id) - 1] ) {
			if ( --(timer_rx_ctr[(softUART->id) - 1]) == 0 ) {
				flag_rx_waiting_for_stop_bit[(softUART->id) - 1] = SU_FALSE;
				softUART->rx.flag_rx_ready = SU_FALSE;
				softUART->rx.inbuf[softUART->rx.qin] = internal_rx_buffer[(softUART->id) - 1];
				if ( ++(softUART->rx.qin) >= SOFTUART_IN_BUF_SIZE ) {
					// overflow - reset inbuf-index
					softUART->rx.qin = 0;
				}
			}
		}
		else {  // rx_test_busy
			if ( softUART->rx.flag_rx_ready == SU_FALSE ) {
				start_bit = _get_rx_pin_status(softUART->id);
				// test for start bit
				if ( start_bit == 0 ) {
					softUART->rx.flag_rx_ready      = SU_TRUE;
					internal_rx_buffer[(softUART->id) - 1] = 0;
					timer_rx_ctr[(softUART->id) - 1]       = 4;
					bits_left_in_rx[(softUART->id) - 1]    = RX_NUM_OF_BITS;
					rx_mask[(softUART->id) - 1]            = 1;
				}
			}
			else {  // rx_busy
				tmp = timer_rx_ctr[(softUART->id) - 1];
				if ( --(tmp) == 0 ) { // if ( --timer_rx_ctr == 0 ) {
					// rcv
					tmp = 3;
					flag_in = _get_rx_pin_status(softUART->id);
					if ( flag_in ) {
						internal_rx_buffer[(softUART->id) - 1] |= rx_mask[(softUART->id) - 1];
					}
					rx_mask[(softUART->id) - 1] <<= 1;
					if ( --(bits_left_in_rx[(softUART->id) - 1]) == 0 ) {
						flag_rx_waiting_for_stop_bit[(softUART->id) - 1] = SU_TRUE;
					}
				}
				timer_rx_ctr[(softUART->id) - 1] = tmp;
			}
		}
	}
}
