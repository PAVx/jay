#include "i2c.h"
#include "i2c_driver.h"
#include <util/delay.h>

void InitializeI2C(void)
{
    i2c_init();
}

uint8_t I2C_ReadByte(uint8_t address, uint8_t regAddr)
{
    uint8_t data = 0;
    i2c_start(address);
    i2c_write(regAddr);
    data = i2c_read_ack();
    i2c_stop();
    return data;
}

uint16_t I2C_ReadTwoBytes(uint8_t r_ad, uint8_t hiReg, uint8_t lowReg)
{
    uint8_t _hi, _lo;

    _hi = I2C_ReadByte(r_ad, hiReg);
    _lo = I2C_ReadByte(r_ad, lowReg);

    return ((_hi << 8) | _lo);
}

void I2C_ReadBytes(uint8_t address, uint8_t regAddr, uint8_t length, uint8_t *data)
{
    i2c_readReg(address, regAddr, data, length);
}

void I2C_WriteByte(uint8_t address, uint8_t regAddr, uint8_t data)
{
    i2c_start(address);
    _delay_ms(20);
    i2c_write(regAddr);
    _delay_ms(20);
    i2c_write(data);
    _delay_ms(20);
    i2c_read_ack();
    i2c_stop();
}

void I2C_WriteBytes(uint8_t address, uint8_t regAddr, uint8_t length, uint8_t *data)
{
    i2c_writeReg(address, regAddr, data, length);
}
