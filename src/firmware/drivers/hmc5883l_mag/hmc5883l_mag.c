// hmc5883l_mag.c

#include "i2c_driver.h"
#include "hmc5883l_mag.h"

#include <math.h>

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

float HMC5883L_get(void) {
	int16_t raw_x = 0;
	int16_t raw_y = 0;
	int16_t raw_z = 0;

	i2c_start(HMC5883L_WRITE);
	i2c_write(0x03); // set pointer to X axis MSB
	i2c_stop();

	i2c_start(HMC5883L_READ);

	raw_x = ((uint8_t)i2c_read_ack())<<8;
	raw_x |= i2c_read_ack();

	raw_z = ((uint8_t)i2c_read_ack())<<8;
	raw_z |= i2c_read_ack();

	raw_y = ((uint8_t)i2c_read_ack())<<8;
	raw_y |= i2c_read_nack();

	i2c_stop();

	return atan2((double)raw_y,(double)raw_x) * 180 / 3.141592654 + 180;
}