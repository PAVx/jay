// d6t8l.c
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "d6t8l.h"
#include "i2c_driver.h"

static uint8_t raw_data[19] = {0};
static uint8_t heatmap[D6T8L_LEN] = {1};
static float t_PTAT = 0;
static uint8_t heat_avg = 0;

static bool initialized = false;

void D6T8L_ReadIR(void);

void D6T8L_Heat_Avg(uint8_t *map);

void InitializeD6T8L(void)
{
    i2c_init();

    // i2c_start(D6T8L_ADDR);
    // i2c_write(0x3E);
    // i2c_write(0x00);
    // i2c_stop();
    //
    // i2c_start(D6T8L_ADDR);
    // i2c_write(0x15);
    // i2c_write(0x07);
    // i2c_stop();
    //
    // i2c_start(D6T8L_ADDR);
    // i2c_write(0x16);
    // i2c_write(0x1E);
    // i2c_stop();
    //
    // i2c_start(D6T8L_ADDR);
    // i2c_write(0x17);
    // i2c_write(0x00);
    // i2c_stop();

    initialized = true;
}


void D6T8L_ReadIR(void)
{
    i2c_start(D6T8L_ADDR);
    i2c_write(D6T8L_CMD);
    i2c_stop();

    i2c_receive(D6T8L_ADDR, raw_data, 19);

    uint8_t i = 0;
    t_PTAT = (raw_data[0] + (raw_data[1] << 8) ) * 0.1;
        for (i = 0; i < D6T8L_LEN; i++) {
          heatmap[i] = (raw_data[(i*2+2)] + (raw_data[(i*2+3)] << 8 )) * 0.1;
    }

    // for (int ii = 0; ii < 8; ii++){
    //         sprintf(testing, " {%d} ", heat[ii]);
    //         UART_SendString(testing);
    // }
    // sprintf(testing, " \n");
    // UART_SendString(testing);

}

void D6T8L_Heat_Avg(uint8_t *map)
{
    uint8_t sum = 0;
    int i;
    for (i = 0; i < D6T8L_LEN; i++)
    {
        sum += map[i];
    }
    heat_avg = sum / D6T8L_LEN;
}

void D6T8L_UpdateData(void)
{
    D6T8L_ReadIR();
    D6T8L_Heat_Avg(heatmap);
}

uint8_t* D6T8L_GetData(void)
{
        return heatmap;
}

uint8_t D6T8L_GetAvgData(void)
{
        return heat_avg;
}
