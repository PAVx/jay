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

float Gyro_GetX(void)
{
    return ITG3200_GetX();
}

float Gyro_GetY(void)
{
    return ITG3200_GetY();
}

float Gyro_GetZ(void)
{
    return ITG3200_GetZ();
}
