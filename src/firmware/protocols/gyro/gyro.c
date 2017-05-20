/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "gyro.h"
#include "itg3200.h"

void InitializeGyro(void)
{
    InitializeITG3200();
}

void Gyro_Update(void)
{
    // TODO: make interrupt based
    ITG3200_UpdateData();
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
