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
float _Calculate_G_X(float _x_raw);
float _Calculate_G_Y(float _y_raw);
float _Calculate_G_Z(float _z_raw);

void ITG3200_ReadGyro(float *temp, float *g_x, float *g_y, float *g_z);

void InitializeITG3200(void)
{
    i2c_init();

    i2c_start(ITG3200_ADDR<<1);
    i2c_write(0x3E);
    i2c_write(0x00);
    i2c_stop();

    i2c_start(ITG3200_ADDR<<1);
    i2c_write(0x15);
    i2c_write(0x07);
    i2c_stop();

    i2c_start(ITG3200_ADDR<<1);
    i2c_write(0x16);
    i2c_write(0x1E);
    i2c_stop();

    i2c_start(ITG3200_ADDR<<1);
    i2c_write(0x17);
    i2c_write(0x00);
    i2c_stop();

//    ITG3200_Calibrate();

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
        tmpx += ITG3200_GetX();
        tmpy += ITG3200_GetY();
        tmpz += ITG3200_GetZ();
        _delay_ms(50);
    }

    g_offx = tmpx / 10;
    g_offy = tmpy / 10;
    g_offz = tmpz / 10;
}



void ITG3200_ReadGyro(float *temp, float *g_x, float *g_y, float *g_z)
{
    uint8_t buff[8];

    i2c_start(ITG3200_ADDR<<1);
    i2c_write(0x1B);
    i2c_stop();

    i2c_receive(ITG3200_ADDR<<1, buff, 8);
    
    *g_x = ((buff[4] << 8) | buff[5]) - g_offx;
    *g_y = ((buff[2] << 8) | buff[3]) - g_offy;
    *g_z = ((buff[6] << 8) | buff[7]) - g_offz;
    *temp = ((buff[0] << 8) | buff[1]);       // temperature
}

void ITG3200_UpdateData(void)
{
    float t, x, y, z;
    ITG3200_ReadGyro(&t, &x, &y, &z);

    _g_t = _Calculate_G_Temp(t);
    _g_x = _Calculate_G_X(x);
    _g_y = _Calculate_G_Y(y);
    _g_z = _Calculate_G_Z(z);
}

float ITG3200_GetX(void)
{
    return _g_x;
}

float ITG3200_GetY(void)
{
    return _g_y;
}

float ITG3200_GetZ(void)
{
    return _g_z;
}

float ITG3200_GetTemp(void)
{
    return _g_t;
}

float _Calculate_G_X(float _x_raw)
{
    return (float)_x_raw / 14.375;
}

float _Calculate_G_Y(float _y_raw)
{
    return (float)_y_raw / 14.375;
}

float _Calculate_G_Z(float _z_raw)
{
    return (float)_z_raw / 14.375;
}

float _Calculate_G_Temp(float _t_raw)
{
    return (35 + (((float)_t_raw + 13200) / 280));;
}
