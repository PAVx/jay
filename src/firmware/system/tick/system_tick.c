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
#define NUM_COMP 4
#define MAX_COMPARE_VALUE 65535
#define SYSTEM_TIMEOUT (20) 
typedef struct {
    uint16_t value;
    uint16_t period;
    bool flag;
    bool overflow;
    bool enable;
} tick_comp_t;
static tick_comp_t compare[NUM_COMP];
static volatile uint64_t _tick_count = 0;
static volatile uint8_t _tick_enabled = false;

#ifndef SW_UART
	#warning "software uart not defining a timer for system tick!"
#endif

void clock_start(void)
{
	_tick_count = 0;
	_tick_enabled = true;
}

uint16_t clock_time(void)
{
	return _tick_count;

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
    if(_tick_enabled) {
        _tick_count++;
        for(uint8_t i = 0; i < NUM_COMP; i++) {
            if (compare[i].enable) {
                if(_tick_count >= compare[i].value && !compare[i].overflow) {  
                
                    compare[i].value += compare[i].period;
                    if (compare[i].value <= _tick_count) {
                        compare[i].overflow = true;    
                    }
                    compare[i].flag = true;
                }   
            }
        }
        if (_tick_count > MAX_COMPARE_VALUE) {
            _tick_count = 0;
            for (uint8_t i = 0; i < NUM_COMP; i++) {
                compare[i].overflow = false;
            }

        }
    }

}
void set_tick_period(uint8_t compare_x, uint64_t us) {
    if (compare_x < NUM_COMP) {
        compare[compare_x].period = (us * 1) / TICK_PERIOD_us;
        compare[compare_x].value = compare[compare_x].period;
        compare[compare_x].enable = true;
    }
}
bool tick_timer_flag(uint8_t compare_x) {
    return compare[compare_x].flag;
}
void clear_tick_timer_flag(uint8_t compare_x) {
    compare[compare_x].flag = false;
}

