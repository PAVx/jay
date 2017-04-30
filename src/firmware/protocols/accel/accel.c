/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "accel.h"
#include "adxl345.h"

void InitializeAccel(void)
{
    InitializeADXL345();
}

void Accel_Update(void)
{
    // TODO: make interrupt based
    ADXL345_UpdateData();
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
