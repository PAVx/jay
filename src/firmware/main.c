#include "system.h"

char buffer[512];

int main (void) {
	system_initialize();

  	while(1) {
		
		//_delay_ms(1000);  		
  		if (system_ticked()) {

  			sprintf(buffer, "A_X = %f\nA_Y = %f\nA_Z = %f\n\nG_X = %f\nG_Y = %f\nG_Z = %f\n\n", Accel_GetX(), Accel_GetY(), Accel_GetZ(), 0.0,0.0,0.0);//Gyro_GetX(), Gyro_GetY(), Gyro_GetZ());
  			UART_SendString(buffer);

  			system_untick();
  		}
  	}
}

