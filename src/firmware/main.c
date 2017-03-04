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
	
	#ifdef UART
		UART_SendString("\nPAVx Jay UAV initialized\n\n");
	#endif
	#ifdef SW_UART
		//softuart_puts("\nPAVx Jay UAV initialized -- SWUART", 0);
	#endif

  	while(1) {

    	#ifdef COM
    		#ifdef UART
				if (!UART_IsEmpty()) {
					packet_receiver();
				}
			#endif	
    	#endif


		if (system_ticked() == TRUE) {

			#ifdef GYRO
    			Gyro_Update();
    		#endif

    		#ifdef ACCEL
    			Accel_Update();
    		#endif
    	
    		#ifdef LEDS
    			toggle_led(SYSTEM_LED);
    		#endif

  		 	#ifdef GYRO
	  	 		sprintf(gbuffer, "G_X = %f\nG_Y = %f\nG_Z = %f\n\n\r", Gyro_GetX(), Gyro_GetY(), Gyro_GetZ());
  		 		UART_SendString(gbuffer);
  		 		UART_SendString("\n");
  		 	#endif

  		 	#ifdef ACCEL
	  	 		sprintf(abuffer, "A_X = %f\nA_Y = %f\nA_Z = %f\n\n\r", Accel_GetX(), Accel_GetY(), Accel_GetZ());
  		 		UART_SendString(abuffer);
  		 	#endif

			#ifdef UART
  		 		UART_SendString("\nHW_UART PRINT\n\r");
  		 	#endif

			#ifdef SW_UART
  		 		//softuart_puts("SW_UART PRINT\n",0);
  		 	#endif

		system_untick();

  		}		
	// todo:
		// update accel registers
		// update gps registers
		// update PID controller for stabality

  	}
}
