/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef _LEDS_H_
#define _LEDS_H_

#include <avr/io.h>

#define SYSTEM_LED (1)
#define SYSTEM_LED_DDR (DDRB)
#define SYSTEM_LED_PORT (PORTB)
#define SYSTEM_LED_PIN (PORTB5)
#define SYSTEM_LED_DDPORT (DDB5)

void leds_init(uint8_t led);
void toggle_led(uint8_t led);
void led_off(uint8_t led);
void led_on(uint8_t led);

#endif
