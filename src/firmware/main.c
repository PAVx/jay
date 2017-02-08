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
#include "xbee.h"

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

char buffer[128];
char serialBuff[256];

int main (void) {
	InitializeUART(9600);
	
	softuart_init();


	// UART_SendString("RESET\n");
	// InitializeMag();
	// InitializeGyro();
	// InitializeTemperature();

	memset(buffer, '\0', 128);
	//char okayResponse[2]; 

  	DDRB |= (1<<DDB5);  //Set the 6th bit on PORTB (i.e. PB5) to 1 => output
  	while(1){
    	PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
    	_delay_ms(200);          //Delay for 1000ms => 1 sec

    	PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off



    	sendPacket("PACKET Justin\r");
    	//hard_to_soft();
    	char *memory = hard_to_memory();
    	softuart_puts(memory);
    	free(memory);
    	
		// UART_SendString("+++\n");
		// UART_SendString("+++");
		// _delay_ms(2000);

		// memset(okayResponse, '\0', sizeof(okayResponse));
		// okayResponse[0] = UART_GetByte();
		// okayResponse[1] = UART_GetByte();
	 //    //softuart_puts("lking for ok\n");

		// // if((okayResponse[0] == 'O') && (okayResponse[1] == 'K'){

	 //    	//softuart_puts("ok\n");


		

		// discoverMode();




		//getSignalStrength();
		//getModuleName();
		// setModuleName();
		// int i;
		// char byteTest = 'y';
		// for ( i = 0; i < 5; i++) {
		// // if(!UART_IsEmpty()){
		// // 	byteTest = UART_GetByte();
		// 	softuart_putchar(byteTest);
		// // }
		// }
		//setModuleName();


    	_delay_ms(100);
	}
}

