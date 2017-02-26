#include <avr/io.h>
#include <avr/interrupt.h>
#include "system.h"
#include "system_tick.h"
#include "system.h"

#include "protocols.h"

#define SYSTEM_TIMEOUT (20) 

static volatile uint64_t _time_ms;
static volatile uint8_t _ticked;

#ifndef SW_UART
	#warning "software uart not defining a timer for system tick!"
#endif

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
