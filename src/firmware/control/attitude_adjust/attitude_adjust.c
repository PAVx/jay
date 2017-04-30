/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

// attitude_adjust.c

#include "attitude_adjust.h"
#include "pid.h"
#include "system.h"

#define IMU_UPDATE_DT 1/PID_RATE

PID_t pidRoll;
PID_t pidPitch;
PID_t pidYaw;

// Yaw Pitch Roll Errors
int16_t rollError;
int16_t pitchError;
int16_t yawError;

uint8_t InitializeAttitudeAdjust(void) {

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
	yawError = PIDUpdate(&pidYaw, yawActual, UPDATE_ERROR);
	pitchError = PIDUpdate(&pidPitch, pitchActual, UPDATE_ERROR);
	rollError = PIDUpdate(&pidRoll, rollActual, UPDATE_ERROR);
}

/*
void AttitudeAdjustCorrectRatePID(
	double rollRateActual, double pitchRateActual, double yawRateActual,
	double rollRateDesired, double pitchRateDesired, double yawRateDesired) {

	PIDSetDesired(&pidRollRate, rollRateDesired);
	rollOutput = saturateSignedInt16(pidUpdate(&pidRollRate, rollRateActual, UPDATE_ERROR));

	PIDSetDesired(&pidPitchRate, pitchRateDesired);
	pitchOutput = saturateSignedInt16(pidUpdate(&pidPitchRate, pitchRateActual, UPDATE_ERROR));

	PIDSetDesired(&pidYawRate, yawRateDesired);
	yawOutput = saturateSignedInt16(pidUpdate(&pidYawRate, yawRateActual, UPDATE_ERROR));
}

void AttitudeAdjustRunPID(
	double eulerRollActual, double eulerPitchActual, double eulerYawActual,
	double eulerRollDesired, double eulerPitchDesired, double eulerYawDesired,
	double* rollRateDesired, double* pitchRateDesired, double* yawRateDesired)	{

	double yawError;

	PIDSetDesired(&pidRoll, eulerRollDesired);
	*rollRateDesired = PIDUpdate(&pidRoll, eulerRollActual, UPDATE_ERROR);

	// Update PID for pitch axis
	PIDSetDesired(&pidPitch, eulerPitchDesired);
	*pitchRateDesired = PIDUpdate(&pidPitch, eulerPitchActual, UPDATE_ERROR);

	// Update PID for yaw axis
	yawError = eulerYawDesired - eulerYawActual;
	if (yawError > 180.0) {
		yawError -= 360.0;
	} else if (yawError < -180.0) {
		yawError += 360.0;
	}
	PIDSetError(&pidYaw, yawError);
	*yawRateDesired = PIDUpdate(&pidYaw, eulerYawActual, NO_UPDATE);
}

void AttitudeAdjustGetActuatorOutput(int16_t* roll, int16_t* pitch, int16_t* yaw) {
	*roll = rollOutput;
	*pitch = pitchOutput;
	*yaw = yawOutput;
}
*/

void AttitudeAdjustGetError(int16_t motor_changes[NUM_MOTORS]){
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

	motor_changes[0] = (-1 * ((int)yawError)) - ((int)pitchError) - ((int)rollError);
	motor_changes[1] =  ((int)yawError) - ((int)pitchError) + ((int)rollError);
	motor_changes[2] =  ((int)yawError) + ((int)pitchError) - ((int)rollError);
	motor_changes[3] = -1 * ((int)yawError) + ((int)pitchError) + ((int)rollError);
}

void AttitudeAdjustSetActuation(int16_t motor_changes[NUM_MOTORS]) {
	int16_t motor_val = 0;

	motor_val = motor_get_speed(MOTOR_ONE);
	if (motor_val + motor_changes[MOTOR_ONE - 1] > MAX_DUTY_CYCLE) {
		motor_val = MAX_DUTY_CYCLE;
	}
	else if (motor_val + motor_changes[MOTOR_ONE - 1] < MIN_DUTY_CYCLE) {
		motor_val = motor_get_speed(MOTOR_ONE);
	}
	motor_set(MOTOR_ONE, motor_val);

	motor_val = motor_get_speed(MOTOR_TWO);
	if (motor_val + motor_changes[MOTOR_TWO - 1] > MAX_DUTY_CYCLE) {
		motor_val = MAX_DUTY_CYCLE;
	}
	else if (motor_val + motor_changes[MOTOR_TWO - 1] < MIN_DUTY_CYCLE) {
		motor_val = motor_get_speed(MOTOR_TWO);
	}
	motor_set(MOTOR_TWO, motor_val);

	motor_val = motor_get_speed(MOTOR_THREE);
	if (motor_val + motor_changes[MOTOR_THREE - 1] > MAX_DUTY_CYCLE) {
		motor_val = MAX_DUTY_CYCLE;
	}
	else if (motor_val + motor_changes[MOTOR_THREE - 1] < MIN_DUTY_CYCLE) {
		motor_val = motor_get_speed(MOTOR_THREE);
	}
	motor_set(MOTOR_THREE, motor_val);

	motor_val = motor_get_speed(MOTOR_FOUR);
	if (motor_val + motor_changes[MOTOR_FOUR - 1] > MAX_DUTY_CYCLE) {
		motor_val = MAX_DUTY_CYCLE;
	}
	else if (motor_val + motor_changes[MOTOR_FOUR - 1] < MIN_DUTY_CYCLE) {
		motor_val = motor_get_speed(MOTOR_FOUR);
	}
	motor_set(MOTOR_FOUR, motor_val);
}