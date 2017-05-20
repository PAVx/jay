/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

// SYSTEM DEFINES
#define COM (1)
#define PACKET (1)
#define LEDS (1)
#define MOTORS (1)
#define SYSTEM_TICK (1)
#define IMU (1)
//#define GPS (1)
#define PID_CONTROLLER (1)
#define BATTERY (1)
#define IR_CAM (1)

#ifdef IR_CAM
	#define IR_CAM_TIMER_ID (2)
	#define IR_CAM_TIMER_UPDATE_TIME_MS (1000000)
#endif

#ifdef IMU
	//#define GYRO (1)
	#define ACCEL (1)
	//#define MAGNOMETER (1)
	#ifdef GYRO
		#define TEMP_SENSOR
	#endif

	#define IMU_UPSIDEDOWN (1)
#endif

//#define SYSTEM_INIT_DEBUG_PRINTOUTS (1)


// CLIB INCLUDES
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

////////// UNIQUE //////////
#define DEVICE_ADDRESS (0x01)
////////////////////////////
#define BROADCAST_ALL_DESTINATION_ADDRESS (0xAA)
#define u_sec 1000000
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#ifndef DEFAULT_NUM_SIBLINGS
	#define DEFAULT_NUM_SIBLINGS (2) // a max value -- be conservative for packet memory reasons
#endif

#ifdef COM
	#define UART (1)
	#define SW_UART (1)

	#ifdef UART
		#ifndef HW_UART_BAUD
			#define HW_UART_BAUD (9600)
		#endif
	#endif

	#ifdef SW_UART
		#ifndef TIMER0_PRESCALE
			#define TIMER0_PRESCALE (64)
		#endif

		#ifndef SOFTUART_BAUD_RATE
			#define SOFTUART_BAUD_RATE (4800)

			#define SOFTUART_TIMERTOP (F_CPU/TIMER0_PRESCALE/SOFTUART_BAUD_RATE/3 - 1)
			#define TIMER0_PERIOD (SOFTUART_TIMERTOP)
            #define TICK_PERIOD_us ((SOFTUART_TIMERTOP * TIMER0_PRESCALE * u_sec)/ F_CPU)
		#endif

		#ifdef IMU
			#define IMU_UPDATE_RATE (2000)
			// timer0 is limited to 52 Hz -- DEAL WITH IT
			#define IMU_UPDATE_PERIOD_SECONDS (0.002420) // measured with OScope
			#define IMU_UPDATE_TIME_MS (500) //us
			#define IMU_TIMER_ID (1)
		#endif

		#ifdef PACKET
			#define PACKET_TIMER_ID (3)
			#define PACKET_UPDATE_TIME_MS (200000)
		#endif

		#define PID_RATE (2777)	// Hz
		#define PID_PERIOD ((TICK_PERIOD_us * u_sec)/PID_RATE)
		// timer0 is limited to 52 Hz -- DEAL WITH IT
		#define PID_UPDATE_PERIOD_SECONDS (0.002420)
		#define PID_TIMER_UPDATE_TIME_MS (666) //ms for timer
		#define PID_TIMER_ID (0)

		#define TIMER_PERIOD ((TIMER0_PERIOD * 1000) / F_CPU)

	#endif
#endif

#ifdef MOTORS
	#define MOTORS_SPIN
#endif

#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
	#ifndef UART
		#define UART (1)
	#endif
#endif

#ifndef TRUE
	#define TRUE (1)
#endif

#ifndef FALSE
	#define FALSE (0)
#endif

// AVR INCLUDES
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

// JAY INCLUDES
#include "drivers.h"
#include "protocols.h"
#include "control.h"
#include "system_tick.h"
#include "system_init.h"
#include "leds.h"
#include "types.h"
#include "nvmem.h"


#endif
