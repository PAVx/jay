// softuart.c
// AVR-port of the generic software uart written in C
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
// 1. get_rx_pin_status()
//    Returns 0 or 1 dependent on whether the receive pin is high or low.
// 2. set_tx_pin_high()
//    Sets the transmit pin to the high state.
// 3. set_tx_pin_low()
//    Sets the transmit pin to the low state.
// 4. idle()
//    Background functions to execute while waiting for input.
// 5. timer_set( BAUD_RATE )
//    Sets the timer to 3 times the baud rate.
// 6. set_timer_interrupt( timer_isr )
//    Enables the timer interrupt.
//
// Functions provided:
// 1. void flush_input_buffer( void )
//    Clears the contents of the input buffer.
// 2. char kbhit( void )
//    Tests whether an input character has been received.
// 3. char getchar( void )
//    Reads a character from the input buffer, waiting if necessary.
// 4. void turn_rx_on( void )
//    Turns on the receive function.
// 5. void turn_rx_off( void )
//    Turns off the receive function.
// 6. void putchar( char )
//    Writes a character to the serial port.
//
// ---------------------------------------------------------------------


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "softuart.h"
#include "system_tick.h"
#include "Queue.h"

#define SU_TRUE    1
#define SU_FALSE   0

// startbit and stopbit parsed internally (see ISR)
#define RX_NUM_OF_BITS (8)

// 1 Startbit, 8 Databits, 1 Stopbit = 10 Bits/Frame
#define TX_NUM_OF_BITS (10)

static softUART channel[SOFTUART_CHANNELS];

void set_tx_pin_high(int i) {
		#ifdef SOFTUART_TXPORT_1
		if(i == 0) {
			( SOFTUART_TXPORT_1 |=  ( 1 << SOFTUART_TXBIT_1 ) );
		}
		#endif

		#ifdef SOFTUART_TXPORT_2
		else if(i == 1){
			( SOFTUART_TXPORT_2 |=  ( 1 << SOFTUART_TXBIT_2 ) );
		}
		#endif

		#ifdef SOFTUART_TXPORT_3
		else if(i == 2){
			( SOFTUART_TXPORT_3 |=  ( 1 << SOFTUART_TXBIT_3 ) );
		}
		#endif

		#ifdef SOFTUART_TXPORT_4
		else if(i == 3){
			( SOFTUART_TXPORT_4 |=  ( 1 << SOFTUART_TXBIT_4 ) );
		}
		#endif
	}

void set_tx_pin_low(int i) {
	#ifdef SOFTUART_TXPORT_1
	if(i == 0) {
		( SOFTUART_TXPORT_1 &= ~( 1 << SOFTUART_TXBIT_1 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_2
	else if(i == 1){
		( SOFTUART_TXPORT_2 &= ~( 1 << SOFTUART_TXBIT_2 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_3
	else if(i == 2){
		( SOFTUART_TXPORT_3 &= ~( 1 << SOFTUART_TXBIT_3 ) );
	}
	#endif

	#ifdef SOFTUART_TXPORT_4
	else if(i == 3){
		( SOFTUART_TXPORT_4 &= ~( 1 << SOFTUART_TXBIT_4 ) );
	}
	#endif
}

int get_rx_pin_status(int i) {
	if(i == 0) {
		return ( SOFTUART_RXPIN_1  &   ( 1 << SOFTUART_RXBIT_1 ) );
	}

	#ifdef SOFTUART_RXPIN_2
	else if(i == 1){
		return ( SOFTUART_RXPIN_2  &   ( 1 << SOFTUART_RXBIT_2 ) );
	}
	#endif

	#ifdef SOFTUART_RXPIN_3
	else if(i == 2){
		return ( SOFTUART_RXPIN_3  &   ( 1 << SOFTUART_RXBIT_3 ) );
	}
	#endif

	#ifdef SOFTUART_RXPIN_4
	else if(i == 3){
		return ( SOFTUART_RXPIN_4  &   ( 1 << SOFTUART_RXBIT_4 ) );
	}
	#endif

	return 0;
}

ISR(SOFTUART_T_COMP_LABEL)
{
	system_tick();
	//int i = 1;
	for(int i = 0; i < SOFTUART_CHANNELS; i++){
		channel[i].isr.flag_rx_waiting_for_stop_bit = SU_FALSE;

		unsigned char start_bit = '0', flag_in = '0';
		unsigned char tmp = '0';

		// Transmitter Section
		if ( channel[i].tx.flag_tx_busy == SU_TRUE ) {

			if(channel[i].tx.flag_ok_to_pop == SU_TRUE){
				channel[i].isr.tx_byte = getFront(channel[i].tx.tx_buffer);
				Dequeue(channel[i].tx.tx_buffer);
				channel[i].tx.flag_ok_to_pop = SU_FALSE;
			}

			tmp = channel[i].tx.timer_tx_ctr;
			if ( --tmp == 0 ) { // if ( --timer_tx_ctr <= 0 )
				if ( channel[i].isr.tx_byte & 0x01 ) {
					set_tx_pin_high(i);
				}
				else {
					set_tx_pin_low(i);
				}

				channel[i].isr.tx_byte >>= 1;
				tmp = 3; // timer_tx_ctr = 3;
				if ( --channel[i].tx.bits_left_in_tx == 0 ) {
					channel[i].tx.flag_tx_busy = SU_FALSE;
					channel[i].tx.flag_ok_to_pop = SU_TRUE;
				} else if ( channel[i].tx.bits_left_in_tx%TX_NUM_OF_BITS == 0 ) {
					channel[i].tx.flag_ok_to_pop = SU_TRUE;
				}
			}
			channel[i].tx.timer_tx_ctr = tmp;
		}

		// Receiver Section
		if ( channel[i].rx.flag_rx_off == SU_FALSE ) {
			if ( channel[i].isr.flag_rx_waiting_for_stop_bit ) {
				if ( --channel[i].isr.timer_rx_ctr == 0 ) {
					channel[i].isr.flag_rx_waiting_for_stop_bit = SU_FALSE;
					channel[i].rx.flag_rx_ready = SU_FALSE;
					channel[i].rx.inbuf[channel[i].rx.qin] = channel[i].isr.internal_rx_buffer;
					if ( ++channel[i].rx.qin >= SOFTUART_IN_BUF_SIZE ) {
						// overflow - reset inbuf-index
						channel[i].rx.qin = 0;
					}
				}
			}
			else {  // rx_test_busy
				if ( channel[i].rx.flag_rx_ready == SU_FALSE ) {
					start_bit = get_rx_pin_status(i);
					// test for start bit
					if ( start_bit == 0 ) {
						channel[i].rx.flag_rx_ready      = SU_TRUE;
						channel[i].isr.internal_rx_buffer = 0;
						channel[i].isr.timer_rx_ctr       = 4;
						channel[i].isr.bits_left_in_rx    = RX_NUM_OF_BITS;
						channel[i].isr.rx_mask            = 1;
					}
				}
				else {  // rx_busy
					tmp = channel[i].isr.timer_rx_ctr;
					if ( --tmp == 0 ) { // if ( --timer_rx_ctr == 0 ) {
						// rcv
						tmp = 3;
						flag_in = get_rx_pin_status(i);
						if ( flag_in ) {
							channel[i].isr.internal_rx_buffer |= channel[i].isr.rx_mask;
						}
						channel[i].isr.rx_mask <<= 1;
						if ( --channel[i].isr.bits_left_in_rx == 0 ) {
							channel[i].isr.flag_rx_waiting_for_stop_bit = SU_TRUE;
						}
					}
					channel[i].isr.timer_rx_ctr = tmp;
				}
			}
		}
	}
}

static void io_init(void)
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

static void timer_init(void)
{
	unsigned char sreg_tmp;

	sreg_tmp = SREG;
	cli();

	SOFTUART_T_COMP_REG = SOFTUART_TIMERTOP;     /* set top */

	SOFTUART_T_CONTR_REGA = SOFTUART_CTC_MASKA | SOFTUART_PRESC_MASKA;
	SOFTUART_T_CONTR_REGB = SOFTUART_CTC_MASKB | SOFTUART_PRESC_MASKB;

	SOFTUART_T_INTCTL_REG |= SOFTUART_CMPINT_EN_MASK;

	SOFTUART_T_CNT_REG = 0; /* reset counter */

	SREG = sreg_tmp;
}

void softuart_init( void )
{
	for(int i = 0; i < SOFTUART_CHANNELS; i++){
		channel[i].tx.flag_tx_busy  = SU_FALSE;
		channel[i].rx.flag_rx_ready = SU_FALSE;
		channel[i].rx.flag_rx_off   = SU_FALSE;
		channel[i].tx.flag_ok_to_pop = SU_TRUE;
		channel[i].tx.tx_buffer = newQueue();
		set_tx_pin_high(i); /* mt: set to high to avoid garbage on init */
	}

	io_init();
	timer_init();
}

static void idle(void)
{
	// timeout handling goes here
	// - but there is a "softuart_kbhit" in this code...
	// add watchdog-reset here if needed
}

void softuart_turn_rx_on( int i )
{
	channel[i].rx.flag_rx_off = SU_FALSE;
}

void softuart_turn_rx_off( int i )
{
	channel[i].rx.flag_rx_off = SU_TRUE;
}

char softuart_getchar( int i )
{
	char ch;

	while ( channel[i].rx.qout == channel[i].rx.qin ) {
		idle();
	}
	ch = channel[i].rx.inbuf[channel[i].rx.qout];
	if ( ++channel[i].rx.qout >= SOFTUART_IN_BUF_SIZE ) {
		channel[i].rx.qout = 0;
	}

	return( ch );
}

unsigned char softuart_kbhit( int i )
{
	return( channel[i].rx.qin != channel[i].rx.qout );
}

void softuart_flush_input_buffer( int i )
{
	channel[i].rx.qin  = 0;
	channel[i].rx.qout = 0;
}

unsigned char softuart_transmit_busy( int i )
{
	return ( channel[i].tx.flag_tx_busy == SU_TRUE ) ? 1 : 0;
}

void softuart_putchar( const char ch , int i)
{
	while ( getLength(channel[i].tx.tx_buffer) >= SOFTUART_OUT_BUF_SIZE-1) {
		; // wait for transmitter ready
		  // add watchdog-reset here if needed;
	}

	// invoke_UART_transmit
	channel[i].tx.timer_tx_ctr       = 3;
	channel[i].tx.bits_left_in_tx    += TX_NUM_OF_BITS;			// V2: add number of bits to total needed to empty
	channel[i].tx.internal_tx_buffer = ( ch << 1 ) | 0x200;
	channel[i].tx.flag_tx_busy       = SU_TRUE;

	Enqueue(channel[i].tx.tx_buffer, (int)channel[i].tx.internal_tx_buffer);								// V2: added a queue as a buffer.
}

void softuart_puts( const char *s , int i)
{
	while ( *s ) {
		softuart_putchar( *s , i);
		s++;
	}
}

void softuart_puts_p( const char *prg_s , int i)
{
	char c;

	while ( ( c = pgm_read_byte( prg_s++ ) ) ) {
		softuart_putchar(c, i);
	}
}
