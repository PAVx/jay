/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#ifndef _ACCEL_H_
#define _ACCEL_H_

void InitializeAccel(void);

void Accel_Update(void);

double Accel_GetX(void);

double Accel_GetY(void);

double Accel_GetZ(void);

#endif
