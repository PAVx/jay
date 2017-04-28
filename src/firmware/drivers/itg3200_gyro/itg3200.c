// i2g3200.c
// gyro.c
#include "itg3200.h"
#include "i2c_driver.h"
#include <util/delay.h>
#include <stdbool.h>


#include "uart.h"

double _g_t = 0;
double _g_x = 0;
double _g_y = 0;
double _g_z = 0;

double g_offx = 0;
double g_offy = 0;
double g_offz = 0;

bool initialized = false;

void ITG3200_Calibrate(void);

double _Calculate_G_Temp(double _t_raw);

void ITG3200_ReadGyro(double *temp, double *g_x, double *g_y, double *g_z);

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
    double tmpx = 0;
    double tmpy = 0;
    double tmpz = 0;

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



void ITG3200_ReadGyro(double *temp, double *g_x, double *g_y, double *g_z)
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
    double t, x, y, z;
    ITG3200_ReadGyro(&t, &x, &y, &z);

    _g_t = _Calculate_G_Temp(t);
    _g_x = x;
    _g_y = y;
    _g_z = z;
}

double ITG3200_GetX(void)
{
    return _g_x / 14.375;
}

double ITG3200_GetY(void)
{
    return _g_y / 14.375;
}

double ITG3200_GetZ(void)
{
    return _g_z / 14.375;
}

double ITG3200_GetTemp(void)
{
    return _Calculate_G_Temp(_g_t);
}

double _Calculate_G_Temp(double _t_raw)
{
    return (35 + (((double)_t_raw + 13200) / 280));;
}
