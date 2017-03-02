#include <avr/io.h>
#include <avr/interrupt.h>
#include "system.h"
#include "system_tick.h"
#include "system.h"

#include "protocols.h"
<<<<<<< HEAD
#define SYSTEM_TIMEOUT (1250)
#define SYSTEM_TICK_TIMERTOP F_CPU/64/1000/10
#if (SOFTUART_TIMERTOP < SYSTEM_TICK_TIMERTOP)
		#warning "Make sure OCR0B is less than OCR0A"
#endif

volatile clock_time_t clock_millis;
=======
>>>>>>> 1324c4148930755d8cd1d9e08076e7146ba4f623

#define SYSTEM_TIMEOUT (20) 

static volatile uint64_t _time_ms;
static volatile uint8_t _ticked;

<<<<<<< HEAD
void clock_init()
{
	// timer mode
	TCCR0A |= _BV(WGM01); // | _BV(WGM00);

	// F_CPU/64/1000 = 125
	OCR0B = SYSTEM_TICK_TIMERTOP;

	// Enable timer set prescalar to 64
	TCCR0B |= _BV(CS01) | _BV(CS00);

	// Enable overflow interrupt
	TIMSK0 |= _BV(OCIE0B); //_BV(TOIE0);

	sei();
=======
#ifndef SW_UART
	#warning "software uart not defining a timer for system tick!"
#endif
>>>>>>> 1324c4148930755d8cd1d9e08076e7146ba4f623

void clock_init(void)
{
	_time_ms = 0;
	_ticked = FALSE;
}

uint64_t clock_time(void)
{
	return _time_ms;
}

// called in timer0, currently defined in the software uart
void system_tick(void) {
	static uint64_t lapsed = 0;

	_time_ms++;

	if ((lapsed >= SYSTEM_TIMEOUT) && (_ticked == FALSE)) {
		_ticked = TRUE;
		lapsed = 0;
	}
	else if ((lapsed < SYSTEM_TIMEOUT) && (_ticked == FALSE)) {
		lapsed++;
	}
}

uint8_t system_ticked(void) {
	return _ticked;
}

void system_untick(void) {
	_ticked = FALSE;
}
<<<<<<< HEAD

ISR(TIMER0_COMPB_vect)
{
	PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
	static volatile uint64_t lapsed = 0;
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;

	lapsed++;

/****************************************/
	if (lapsed > SYSTEM_TIMEOUT) {
		#ifdef GYRO
    		Gyro_Update();
    	#endif

    	#ifdef ACCEL
    		Accel_Update();
    	#endif

    	#ifdef COM
	    //	receive_packet();
    	//	packet_send();
    	#endif
    	#ifdef LEDS
    	//	toggle_led(SYSTEM_LED);
    	#endif

	// todo:
		// (???if crashing)(disable all other interrupts)
		// update accel registers
		// update gps registers
		// update PID controller for stabality
		// (???if crashing)(enable all other interrupts)

		lapsed = 0;
		_ticked = TRUE;
	}

}
=======
>>>>>>> 1324c4148930755d8cd1d9e08076e7146ba4f623
