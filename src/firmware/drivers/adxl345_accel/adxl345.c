// adxl345.c
// accel.c
#include "adxl345.h"
#include "i2c.h"
#include "system.h"
#include <util/delay.h>
#include <stdbool.h>

double _a_x;
double _a_y;
double _a_z;

double _g_x;
double _g_y;
double _g_z;
double _g_t;

double _lowpass_x;
double _lowpass_y;
double _lowpass_z;

double _m_x;
double _m_y;
double _m_z;

void ADXL345_ReadAccel(double *a_x, double *a_y, double *a_z,
        double *g_x, double *g_y, double *g_z, double *g_t,
        double *m_x, double *m_y, double *m_z);
void ADXL345_LowPass_Filter(double x, double y, double z);
void ADXL345_Calibrate(void);

static double alpha = 0.2;

static double ux = 0;
static double uy = 0;
static double uz = 0;

static double gx = 0;
static double gy = 0;
static double gz = 0;
static double gt = 0;

static double mx = 0;
static double my = 0;
static double mz = 0;


void InitializeADXL345(void)
{
    //uint8_t zero = 0;
    initI2C();
    // AN-1077: Minimum init

    writeI2Cbyte(ADXL345_ADDR, 0x31, 0x0B);
    // i2c_start(ADXL345_ADDR);
    // i2c_write(0x31);
    // i2c_write(0x0B);    // +/- 16g, 13-bit Mode
    // i2c_stop();

    writeI2Cbyte(ADXL345_ADDR, 0x2D, 0x08);
    // i2c_start(ADXL345_ADDR);
    // i2c_write(0x2D);
    // i2c_write(0x08);    // Start measurement
    // i2c_stop();

    writeI2Cbyte(ADXL345_ADDR, 0x2E, 0x80);
    // i2c_start(ADXL345_ADDR);
    // i2c_write(0x2E);
    // i2c_write(0x80);    // Enable DATA_READY interrupt
    // i2c_stop();

    // Set offset reg to zero
    // writeI2CReg(ADXL345_ADDR, 0x1E, &zero, 1);
    // writeI2CReg(ADXL345_ADDR, 0x1F, &zero, 1);
    // writeI2CReg(ADXL345_ADDR, 0x20, &zero, 1);

    _a_x = 0;
    _a_y = 0;
    _a_z = 0;

    _lowpass_x = 0;
    _lowpass_x = 0;
    _lowpass_x = 0;

    //_delay_ms(2); // Arbitrary delay amount
    //ADXL345_Calibrate();
}

void ADXL345_Calibrate(void)
{
    double tmpx = 0;
    double tmpy = 0;
    double tmpz = 0;

    uint8_t a_offx = 0;
    uint8_t a_offy = 0;
    uint8_t a_offz = 0;


    // take the mean from 1000 accel probes and divide
    // it from the current probe
    for (uint16_t i = 0; i < 1000; i++)
    {
        ADXL345_UpdateData();
        tmpx += _a_x;
        tmpy += _a_y;
        tmpz += _a_z;
        _delay_ms(1); // Arbitrary delay amount
    }

    // Each LSB of output in full-resolution is one-quarter of an
    // LSB of the offset register. (Datasheet: Offset Calibration)
    a_offx = -round(tmpx / 1000 / 4);
    a_offy = -round(tmpy / 1000 / 4) ;
    a_offz = -round(((tmpz / 1000) - 256) / 4) ;

    // Set calibrated offset values
    writeI2CReg(ADXL345_ADDR, 0x1E, &a_offx, 1);
    writeI2CReg(ADXL345_ADDR, 0x1F, &a_offy, 1);
    writeI2CReg(ADXL345_ADDR, 0x20, &a_offz, 1);

}

void ADXL345_ReadAccel(double *a_x, double *a_y, double *a_z,
        double *g_x, double *g_y, double *g_z, double *g_t,
        double *m_x, double *m_y, double *m_z)
{
        uint8_t buff2[8];

        readI2CbyteArray(ITG3200_ADDR, 0x1B, buff2, 8);
        // i2c_start(ITG3200_ADDR);
        // i2c_write(0x1B);
        // i2c_stop();
        //
        // i2c_receive(ITG3200_ADDR, buff, 8);

        *g_x = ((buff2[2] << 8) | buff2[3]); //- g_offx;
        *g_y = ((buff2[4] << 8) | buff2[5]); //- g_offy;
        *g_z = ((buff2[6] << 8) | buff2[7]); //- g_offz;
        *g_t = ((buff2[0] << 8) | buff2[1]); // temperature

    uint8_t buff[6];

    readI2CbyteArray(ADXL345_ADDR, 0x32, buff, 6);
    // i2c_start(ADXL345_ADDR);
    // i2c_write(0x32);
    // i2c_stop();
    //
    // i2c_receive(ADXL345_ADDR, buff, 6);

    *a_x = ((buff[1] << 8) | buff[0]);
    *a_y = ((buff[3] << 8) | buff[2]);
    *a_z = ((buff[5] << 8) | buff[4]);



    readI2CbyteArray(HMC5883L_WRITE, 0x03, buff, 6);

    *m_x = (buff[0]<<8) | buff[1];
    *m_y = (buff[2]<<8) | buff[3];
    *m_z = (buff[4]<<8) | buff[5];



}

void ADXL345_UpdateData(void)
{
    ADXL345_ReadAccel(&ux, &uy, &uz, &gx, &gy, &gz, &gt, &mx, &my, &mz);
    ADXL345_LowPass_Filter(ux, uy, uz);

    #ifdef IMU_UPSIDEDOWN
        _a_x = -1 * _lowpass_x;
        _a_z = -1 * _lowpass_z;
    #else
        _a_x = _lowpass_x;
        _a_z = _lowpass_z;
    #endif

    _a_y = _lowpass_y;

   _g_x = gx;
   _g_y = gy;
   _g_z = gz;
   _g_t = gt;

   _m_x = mx;
   _m_y = my;
   _m_z = mz;
}

void ADXL345_LowPass_Filter(double x, double y, double z)
{
  _lowpass_x = x * alpha + (_lowpass_x * (1.0 - alpha));
  _lowpass_y = y * alpha + (_lowpass_y * (1.0 - alpha));
  _lowpass_z = z * alpha + (_lowpass_z * (1.0 - alpha));

}

double ADXL345_GetX(void)
{
    return _a_x / 256;
}

double ADXL345_GetY(void)
{
    return _a_y / 256;
}

double ADXL345_GetZ(void)
{
    return _a_z / 256;
}

double Gyro_ADXL345_GetX(void)
{
    return _g_x / GYRO_SENSITIVITY;
}

double Gyro_ADXL345_GetY(void)
{
    return _g_y / GYRO_SENSITIVITY;
}

double Gyro_ADXL345_GetZ(void)
{
    return _g_z / GYRO_SENSITIVITY;
}

double Mag_ADXL345_GetX(void)
{
    return _m_x;
}

double Mag_ADXL345_GetY(void)
{
    return _m_y;
}

double Mag_ADXL345_GetZ(void)
{
    return _m_z;
}
