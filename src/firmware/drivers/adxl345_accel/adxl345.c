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
void ADXL345_Calibrate(void);

void InitializeADXL345(void)
{
    uint8_t zero = 0;
    i2c_init();
    _delay_ms(2);
    // AN-1077: Minimum init
    i2c_start(ADXL345_ADDR);
    i2c_write(0x31);
    i2c_write(0x0B);    // +/- 16g, 13-bit Mode
    i2c_stop();

    i2c_start(ADXL345_ADDR);
    i2c_write(0x2D);
    i2c_write(0x08);    // Start measurement
    i2c_stop();

    i2c_start(ADXL345_ADDR);
    i2c_write(0x2E);
    i2c_write(0x80);    // Enable DATA_READY interrupt
    i2c_stop();
    _delay_ms(2);
    
    // Set offset reg to zero
    i2c_writeReg(ADXL345_ADDR, 0x1E, &zero, 1);
    i2c_writeReg(ADXL345_ADDR, 0x1F, &zero, 1);
    i2c_writeReg(ADXL345_ADDR, 0x20, &zero, 1);
    
    _a_x = 0;
    _a_y = 0;
    _a_z = 0;
    _delay_ms(20);
    ADXL345_Calibrate();

}
void ADXL345_Calibrate(void)
{
    float tmpx = 0;
    float tmpy = 0;
    float tmpz = 0;

    uint8_t a_offx = 0;
    uint8_t a_offy = 0;
    uint8_t a_offz = 0;

    
    // take the mean from 10 gyro probes and divide
    // it from the current probe
    for (uint8_t i = 0; i < 10; i++)
    {
        ADXL345_UpdateData();
        tmpx += _a_x;
        tmpy += _a_y;
        tmpz += _a_z;
        _delay_ms(20);
    }

    a_offx = -round(tmpx / 10 / 4);
    a_offy = -round(tmpy / 10 / 4) ;
    a_offz = -round(((tmpz / 10) - 256) / 4) ;

    i2c_writeReg(ADXL345_ADDR, 0x1E, &a_offx, 1);
    i2c_writeReg(ADXL345_ADDR, 0x1F, &a_offy, 1);
    i2c_writeReg(ADXL345_ADDR, 0x20, &a_offz, 1);

}



void ADXL345_ReadAccel(float *a_x, float *a_y, float *a_z)
{
    uint8_t buff[6];

    i2c_start(ADXL345_ADDR);
    i2c_write(0x32);
    i2c_stop();


    i2c_receive(ADXL345_ADDR, buff, 6);
    
    *a_x = ((buff[1] << 8) | buff[0]);
    *a_y = ((buff[3] << 8) | buff[2]);
    *a_z = ((buff[5] << 8) | buff[4]);

}

void ADXL345_UpdateData(void)
{
    float x, y, z;
    ADXL345_ReadAccel(&x, &y, &z);

    _a_x = x;
    _a_y = y;
    _a_z = z;
}

float ADXL345_GetX(void)
{
    return _a_x / 256;
}

float ADXL345_GetY(void)
{
    return _a_y / 256;
}

float ADXL345_GetZ(void)
{
    return _a_z / 256;
}



