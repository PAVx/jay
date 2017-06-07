/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga
	March 2017
*/

#ifndef _IMU_H_
#define _IMU_H_

void IMU_Init(void);
void IMU_Update(void);
void IMU_Calibrate(void);

double Accel_GetX(void);
double Accel_GetY(void);
double Accel_GetZ(void);

double Gyro_GetX(void);
double Gyro_GetY(void);
double Gyro_GetZ(void);

double Mag_GetX(void);
double Mag_GetY(void);
double Mag_GetZ(void);

#endif
