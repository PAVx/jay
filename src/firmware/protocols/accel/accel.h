/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef _ACCEL_H_
#define _ACCEL_H_

#include "adxl345.h"

void InitializeAccel(void);

void Accel_Update(void);

float Accel_GetX(void);

float Accel_GetY(void);

float Accel_GetZ(void);

#endif
