// hmc5883l_mag.c

#include "i2c_driver.h"
#include "hmc5883l_mag.h"

#include <math.h>

float _m_x;
float _m_y;
float _m_z;

void HMC5883L_init(void){
	i2c_init();

	i2c_start(HMC5883L_WRITE);
	i2c_write(0x00); // set pointer to CRA
	i2c_write(0x70); // write 0x70 to CRA
	i2c_stop();

	i2c_start(HMC5883L_WRITE);
	i2c_write(0x01); // set pointer to CRB
	i2c_write(0xA0);
	i2c_stop();

	i2c_start(HMC5883L_WRITE);
	i2c_write(0x02); // set pointer to measurement mode
	i2c_write(0x00); // continous measurement
	i2c_stop();
}

void HMC5883L_Update(void) {
	_m_x = 0;
	_m_y = 0;
	_m_z = 0;

	i2c_start(HMC5883L_WRITE);
	i2c_write(0x03); // set pointer to X axis MSB
	i2c_stop();

	i2c_start(HMC5883L_READ);

	_m_x = ((uint8_t)i2c_read_ack())<<8;
	_m_x |= i2c_read_ack();

	_m_z = ((uint8_t)i2c_read_ack())<<8;
	_m_z |= i2c_read_ack();

	_m_y = ((uint8_t)i2c_read_ack())<<8;
	_m_y |= i2c_read_nack();

	i2c_stop();
}

float HMC5883L_GetX(void){
	return _m_x;
}

float HMC5883L_GetY(void){
	return _m_y;
}
float HMC5883L_GetZ(void){
	return _m_z;
}
