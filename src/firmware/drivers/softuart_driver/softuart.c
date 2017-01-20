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
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

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

/* HELPER FUNCTION*/
void isr_routine(softUART *softUART);

void _set_tx_pin_high(softUART *softUART)
{
	softUART->pins.txport |=  ( 1 << softUART->pins.txbit );		//CHANGE: make this into a functino with pin struct
}
void _set_tx_pin_low(softUART *softUART)
{
	softUART->pins.txport &= ~( 1 << softUART->pins.txbit );
}
uint8_t _get_rx_pin_status(softUART *softUART)
{
	return softUART->pins.rxpin & ( 1 << softUART->pins.rxbit );
}

ISR(SOFTUART_T_COMP_LABEL)
{
	int i = 0;
	for(i = 0; i < SOFTUART_CHANNELS; i++)
	{
		isr_routine(&softUART_array[i]);
	}
	// // static unsigned char flag_rx_waiting_for_stop_bit = SU_FALSE;
	// // static unsigned char rx_mask;
	// //
	// // static unsigned char timer_rx_ctr;
	// // static unsigned char bits_left_in_rx;
	// // static unsigned char internal_rx_buffer;
	// //
	// // unsigned char start_bit, flag_in;
	// // unsigned char tmp;
	//
	// // Transmitter Section
	// if ( flag_tx_busy == SU_TRUE ) {
	// 	tmp = timer_tx_ctr;
	// 	if ( --tmp == 0 ) { // if ( --timer_tx_ctr <= 0 )
	// 		if ( internal_tx_buffer & 0x01 ) {
	// 			_set_tx_pin_high();
	// 		}
	// 		else {
	// 			_set_tx_pin_low();
	// 		}
	// 		internal_tx_buffer >>= 1;
	// 		tmp = 3; // timer_tx_ctr = 3;
	// 		if ( --bits_left_in_tx == 0 ) {
	// 			flag_tx_busy = SU_FALSE;
	// 		}
	// 	}
	// 	timer_tx_ctr = tmp;
	// }
	//
	// // Receiver Section
	// if ( flag_rx_off == SU_FALSE ) {
	// 	if ( flag_rx_waiting_for_stop_bit ) {
	// 		if ( --timer_rx_ctr == 0 ) {
	// 			flag_rx_waiting_for_stop_bit = SU_FALSE;
	// 			flag_rx_ready = SU_FALSE;
	// 			inbuf[qin] = internal_rx_buffer;
	// 			if ( ++qin >= SOFTUART_IN_BUF_SIZE ) {
	// 				// overflow - reset inbuf-index
	// 				qin = 0;
	// 			}
	// 		}
	// 	}
	// 	else {  // rx_test_busy
	// 		if ( flag_rx_ready == SU_FALSE ) {
	// 			start_bit = _get_rx_pin_status();
	// 			// test for start bit
	// 			if ( start_bit == 0 ) {
	// 				flag_rx_ready      = SU_TRUE;
	// 				internal_rx_buffer = 0;
	// 				timer_rx_ctr       = 4;
	// 				bits_left_in_rx    = RX_NUM_OF_BITS;
	// 				rx_mask            = 1;
	// 			}
	// 		}
	// 		else {  // rx_busy
	// 			tmp = timer_rx_ctr;
	// 			if ( --tmp == 0 ) { // if ( --timer_rx_ctr == 0 ) {
	// 				// rcv
	// 				tmp = 3;
	// 				flag_in = _get_rx_pin_status();
	// 				if ( flag_in ) {
	// 					internal_rx_buffer |= rx_mask;
	// 				}
	// 				rx_mask <<= 1;
	// 				if ( --bits_left_in_rx == 0 ) {
	// 					flag_rx_waiting_for_stop_bit = SU_TRUE;
	// 				}
	// 			}
	// 			timer_rx_ctr = tmp;
	// 		}
	// 	}
	// }
}

static void _io_init(softUART *container)
{
	// TX-Pin as output
	container->pins.txddr |=  ( 1 << container->pins.txbit );
	// RX-Pin as input
	container->pins.rxddr &= ~( 1 << container->pins.rxbit );
}

static void _timer_init(void)
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

void _softuart_init( softUART *softUART )
{

	/****************************************************************************/
	// These are the initialization for Software UART Channel 1
	#if SOFTUART_CHANNELS > 0
	softUART_array[0].pins.rxpin = SOFTUART_RXPIN_1;
	softUART_array[0].pins.rxddr = SOFTUART_RXDDR_1;
	softUART_array[0].pins.rxbit = SOFTUART_RXBIT_1;

	softUART_array[0].pins.txport = SOFTUART_TXPORT_1;
	softUART_array[0].pins.txddr = SOFTUART_TXDDR_1;
	softUART_array[0].pins.txbit = SOFTUART_TXBIT_1;
	#endif

	/****************************************************************************/
	// These are the initialization for Software UART Channel 2
	#if SOFTUART_CHANNELS > 1
	softUART_array[1].pins.rxpin = SOFTUART_RXPIN_2;
	softUART_array[1].pins.rxddr = SOFTUART_RXDDR_2;
	softUART_array[1].pins.rxbit = SOFTUART_RXBIT_2;

	softUART_array[1].pins.txport = SOFTUART_TXPORT_2;
	softUART_array[1].pins.txddr = SOFTUART_TXDDR_2;
	softUART_array[1].pins.txbit = SOFTUART_TXBIT_2;
	#endif

	/****************************************************************************/
	// These are the initialization for Software UART Channel 3
	#if SOFTUART_CHANNELS > 2
	softUART_array[2].pins.rxpin = SOFTUART_RXPIN_3;
	softUART_array[2].pins.rxddr = SOFTUART_RXDDR_3;
	softUART_array[2].pins.rxbit = SOFTUART_RXBIT_3;

	softUART_array[2].pins.txport = SOFTUART_TXPORT_3;
	softUART_array[2].pins.txddr = SOFTUART_TXDDR_3;
	softUART_array[2].pins.txbit = SOFTUART_TXBIT_3;
	#endif

	/****************************************************************************/
	// These are the initialization for Software UART Channel 4
	#if SOFTUART_CHANNELS > 3
	softUART_array[3].pins.rxpin = SOFTUART_RXPIN_4;
	softUART_array[3].pins.rxddr = SOFTUART_RXDDR_4;
	softUART_array[3].pins.rxbit = SOFTUART_RXBIT_4;

	softUART_array[3].pins.txport = SOFTUART_TXPORT_4;
	softUART_array[3].pins.txddr = SOFTUART_TXDDR_4;
	softUART_array[3].pins.txbit = SOFTUART_TXBIT_4;
	#endif

	int i = 0;
	for(i = 0; i < SOFTUART_CHANNELS; i++){
		softUART[i].tx.flag_tx_busy  = SU_FALSE;
		softUART[i].rx.flag_rx_ready = SU_FALSE;
		softUART[i].rx.flag_rx_off   = SU_FALSE;

		_set_tx_pin_high(&softUART[i]); /* mt: set to high to avoid garbage on init */
		_io_init(&softUART[i]);
	}

	_timer_init();
}

static void _idle(void)
{
	// timeout handling goes here
	// - but there is a "softuart_kbhit" in this code...
	// add watchdog-reset here if needed
}

void _softuart_turn_rx_on( softUART *softUART )
{
	softUART->rx.flag_rx_off = SU_FALSE;
}

void _softuart_turn_rx_off( softUART *softUART )
{
	softUART->rx.flag_rx_off = SU_TRUE;
}

char _softuart_getchar( softUART *softUART )
{
	char ch;

	while ( softUART->rx.qout == softUART->rx.qin ) {
		_idle();
	}
	ch = softUART->rx.inbuf[softUART->rx.qout];
	if ( ++(softUART->rx.qout) >= SOFTUART_IN_BUF_SIZE ) {
		softUART->rx.qout = 0;
	}

	return( ch );
}

unsigned char _softuart_kbhit( softUART *softUART )
{
	return( softUART->rx.qin != softUART->rx.qout );
}

void _softuart_flush_input_buffer( softUART *softUART )
{
	softUART->rx.qin  = 0;
	softUART->rx.qout = 0;
}

unsigned char _softuart_transmit_busy( softUART *softUART )
{
	return ( softUART->tx.flag_tx_busy == SU_TRUE ) ? 1 : 0;
}

void _softuart_putchar( const char ch, softUART *softUART )
{
	while ( softUART->tx.flag_tx_busy == SU_TRUE ) {
		; // wait for transmitter ready
		  // add watchdog-reset here if needed;
	}

	// invoke_UART_transmit
	softUART->tx.timer_tx_ctr       = 3;
	softUART->tx.bits_left_in_tx    = TX_NUM_OF_BITS;
	softUART->tx.internal_tx_buffer = ( ch << 1 ) | 0x200;
	softUART->tx.flag_tx_busy       = SU_TRUE;
}

void _softuart_puts( const char *s, softUART *softUART )
{
	while ( *s ) {
		_softuart_putchar( *s, softUART );
		s++;
		_delay_us(113);
	}
}

void _softuart_puts_p( const char *prg_s, softUART *softUART )
{
	char c;

	while ( ( c = pgm_read_byte( prg_s++ ) ) ) {
		_softuart_putchar(c, softUART);
	}
}

void isr_routine(softUART *softUART){
	softUART->isr.flag_rx_waiting_for_stop_bit = SU_FALSE;

	// Transmitter Section
	if ( softUART->tx.flag_tx_busy == SU_TRUE ) {
		softUART->isr.tmp = softUART->tx.timer_tx_ctr;
		if ( --(softUART->isr.tmp) == 0 ) { // if ( --timer_tx_ctr <= 0 )
			if ( softUART->tx.internal_tx_buffer & 0x01 ) {
				_set_tx_pin_high(softUART);
			}
			else {
				_set_tx_pin_low(softUART);
			}
			softUART->tx.internal_tx_buffer >>= 1;
			softUART->isr.tmp = 3; // timer_tx_ctr = 3;
			if ( --(softUART->tx.bits_left_in_tx) == 0 ) {
				softUART->tx.flag_tx_busy = SU_FALSE;
			}
		}
		softUART->tx.timer_tx_ctr = softUART->isr.tmp;
	}

	// Receiver Section
	if ( softUART->rx.flag_rx_off == SU_FALSE ) {
		if ( softUART->isr.flag_rx_waiting_for_stop_bit ) {
			if ( --(softUART->isr.timer_rx_ctr) == 0 ) {
				softUART->isr.flag_rx_waiting_for_stop_bit = SU_FALSE;
				softUART->rx.flag_rx_ready = SU_FALSE;
				softUART->rx.inbuf[softUART->rx.qin] = softUART->isr.internal_rx_buffer;
				if ( ++(softUART->rx.qin) >= SOFTUART_IN_BUF_SIZE ) {
					// overflow - reset inbuf-index
					softUART->rx.qin = 0;
				}
			}
		}
		else {  // rx_test_busy
			if ( softUART->rx.flag_rx_ready == SU_FALSE ) {
				softUART->isr.start_bit = _get_rx_pin_status(softUART);
				// test for start bit
				if ( softUART->isr.start_bit == 0 ) {
					softUART->rx.flag_rx_ready      = SU_TRUE;
					softUART->isr.internal_rx_buffer = 0;
					softUART->isr.timer_rx_ctr       = 4;
					softUART->isr.bits_left_in_rx    = RX_NUM_OF_BITS;
					softUART->isr.rx_mask            = 1;
				}
			}
			else {  // rx_busy
				softUART->isr.tmp = softUART->isr.timer_rx_ctr;
				if ( --(softUART->isr.tmp) == 0 ) { // if ( --timer_rx_ctr == 0 ) {
					// rcv
					softUART->isr.tmp = 3;
					softUART->isr.flag_in = _get_rx_pin_status(softUART);
					if ( softUART->isr.flag_in ) {
						softUART->isr.internal_rx_buffer |= softUART->isr.rx_mask;
					}
					softUART->isr.rx_mask <<= 1;
					if ( --(softUART->isr.bits_left_in_rx) == 0 ) {
						softUART->isr.flag_rx_waiting_for_stop_bit = SU_TRUE;
					}
				}
				softUART->isr.timer_rx_ctr = softUART->isr.tmp;
			}
		}
	}
}
