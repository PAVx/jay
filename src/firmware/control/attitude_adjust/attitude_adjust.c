// attitude_adjust.c

#include "control/attitude_adjust/attitude_adjust.h"

Sensor_t *_sensor;
Copter_t *_copter;

PID_t pidRollRate;
PID_t pidPitchRate;
PID_t pidYawRate;

PID_t pidRoll;
PID_t pidPitch;
PID_t pidYaw;

int16_t rollOutput;
int16_t pitchOutput;
int16_t yawOutput;

Success_t InitializeAttitudeAdjust(void) {
	if ((_sensor ) && (_copter)) {
		return ALREADY_INITIALIZED;
	}

	if (SensorInitialize == ALREADY_INITIALIZED) {
		_sensor = GetSensorProtocol();
	}

	if (CopterInitialize == ALREADY_INITIALIZED) {
		_copter = GetCopterProtocol();
	}

	PIDCreateInstance(&pidRollRate, 0, PID_ROLL_RATE_KP, PID_ROLL_RATE_KI, PID_ROLL_RATE_KD, IMU_UPDATE_DT);
	PIDCreateInstance(&pidPitchRate, 0, PID_PITCH_RATE_KP, PID_PITCH_RATE_KI, PID_PITCH_RATE_KD, IMU_UPDATE_DT);
	PIDCreateInstance(&pidYawRate, 0, PID_YAW_RATE_KP, PID_YAW_RATE_KI, PID_YAW_RATE_KD, IMU_UPDATE_DT);
	PIDSetIntegralLimit(&pidRollRate, PID_ROLL_RATE_INTEGRATION_LIMIT);
	PIDSetIntegralLimit(&pidPitchRate, PID_PITCH_RATE_INTEGRATION_LIMIT);
	PIDSetIntegralLimit(&pidYawRate, PID_YAW_RATE_INTEGRATION_LIMIT);

	PIDCreateInstance(&pidRoll, 0, PID_ROLL_KP, PID_ROLL_KI, PID_ROLL_KD, IMU_UPDATE_DT);
	PIDCreateInstance(&pidPitch, 0, PID_PITCH_KP, PID_PITCH_KI, PID_PITCH_KD, IMU_UPDATE_DT);
	PIDCreateInstance(&pidYaw, 0, PID_YAW_KP, PID_YAW_KI, PID_YAW_KD, IMU_UPDATE_DT);
	PIDSetIntegralLimit(&pidRoll, PID_ROLL_INTEGRATION_LIMIT);
	PIDSetIntegralLimit(&pidPitch, PID_PITCH_INTEGRATION_LIMIT);
	PIDSetIntegralLimit(&pidYaw, PID_YAW_INTEGRATION_LIMIT);

	return SUCCESS;
}

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

void AttitudeAdjustGetActuatorOutput(int16_t* roll, int16_t* pitch, int16_t* yaw)	{
	*roll = rollOutput;
	*pitch = pitchOutput;
	*yaw = yawOutput;
}


// Manual Test Function -- AVOID USE //

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
			if ((_copter->RotorOne->current_thrust < MAX_ROTOR_1_THRUST) &&
				(_copter->RotorThree->current_thrust < MAX_ROTOR_3_THRUST)) {

				_copter->RotorOne->Control((_copter->RotorOne->gpio),
					_copter->RotorOne->current_thrust + YAW_CW_MARGIN);

				_copter->RotorThree->Control((_copter->RotorThree->gpio),
					_copter->RotorThree->current_thrust + YAW_CW_MARGIN);
			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;

		case YAW_CCW:
			if ((_copter->RotorTwo->current_thrust < MAX_ROTOR_2_THRUST) &&
				(_copter->RotorFour->current_thrust < MAX_ROTOR_4_THRUST)) {

				_copter->RotorTwo->Control((_copter->RotorTwo->gpio),
					_copter->RotorTwo->current_thrust + YAW_CCW_MARGIN);

				_copter->RotorFour->Control((_copter->RotorFour->gpio),
					_copter->RotorFour->current_thrust + YAW_CCW_MARGIN);
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
		(angle < MAXIMUM_ALLOWABLE_PITCH_ANGLE))
		||
		((angle < MINIMUM_ALLOWABLE_PITCH_ANGLE) &&
		(angle > -1 * MAXIMUM_ALLOWABLE_PITCH_ANGLE))) {
		return VALID_PARAMETER;
	}
	return INVALID_PARAMETER;
}

Success_t _Pitch(AttitudeDirection_t direction) {
	switch (direction) {
		case PITCH_FORWARD:
			if ((_copter->RotorOne->current_thrust < MAX_ROTOR_1_THRUST) &&
				(_copter->RotorTwo->current_thrust < MAX_ROTOR_2_THRUST) {

				_copter->RotorOne->Control((_copter->RotorOne->gpio),
					_copter->RotorOne->current_thrust + PITCH_POSITIVE_MARGIN);

				_copter->RotorTwo->Control((_copter->RotorTwo->gpio),
					_copter->RotorTwo->current_thrust + PITCH_POSITIVE_MARGIN);
			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;

		case PITCH_BACKWARD:
			if ((RotorThree->current_thrust < MAX_ROTOR_3_THRUST) &&
				(RotorFour->current_thrust < MAX_ROTOR_4_THRUST)) {

				_copter->RotorThree->Control((_copter->RotorThree->gpio),
					_copter->RotorThree->current_thrust + PITCH_POSITIVE_MARGIN);

				_copter->RotorFour->Control((_copter->RotorFour->gpio),
					_copter->RotorFour->current_thrust + PITCH_POSITIVE_MARGIN);
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
			if ((_copter->RotorTwo->current_thrust < MAX_ROTOR_2_THRUST) &&
				(_copter->RotorFour->current_thrust < MAX_ROTOR_4_THRUST) {

				_copter->RotorTwo->Control((_copter->RotorTwo->gpio),
					_copter->RotorTwo->current_thrust + ROLL_POSITIVE_MARGIN);

				_copter->RotorFour->Control((_copter->RotorFour->gpio),
					_copter->RotorFour->current_thrust + ROLL_POSITIVE_MARGIN);
			} else {
				return MAX_MOTOR_THRUST_ACHIEVED;
			}
			break;

		case ROLL_RIGHT:
			if ((RotorOne->current_thrust < MAX_ROTOR_1_THRUST) &&
				(RotorThree->current_thrust < MAX_ROTOR_3_THRUST)) {

				_copter->RotorOne->Control((_copter->RotorOne->gpio),
					_copter->RotorOne->current_thrust + ROLL_POSITIVE_MARGIN);

				_copter->RotorThree->Control((_copter->RotorThree->gpio),
					_copter->RotorThree->current_thrust + ROLL_POSITIVE_MARGIN);
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
			if ((_copter->RotorOne->current_thrust > MINIMUM_ROTOR_1_THRUST) &&
				(_copter->RotorTwo->current_thrust > MINIMUM_ROTOR_2_THRUST) &&
				(_copter->RotorThree->current_thrust > MINIMUM_ROTOR_3_THRUST) &&
				(_copter->RotorFour->current_thrust > MINIMUM_ROTOR_4_THRUST)) {

				_copter->RotorOne->Control((_copter->RotorOne->gpio),
					_copter->RotorOne->current_thrust - ALTITUDE_LOWER_MARGIN);

				_copter->RotorTwo->Control((_copter->RotorTwo->gpio),
					_copter->RotorTwo->current_thrust - ALTITUDE_LOWER_MARGIN);

				_copter->RotorThree->Control((_copter->RotorThree->gpio),
					_copter->RotorThree->current_thrust - ALTITUDE_LOWER_MARGIN);

				_copter->RotorFour->Control((_copter->RotorFour->gpio),
					_copter->RotorFour->current_thrust - ALTITUDE_LOWER_MARGIN);
			} else {
				return MINIMUM_MOTOR_THRUST_ACHIEVED;
			}
			break;

		case RISE_ALTITUDE:
			if ((_copter->RotorOne->current_thrust < MAXIMUM_ROTOR_1_THRUST) &&
				(_copter->RotorTwo->current_thrust < MAXIMUM_ROTOR_2_THRUST) &&
				(_copter->RotorThree->current_thrust < MAXIMUM_ROTOR_3_THRUST) &&
				(_copter->RotorFour->current_thrust < MAXIMUM_ROTOR_4_THRUST)) {

				_copter->RotorOne->Control((_copter->RotorOne->gpio),
					_copter->RotorOne->current_thrust + ALTITUDE_RISE_MARGIN);

				_copter->RotorTwo->Control((_copter->RotorTwo->gpio),
					_copter->RotorTwo->current_thrust + ALTITUDE_RISE_MARGIN);

				_copter->RotorThree->Control((_copter->RotorThree->gpio),
					_copter->RotorThree->current_thrust + ALTITUDE_RISE_MARGIN);

				_copter->RotorFour->Control((_copter->RotorFour->gpio),
					_copter->RotorFour->current_thrust + ALTITUDE_RISE_MARGIN);
				} else {
					return MAX_MOTOR_THRUST_ACHIEVED;
				}
			break;
	}

	return SUCCESS;
}
