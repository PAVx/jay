#ifndef _ACCEL_H_
#define _ACCEL_H_

#include "adxl345.h"

void InitializeAccel(void);

void Accel_Update(void);

float Accel_GetX(void);

float Accel_GetY(void);

float Accel_GetZ(void);

#endif
