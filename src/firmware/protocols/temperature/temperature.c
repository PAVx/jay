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
