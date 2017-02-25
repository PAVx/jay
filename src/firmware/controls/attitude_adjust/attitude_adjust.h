// attitude_adjust.h
#ifndef _ATTITUDE_ADJUST_H_
#define _ATTITUDE_ADJUST_H_

#include "protocols/copter/copter.h"
#include "protocols/sensor/sensor.h"
#include "protocols/attitude/attitude.h"
#include "control/pid/pid.h"

enum AttitudeDirection {
    YAW_CW,
    YAW_CCW,

    ROLL_LEFT,
    ROLL_RIGHT,

    PITCH_FORWARD,
    PITCH_BACKWARD,

    RISE_ALTITUDE,
    LOWER_ALTITUDE

};
typedef enum AttitudeDirection AttitudeDirection_t;

#define MAX_ROTOR_1_THRUST (100)
#define MAX_ROTOR_2_THRUST (100)
#define MAX_ROTOR_3_THRUST (100)
#define MAX_ROTOR_4_THRUST (100)

#define MIN_ROTOR_1_THRUST (0)
#define MIN_ROTOR_2_THRUST (0)
#define MIN_ROTOR_3_THRUST (0)
#define MIN_ROTOR_4_THRUST (0)

#define YAW_NEGATIVE_DEADBAND_OFFSET (2)
#define YAW_POSITIVE_DEADBAND_OFFSET (2)
#define YAW_CW_MARGIN (MOTOR_THRUST_INCREASE_MARGIN)
#define YAW_CCW_MARGIN (MOTOR_THRUST_INCREASE_MARGIN)
#define MINIMUM_ALLOWABLE_YAW_ANGLE (0)
#define MAXIMUM_ALLOWABLE_YAW_ANGLE (360)

#define PITCH_NEGATIVE_DEADBAND_OFFSET (2)
#define PITCH_POSITIVE_DEADBAND_OFFSET (2)
#define PITCH_POSITIVE_MARGIN (MOTOR_THRUST_INCREASE_MARGIN)
#define MINIMUM_ALLOWABLE_PITCH_ANGLE (0)
#define MAXIMUM_ALLOWABLE_PITCH_ANGLE (180)

#define ROLL_NEGATIVE_DEADBAND_OFFSET (2)
#define ROLL_POSITIVE_DEADBAND_OFFSET (2)
#define ROLL_POSITIVE_MARGIN (MOTOR_THRUST_INCREASE_MARGIN)
#define MINIMUM_ALLOWABLE_ROLL_ANGLE (0)
#define MAXIMUM_ALLOWABLE_ROLL_ANGLE (180)

#define ALTITUDE_NEGATIVE_DEADBAND_OFFSET (2)
#define ALTITUDE_POSITIVE_DEADBAND_OFFSET (2)
#define ALTITUDE_RISE_MARGIN (MOTOR_THRUST_INCREASE_MARGIN)
#define ALTITUDE_LOWER_MARGIN (MOTOR_THRUST_DECREASE_MARGIN)
#define MINIMUM_ALLOWABLE_ALTITUDE (0)
#define MAXIMUM_ALLOWABLE_ALTITUDE (300)

// Feedback Controlled Functions

static inline int16_t saturateSignedInt16(double in) {
    if (in > INT16_MAX) {
        return INT16_MAX;
    } else if (in < -INT16_MAX) {
        return -INT16_MAX;
    } else {
        return (int16_t)in;
    }
}

Success_t InitializeAttitudeAdjust(void);

void AttitudeAdjustCorrectRatePID(
    double rollRateActual, double pitchRateActual, double yawRateActual,
    double rollRateDesired, double pitchRateDesired, double yawRateDesired);

void AttitudeAdjustRunPID(
    double eulerRollActual, double eulerPitchActual, double eulerYawActual,
    double eulerRollDesired, double eulerPitchDesired, double eulerYawDesired,
    double* rollRateDesired, double* pitchRateDesired, double* yawRateDesired);

void AttitudeAdjustGetActuatorOutput(int16_t* roll, int16_t* pitch, int16_t* yaw);

///////////////////////////////////////////////
/*********************************************/
///////////////////////////////////////////////

// Manual Function -- AVOID USE //
Success_t Adjust_Yaw(double angle);
Success_t Adjust_Pitch(double angle);
Success_t Adjust_Roll(double angle);
Success_t Adjust_Altitude(double angle);

Success_t _Yaw(AttitudeDirection_t direction);
Success_t _Pitch(AttitudeDirection_t direction);
Success_t _Roll(AttitudeDirection_t direction);
Success_t _Altitude(AttitudeDirection_t direction);

Success_t _IsValidYawAngle(double angle);
Success_t _IsValidPitchAngle(double angle);
Success_t _IsValidRollAngle(double angle);
Success_t _IsValidAltitude(double altitude);

#endif
