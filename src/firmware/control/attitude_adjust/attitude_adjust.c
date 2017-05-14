/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

// attitude_adjust.c

#include "attitude_adjust.h"
#include "pid.h"
#include "system.h"

#define IMU_UPDATE_DT (PID_UPDATE_PERIOD_SECONDS)

PID_t pidRoll;
PID_t pidPitch;
PID_t pidYaw;

static int32_t _throttle = 0;

// Yaw Pitch Roll Errors
double rollError = 0.0;
double pitchError = 0.0;
double yawError = 0.0;

char testing[30];
static uint8_t pid_print_flag = 0;

uint8_t InitializeAttitudeAdjust(void) {

	_throttle = 0;
	PIDInit(&pidRoll, 0, PID_ROLL_KP, PID_ROLL_KI, PID_ROLL_KD, IMU_UPDATE_DT);
	PIDInit(&pidPitch, 0, PID_PITCH_KP, PID_PITCH_KI, PID_PITCH_KD, IMU_UPDATE_DT);
	PIDInit(&pidYaw, 0, PID_YAW_KP, PID_YAW_KI, PID_YAW_KD, IMU_UPDATE_DT);
	PIDSetIntegralLimit(&pidRoll, PID_ROLL_INTEGRATION_LIMIT);
	PIDSetIntegralLimit(&pidPitch, PID_PITCH_INTEGRATION_LIMIT);
	PIDSetIntegralLimit(&pidYaw, PID_YAW_INTEGRATION_LIMIT);

	return 1;
}

void AttituteAdjustSetDesired(double yawDesired, double pitchDesired, double rollDesired){
	PIDSetDesired(&pidYaw, yawDesired);
	PIDSetDesired(&pidPitch, pitchDesired);
	PIDSetDesired(&pidRoll, rollDesired);
}

void AttituteAdjustUpdatePID(double yawActual, double pitchActual, double rollActual){
	if (pitchActual < (1.3 + pidPitch.desired) && pitchActual > (pidPitch.desired - 1.3)) {
		pitchActual = pidPitch.desired;
		PIDReset(&pidPitch);
	}
	yawError = PIDUpdate(&pidYaw, yawActual, UPDATE_ERROR);
	pitchError = PIDUpdate(&pidPitch, pitchActual, UPDATE_ERROR);
	rollError = PIDUpdate(&pidRoll, rollActual, UPDATE_ERROR);
}

void AttitudeSetThrottle(int32_t throttle) {
	_throttle = throttle;
}

void AttitudeAdjustGetError(int motor_changes[NUM_MOTORS]){
	int yawErrorInt = 0;
	int pitchErrorInt = 0;
	int rollErrorInt = 0;

	/* Motor Array: [mot1, mot2, mot3, mot4]
		mot1				mot2
		cw				ccw
		    	  	^
			      drone
		mot3				mot4
		ccw				cw
	*/

	// pitch forward = positive, pitch backwards = negative
	// roll left = positive, roll right = negative
	yawErrorInt 	= (int)(yawError * 10);
	pitchErrorInt 	= (int)(pitchError * 10);
	rollErrorInt 	= (int)(rollError * 10);

	yawErrorInt		/= 10;
	pitchErrorInt	/= 10;
	rollErrorInt	/= 10;

	motor_changes[MOTOR_ONE - 1] = _throttle - pitchErrorInt - rollErrorInt + yawErrorInt; 	//Calculate the pulse for esc 4 (front-left - CW)
	motor_changes[MOTOR_TWO - 1] = _throttle - pitchErrorInt + rollErrorInt - yawErrorInt; 	//Calculate the pulse for esc 1 (front-right - CCW)
	motor_changes[MOTOR_THREE - 1] = _throttle + pitchErrorInt - rollErrorInt - yawErrorInt; //Calculate the pulse for esc 3 (rear-left - CCW)
	motor_changes[MOTOR_FOUR - 1] = _throttle + pitchErrorInt + rollErrorInt + yawErrorInt; 	//Calculate the pulse for esc 2 (rear-right - CW)

	_throttle = 0;

	if(pid_print_flag == 100){
		// sprintf(testing, "           Y_ERR: {%d} | ", yawErrorInt);
		// UART_SendString(testing);
		// sprintf(testing, " P_ERR: {%d} | ", pitchErrorInt);
		// UART_SendString(testing);
		// sprintf(testing, " R_ERR: {%d} | ", rollErrorInt);
		// UART_SendString(testing);
		pid_print_flag = 0;
	}
	else pid_print_flag ++;

}

void AttitudeAdjustSetActuation(int motor_changes[NUM_MOTORS]) {
	int motor_val = 0;

	motor_val = (int)motor_get_speed(MOTOR_ONE) + motor_changes[MOTOR_ONE - 1];
	if (motor_val > MAX_DUTY_CYCLE) {
		motor_val = MAX_DUTY_CYCLE;
	}
	else if (motor_val < MIN_DUTY_CYCLE) {
		motor_val = motor_get_speed(MOTOR_ONE);
	}
	motor_set(MOTOR_ONE, (int32_t)motor_val);

	motor_val = (int)motor_get_speed(MOTOR_TWO) + motor_changes[MOTOR_TWO - 1];
	if (motor_val > MAX_DUTY_CYCLE) {
		motor_val = MAX_DUTY_CYCLE;
	}
	else if (motor_val < MIN_DUTY_CYCLE) {
		motor_val = motor_get_speed(MOTOR_TWO);
	}
	motor_set(MOTOR_TWO, (int32_t)motor_val);

	motor_val = (int)motor_get_speed(MOTOR_THREE) + motor_changes[MOTOR_THREE - 1];
	if (motor_val > MAX_DUTY_CYCLE) {
		motor_val = MAX_DUTY_CYCLE;
	}
	else if (motor_val < MIN_DUTY_CYCLE) {
		motor_val = motor_get_speed(MOTOR_THREE);
	}
	motor_set(MOTOR_THREE, (int32_t)motor_val);

	motor_val = (int)motor_get_speed(MOTOR_FOUR) + motor_changes[MOTOR_FOUR - 1];
	if (motor_val > MAX_DUTY_CYCLE) {
		motor_val = MAX_DUTY_CYCLE;
	}
	else if (motor_val < MIN_DUTY_CYCLE) {
		motor_val = motor_get_speed(MOTOR_FOUR);
	}
	motor_set(MOTOR_FOUR, (int32_t)motor_val);
}
