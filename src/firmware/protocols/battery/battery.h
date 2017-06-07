/*
    PAVx -- Pod-Based Autonomous Vehicles
    Library Created By: Niraj Raniga
    March 2017
*/

#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "adc.h"
#include <stdbool.h>

#define BATTERY_PIN (0) // Channel 0 == PC0
#define BATTERY_LOW_THRESHOLD (10.2) // volts

#define BATTERY_SCALING_R1 (33000) // Ohms
#define BATTERY_SCALING_R2 (22000) // Ohms

#define BATTERY_REVERSE_SCALING ((BATTERY_SCALING_R1 + BATTERY_SCALING_R2) / BATTERY_SCALING_R2)

void battery_initialize(void);
uint16_t battery_get_voltage(void);
bool battery_charged(void);

#endif
