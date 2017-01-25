// adxl345.c
// accel.c
#include "adxl345.h"
#include "i2c_driver.h"
#include <util/delay.h>
#include <stdbool.h>

float _a_x;
float _a_y;
float _a_z;

void ADXL345_ReadAccel(float *a_x, float *a_y, float *a_z);
float _Calculate_A_X(float _x_raw);
float _Calculate_A_Y(float _y_raw);
float _Calculate_A_Z(float _z_raw);

void InitializeADXL345(void)
{
    i2c_init();
    // AN-1077: Minimum init
    i2c_start(ADXL345_ADDR);
    i2c_read_ack();
    i2c_write(0x31);
    i2c_read_ack();
    i2c_write(0x0B);    // +/- 16g, 13-bit Mode
    i2c_read_ack();
    i2c_stop();

    i2c_start(ADXL345_ADDR);
    i2c_read_ack();
    i2c_write(0x2D);
    i2c_read_ack();
    i2c_write(0x08);    // Start measurement
    i2c_read_ack();
    i2c_stop();

    i2c_start(ADXL345_ADDR);
    i2c_read_ack();
    i2c_write(0x2E);
    i2c_read_ack();
    i2c_write(0x80);    // Enable DATA_READY interrupt
    i2c_read_ack();
    i2c_stop();
}


void ADXL345_ReadAccel(float *a_x, float *a_y, float *a_z)
{
    uint8_t buff[6];

    i2c_start(ADXL345_ADDR);
    i2c_read_ack();
    i2c_write(0x32);
    i2c_read_ack();
    i2c_stop();


    i2c_receive(ADXL345_ADDR<<1, buff, 6);
    
    *a_x = ((buff[1] << 8) | buff[0]);
    *a_y = ((buff[3] << 8) | buff[2]);
    *a_z = ((buff[5] << 8) | buff[4]);
}

void ADXL345_UpdateData(void)
{
    float x, y, z;
    ADXL345_ReadAccel(&x, &y, &z);

    _a_x = _Calculate_A_X(x);
    _a_y = _Calculate_A_Y(y);
    _a_z = _Calculate_A_Z(z);
}

float ADXL345_GetX(void)
{
    return _a_x;
}

float ADXL345_GetY(void)
{
    return _a_y;
}

float ADXL345_GetZ(void)
{
    return _a_z;
}


float _Calculate_A_X(float _x_raw)
{
    return (float)_x_raw * 3.9;
}

float _Calculate_A_Y(float _y_raw)
{
    return (float)_y_raw * 3.9;
}

float _Calculate_A_Z(float _z_raw)
{
    return (float)_z_raw * 3.9;
}
