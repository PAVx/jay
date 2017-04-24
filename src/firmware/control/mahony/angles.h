/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga
	March 2017
*/

// angle.h

// Reference: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

#ifndef _ANGLES_H_
#define _ANGLES_H_

#define YAW_ANGLE (0)
#define PITCH_ANGLE (1)
#define ROLL_ANGLE (2)

double* imu2euler(double accX, double accY, double accZ, double magX, double magY){
double* euler2quat(double yaw, double pitch, double roll);
double* quat2euler(double* q);

#endif	// _ANGLES_H_
