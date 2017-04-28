// d6t8l.c

#include "d6t8l.h"
#include "i2c_driver.h"
#include <stdbool.h>

uint8_t heatmap[D6T8L_LEN] = {0};
uint8_t heat_avg = 0;

bool initialized = false;

void D6T8L_ReadIR(uint8_t *map, uint8_t *avg);

void D6T8L_Heat_Avg(uint8_t *map, uint8_t *avg);

void InitializeD6T8L(void)
{
    i2c_init();

    i2c_start(D6T8L_ADDR);
    i2c_write(0x3E);
    i2c_write(0x00);
    i2c_stop();

    i2c_start(D6T8L_ADDR);
    i2c_write(0x15);
    i2c_write(0x07);
    i2c_stop();

    i2c_start(D6T8L_ADDR);
    i2c_write(0x16);
    i2c_write(0x1E);
    i2c_stop();

    i2c_start(D6T8L_ADDR);
    i2c_write(0x17);
    i2c_write(0x00);
    i2c_stop();

    initialized = true;
}


void D6T8L_ReadIR(uint8_t *map)
{
    uint8_t buff[8];

    i2c_start(D6T8L_ADDR);
    i2c_write(D6T8L_CMD);
    i2c_stop();

    i2c_receive(D6T8L_ADDR, buff, 8);
    *map = buff;
}

void D6T8L_Heat_Avg(uint8_t *map, uint8_t *avg)
{
    uint8_t sum = 0;
    int i;
    for (i = 0; i < D6T8L_LEN; i++)
    {
        sum += map[i];
    }
    *avg = sum / D6T8L_LEN;
}

void D6T8L_UpdateData(void)
{
    D6T8L_ReadGyro(&heatmap);
    D6T8L_Heat_Avg(&heatmap, &heat_avg);
}

