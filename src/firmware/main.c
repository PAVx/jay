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
char gpsbuffer[50];

int main (void) {

	system_initialize();
	softuart_init();
	led_off(SYSTEM_LED);

	#ifdef UART
		UART_SendString("\nPAVx Jay UAV initialized\n\n");
	#endif
	#ifdef SW_UART
		softuart_puts("\nPavx", 0);
	#endif

	while(1) {

		switch(command):
		case MANUAL {
			AttituteAdjustSetDesired(yawDesired, pitchDesired, rollDesires);
			break;
		}
		case GO_TO_LOCATION {
			calculatePath();
			// might want to stabalize before changing
			AttituteAdjustSetDesired(yawDesired, pitchDesired, rollDesires);
			break;
		}
		case GO_HOME {
			break;
		}
		default {
			break;
		}

<<<<<<< 9ec24ab8a7228e3435029260bd1ad411cd249c87
		if (1) { //if (system_ticked() == TRUE) {

			#ifdef GYRO
  			Gyro_Update();
  		#endif

  		#ifdef ACCEL
  			Accel_Update();
  		#endif

        #ifdef GPS
            NEO6M_GetChar();
        #endif

  		#ifdef LEDS
  			//toggle_led(SYSTEM_LED);
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

            #ifdef GPS

                if(GPS_NewDataReady()) {

                    memset(gpsbuffer, '\0', 50);
                    GPS_UpdateData();
                    struct tm time = GPS_GetTime();
                    sprintf(gpsbuffer, "T: %02d:%02d:%02d\nL: %.0f\nL: %.0f\nS: %.2f\nA: %.2f\n", time.tm_hour, time.tm_min, time.tm_sec, GPS_GetLatitude(), GPS_GetLongitude(), GPS_GetSpeed(), GPS_GetAltitude());
                    int i = 0;
                    while (gpsbuffer[i] != '\0') {
                        _uart_driver_SendByte(gpsbuffer[i]);
                        i++;
                    }
                }

            #endif

			#ifdef UART
  		 		//UART_SendString("\nHW_UART PRINT\n\r");
  		#endif

			#ifdef SW_UART
		 		//softuart_puts("S\n",0);
=======
		if (system_ticked() == TRUE) {
			#ifdef CAM
				Cam_Update();
>>>>>>> progress on PID loop
			#endif
			Gyro_Update();
			Accel_Update();

			if (GPS_NewDataReady()) {
				memset(gpsbuffer, '\0', 50);
				GPS_UpdateData();
				struct tm time = GPS_GetTime();
				sprintf(gpsbuffer, "T: %02d:%02d:%02d\nL: %.0f\nL: %.0f\nS: %.2f\nA: %.2f\n", time.tm_hour, time.tm_min, time.tm_sec, GPS_GetLatitude(), GPS_GetLongitude(), GPS_GetSpeed(), GPS_GetAltitude());
			}

			/*
			if (xbee ready) {
				getPacketXbee();
				processPacket();
			}
			*/

			// Update PID
			double* ypr = imu2euler(Accel_GetX(), Accel_GetY(), Accel_GetZ(), Mag_Get());
			AttituteAdjustUpdatePID(ypr[0], ypr[1], ypr[2]);

			// Update Motors
			int16_t* motor_delta = getMotorDeltal();
			motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) + motor_delta[0]);
			motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TW0) + motor_delta[1]);
			motor_set(MOTOR_THREE, motor_get_speed(MOTOR_ONE) + motor_delta[2]);
			motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_ONE) + motor_delta[3]);

			sendPacket();

  			toggle_led(SYSTEM_LED);
			system_untick();
		}

<<<<<<< 9ec24ab8a7228e3435029260bd1ad411cd249c87
		//softuart_puts("sup\n",0);
		//UART_SendString("\nHW_UART PRINT\n\r");

		// todo:
		// update accel registers
		// update gps registers
		// update PID controller for stabality
=======
>>>>>>> progress on PID loop
	}
}
