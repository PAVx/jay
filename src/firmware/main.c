
#include <stdio.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <string.h>

#include "uart.h"
#include "i2c_driver.h"

#include "gyro.h"
#include "temperature.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define HMC5883L_WRITE 0x3C
#define HMC5883L_READ 0x3D 

char buffer[256];

void init_HMC5883L(void){

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


int main (void) {
	InitializeUART(38400);

	UART_SendString("RESET\n");
	
	i2c_init();
	init_HMC5883L();
	InitializeGyro();
	InitializeTemperature();

	memset(buffer, '\0', 128);

	int16_t raw_x = 0;
	int16_t raw_y = 0;
	int16_t raw_z = 0;

  	DDRB |= (1<<DDB5);  //Set the 6th bit on PORTB (i.e. PB5) to 1 => output
  	while(1){
    	PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
    	_delay_ms(200);          //Delay for 1000ms => 1 sec

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

    	PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off

		
		//dtostrf(headingDegrees, 6, 2, buffer);
		
		sprintf(buffer, "M = %f\n", atan2((double)raw_y,(double)raw_x) * 180 / 3.141592654 + 180);
		UART_SendString(buffer);
		memset(buffer, '\0', 128);

    

		Gyro_Update();
    	sprintf(buffer, "G(x,y,z) = %f,%f,%f\ntemp=%f\n\n", Gyro_GetX(), Gyro_GetY(), Gyro_GetZ(), Temperature_Get());

		UART_SendString(buffer);
		memset(buffer, '\0', 128);
    	_delay_ms(200);          //Delay for 1000ms => 1 sec
		

	}
}

