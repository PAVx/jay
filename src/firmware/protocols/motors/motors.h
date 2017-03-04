/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#include <stdint.h>

#define MOTOR_ONE (1) // OCR2B -- PD3 -- ~3
#define MOTOR_TWO (2) // OCR2A -- PB3 -- ~11
#define MOTOR_THREE (3) // OCR1B -- PB2 -- ~10
#define MOTOR_FOUR (4) // OCR1A -- PB1 -- ~9

#define MAX_MOTOR_SPEED (0xFF)
#define MIN_MOTOR_SPEED (0x00)

void motors_initialize(void);
void motor_set(uint8_t motor_id, uint8_t speed);
uint8_t motor_get_speed(uint8_t motor_id);