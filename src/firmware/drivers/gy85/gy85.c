/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga
	March 2017
*/

#include "gy85.h"
#include "i2c.h"
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdbool.h>

#define LOWPASS_ACTIVE

#ifndef NO_SYSTEM_H
    #include "system.h"
#endif

double _a_x = 0;
double _a_y = 0;
double _a_z = 0;

double a_offx = 0.0;
double a_offy = 0.0;
double a_offz = 0.0;

double _g_x = 0;
double _g_y = 0;
double _g_z = 0;
double _g_t = 0;

double g_offx = 0;
double g_offy = 0;
double g_offz = 0;

double _m_x = 0;
double _m_y = 0;
double _m_z = 0;

#ifdef LOWPASS_ACTIVE
    double _accel_lowpass_x = 0;
    double _accel_lowpass_y = 0;
    double _accel_lowpass_z = 0;

    double _gyro_lowpass_x = 0;
    double _gyro_lowpass_y = 0;
    double _gyro_lowpass_z = 0;

    double _mag_lowpass_x = 0;
    double _mag_lowpass_y = 0;
    double _mag_lowpass_z = 0;

    static double alpha = 0.85;
#endif

void GY85_Read(double *a_x, double *a_y, double *a_z,
        double *g_x, double *g_y, double *g_z, double *g_t,
        double *m_x, double *m_y, double *m_z);

#ifdef LOWPASS_ACTIVE
    void Accel_LowPass_Filter(double *x, double *y, double *z);
    void Gyro_LowPass_Filter(double *x, double *y, double *z);
    void Mag_LowPass_Filter(double *x, double *y, double *z);
#endif

void ADXL345_Calibrate(void);
void ITG3200_Calibrate(void);

void GY85_Init(void)
{
    initI2C();

    #ifdef ACCEL
    writeI2Cbyte(ADXL345_ADDR, 0x31, 0x0B);     // +/- 16g, 13-bit Mode
    writeI2Cbyte(ADXL345_ADDR, 0x2D, 0x08);     // Start measurement
    writeI2Cbyte(ADXL345_ADDR, 0x2E, 0x80);     // Enable DATA_READY interrupt

    // Set offset reg to zero
    // writeI2CReg(ADXL345_ADDR, 0x1E, &zero, 1);
    // writeI2CReg(ADXL345_ADDR, 0x1F, &zero, 1);
    // writeI2CReg(ADXL345_ADDR, 0x20, &zero, 1);

    //_delay_ms(2); // Arbitrary delay amount
   // ADXL345_Calibrate();
    #endif

    #ifdef GYRO
    writeI2Cbyte(ITG3200_ADDR, 0x3E, 0x00);
    writeI2Cbyte(ITG3200_ADDR, 0x15, 0x07);
    writeI2Cbyte(ITG3200_ADDR, 0x16, 0x1E);
    writeI2Cbyte(ITG3200_ADDR, 0x17, 0x00);

    ITG3200_Calibrate();
    #endif

    #ifdef MAGNOMETER
    writeI2Cbyte(HMC5883L_ADDR, 0x00, 0x70);
    writeI2Cbyte(HMC5883L_ADDR, 0x01, 0xA0);
    writeI2Cbyte(HMC5883L_ADDR, 0x02, 0x00);    // continous measurement
    #endif
}

void GY85_Calibrate(void){
    initI2C();

    writeI2Cbyte(ADXL345_ADDR, 0x31, 0x0B);     // +/- 16g, 13-bit Mode
    writeI2Cbyte(ADXL345_ADDR, 0x2D, 0x08);     // Start measurement
    writeI2Cbyte(ADXL345_ADDR, 0x2E, 0x80);     // Enable DATA_READY interrupt

    writeI2Cbyte(ITG3200_ADDR, 0x3E, 0x00);
    writeI2Cbyte(ITG3200_ADDR, 0x15, 0x07);
    writeI2Cbyte(ITG3200_ADDR, 0x16, 0x1E);
    writeI2Cbyte(ITG3200_ADDR, 0x17, 0x00);

    writeI2Cbyte(HMC5883L_ADDR, 0x00, 0x70);
    writeI2Cbyte(HMC5883L_ADDR, 0x01, 0xA0);
    writeI2Cbyte(HMC5883L_ADDR, 0x02, 0x00);    // continous measurement


    uint8_t a_offx = 0;
    uint8_t a_offy = 0;
    uint8_t a_offz = 0;

    double tmpx_accel = 0;
    double tmpy_accel = 0;
    double tmpz_accel = 0;

    double tmpx_gyro = 0;
    double tmpy_gyro = 0;
    double tmpz_gyro = 0;


    // take the mean from 1000 accel probes and divide
    // it from the current probe
    for (uint16_t i = 0; i < 10000; i++)
    {
        GY85_UpdateData();
        tmpx_accel += _a_x;
        tmpy_accel += _a_y;
        tmpz_accel += _a_z;

        tmpx_gyro += _g_x;
        tmpy_gyro += _g_y;
        tmpz_gyro += _g_z;

        _delay_ms(1); // Arbitrary delay amount
    }

    // Each LSB of output in full-resolution is one-quarter of an
    // LSB of the offset register. (Datasheet: Offset Calibration)
    a_offx = -round(tmpx_accel / 1000 / 4);
    a_offy = -round(tmpy_accel / 1000 / 4) ;
    a_offz = -round(((tmpz_accel / 1000) - 256) / 4) ;

    g_offx = tmpx_gyro / GYRO_CALIBRATION_ITERATIONS;
    g_offy = tmpy_gyro / GYRO_CALIBRATION_ITERATIONS;
    g_offz = tmpz_gyro / GYRO_CALIBRATION_ITERATIONS;

    eeprom_write_block(&a_offx, (unsigned int*)0x4, sizeof(double));
    eeprom_write_block(&a_offy, (unsigned int*)0x8, sizeof(double));
    eeprom_write_block(&a_offz, (unsigned int*)0xC, sizeof(double));


    eeprom_write_block(&g_offx, (unsigned int*)0x10, sizeof(double));
    eeprom_write_block(&g_offy, (unsigned int*)0x14, sizeof(double));
    eeprom_write_block(&g_offz, (unsigned int*)0x18, sizeof(double));

}

void ADXL345_Calibrate(void)
{
    double a_offx = 0;
    double a_offy = 0;
    double a_offz = 0;

    #ifdef ACCEL_CALIBRATE
        double tmpx = 0;
        double tmpy = 0;
        double tmpz = 0;

        // take the mean from 1000 accel probes and divide
        // it from the current probe
        for (uint16_t i = 0; i < 10000; i++)
        {
            GY85_UpdateData();
            tmpx += _a_x;
            tmpy += _a_y;
            tmpz += _a_z;
            _delay_ms(1); // Arbitrary delay amount
        }

        // Each LSB of output in full-resolution is one-quarter of an
        // LSB of the offset register. (Datasheet: Offset Calibration)
        a_offx = -1 * round(tmpx / 1000 / 4);
        a_offy = -1 * round(tmpy / 1000 / 4) ;
        a_offz = -1 * round(((tmpz / 1000) - 256) / 4) ;

        eeprom_write_block(&a_offx, (uint8_t*)0x4, sizeof(double));
        eeprom_write_block(&a_offy, (uint8_t*)0x8, sizeof(double));
        eeprom_write_block(&a_offz, (uint8_t*)0xC, sizeof(double));
    #else
        eeprom_read_block(&a_offx, (uint8_t*)0x4, sizeof(double));
        eeprom_read_block(&a_offy, (uint8_t*)0x8, sizeof(double));
        eeprom_read_block(&a_offz, (uint8_t*)0xC, sizeof(double));
    #endif


    // Set calibrated offset values
  /*
    writeI2CReg(ADXL345_ADDR, 0x1E, &a_offx, 1);
    writeI2CReg(ADXL345_ADDR, 0x1F, &a_offy, 1);
    writeI2CReg(ADXL345_ADDR, 0x20, &a_offz, 1);
    */
}

void ITG3200_Calibrate(void)
{
    #ifdef GYRO_CALIBRATE
        double tmpx = 0;
        double tmpy = 0;
        double tmpz = 0;

        // take the mean from GYRO_CALIBRATION_ITERATIONS gyro probes and divide
        // it from the current probe
        for (uint16_t i = 0; i < GYRO_CALIBRATION_ITERATIONS; i++)
        {
            GY85_UpdateData();
            tmpx += _g_x;
            tmpy += _g_y;
            tmpz += _g_z;
            _delay_ms(2);
        }

        g_offx = tmpx / GYRO_CALIBRATION_ITERATIONS;
        g_offy = tmpy / GYRO_CALIBRATION_ITERATIONS;
        g_offz = tmpz / GYRO_CALIBRATION_ITERATIONS;

        eeprom_write_block(&g_offx, (uint8_t *)0x10, sizeof(double));
        eeprom_write_block(&g_offy, (uint8_t *)0x14, sizeof(double));
        eeprom_write_block(&g_offz, (uint8_t *)0x18, sizeof(double));
    #else
        eeprom_read_block(&g_offx, (uint8_t *)0x10, sizeof(double));
        eeprom_read_block(&g_offy, (uint8_t *)0x14, sizeof(double));
        eeprom_read_block(&g_offz, (uint8_t *)0x18, sizeof(double));
    #endif

}

void GY85_Read(double *a_x, double *a_y, double *a_z,
        double *g_x, double *g_y, double *g_z, double *g_t,
        double *m_x, double *m_y, double *m_z)
{
    // GYRO ROUTINE
    uint8_t buff[8];

    #ifdef GYRO
    readI2CbyteArray(ITG3200_ADDR, 0x1B, buff, 8);

    *g_x = ((buff[2] << 8) | buff[3]) - g_offx;
    *g_y = ((buff[4] << 8) | buff[5]) - g_offy;
    *g_z = ((buff[6] << 8) | buff[7]) - g_offz;
    *g_t = ((buff[0] << 8) | buff[1]); // temperature
    #endif

    // ACCEL ROUTINE
    #ifdef ACCEL
    readI2CbyteArray(ADXL345_ADDR, 0x32, buff, 6);

    *a_x = ((buff[1] << 8) | buff[0]);
    *a_y = ((buff[3] << 8) | buff[2]);
    *a_z = ((buff[5] << 8) | buff[4]);
    #endif

    // MAG ROUTINE
    #ifdef MAGNOMETER
    readI2CbyteArray(HMC5883L_ADDR, 0x03, buff, 6);

    *m_x = (buff[0]<<8) | buff[1];
    *m_y = (buff[2]<<8) | buff[3];
    *m_z = (buff[4]<<8) | buff[5];
    #endif

}

void GY85_UpdateData(void)
{
        GY85_Read(&_a_x, &_a_y, &_a_z, &_g_x, &_g_y, &_g_z, &_g_t, &_m_x, &_m_y, &_m_z);

        #ifdef LOWPASS_ACTIVE
            Accel_LowPass_Filter(&_a_x, &_a_y, &_a_z);
            Gyro_LowPass_Filter(&_g_x, &_g_y, &_g_z);
            Mag_LowPass_Filter(&_m_x, &_m_y, &_m_z);

            #ifdef IMU_UPSIDEDOWN
                _a_x = -1 * _accel_lowpass_x;
                _a_z = -1 * _accel_lowpass_z;
            #else
                _a_x = _accel_lowpass_x;
                _a_z = _accel_lowpass_z;
            #endif

            _a_y = _accel_lowpass_y;

            _g_x = _gyro_lowpass_x;
            _g_y = _gyro_lowpass_y;
            _g_z = _gyro_lowpass_z;
            _g_t = _g_t;

            _m_x = _mag_lowpass_x;
            _m_y = _mag_lowpass_y;
            _m_z = _mag_lowpass_z;
        #else
            #ifdef IMU_UPSIDEDOWN
                _a_x *= -1;
                _a_z *= -1;
            #endif
        #endif
}

#ifdef LOWPASS_ACTIVE
void Accel_LowPass_Filter(double *x, double *y, double *z)
{
  _accel_lowpass_x = (*x) * alpha + (_accel_lowpass_x * (1.0 - alpha));
  _accel_lowpass_y = (*y) * alpha + (_accel_lowpass_y * (1.0 - alpha));
  _accel_lowpass_z = (*z) * alpha + (_accel_lowpass_z * (1.0 - alpha));
}

void Gyro_LowPass_Filter(double *x, double *y, double *z)
{
  _gyro_lowpass_x = (*x) * alpha + (_gyro_lowpass_x * (1.0 - alpha));
  _gyro_lowpass_y = (*y) * alpha + (_gyro_lowpass_y * (1.0 - alpha));
  _gyro_lowpass_z = (*z) * alpha + (_gyro_lowpass_z * (1.0 - alpha));
}

void Mag_LowPass_Filter(double *x, double *y, double *z)
{
  _mag_lowpass_x = (*x) * alpha + (_mag_lowpass_x * (1.0 - alpha));
  _mag_lowpass_y = (*y) * alpha + (_mag_lowpass_y * (1.0 - alpha));
  _mag_lowpass_z = (*z) * alpha + (_mag_lowpass_z * (1.0 - alpha));
}
#endif

double Accel_GY85_GetX(void)
{
    return (_a_x - a_offx) / 256;
}

double Accel_GY85_GetY(void)
{
    return (_a_y - a_offy) / 256;
}

double Accel_GY85_GetZ(void)
{
    return (_a_z - a_offx) / 256;
}

double Gyro_GY85_GetX(void)
{
    return _g_x / GYRO_SENSITIVITY;
}

double Gyro_GY85_GetY(void)
{
    return _g_y / GYRO_SENSITIVITY;
}

double Gyro_GY85_GetZ(void)
{
    return _g_z / GYRO_SENSITIVITY;
}

double Mag_GY85_GetX(void)
{
    return _m_x;
}

double Mag_GY85_GetY(void)
{
    return _m_y;
}

double Mag_GY85_GetZ(void)
{
    return _m_z;
}
