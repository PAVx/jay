#include "system.h"

char gbuffer[128];
char abuffer[128];

int main (void) {
	system_initialize();

  	while(1) {
  		if (system_ticked()) {
  			#ifdef GYRO
	  			sprintf(gbuffer, "G_X = %f\nG_Y = %f\nG_Z = %f\n\n\r", Gyro_GetX(), Gyro_GetY(), Gyro_GetZ());
  				UART_SendString(gbuffer);
  				UART_SendString("\n");
  			#endif
  			#ifdef ACCEL
	  			sprintf(abuffer, "A_X = %f\nA_Y = %f\nA_Z = %f\n\n\r", Accel_GetX(), Accel_GetY(), Accel_GetZ());
  				UART_SendString(abuffer);
  			#endif

  			UART_SendString("\nTICKED\n\r");
  			system_untick();
  		}
  	}
}

