/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "gyro.h"
#include "adxl345.h"
#include "imu.h"

void InitializeGyro(void)
{
    ITG3200_Init();
}

void Gyro_Update(void)
{
    ITG3200_Read();
}

double Gyro_GetX(void)
{
    return Gyro_ADXL345_GetX();
}

double Gyro_GetY(void)
{
    return Gyro_ADXL345_GetY();
}

double Gyro_GetZ(void)
{
    return Gyro_ADXL345_GetZ();
}

double Gyro_GetTemp(void)
{
    return ITG3200_GetTemp();
}
