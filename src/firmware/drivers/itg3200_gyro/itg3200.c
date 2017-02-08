// i2g3200.c
// gyro.c
#include "itg3200.h"
#include "i2c_driver.h"
#include <util/delay.h>
#include <stdbool.h>


#include "uart.h"

float _g_t;
float _g_x;
float _g_y;
float _g_z;

float g_offx;
float g_offy;
float g_offz;

bool initialized = false;

void ITG3200_Calibrate(void);

float _Calculate_G_Temp(float _t_raw);

void ITG3200_ReadGyro(float *temp, float *g_x, float *g_y, float *g_z);

void InitializeITG3200(void)
{
    i2c_init();

    i2c_start(ITG3200_ADDR);
    i2c_write(0x3E);
    i2c_write(0x00);
    i2c_stop();

    i2c_start(ITG3200_ADDR);
    i2c_write(0x15);
    i2c_write(0x07);
    i2c_stop();

    i2c_start(ITG3200_ADDR);
    i2c_write(0x16);
    i2c_write(0x1E);
    i2c_stop();

    i2c_start(ITG3200_ADDR);
    i2c_write(0x17);
    i2c_write(0x00);
    i2c_stop();

    ITG3200_Calibrate();

    _g_t = 0.0;
    _g_x = 0.0;
    _g_y = 0.0;
    _g_z = 0.0;

    initialized = true;
}

void ITG3200_Calibrate(void)
{
    float tmpx = 0;
    float tmpy = 0;
    float tmpz = 0;

    g_offx = 0;
    g_offy = 0;
    g_offz = 0;

    // take the mean from 10 gyro probes and divide
    // it from the current probe
    for (uint8_t i = 0; i < 10; i++)
    {
        ITG3200_UpdateData();
        tmpx += _g_x;
        tmpy += _g_y;
        tmpz += _g_z;
        _delay_ms(50);
    }

    g_offx = tmpx / 10;
    g_offy = tmpy / 10;
    g_offz = tmpz / 10;
}



void ITG3200_ReadGyro(float *temp, float *g_x, float *g_y, float *g_z)
{
    uint8_t buff[8];

    i2c_start(ITG3200_ADDR);
    i2c_write(0x1B);
    i2c_stop();

    i2c_receive(ITG3200_ADDR, buff, 8);
    
    *g_x = ((buff[2] << 8) | buff[3]) - g_offx;
    *g_y = ((buff[4] << 8) | buff[5]) - g_offy;
    *g_z = ((buff[6] << 8) | buff[7]) - g_offz;
    *temp = ((buff[0] << 8) | buff[1]);       // temperature
}

void ITG3200_UpdateData(void)
{
    float t, x, y, z;
    ITG3200_ReadGyro(&t, &x, &y, &z);

    _g_t = _Calculate_G_Temp(t);
    _g_x = x;
    _g_y = y;
    _g_z = z;
}

float ITG3200_GetX(void)
{
    return _g_x / 14.375;
}

float ITG3200_GetY(void)
{
    return _g_y / 14.375;
}

float ITG3200_GetZ(void)
{
    return _g_z / 14.375;
}

float ITG3200_GetTemp(void)
{
    return _g_t;
}

float _Calculate_G_Temp(float _t_raw)
{
    return (35 + (((float)_t_raw + 13200) / 280));;
}
