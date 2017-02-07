#include <avr/io.h>
#include <avr/interrupt.h>
#include "system_tick.h"

#include "protocols.h"
#define SYSTEM_TIMEOUT (125)

volatile clock_time_t clock_millis;

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

void clock_init()
{
	// timer mode
	TCCR0A |= _BV(WGM01) | _BV(WGM00);

	// F_CPU/64/1000 = 125
	OCR0A = F_CPU/64/1000;
	// Enable timer set prescalar to 64
	TCCR0B |= _BV(CS01) | _BV(CS00);

	// Enable overflow interrupt
	TIMSK0 = _BV(TOIE0);

	sei();
}

clock_time_t clock_time()
{
	clock_time_t m;
	uint8_t oldSREG = SREG;
	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();

	m = timer0_millis;
	SREG = oldSREG;

	return m;

	//return clock_millis;
}

clock_time_t clock_time_micros()
{
	clock_time_t m;
	uint8_t oldSREG = SREG;
	uint8_t t;

	cli();

	m = timer0_overflow_count;
	t = TCNT0;

	if ((TIFR0 & _BV(TOV0)) && (t < 255))
			m++;

	SREG = oldSREG;

	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}


ISR(TIMER0_OVF_vect)
{
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
    		ITG3200_UpdateData();
    	#endif
    	#ifdef COM
	    	receive_packet();
    		packet_send();
    	#endif
    	#ifdef LEDS
    		toggle_led(SYSTEM_LED);
    	#endif

	// todo:
		// (???if crashing)(disable all other interrupts)
		// update accel registers
		// update gps registers
		// update PID controller for stabality 
		// (???if crashing)(enable all other interrupts)

		lapsed = 0;
	}

}