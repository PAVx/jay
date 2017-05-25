// hmc5883l_mag.c

#include <math.h>
#include "i2c.h"
#include "hmc5883l_mag.h"

int16_t _m_x;
int16_t _m_y;
int16_t _m_z;

double _lowpass_x;
double _lowpass_y;
double _lowpass_z;

void HMC5883L_LowPass_Filter(int16_t x, int16_t y, int16_t z);

void HMC5883L_init(void){

	initI2C();

	writeI2Cbyte(HMC5883L_WRITE, 0x00, 0x70);
	// i2c_start(HMC5883L_WRITE);
	// i2c_write(0x00); // set pointer to CRA
	// i2c_write(0x70); // write 0x70 to CRA
	// i2c_stop();

	writeI2Cbyte(HMC5883L_WRITE, 0x01, 0xA0);
	// i2c_start(HMC5883L_WRITE);
	// i2c_write(0x01); // set pointer to CRB
	// i2c_write(0xA0);
	// i2c_stop();

	writeI2Cbyte(HMC5883L_WRITE, 0x02, 0x00);
	// i2c_start(HMC5883L_WRITE);
	// i2c_write(0x02); // set pointer to measurement mode
	// i2c_write(0x00); // continous measurement
	// i2c_stop();

	_m_x = 0;
	_m_y = 0;
	_m_z = 0;
}

void HMC5883L_Update(void) {

	uint8_t buff[6];

	readI2CbyteArray(HMC5883L_READ, 0x03, buff, 6);
	// i2c_start(HMC5883L_WRITE);
	// i2c_write(0x03); // set pointer to X axis MSB
	// i2c_stop();
	//
	// i2c_start(HMC5883L_READ);
	//
	// _m_x = ((uint8_t)i2c_read_ack())<<8;
	// _m_x |= i2c_read_ack();
	//
	// _m_z = ((uint8_t)i2c_read_ack())<<8;
	// _m_z |= i2c_read_ack();
	//
	// _m_y = ((uint8_t)i2c_read_ack())<<8;
	// _m_y |= i2c_read_nack();

	_m_x = (uint8_t)buff[0]<<8 | buff[1];
	_m_y = (uint8_t)buff[2]<<8 | buff[3];
	_m_z = (uint8_t)buff[4]<<8 | buff[5];


	// i2c_stop();

	HMC5883L_LowPass_Filter(_m_x, _m_y, _m_z);
	_m_x = (int16_t)_lowpass_x;
	_m_y = (int16_t)_lowpass_y;
	_m_z = (int16_t)_lowpass_z;

}

void HMC5883L_LowPass_Filter(int16_t x, int16_t y, int16_t z)
{
  double alpha = 0.2;

  _lowpass_x = x * alpha + (_lowpass_x * (1.0 - alpha));
  _lowpass_y = y * alpha + (_lowpass_y * (1.0 - alpha));
  _lowpass_z = z * alpha + (_lowpass_z * (1.0 - alpha));

}

double HMC5883L_GetX(void){
	return (double)_m_x;
}

double HMC5883L_GetY(void){
	return (double)_m_y;
}
double HMC5883L_GetZ(void){
	return (double)_m_z;
}
