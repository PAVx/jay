/*
    PAVx -- Pod-Based Autonomous Vehicles
    Library Created By: Niraj Raniga
    March 2017
*/

#include "battery.h"
#include "uart.h"

static uint16_t _last_voltage = 0.0;

void battery_initialize(void) {
	_last_voltage = 0.0;
}

uint16_t battery_get_voltage(void) {
	return _last_voltage;
}

bool battery_charged(void) {
	_last_voltage = adc_read(ADC_PRESCALER_128, ADC_VREF_AREF, BATTERY_PIN);
	if (_last_voltage < 790) {
		UART_SendString(" WARNING -- BATTERY LOW");
		return false;
	}
	return true;
}
