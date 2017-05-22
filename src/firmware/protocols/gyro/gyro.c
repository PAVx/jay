/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "gyro.h"
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
    return ITG3200_GetX();
}

double Gyro_GetY(void)
{
    return ITG3200_GetY();
}

double Gyro_GetZ(void)
{
    return ITG3200_GetZ();
}

double Gyro_GetTemp(void)
{
    return ITG3200_GetTemp();
}
