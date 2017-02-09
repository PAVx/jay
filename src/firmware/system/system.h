#ifndef _SYSTEM_H_
#define _SYSTEM_H_

// AVR INCLUDES
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

// CLIB INCLUDES
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// JAY INCLUDES
#include "drivers.h"
#include "protocols.h"
#include "system_tick.h"
#include "system_init.h"
#include "leds.h"

// SYSTEM DEFINES
#define COM (1)
#define MOTORS (1)
#define SYSTEM_TICK (1)
#define GYRO (1)
#define ACCEL (1)

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#ifndef TRUE
	#define TRUE (1)
#endif 

#ifndef FALSE
	#define FALSE (0)
#endif

#ifndef HW_UART_BAUD
	#define HW_UART_BAUD (9600)
#endif

#endif