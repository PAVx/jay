/*
    PAVx -- Pod-Based Autonomous Vehicles
    Library Created By: Sargis S Yonan
    March 2017
*/

// pid.c

// Some PID concepts borrowed the bitcraze/crazieflie-firmware repository
// https://www.bitcraze.io

#include "pid.h"

static uint8_t _pidFlag = 0;

void PIDInit(PID_t* pid, const double desired, const double kp,
             const double ki, const double kd, const double dt) {
    pid->error = 0;
    pid->lastError = 0;
    pid->integral = 0;
    pid->derivative = 0;
    pid->desired = desired;
    pid->pOutput = 0.0;
    pid->iOutput = 0.0;
    pid->dOutput = 0.0;
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->iCeiling = DEFAULT_PID_INTEGRATION_LIMIT;
    pid->iFloor = -DEFAULT_PID_INTEGRATION_LIMIT;
    pid->dt = dt;
}

double PIDUpdate(PID_t* pid, const double measured, const UpdateFlag_t updateError) {
    double output;

    if (updateError == UPDATE_ERROR) {
        pid->error = pid->desired - measured;
    }

    pid->integral += pid->error * pid->dt;
    if (pid->integral > pid->iCeiling) {
        pid->integral = pid->iCeiling;
    }
    else if (pid->integral < pid->iFloor) {
        pid->integral = pid->iFloor;
    }

    pid->derivative = (pid->error - pid->lastError) / pid->dt;

    pid->pOutput = pid->kp * pid->error;
    pid->iOutput = pid->ki * pid->integral;
    pid->dOutput = pid->kd * pid->derivative;

    output = pid->pOutput + pid->iOutput + pid->dOutput;

    pid->lastError = pid->error;

    return output;
}

void PIDSetIntegralLimit(PID_t* pid, const double limit) {
    pid->iCeiling = limit;
}


void PIDSetIntegralLimitLow(PID_t* pid, const double limitLow) {
    pid->iFloor = limitLow;
}

void PIDReset(PID_t* pid) {
    pid->error = 0;
    pid->lastError = 0;
    pid->integral = 0;
    pid->derivative = 0;
}

void PIDSetError(PID_t* pid, const double error) {
    pid->error = error;
}

void PIDSetDesired(PID_t* pid, const double desired) {
    pid->desired = desired;
}

double PIDGetDesired(PID_t* pid) {
    return pid->desired;
}

Active_t PIDIsActive(PID_t* pid) {
    if((pid->kp < PID_INACTIVE_LOWER_THRESHOLD) &&
        (pid->ki < PID_INACTIVE_LOWER_THRESHOLD) &&
        (pid->kd < PID_INACTIVE_LOWER_THRESHOLD)) {
        return PID_INACTIVE;
    }

    return PID_ACTIVE;
}

void PIDSetKp(PID_t* pid, const double kp) {
    pid->kp = kp;
}

void PIDSetKi(PID_t* pid, const double ki) {
    pid->ki = ki;
}

void PIDSetKd(PID_t* pid, const double kd) {
    pid->kd = kd;
}
void PIDSetDt(PID_t* pid, const double dt) {
    pid->dt = dt;
}

void PIDSetFlag(){
        _pidFlag = 1;
}

void PIDResetFlag(){
        _pidFlag = 0;
}

uint8_t PIDGetFlag(){
        return _pidFlag;
}
