/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#ifndef _GYRO_H_
#define _GYRO_H_

#include "itg3200.h"

void InitializeGyro(void);

void Gyro_Update(void);

double Gyro_GetX(void);

double Gyro_GetY(void);

double Gyro_GetZ(void);

double Gyro_GetTemp(void);

#endif
