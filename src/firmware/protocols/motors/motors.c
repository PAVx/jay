/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "motors.h"
#include "pwm_driver.h"
#include <util/delay.h>
#include <avr/io.h>

#define NUM_MOTORS (4)

struct motor {
	// duty cycle (<= 255)
	uint8_t current_speed;
};
typedef struct motor Motor_t;

static volatile Motor_t _motors[NUM_MOTORS];

void motors_initialize(void) {
	uint8_t speed = 0;

	_motors[MOTOR_ONE - 1].current_speed = 0;
	_motors[MOTOR_TWO - 1].current_speed = 0;
	_motors[MOTOR_THREE - 1].current_speed = 0;
	_motors[MOTOR_FOUR - 1].current_speed = 0;

	pwm_init();
	_delay_ms(3000);

	DDRD |= (1<<DDD3);
	DDRB |= (1<<DDB1);
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB3);

	// calibrate ESCs by varying the duty cycle of each pin from max to low
	for (speed = 0; speed < MAX_MOTOR_SPEED; speed++) {
		pwm_setval(speed, MOTOR_ONE);
		pwm_setval(speed, MOTOR_TWO);
		pwm_setval(speed, MOTOR_THREE);
		pwm_setval(speed, MOTOR_FOUR);

		_delay_ms(1);
	}
	for (speed = MAX_MOTOR_SPEED; speed > MIN_MOTOR_SPEED; speed--) {
		pwm_setval(speed, MOTOR_ONE);
		pwm_setval(speed, MOTOR_TWO);
		pwm_setval(speed, MOTOR_THREE);
		pwm_setval(speed, MOTOR_FOUR);

		_delay_ms(1);
	}
	for (speed = 0; speed < MAX_MOTOR_SPEED; speed++) {
		pwm_setval(speed, MOTOR_ONE);
		pwm_setval(speed, MOTOR_TWO);
		pwm_setval(speed, MOTOR_THREE);
		pwm_setval(speed, MOTOR_FOUR);

		_delay_ms(1);
	}
	for (speed = MAX_MOTOR_SPEED; speed > MIN_MOTOR_SPEED; speed--) {
		pwm_setval(speed, MOTOR_ONE);
		pwm_setval(speed, MOTOR_TWO);
		pwm_setval(speed, MOTOR_THREE);
		pwm_setval(speed, MOTOR_FOUR);

		_delay_ms(1);
	}
	for (speed = 0; speed < MAX_MOTOR_SPEED; speed++) {
		pwm_setval(speed, MOTOR_ONE);
		pwm_setval(speed, MOTOR_TWO);
		pwm_setval(speed, MOTOR_THREE);
		pwm_setval(speed, MOTOR_FOUR);

		_delay_ms(1);
	}
	for (speed = MAX_MOTOR_SPEED; speed > MIN_MOTOR_SPEED; speed--) {
		pwm_setval(speed, MOTOR_ONE);
		pwm_setval(speed, MOTOR_TWO);
		pwm_setval(speed, MOTOR_THREE);
		pwm_setval(speed, MOTOR_FOUR);

		_delay_ms(1);
	}
	for (speed = 0; speed < MAX_MOTOR_SPEED; speed++) {
		pwm_setval(speed, MOTOR_ONE);
		pwm_setval(speed, MOTOR_TWO);
		pwm_setval(speed, MOTOR_THREE);
		pwm_setval(speed, MOTOR_FOUR);

		_delay_ms(1);
	}
	for (speed = MAX_MOTOR_SPEED; speed > MIN_MOTOR_SPEED; speed--) {
		pwm_setval(speed, MOTOR_ONE);
		pwm_setval(speed, MOTOR_TWO);
		pwm_setval(speed, MOTOR_THREE);
		pwm_setval(speed, MOTOR_FOUR);

		_delay_ms(1);
	}

}
void motor_set(uint8_t motor_id, uint8_t speed) {
	if (motor_id > NUM_MOTORS) return;
	_motors[motor_id - 1].current_speed = speed;
	pwm_setval(speed, motor_id);
}

uint8_t motor_get_speed(uint8_t motor_id) {
	if (motor_id > NUM_MOTORS) return 0;
	return _motors[motor_id - 1].current_speed;
}
