#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "uart.h"
#include "softuart.h"

#include "gyro.h"
#include "mag.h"
#include "temperature.h"

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

char buffer[256];
//extern softUART softUART_array[SOFTUART_CHANNELS];

int main (void) {

	InitializeUART(38400);
	_softuart_init();
	//InitializeMag();
	//InitializeGyro();
	//InitializeTemperature();
	UART_SendString("RESET\n");

	memset(buffer, '\0', 128);

  	DDRB |= (1<<DDB5);  //Set the 6th bit on PORTB (i.e. PB5) to 1 => output
		PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off

		while(1){
    	//PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
			UART_SendString("Sup\n");
    	_delay_ms(200);          //Delay for 1000ms => 1 sec
    	PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off
			UART_SendString("Dog\n");
			_delay_ms(200);          //Delay for 1000ms => 1 sec
			_softuart_putchar('a', 1);
			_softuart_putchar('b', 2);

	}
}
