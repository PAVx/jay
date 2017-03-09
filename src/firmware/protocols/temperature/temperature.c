/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

// temperature.c

#include "temperature.h"
#include "itg3200.h"

#include <stdbool.h>

void InitializeTemperature(void)
{
    InitializeITG3200();
}

float Temperature_Get(void)
{
	ITG3200_UpdateData();
    return ITG3200_GetTemp();
}
