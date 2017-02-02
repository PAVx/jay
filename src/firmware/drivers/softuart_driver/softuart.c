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
#include "Queue.h"

#define SU_TRUE    1
#define SU_FALSE   0

// startbit and stopbit parsed internally (see ISR)
#define RX_NUM_OF_BITS (8)
// static char           inbuf[SOFTUART_IN_BUF_SIZE];
// static unsigned char  qin;
// static unsigned char           qout;
// static unsigned char  flag_rx_off;
// static unsigned char  flag_rx_ready;

// 1 Startbit, 8 Databits, 1 Stopbit = 10 Bits/Frame
#define TX_NUM_OF_BITS (10)
// static unsigned char  flag_tx_busy;
// static unsigned char  timer_tx_ctr;
// static unsigned char  bits_left_in_tx;
// static unsigned short internal_tx_buffer; /* ! mt: was type uchar - this was wrong */	// V2: This shouldn't be static

// Niraj Flags
// static unsigned char 	flag_ok_to_pop;
// Queue tx_buffer;

#define set_tx_pin_high()      ( SOFTUART_TXPORT_1 |=  ( 1 << SOFTUART_TXBIT_1 ) )
#define set_tx_pin_low()       ( SOFTUART_TXPORT_1 &= ~( 1 << SOFTUART_TXBIT_1 ) )
#define get_rx_pin_status()    ( SOFTUART_RXPIN_1  &   ( 1 << SOFTUART_RXBIT_1 ) )

static softUART channel_1;

ISR(SOFTUART_T_COMP_LABEL)
{
	channel_1.isr.flag_rx_waiting_for_stop_bit = SU_FALSE;
	// static unsigned char flag_rx_waiting_for_stop_bit = SU_FALSE;
	// static unsigned char rx_mask;
	//
	// static unsigned char timer_rx_ctr;
	// static unsigned char bits_left_in_rx;
	// static unsigned char internal_rx_buffer;
	//
	// static int tx_byte;

	unsigned char start_bit, flag_in;
	unsigned char tmp;

	// Transmitter Section
	if ( channel_1.tx.flag_tx_busy == SU_TRUE ) {

		if(channel_1.tx.flag_ok_to_pop == SU_TRUE){
			channel_1.isr.tx_byte = getFront(channel_1.tx.tx_buffer);
			Dequeue(channel_1.tx.tx_buffer);
			channel_1.tx.flag_ok_to_pop = SU_FALSE;
		}

		tmp = channel_1.tx.timer_tx_ctr;
		if ( --tmp == 0 ) { // if ( --timer_tx_ctr <= 0 )
			if ( channel_1.isr.tx_byte & 0x01 ) {
				set_tx_pin_high();
			}
			else {
				set_tx_pin_low();
			}

			channel_1.isr.tx_byte >>= 1;
			tmp = 3; // timer_tx_ctr = 3;
			if ( --channel_1.tx.bits_left_in_tx == 0 ) {
				channel_1.tx.flag_tx_busy = SU_FALSE;
				channel_1.tx.flag_ok_to_pop = SU_TRUE;
			} else if ( channel_1.tx.bits_left_in_tx%TX_NUM_OF_BITS == 0 ) {
				channel_1.tx.flag_ok_to_pop = SU_TRUE;
			}
		}
		channel_1.tx.timer_tx_ctr = tmp;
	}

	// Receiver Section
	if ( channel_1.rx.flag_rx_off == SU_FALSE ) {
		if ( channel_1.isr.flag_rx_waiting_for_stop_bit ) {
			if ( --channel_1.isr.timer_rx_ctr == 0 ) {
				channel_1.isr.flag_rx_waiting_for_stop_bit = SU_FALSE;
				channel_1.rx.flag_rx_ready = SU_FALSE;
				channel_1.rx.inbuf[channel_1.rx.qin] = channel_1.isr.internal_rx_buffer;
				if ( ++channel_1.rx.qin >= SOFTUART_IN_BUF_SIZE ) {
					// overflow - reset inbuf-index
					channel_1.rx.qin = 0;
				}
			}
		}
		else {  // rx_test_busy
			if ( channel_1.rx.flag_rx_ready == SU_FALSE ) {
				start_bit = get_rx_pin_status();
				// test for start bit
				if ( start_bit == 0 ) {
					channel_1.rx.flag_rx_ready      = SU_TRUE;
					channel_1.isr.internal_rx_buffer = 0;
					channel_1.isr.timer_rx_ctr       = 4;
					channel_1.isr.bits_left_in_rx    = RX_NUM_OF_BITS;
					channel_1.isr.rx_mask            = 1;
				}
			}
			else {  // rx_busy
				tmp = channel_1.isr.timer_rx_ctr;
				if ( --tmp == 0 ) { // if ( --timer_rx_ctr == 0 ) {
					// rcv
					tmp = 3;
					flag_in = get_rx_pin_status();
					if ( flag_in ) {
						channel_1.isr.internal_rx_buffer |= channel_1.isr.rx_mask;
					}
					channel_1.isr.rx_mask <<= 1;
					if ( --channel_1.isr.bits_left_in_rx == 0 ) {
						channel_1.isr.flag_rx_waiting_for_stop_bit = SU_TRUE;
					}
				}
				channel_1.isr.timer_rx_ctr = tmp;
			}
		}
	}
}

static void io_init(void)
{
	// TX-Pin as output
	SOFTUART_TXDDR_1 |=  ( 1 << SOFTUART_TXBIT_1 );
	// RX-Pin as input
	SOFTUART_RXDDR_1 &= ~( 1 << SOFTUART_RXBIT_1 );
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
	channel_1.tx.flag_tx_busy  = SU_FALSE;
	channel_1.rx.flag_rx_ready = SU_FALSE;
	channel_1.rx.flag_rx_off   = SU_FALSE;
	channel_1.tx.flag_ok_to_pop = SU_TRUE;

	channel_1.tx.tx_buffer = newQueue();

	set_tx_pin_high(); /* mt: set to high to avoid garbage on init */

	io_init();
	timer_init();
}

static void idle(void)
{
	// timeout handling goes here
	// - but there is a "softuart_kbhit" in this code...
	// add watchdog-reset here if needed
}

void softuart_turn_rx_on( void )
{
	channel_1.rx.flag_rx_off = SU_FALSE;
}

void softuart_turn_rx_off( void )
{
	channel_1.rx.flag_rx_off = SU_TRUE;
}

char softuart_getchar( void )
{
	char ch;

	while ( channel_1.rx.qout == channel_1.rx.qin ) {
		idle();
	}
	ch = channel_1.rx.inbuf[channel_1.rx.qout];
	if ( ++channel_1.rx.qout >= SOFTUART_IN_BUF_SIZE ) {
		channel_1.rx.qout = 0;
	}

	return( ch );
}

unsigned char softuart_kbhit( void )
{
	return( channel_1.rx.qin != channel_1.rx.qout );
}

void softuart_flush_input_buffer( void )
{
	channel_1.rx.qin  = 0;
	channel_1.rx.qout = 0;
}

unsigned char softuart_transmit_busy( void )
{
	return ( channel_1.tx.flag_tx_busy == SU_TRUE ) ? 1 : 0;
}

void softuart_putchar( const char ch )
{
	while ( getLength(channel_1.tx.tx_buffer) >= SOFTUART_OUT_BUF_SIZE-1) {
		; // wait for transmitter ready
		  // add watchdog-reset here if needed;
	}

	// invoke_UART_transmit
	channel_1.tx.timer_tx_ctr       = 3;
	channel_1.tx.bits_left_in_tx    = channel_1.tx.bits_left_in_tx + TX_NUM_OF_BITS;			// V2: add number of bits to total needed to empty
	channel_1.tx.internal_tx_buffer = ( ch << 1 ) | 0x200;
	channel_1.tx.flag_tx_busy       = SU_TRUE;

	Enqueue(channel_1.tx.tx_buffer, (int)channel_1.tx.internal_tx_buffer);								// V2: added a queue as a buffer.
}

void softuart_puts( const char *s )
{
	while ( *s ) {
		softuart_putchar( *s );
		s++;
	}
}

void softuart_puts_p( const char *prg_s )
{
	char c;

	while ( ( c = pgm_read_byte( prg_s++ ) ) ) {
		softuart_putchar(c);
	}
}
