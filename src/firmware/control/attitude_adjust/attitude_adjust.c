/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

// attitude_adjust.c

#include "attitude_adjust.h"
#include "motors.h"

//#define USE_RATES

PID_t pidRollRate;
PID_t pidPitchRate;
PID_t pidYawRate;

PID_t pidRoll;
PID_t pidPitch;
PID_t pidYaw;

// Yaw Pitch Roll Rate Errors
int16_t rollOutput;
int16_t pitchOutput;
int16_t yawOutput;

// Yaw Pitch Roll Errors
int16_t rollError;
int16_t pitchError;
int16_t yawError;

Success_t InitializeAttitudeAdjust(void) {

	#ifdef USE_RATES
		PIDCreateInstance(&pidRollRate, 0, PID_ROLL_RATE_KP, PID_ROLL_RATE_KI, PID_ROLL_RATE_KD, IMU_UPDATE_DT);
		PIDCreateInstance(&pidPitchRate, 0, PID_PITCH_RATE_KP, PID_PITCH_RATE_KI, PID_PITCH_RATE_KD, IMU_UPDATE_DT);
		PIDCreateInstance(&pidYawRate, 0, PID_YAW_RATE_KP, PID_YAW_RATE_KI, PID_YAW_RATE_KD, IMU_UPDATE_DT);
		PIDSetIntegralLimit(&pidRollRate, PID_ROLL_RATE_INTEGRATION_LIMIT);
		PIDSetIntegralLimit(&pidPitchRate, PID_PITCH_RATE_INTEGRATION_LIMIT);
		PIDSetIntegralLimit(&pidYawRate, PID_YAW_RATE_INTEGRATION_LIMIT);
	#endif

	PIDCreateInstance(&pidRoll, 0, PID_ROLL_KP, PID_ROLL_KI, PID_ROLL_KD, IMU_UPDATE_DT);
	PIDCreateInstance(&pidPitch, 0, PID_PITCH_KP, PID_PITCH_KI, PID_PITCH_KD, IMU_UPDATE_DT);
	PIDCreateInstance(&pidYaw, 0, PID_YAW_KP, PID_YAW_KI, PID_YAW_KD, IMU_UPDATE_DT);
	PIDSetIntegralLimit(&pidRoll, PID_ROLL_INTEGRATION_LIMIT);
	PIDSetIntegralLimit(&pidPitch, PID_PITCH_INTEGRATION_LIMIT);
	PIDSetIntegralLimit(&pidYaw, PID_YAW_INTEGRATION_LIMIT);

	return SUCCESS;
}

void AttituteAdjustSetDesired(double rollDesired, double pitchDesired, double yawDesired){
	PIDSetDesired(&pidYaw, YawDesired);
	PIDSetDesired(&pidPitch, pitchDesired);
	PIDSetDesired(&pidRoll, RollDesired);
}

void AttituteAdjustUpdatePID(double rollActual, double pitchActual, double yawActual){
	yawError = saturateSignedInt16(pidUpdate(&pidYaw, pidYaw.desired, UPDATE_ERROR));
	pitchError = saturateSignedInt16(pidUpdate(&pidPitch, pidPitch.desired, UPDATE_ERROR));
	rollError = saturateSignedInt16(pidUpdate(&pidRoll, pidRoll.desired, UPDATE_ERROR));
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

int16_t* AttitudeAdjustGetActuation(void){
	/* Motor Array: [mot1, mot2, mot3, mot4]
		mot1				mot2
		cw				ccw
		    	  	^
			      drone
		mot3				mot4
		ccw				cw
	*/
	uint16_t motor_changes[4];
	motor_changes[0] = -yawError -pitchError -rollError;
	motor_changes[1] =  yawError -pitchError +rollError;
	motor_changes[2] =  yawError +pitchError -rollError;
	motor_changes[3] = -yawError +pitchError -rollError;
	return &motor_changes;
}

// Manual Test Function -- AVOID USE //

// These adjust functions are not necessary, they can be done by setting the
// appropriate euler angles needed. 
Success_t Adjust_Yaw(double angle) {
	if (_IsValidYawAngle(angle) == INVALID_PARAMETER) {
		return INVALID_PARAMETER;
	}

	while ((_sensor->attitude->FetchYaw() == SUCCESS) &&
			((_sensor->attitude.yaw < (angle - YAW_NEGATIVE_DEADBAND_OFFSET)) ||
			(_sensor->attitude.yaw > (angle + YAW_POSITIVE_DEADBAND_OFFSET))
		)) {
		if (angle > _sensor->attitude.yaw) {
			_Yaw(YAW_CW);
		}
		else if (angle > 0) {
			_Yaw(YAW_CCW);
		} else {
			break;
		}

		THREAD_SLEEP(5);
	}
}

Success_t _IsValidYawAngle(double angle) {
	if ((angle > MINIMUM_ALLOWABLE_YAW_ANGLE) &&
		(angle < MAXIMUM_ALLOWABLE_YAW_ANGLE)) {
		return VALID_PARAMETER;
	}
	return INVALID_PARAMETER;
}

Success_t _Yaw(AttitudeDirection_t direction) {
	switch (direction) {
		case YAW_CW:
			if ((motor_get_speed(MOTOR_ONE) < MAX_ROTOR_1_THRUST) &&
				(motor_get_speed(MOTOR_FOUR) < MAX_ROTOR_4_THRUST) &&
				(motor_get_speed(MOTOR_TWO) > MIN_ROTOR_2_THRUST) &&
				(motor_get_speed(MOTOR_THREE) > MIN_ROTOR_3_THRUST) {

				motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) + YAW_CW_MARGIN);
				motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_FOUR) + YAW_CW_MARGIN);

				motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TWO) - YAW_CW_MARGIN);
				motor_set(MOTOR_THREE, motor_get_speed(MOTOR_THREE) - YAW_CW_MARGIN);

			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;

		case YAW_CCW:
			if ((motor_get_speed(MOTOR_TWO) < MAX_ROTOR_2_THRUST) &&
				(motor_get_speed(MOTOR_THREE) < MAX_ROTOR_3_THRUST) &&
				(motor_get_speed(MOTOR_ONE) > MIN_ROTOR_1_THRUST) &&
				(motor_get_speed(MOTOR_FOUR) > MIN_ROTOR_4_THRUST) {

				motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TWO) + YAW_CCW_MARGIN);
				motor_set(MOTOR_THREE, motor_get_speed(MOTOR_THREE) + YAW_CCW_MARGIN);

				motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) - YAW_CCW_MARGIN);
				motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_FOUR) - YAW_CCW_MARGIN);

			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;

		default:
			return INVALID_PARAMETER;
			break;
	}
	return SUCCESS;
}

Success_t Adjust_Pitch(double angle) {
  if (_IsValidPitchAngle(angle) == INVALID_PARAMETER) {
		return INVALID_PARAMETER;
	}

	while ((_sensor->attitude->FetchPitch() == SUCCESS) &&
		((_sensor->attitude.pitch < (angle - PITCH_NEGATIVE_DEADBAND_OFFSET)) ||
		(_sensor->attitude.pitch > (angle + PITCH_POSITIVE_DEADBAND_OFFSET))
		)) {
		if (angle > _sensor->attitude.pitch) {
			if (_Pitch(PITCH_FORWARD) == FAILURE) {
				return FAILURE;
			}
		}
		else if (angle < _sensor->attitude.pitch) {
			if (_Pitch(PITCH_BACKWARD) == FAILURE) {
				return FAILURE;
			}
		} else {
			break;
		}

		THREAD_SLEEP(5);
	}
	return SUCCESS;
}

Success_t _IsValidPitchAngle(double angle) {
	if (((angle > MINIMUM_ALLOWABLE_PITCH_ANGLE) &&
		(angle < MAXIMUM_ALLOWABLE_PITCH_ANGLE)) ||
		((angle < MINIMUM_ALLOWABLE_PITCH_ANGLE) &&
		(angle > -1 * MAXIMUM_ALLOWABLE_PITCH_ANGLE))) {
		return VALID_PARAMETER;
	}
	return INVALID_PARAMETER;
}

Success_t _Pitch(AttitudeDirection_t direction) {
	switch (direction) {
		case PITCH_FORWARD:
			if ((motor_get_speed(MOTOR_THREE) < MAX_ROTOR_3_THRUST) &&
				(motor_get_speed(MOTOR_FOUR) < MAX_ROTOR_4_THRUST) &&
				(motor_get_speed(MOTOR_ONE) > MIN_ROTOR_1_THRUST) &&
				(motor_get_speed(MOTOR_TWO) > MIN_ROTOR_2_THRUST) {

				motor_set(MOTOR_THREE, motor_get_speed(MOTOR_THREE) + PITCH_POSITIVE_MARGIN);
				motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_FOUR) + PITCH_POSITIVE_MARGIN);

				motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) - PITCH_POSITIVE_MARGIN);
				motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TWO) - PITCH_POSITIVE_MARGIN);

			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;

		case PITCH_BACKWARD:
			if ((motor_get_speed(MOTOR_ONE) < MAX_ROTOR_1_THRUST) &&
				(motor_get_speed(MOTOR_TWO) < MAX_ROTOR_2_THRUST) &&
				(motor_get_speed(MOTOR_THREE) > MIN_ROTOR_3_THRUST) &&
				(motor_get_speed(MOTOR_FOUR) > MIN_ROTOR_4_THRUST) {

				motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) + PITCH_POSITIVE_MARGIN);
				motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TWO) + PITCH_POSITIVE_MARGIN);

				motor_set(MOTOR_THREE, motor_get_speed(MOTOR_THREE) - PITCH_POSITIVE_MARGIN);
				motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_FOUR) - PITCH_POSITIVE_MARGIN);

			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;
		default:
			return INVALID_PARAMETER;
			break;
	}
	return SUCCESS;
}

Success_t Adjust_Roll(double angle) {
	if (_IsValidRollAngle(angle) == INVALID_PARAMETER) {
		return INVALID_PARAMETER;
	}

	while ((_sensor->attitude->FetchRoll() == SUCCESS) &&
			((_sensor->attitude.roll < (angle - ROLL_NEGATIVE_DEADBAND_OFFSET)) ||
			(_sensor->attitude.roll > (angle + ROLL_POSITIVE_DEADBAND_OFFSET))
		)) {
		if (angle > _sensor->attitude.roll) {
			if (_Roll(ROLL_RIGHT) == FAILURE) {
				return FAILURE;
			}
		}
		else if (angle < _sensor->attitude.roll) {
			if (_Roll(ROLL_LEFT) == FAILURE) {
				return FAILURE;
			}
		} else {
			break;
		}

		THREAD_SLEEP(5);
	}
	return SUCCESS;
}

Success_t _IsValidRollAngle(double angle) {
	if (((angle > MINIMUM_ALLOWABLE_ROLL_ANGLE) &&
		(angle < MAXIMUM_ALLOWABLE_ROLL_ANGLE))
		||
		((angle < MINIMUM_ALLOWABLE_ROLL_ANGLE) &&
		(angle > -1 * MAXIMUM_ALLOWABLE_ROLL_ANGLE))) {
		return VALID_PARAMETER;
	}
	return INVALID_PARAMETER;
}

Success_t _Roll(AttitudeDirection_t direction) {
	switch (direction) {
		case ROLL_LEFT:
			if ((motor_get_speed(MOTOR_TWO) < MAX_ROTOR_2_THRUST) &&
				(motor_get_speed(MOTOR_FOUR) < MAX_ROTOR_4_THRUST) &&
				(motor_get_speed(MOTOR_ONE) > MIN_ROTOR_1_THRUST) &&
				(motor_get_speed(MOTOR_THREE) > MIN_ROTOR_3_THRUST) {

				motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TWO) + ROLL_POSITIVE_MARGIN);
				motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_FOUR) + ROLL_POSITIVE_MARGIN);

				motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) - ROLL_POSITIVE_MARGIN);
				motor_set(MOTOR_THREE, motor_get_speed(MOTOR_THREE) - ROLL_POSITIVE_MARGIN);

			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;

		case ROLL_RIGHT:
			if ((motor_get_speed(MOTOR_ONE) < MAX_ROTOR_1_THRUST) &&
				(motor_get_speed(MOTOR_THREE) < MAX_ROTOR_3_THRUST) &&
				(motor_get_speed(MOTOR_TWO) > MIN_ROTOR_2_THRUST) &&
				(motor_get_speed(MOTOR_FOUR) > MIN_ROTOR_4_THRUST) {

				motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) + ROLL_POSITIVE_MARGIN);
				motor_set(MOTOR_THREE, motor_get_speed(MOTOR_THREE) + ROLL_POSITIVE_MARGIN);

				motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TWO) - ROLL_POSITIVE_MARGIN);
				motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_FOUR) - ROLL_POSITIVE_MARGIN);

			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;
		default:
			return INVALID_PARAMETER;
			break;
	}
	return SUCCESS;
}

Success_t Adjust_Altitude(double altitude) {
	if (_IsValidAltitude(altitude) == INVALID_PARAMETER) {
		return INVALID_PARAMETER;
	}

	while ((_sensor->GPS->FetchAltitude() == SUCCESS) &&
		(   (_sensor->GPS.altitude < (altitude - ALTITUDE_NEGATIVE_DEADBAND_OFFSET)) ||
			(_sensor->GPS.altitude > (altitude + ALTITUDE_POSITIVE_DEADBAND_OFFSET))
		)) {
		if (altitude > _sensor->GPS.altitude) {
			_Altitude(RISE_ALTITUDE); // rise copter
		}
		else if (altitude < _sensor->GPS.altitude) {
			_Altitude(LOWER_ALTITUDE); // lower copter
		} else {
			break;
		}

		THREAD_SLEEP(5);
	}
}

Success_t _IsValidAltitude(double altitude) {
	if ((altitude > MINIMUM_ALLOWABLE_ALTITUDE) &&
		(altitude < MAXIMUM_ALLOWABLE_ALTITUDE) {
		return SUCCESS;
	} else {
		return INVALID_PARAMETER;
	}
}

Success_t _Altitude(AttitudeDirection_t altitude_parameter) {
	Success_t ret;
	switch (altitude_parameter) {
		case LOWER_ALTITUDE:
			if ((motor_get_speed(MOTOR_ONE) > MIN_ROTOR_1_THRUST) &&
				(motor_get_speed(MOTOR_TWO) > MIN_ROTOR_2_THRUST) &&
				(motor_get_speed(MOTOR_THREE) > MIN_ROTOR_3_THRUST) &&
				(motor_get_speed(MOTOR_FOUR) > MIN_ROTOR_4_THRUST) {

				motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) - ALTITUDE_LOWER_MARGIN);
				motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TWO) - ALTITUDE_LOWER_MARGIN);
				motor_set(MOTOR_THREE, motor_get_speed(MOTOR_THREE) - ALTITUDE_LOWER_MARGIN);
				motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_FOUR) - ALTITUDE_LOWER_MARGIN);

			} else {
				return MIN_MOTOR_THRUST_ACHIEVED;
			}
			break;

		case RISE_ALTITUDE:
			if ((motor_get_speed(MOTOR_ONE) < MAX_ROTOR_1_THRUST) &&
				(motor_get_speed(MOTOR_TWO) < MAX_ROTOR_2_THRUST) &&
				(motor_get_speed(MOTOR_THREE) < MAX_ROTOR_3_THRUST) &&
				(motor_get_speed(MOTOR_FOUR) < MAX_ROTOR_4_THRUST) {

				motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) + ALTITUDE_RISE_MARGIN);
				motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TWO) + ALTITUDE_RISE_MARGIN);
				motor_set(MOTOR_THREE, motor_get_speed(MOTOR_THREE) + ALTITUDE_RISE_MARGIN);
				motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_FOUR) + ALTITUDE_RISE_MARGIN);

			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;
	}

	return SUCCESS;
}
