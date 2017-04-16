/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "system.h"
#include "system_tick.h"
#include "system.h"
#include "drivers.h"
#include "protocols.h"

#define SYSTEM_TIMEOUT (20) 
typedef struct {
    uint64_t ticked;
    uint16_t compare;
    bool flag;
    bool enable;
} tick_t;
static volatile tick_t tick_timer0;
static volatile uint64_t _tick_count;
static volatile uint8_t _ticked;

#ifndef SW_UART
	#warning "software uart not defining a timer for system tick!"
#endif

void clock_init(void)
{
    
	_tick_count = 0;
	_ticked = false;
}

uint16_t clock_time(void)
{
	return tick_timer0.compare;
}

// called in timer0, currently defined in the software uart
void system_tick(void) {
//	static uint64_t lapsed = 0;
/*
	_tick_count++;

	if ((lapsed >= SYSTEM_TIMEOUT) && (_ticked == FALSE)) {
    
		_ticked = TRUE;
		lapsed = 0;
	}
	else if ((lapsed < SYSTEM_TIMEOUT) && (_ticked == FALSE)) {
		lapsed++;
	}
    */
    if(tick_timer0.enable) {
        tick_timer0.ticked++;
        if (tick_timer0.ticked >= tick_timer0.compare) {  
            tick_timer0.flag = true;
            tick_timer0.ticked = 0;
        }
    }
}
void set_tick_timer(uint64_t ms) {
    tick_timer0.compare = (ms * 1000) / TICK_PERIOD_us;
    tick_timer0.enable = true;
}
bool tick_timer_flag(void) {
    return tick_timer0.flag;
}
void clear_tick_timer_flag(void) {
    tick_timer0.flag = false;
}
uint8_t system_ticked(void) {
	return _ticked;
}

void system_untick(void) {
	_ticked = false;
}

