/*
    PAVx -- Pod-Based Autonomous Vehicles
    Library Created By: Sargis S Yonan
    March 2017
*/

// attitude_adjust.h
#ifndef _ATTITUDE_ADJUST_H_
#define _ATTITUDE_ADJUST_H_

#include "pid.h"
#include "motors.h"

#define MAX_ROTOR_1_THRUST (255)
#define MAX_ROTOR_2_THRUST (255)
#define MAX_ROTOR_3_THRUST (255)
#define MAX_ROTOR_4_THRUST (255)

#define MIN_ROTOR_1_THRUST (100)
#define MIN_ROTOR_2_THRUST (100)
#define MIN_ROTOR_3_THRUST (100)
#define MIN_ROTOR_4_THRUST (100)

// Feedback Controlled Functions

Success_t InitializeAttitudeAdjust(void);
void AttituteAdjustSetDesired(double yawDesired, double pitchDesired, double rollDesired);
void AttituteAdjustUpdatePID(double yawActual, double pitchActual, double rollActual);
void AttitudeAdjustGetActuation(int16_t* motor_changes);

#endif
