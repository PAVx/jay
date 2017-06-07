/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga, Sargis S Yonan
	March 2017
*/

#include <stdint.h>

#define MOTOR_ONE (1) // OCR2B -- PD3 -- ~3
#define MOTOR_TWO (2) // OCR2A -- PB3 -- ~11
#define MOTOR_THREE (3) // OCR1B -- PB2 -- ~10
#define MOTOR_FOUR (4) // OCR1A -- PB1 -- ~9

#define NUM_MOTORS (4)

#define MAX_MOTOR_SPEED (0xFF) // actual motor value
#define MIN_MOTOR_SPEED (0x00) // actual motor value
#define MOTOR_SCALED_SPEED (10000)	// 100 = full motor range

#define MIN_DUTY_CYCLE (2000)
#define MAX_DUTY_CYCLE (10000)

void motors_initialize(void);

// speed -- a value representing a PWM duty cycle 0-10000
void motor_set(uint8_t motor_id, int32_t speed);

uint32_t motor_get_speed(uint8_t motor_id);
