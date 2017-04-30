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

void imu2euler(double* ypr, double accX, double accY, double accZ, double magX, double magY);
void euler2quat(double* q, double yaw, double pitch, double roll);
void quat2euler(double* ypr, double* q);

#endif	// _ANGLES_H_
