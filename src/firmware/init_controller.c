/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga
	June 2017
*/

#include <avr/eeprom.h>
#include <avr/io.h>
#include "imu.h"
#include "leds.h"

#ifndef DEVICE_ID_EEPROM_ADDRESS
	#define DEVICE_ID_EEPROM_ADDRESS (0x00)
#endif
#ifndef ACCEL_CALIBRATE_EEPROM_START_ADDR
	#define ACCEL_CALIBRATE_EEPROM_START_ADDR (0x04)
#endif
#ifndef GYRO_CALIBRATE_EEPROM_START_ADDR
	#define GYRO_CALIBRATE_EEPROM_START_ADDR (0x10)
#endif

#define ACCEL (1)
#define GYRO (1)
#define MAGNOMETER (1)

int main (void) {

	leds_init(GP_LED1);
	led_on(GP_LED1);

	uint32_t device_id = DEVICE_ID;
	eeprom_write_block(&device_id, DEVICE_ID_EEPROM_ADDRESS, sizeof(uint32_t));
	IMU_Calibrate();

	led_off(GP_LED1);

	while(1);

}
