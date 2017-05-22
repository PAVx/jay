/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "accel.h"
#include "imu.h"

void InitializeAccel(void)
{
    ADXL345_Init();
}

void Accel_Update(void)
{
    // TODO: make interrupt based
    ADXL345_Read();
}

double Accel_GetX(void)
{
    return ADXL345_GetX();
}

double Accel_GetY(void)
{
    return ADXL345_GetY();
}

double Accel_GetZ(void)
{
    return ADXL345_GetZ();
}
