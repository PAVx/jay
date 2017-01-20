#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "uart.h"

#include "gyro.h"
#include "mag.h"
#include "temperature.h"

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

char buffer[256];

int main (void) {
	InitializeUART(38400, HW);

	UART_SendString("RESET\n", HW);
	InitializeMag();
	InitializeGyro();
	InitializeTemperature();

	memset(buffer, '\0', 128);

  	DDRB |= (1<<DDB5);  //Set the 6th bit on PORTB (i.e. PB5) to 1 => output

		while(1){
    	PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
    	_delay_ms(200);          //Delay for 1000ms => 1 sec

    	PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off

			sprintf(buffer, "M = %f\n", Mag_Get());
			UART_SendString(buffer, HW);
			memset(buffer, '\0', 128);

			Gyro_Update();
    	sprintf(buffer, "G(x,y,z) = %f,%f,%f\ntemp=%f\n\n", Gyro_GetX(), Gyro_GetY(), Gyro_GetZ(), Temperature_Get());

			UART_SendString(buffer, HW);
			memset(buffer, '\0', 128);
    	_delay_ms(200);          //Delay for 1000ms => 1 sec

	}
}
