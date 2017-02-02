#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "uart.h"
#include "gyro.h"
#include "mag.h"
#include "temperature.h"
#include "drivers/softuart_driver/softuart.h"

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

void _main_delay(long x);

char buffer[256];

int main (void) {

	InitializeUART(38400);
	UART_SendString("RESET\n");
	softuart_init();


  DDRB |= (1<<DDB5);  //Set the 6th bit on PORTB (i.e. PB5) to 1 => output

	while(1){
    PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
    //sleep(1);          //Delay for 1000ms => 1 sec
    PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off
    //sleep(1);          //Delay for 1000ms => 1 sec

		softuart_puts("hey\n");
	}
}

void _main_delay(long x){
	for(int i = 0; i < x; i++){
		for(int j = 0; i < x; j++){
		}
	}
}
