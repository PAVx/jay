#include "system.h"

#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

char gbuffer[128];
char abuffer[128];

int main (void) {
	system_initialize();
	softuart_init();
	DDRB |= (1<<DDB5);  //Set the 6th bit on PORTB (i.e. PB5) to 1 => output

  	while(1) {
			//PORTB |= (1<<PORTB5);     //Turn 6th bit on PORTB (i.e. PB5) to 1 => on

			// if (system_ticked()) {
  		// 	#ifdef GYRO
	  	// 		sprintf(gbuffer, "G_X = %f\nG_Y = %f\nG_Z = %f\n\n\r", Gyro_GetX(), Gyro_GetY(), Gyro_GetZ());
  		// 		UART_SendString(gbuffer);
  		// 		UART_SendString("\n");
  		// 	#endif
  		// 	#ifdef ACCEL
	  	// 		sprintf(abuffer, "A_X = %f\nA_Y = %f\nA_Z = %f\n\n\r", Accel_GetX(), Accel_GetY(), Accel_GetZ());
  		// 		UART_SendString(abuffer);
  		// 	#endif
			//
  		// 	UART_SendString("\nTICKED\n\r");
  		// 	system_untick();
  		// }

			softuart_puts("hey\n",0);
			//PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off


  	}
}
