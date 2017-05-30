//***********************************************************
//* i2c.c
//***********************************************************

//***********************************************************
//* Includes
//***********************************************************

#include <avr/io.h>
#include "i2c.h"
#include <stdint.h>

//************************************************************
// Prototypes
//************************************************************

void writeI2Cbyte(uint8_t address, uint8_t location, uint8_t value);
void readI2CbyteArray(uint8_t address, uint8_t location, uint8_t *array,uint8_t size);

//************************************************************
// KK2.1 gyro-specifc code
//************************************************************
void initI2C(void)
{
    i2c_init();
}

void writeI2Cbyte(uint8_t address, uint8_t location, uint8_t value)
{
    i2c_start_wait(address+I2C_WRITE);				// Set up device address
    i2c_write(location);							// Set up register address
    i2c_write(value); 								// Write byte
    i2c_stop();
}

void readI2CbyteArray(uint8_t address, uint8_t location, uint8_t *array,uint8_t size)
{
    i2c_start_wait(address+I2C_WRITE);
    i2c_write(location);							// Set up register address
    i2c_rep_start(address+I2C_READ);
	int i=0;
	while(i < size)
	{
		if ((i+1)!=size)
			array[i]=i2c_readAck();
		else array[i]=i2c_readNak(); 				// Read without ACK on last byte
		i++;
	}
    i2c_stop();
}

uint8_t writeI2CReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length)
{
    i2c_start_wait(devaddr | 0x00);

    i2c_write(regaddr);

    for (uint16_t i = 0; i < length; i++)
    {
        if (i2c_write(data[i])) return 1;
    }

    i2c_stop();

    return 0;
}
