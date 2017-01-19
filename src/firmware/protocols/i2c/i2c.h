#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_

#include <stdint.h>

void InitializeI2C(void);

uint8_t I2C_ReadByte(uint8_t address, uint8_t regAddr);
uint16_t I2C_ReadTwoBytes(uint8_t r_ad, uint8_t hiReg, uint8_t lowReg);

void I2C_WriteByte(uint8_t address, uint8_t regAddr, uint8_t data);
void I2C_WriteBytes(uint8_t address, uint8_t regAddr, uint8_t length, uint8_t *data);

#endif
