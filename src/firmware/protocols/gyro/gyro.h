#ifndef _GYRO_H_
#define _GYRO_H_

#include "itg3200.h"

void InitializeGyro(void);

void Gyro_Update(void);

float Gyro_GetX(void);

float Gyro_GetY(void);

float Gyro_GetZ(void);

#endif
