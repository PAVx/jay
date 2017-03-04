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

float Gyro_GetX(void);

float Gyro_GetY(void);

float Gyro_GetZ(void);

#endif
