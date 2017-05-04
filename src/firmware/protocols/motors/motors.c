/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "motors.h"
#include "system.h"
#include "pwm_driver.h"
#include <util/delay.h>
#include <avr/io.h>

#define NUM_MOTORS (4)

struct motor {
	// duty cycle (<= 100)
	uint32_t current_speed;
};
typedef struct motor Motor_t;

static volatile Motor_t _motors[NUM_MOTORS];

void motors_initialize(void) {
	#ifdef MOTORS_SPIN
		uint8_t speed = 0;
	#endif

	_motors[MOTOR_ONE - 1].current_speed = 0;
	_motors[MOTOR_TWO - 1].current_speed = 0;
	_motors[MOTOR_THREE - 1].current_speed = 0;
	_motors[MOTOR_FOUR - 1].current_speed = 0;

	#ifdef MOTORS_SPIN
		pwm_init();
		_delay_ms(1000);

		// calibrate ESCs by varying the duty cycle of each pin from max to low
		for (speed = 0; speed < MAX_MOTOR_SPEED; speed++) {
			pwm_setval(speed, MOTOR_ONE);
			pwm_setval(speed, MOTOR_TWO);
			pwm_setval(speed, MOTOR_THREE);
			pwm_setval(speed, MOTOR_FOUR);

			_delay_ms(3);
		}
		for (speed = MAX_MOTOR_SPEED; speed > MIN_MOTOR_SPEED; speed--) {
			pwm_setval(speed, MOTOR_ONE);
			pwm_setval(speed, MOTOR_TWO);
			pwm_setval(speed, MOTOR_THREE);
			pwm_setval(speed, MOTOR_FOUR);

			_delay_ms(3);
		}

	#endif

}
void motor_set(uint8_t motor_id, int32_t speed) {
	#ifdef MOTORS_SPIN
		uint32_t motor_val = 0; // we need a big enough container for the multiplication
	#endif

	if (motor_id > NUM_MOTORS) {
		return;
	}

	if (speed < 0) {
		return; // this means that there was a sign error
		// we'll return to avoid actuating the motors based on an error
	}

	if (speed > MAX_DUTY_CYCLE) {
		return; // this would be an error
	}

	_motors[motor_id - 1].current_speed = (uint32_t)speed; // speed is 0-100

	#ifdef MOTORS_SPIN
		motor_val = ((speed * MAX_MOTOR_SPEED) / MOTOR_SCALED_SPEED);
		pwm_setval((uint8_t)motor_val, motor_id); // motor_val is 0-255 -- PWM needs this
	#endif
}

uint32_t motor_get_speed(uint8_t motor_id) {
	if (motor_id > NUM_MOTORS) return 0;
	return _motors[motor_id - 1].current_speed;
}
