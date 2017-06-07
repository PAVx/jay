/*
    PAVx -- Pod-Based Autonomous Vehicles
    Library Created By: Niraj Raniga
    March 2017
*/

// Externals
#ifndef _I2C_PROTOCOL
#define _I2C_PROTOCOL

#include <stdint.h>
#include "../../drivers/i2c_driver/i2cmaster.h"

extern void initI2C(void);
extern void writeI2Cbyte(uint8_t address, uint8_t location, uint8_t value);
extern void readI2CbyteArray(uint8_t address, uint8_t location, uint8_t *array, uint8_t size);
extern uint8_t writeI2CReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length);

#endif
