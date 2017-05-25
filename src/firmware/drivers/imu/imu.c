#include "i2c.h"

//----------------------------------------

double _a_x = 0;
double _a_y = 0;
double _a_z = 0;

void ADXL345_Init()
{
    //Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
    i2c_beginTransmission( ADXL345 );      // start transmission to device
    i2c_write( 0x31 );                     // send register address
    i2c_write( 0x01 );                     // send value to write
    i2c_endTransmission();                 // end transmission

    //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
    i2c_beginTransmission( ADXL345 );      // start transmission to device
    i2c_write( 0x2D );                     // send register address  //Power Control Register
    i2c_write( 0x08 );                     // send value to write
    i2c_endTransmission();                 // end transmission
}

void ADXL345_Read()
{
    int buff[6];

    i2c_beginTransmission( ADXL345 );      // start transmission to device
    i2c_write( DATAX0 );                   // sends address to read from
    i2c_endTransmission();                 // end transmission

    i2c_beginTransmission( ADXL345 );      // start transmission to device
    i2c_requestFrom( ADXL345, 6 );         // request 6 bytes from device

    uint8_t i = 0;
    while(i2c_available())                 // device may send less than requested (abnormal)
    {
        buff[i] = i2c_read();              // receive a byte
        i++;
    }
    i2c_endTransmission();                 // end transmission

    _a_x = ((buff[1]) << 8) | buff[0];
    _a_y = ((buff[3]) << 8) | buff[2];
    _a_z = ((buff[5]) << 8) | buff[4];
}

double ADXL345_GetX(void)
{
    return _a_x;	// maybe: return _a_x / 256;
}

double ADXL345_GetY(void)
{
    return _a_y;
}

double ADXL345_GetZ(void)
{
    return _a_z;
}

//----------------------------------------

double _m_x = 0;
double _m_y = 0;
double _m_z = 0;

void HMC5883L_Init()
{
    //Put the HMC5883 IC into the correct operating mode
    i2c_beginTransmission( HMC5883 );      //open communication with HMC5883
    i2c_write( 0x02 );                     //select mode register
    i2c_write( 0x00 );                     //continuous measurement mode
    i2c_endTransmission();
}

void HMC5883L_Read()
{
    //Tell the HMC5883 where to begin reading data
    i2c_beginTransmission( HMC5883 );
    i2c_write( 0x03 );               //select register 3, X MSB register
    i2c_endTransmission();

    //Read data from each axis, 2 registers per axis
    i2c_requestFrom( HMC5883, 6 );
    if(6<=i2c_available()){
        _m_x = i2c_read()<<8;           //X msb
        _m_x |= i2c_read();             //X lsb
        _m_y = i2c_read()<<8;           //Z msb
        _m_y |= i2c_read();             //Z lsb
        _m_z = i2c_read()<<8;           //Y msb
        _m_z |= i2c_read();             //Y lsb
    }
}

double HMC5883L_GetX(void){
	return _m_x;
}

double HMC5883L_GetY(void){
	return _m_y;
}
double HMC5883L_GetZ(void){
	return _m_z;
}

//----------------------------------------

double _g_t = 0;
double _g_x = 0;
double _g_y = 0;
double _g_z = 0;

double g_offx = 0;
double g_offy = 0;
double g_offz = 0;

void ITG3200_Init()
{
    i2c_beginTransmission( ITG3200 );
    i2c_write( 0x3E );
    i2c_write( 0x00 );
    i2c_endTransmission();

    i2c_beginTransmission( ITG3200 );
    i2c_write( 0x15 );
    i2c_write( 0x07 );
    i2c_endTransmission();

    i2c_beginTransmission( ITG3200 );
    i2c_write( 0x16 );
    i2c_write( 0x1E );                         // +/- 2000 dgrs/sec, 1KHz, 1E, 19
    i2c_endTransmission();

    i2c_beginTransmission( ITG3200 );
    i2c_write( 0x17 );
    i2c_write( 0x00 );
    i2c_endTransmission();

    _delay_ms(10);

    GyroCalibrate();
}

void ITG3200_Calibrate()
{
    static int tmpx = 0;
    static int tmpy = 0;
    static int tmpz = 0;

    g_offx = 0;
    g_offy = 0;
    g_offz = 0;

    for( uint8_t i = 0; i < 10; i ++ ) //take the mean from 10 gyro probes and divide it from the current probe
    {
        _delay_ms(10);
        float* gp = readGyro();
        tmpx += *(  gp);
        tmpy += *(++gp);
        tmpz += *(++gp);
    }
    g_offx = tmpx/10;
    g_offy = tmpy/10;
    g_offz = tmpz/10;
}

void ITG3200_Read()
{

    i2c_beginTransmission( ITG3200 );
    i2c_write( 0x1B );
    i2c_endTransmission();

    i2c_beginTransmission( ITG3200 );
    i2c_requestFrom( ITG3200, 8 );             // request 8 bytes from ITG3200

    int i = 0;
    uint8_t buff[8];
    while(i2c_available())
    {
        buff[i] = i2c_read();
        i++;
    }
    i2c_endTransmission();

    _g_x[0] = ((buff[4] << 8) | buff[5]) - g_offx;
    _g_y[1] = ((buff[2] << 8) | buff[3]) - g_offy;
    _g_z[2] = ((buff[6] << 8) | buff[7]) - g_offz;
    _g_t[3] = ((buff[0] << 8) | buff[1]);       // temperature
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
    return 35 + ( g_t + 13200 ) / 280;
}
