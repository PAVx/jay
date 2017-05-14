/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

// leds.c

#include "leds.h"



void leds_init(uint8_t led) {
	if (led == SYSTEM_LED) {
		SYSTEM_LED_DDR |= (1 << SYSTEM_LED_DDPORT);
	}
	else if (led == GP_LED1) {
		GP_LED1_DDR |= (1 << GP_LED1_DDPORT);
	}
	else if (led == GP_LED2) {
		GP_LED2_DDR |= (1 << GP_LED2_DDPORT);
	}
	else if (led == DIGITAL_PIN_1) {
		DIGITAL_PIN_1_DDR |= (1 << DIGITAL_PIN_1_DDPORT);
	}
}
void toggle_led(uint8_t led) {
	if (led == SYSTEM_LED) {
		SYSTEM_LED_PORT ^= (1 << SYSTEM_LED_PIN);
	}
	else if (led == GP_LED1) {
		GP_LED1_PORT ^= (1 << GP_LED1_PIN);
	}
	else if (led == GP_LED2) {
		GP_LED2_PORT ^= (1 << GP_LED2_PIN);
	}
	else if (led == DIGITAL_PIN_1) {
		DIGITAL_PIN_1_PORT ^= (1 << DIGITAL_PIN_1_PIN);
	}
}
void led_off(uint8_t led){
	if (led == SYSTEM_LED) {
		SYSTEM_LED_PORT &= ~(1 << SYSTEM_LED_PIN);
	}
	else if (led == GP_LED1) {
		GP_LED1_PORT &= ~(1 << GP_LED1_PIN);
	}
	else if (led == GP_LED2) {
		GP_LED2_PORT &= ~(1 << GP_LED2_PIN);
	}
	else if (led == DIGITAL_PIN_1) {
		DIGITAL_PIN_1_PORT &= ~(1 << DIGITAL_PIN_1_PIN);
	}

}
void led_on(uint8_t led){
	if (led == SYSTEM_LED) {
		SYSTEM_LED_PORT |= (1 << SYSTEM_LED_PIN);
	}
	else if (led == GP_LED1) {
		GP_LED1_PORT |= (1 << GP_LED1_PIN);
	}
	else if (led == GP_LED2) {
		GP_LED2_PORT |= (1 << GP_LED2_PIN);
	}
		else if (led == DIGITAL_PIN_1) {
		DIGITAL_PIN_1_PORT |= (1 << DIGITAL_PIN_1_PIN);
	}
}
