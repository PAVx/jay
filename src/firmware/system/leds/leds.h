/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef _LEDS_H_
#define _LEDS_H_

#include <avr/io.h>

#define SYSTEM_LED (1)
#define SYSTEM_LED_DDR (DDRC)
#define SYSTEM_LED_PORT (PORTC)
#define SYSTEM_LED_PIN (PORTC1)
#define SYSTEM_LED_DDPORT (DDC1)

#define GP_LED1 (2)
#define GP_LED1_DDR (DDRC)
#define GP_LED1_PORT (PORTC)
#define GP_LED1_PIN (PORTC2)
#define GP_LED1_DDPORT (DDC2)

#define GP_LED2 (3)
#define GP_LED2_DDR (DDRC)
#define GP_LED2_PORT (PORTC)
#define GP_LED2_PIN (PORTC3)
#define GP_LED2_DDPORT (DDC3)

void leds_init(uint8_t led);
void toggle_led(uint8_t led);
void led_off(uint8_t led);
void led_on(uint8_t led);

#endif
