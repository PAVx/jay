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

// Feedback Control Functions

uint8_t InitializeAttitudeAdjust(void);
void AttituteAdjustSetDesired(double yawDesired, double pitchDesired, double rollDesired);
void AttituteAdjustUpdatePID(double yawActual, double pitchActual, double rollActual);
void AttitudeAdjustGetError(int16_t motor_changes[NUM_MOTORS]);
void AttitudeAdjustSetActuation(int16_t motor_changes[NUM_MOTORS]);
#endif
