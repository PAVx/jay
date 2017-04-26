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

		if (system_ticked() == TRUE) {
			#ifdef CAM
				Cam_Update();
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
			// Not too sure how packets will be parsed
			if (xbee ready) {
				getPacketXbee();
				processPacket();
			}
			*/

			// Update PID
			double* ypr = imu2euler(Accel_GetX(), Accel_GetY(), Accel_GetZ(), Mag_GetX(), Mag_GetY());
			AttituteAdjustUpdatePID(ypr[0], ypr[1], ypr[2]);

			// Update Motors
			int16_t* motor_delta = AttitudeAdjustGetActuation();
			motor_set(MOTOR_ONE, motor_get_speed(MOTOR_ONE) + motor_delta[0]);
			motor_set(MOTOR_TWO, motor_get_speed(MOTOR_TW0) + motor_delta[1]);
			motor_set(MOTOR_THREE, motor_get_speed(MOTOR_ONE) + motor_delta[2]);
			motor_set(MOTOR_FOUR, motor_get_speed(MOTOR_ONE) + motor_delta[3]);

			sendPacket();

  			toggle_led(SYSTEM_LED);
			system_untick();
		}
	}
}
