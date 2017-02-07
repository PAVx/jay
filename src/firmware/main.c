#include "system.h"

int main (void) {

	system_initialize();
	
	// 50% duty cycle 127/255
	motor_set(MOTOR_ONE, 0x80);
	motor_set(MOTOR_TWO, 0x80);
	motor_set(MOTOR_THREE, 0x80);
	motor_set(MOTOR_FOUR, 0x80);

  	while(1);
}

