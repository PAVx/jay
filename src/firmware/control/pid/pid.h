/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

// pid.h

// Some PID concepts borrowed the bitcraze/crazieflie-firmware repository
// https://www.bitcraze.io



#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>

#define PID_ROLL_KP  5.5
#define PID_ROLL_KI  0.001
#define PID_ROLL_KD  1.8
#define PID_ROLL_INTEGRATION_LIMIT    30.0

#define PID_PITCH_KP  4.0
#define PID_PITCH_KI  0.0
#define PID_PITCH_KD  1.0
#define PID_PITCH_INTEGRATION_LIMIT   30.0

#define PID_YAW_KP  0.0
#define PID_YAW_KI  0.0
#define PID_YAW_KD  0.0
#define PID_YAW_INTEGRATION_LIMIT     360.0

#define PID_ROLL_RATE_KP  0.1
#define PID_ROLL_RATE_KI  0.0
#define PID_ROLL_RATE_KD  0.0
#define PID_ROLL_RATE_INTEGRATION_LIMIT    30.0

#define PID_PITCH_RATE_KP  0.1
#define PID_PITCH_RATE_KI  0.0
#define PID_PITCH_RATE_KD  0.0
#define PID_PITCH_RATE_INTEGRATION_LIMIT   30.0

#define PID_YAW_RATE_KP  0.0
#define PID_YAW_RATE_KI  0.0
#define PID_YAW_RATE_KD  0.0
#define PID_YAW_RATE_INTEGRATION_LIMIT     30.0

#define PID_INACTIVE_LOWER_THRESHOLD (0.000001)
#define DEFAULT_PID_INTEGRATION_LIMIT  30.0

typedef enum Success_t{
	NO_BUENO = 0,
	SUCCESS = 1
}Success_t;

enum Active {
	PID_ACTIVE,
	PID_INACTIVE
};
typedef enum Active Active_t;

enum UpdateFlag {
	UPDATE_ERROR,
	NO_UPDATE
};
typedef enum UpdateFlag UpdateFlag_t;

struct PID {
	double desired;
	double error;
	double lastError;
	double integral;
	double derivative;
	double kp;
	double ki;
	double kd;
	double pOutput;
	double iOutput;
	double dOutput;
	double iCeiling;
	double iFloor;
	double dt;
};
typedef struct PID PID_t;

void PIDInit(PID_t* pid, const double desired, const double kp,
             const double ki, const double kd, const double dt);

void PIDSetIntegralLimit(PID_t* pid, const double limit);

// Reset the PID error values
void PIDReset(PID_t* pid);

// Update the PID parameters.
double PIDUpdate(PID_t* pid, const double measured, const UpdateFlag_t updateError);

// Set a new set point for the PID to track.
void PIDSetDesired(PID_t* pid, const double desired);

// Set a new set point for the PID to track.
double PIDGetDesired(PID_t* pid);

// Find out if PID is active
Active_t PIDIsActive(PID_t* pid);

// Set a new error. Use if a special error calculation is needed.
void PIDSetError(PID_t* pid, const double error);

// Set a new proportional gain for the PID.
void PIDSetKp(PID_t* pid, const double kp);

// Set a new integral gain for the PID.
void PIDSetKi(PID_t* pid, const double ki);

// Set a new derivative gain for the PID.
void PIDSetKd(PID_t* pid, const double kd);

// Set a new dt gain for the PID. Defaults to IMU_UPDATE_DT upon construction
void PIDSetDt(PID_t* pid, const double dt);

void PIDSetFlag();

void PIDResetFlag();

uint8_t PIDGetFlag();

#endif
