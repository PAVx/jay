/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Serena Mak, Niraj Raniga, Sargis S Yonan
	March 2017
*/

#ifndef _SYSTEM_TICK_H_
#define _SYSTEM_TICK_H_

#include <stdint.h>

void clock_start(void);
uint16_t clock_time(void);
void system_tick(void);
uint8_t system_ticked(void);
void system_untick(void);

void clear_tick_timer_flag(uint8_t compare_x);
bool tick_timer_flag(uint8_t compare_x);
void set_tick_period(uint8_t compare_x, uint64_t ms);

#endif /* _SYSTEM_TICK_H_ */
