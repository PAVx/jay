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

double _lowpass_x = 0.0;
double _lowpass_y = 0.0;
double _lowpass_z = 0.0;

bool initialized = false;

void ITG3200_LowPass_Filter(double x, double y, double z);
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

    // take the mean from GYRO_CALIBRATION_ITERATIONS gyro probes and divide
    // it from the current probe
    for (uint16_t i = 0; i < GYRO_CALIBRATION_ITERATIONS; i++)
    {
        ITG3200_UpdateData();
        tmpx += _g_x;
        tmpy += _g_y;
        tmpz += _g_z;
        _delay_ms(2);
    }

    g_offx = tmpx / GYRO_CALIBRATION_ITERATIONS;
    g_offy = tmpy / GYRO_CALIBRATION_ITERATIONS;
    g_offz = tmpz / GYRO_CALIBRATION_ITERATIONS;
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

    ITG3200_LowPass_Filter(x, y, z);

    _g_t = _Calculate_G_Temp(t);
    _g_x = _lowpass_x;
    _g_y = _lowpass_y;
    _g_z = _lowpass_z;
}

void ITG3200_LowPass_Filter(double x, double y, double z)
{
  double alpha = 0.2;

  _lowpass_x = x * alpha + (_lowpass_x * (1.0 - alpha));
  _lowpass_y = y * alpha + (_lowpass_y * (1.0 - alpha));
  _lowpass_z = z * alpha + (_lowpass_z * (1.0 - alpha));

}

double ITG3200_GetX(void)
{
    return _g_x / GYRO_SENSITIVITY;
}

double ITG3200_GetY(void)
{
    return _g_y / GYRO_SENSITIVITY;
}

double ITG3200_GetZ(void)
{
    return _g_z / GYRO_SENSITIVITY;
}

double ITG3200_GetTemp(void)
{
    return _g_t;//_Calculate_G_Temp(_g_t);
}

double _Calculate_G_Temp(double _t_raw)
{
    return (35 + ((_t_raw + 13200) / 280));;
}
