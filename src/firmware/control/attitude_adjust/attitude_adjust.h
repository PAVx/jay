/*
    PAVx -- Pod-Based Autonomous Vehicles
    Library Created By: Sargis S Yonan
    March 2017
*/

// attitude_adjust.h
#ifndef _ATTITUDE_ADJUST_H_
#define _ATTITUDE_ADJUST_H_

#include <stdint.h>
#include "motors.h"
#include "pid.h"

PID_t pidRoll;
PID_t pidPitch;
PID_t pidYaw;

PID_t pidRollRate;
PID_t pidPitchRate;
PID_t pidYawRate;

// Feedback Control Functions

uint8_t InitializeAttitudeAdjust(void);
void AttituteAdjustSetDesired(double yawDesired, double pitchDesired, double rollDesired);
void AttituteAdjustUpdatePID(double pitchRate, double rollRate,
	double yawActual, double pitchActual, double rollActual);
void AttitudeSetThrottle(int32_t throttle);
void AttitudeAdjustGetError(int motor_changes[NUM_MOTORS]);
void AttitudeAdjustSetActuation(int motor_changes[NUM_MOTORS]);
#endif
