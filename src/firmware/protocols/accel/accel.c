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

float Accel_GetX(void)
{
    return ADXL345_GetX();
}

float Accel_GetY(void)
{
    return ADXL345_GetY();
}

float Accel_GetZ(void)
{
    return ADXL345_GetZ();
}
