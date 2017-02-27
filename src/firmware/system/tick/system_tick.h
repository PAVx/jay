#ifndef _SYSTEM_TICK_H_
#define _SYSTEM_TICK_H_

#include <stdint.h>

void clock_init(void);
uint64_t clock_time(void);
void system_tick(void);
uint8_t system_ticked(void);
void system_untick(void);

#endif /* _SYSTEM_TICK_H_ */