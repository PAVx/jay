#include "system.h"

#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

//#define MOTOR_TEST
//#define IMU_DEBUG
//#define GPS_DEBUG
#define LED_DEBUG
//#define PACKET_DEBUG
//#define PID_DEBUG
//#define FILTER_DEBUG
#define YPR
double ypr[3];

char testing[10];
static uint8_t o = 0;
char sys_print[32];

#ifdef IMU_DEBUG
	char gbuffer[15];
	char abuffer[15];
#endif

#ifdef GPS_DEBUG
	char gpsbuffer[50];
	int i = 0;
#endif

#ifdef MOTOR_TEST
	uint8_t byte = 0;
	uint16_t motor_vals[4] = {0};
	uint8_t motor = 0;
#endif

#ifdef PID_DEBUG
	double ypr[3];
	int motor_delta[4];

	static uint8_t o = 0;
#endif

#ifdef FILTER_DEBUG
	double compPitch, compRoll;
#endif

int main (void) {
	system_initialize();

	AttituteAdjustSetDesired(0, 0, 0);

	CompInit(PID_UPDATE_PERIOD_SECONDS, 2);
	CompStart();


	for(;;) {

		#ifdef UART
			if(!UART_IsEmpty()){
				if ((UART_GetByte() == 's') && (o == 0)) {
					motor_set(MOTOR_ONE, 8);
					motor_set(MOTOR_TWO, 8);
					motor_set(MOTOR_THREE, 8);
					motor_set(MOTOR_FOUR, 8);

					o = 1;
				}
			}
			if(!UART_IsEmpty()) {
				if(UART_GetByte() == 'k'){
					motor_set(MOTOR_ONE, 0);
					motor_set(MOTOR_TWO, 0);
					motor_set(MOTOR_THREE, 0);
					motor_set(MOTOR_FOUR, 0);

					o = 0;
				}
			}
		#endif

		#ifdef MOTOR_TEST
			if (!UART_IsEmpty()) {
				byte = UART_GetByte();
				toggle_led(SYSTEM_LED);

				switch (byte) {

					case '1':
						motor = 1;
						break;
					case '2':
						motor = 2;
						break;
					case '3':
						motor = 3;
						break;
					case '4':
						motor = 4;
						break;

					case '5':
						motor = 5;
						break;

					case 'w':

						if (motor == 5) {
							motor_vals[0] += 10;
							if (motor_vals[0] > 255) {
								motor_vals[0] = 255;
							}
							motor_vals[1] += 10;
							if (motor_vals[1] > 255) {
								motor_vals[1] = 255;
							}
							motor_vals[2] += 10;
							if (motor_vals[2] > 255) {
								motor_vals[2] = 255;
							}
							motor_vals[3] += 10;
							if (motor_vals[3] > 255) {
								motor_vals[3] = 255;
							}

							motor_set(1, motor_vals[0]);
							motor_set(2, motor_vals[1]);
							motor_set(3, motor_vals[2]);
							motor_set(4, motor_vals[3]);


						} else {
							motor_vals[motor - 1] += 10;
							if (motor_vals[motor - 1] > 255) {
								motor_vals[motor - 1] = 255;
							}
							motor_set(motor, (uint8_t)motor_vals[motor - 1]);
						}

						break;
					case 's':
						if (motor == 5) {
							if (motor_vals[0] > 10) {
								motor_vals[0] -= 10;
							} else {
								motor_vals[0] = 0;
							}

							if (motor_vals[1] > 10) {
								motor_vals[1] -= 10;
							} else {
								motor_vals[1] = 0;
							}

							if (motor_vals[2] > 10) {
								motor_vals[2] -= 10;
							} else {
								motor_vals[2] = 0;
							}

							if (motor_vals[2] > 10) {
								motor_vals[2] -= 10;
							} else {
								motor_vals[2] = 0;
							}

							motor_set(1, motor_vals[0]);
							motor_set(2, motor_vals[1]);
							motor_set(3, motor_vals[2]);
							motor_set(4, motor_vals[3]);
						} else {
							if (motor_vals[motor - 1] > 10) {
								motor_vals[motor - 1] -= 10;
							} else {
								motor_vals[motor - 1] = 0;
							}

							motor_set(motor, (uint8_t)motor_vals[motor - 1]);
						}

						break;

					case 'k':

						motor_set(1, 0);
						motor_set(2, 0);
						motor_set(3, 0);
						motor_set(4, 0);

						motor_vals[0] = 0;
						motor_vals[1] = 0;
						motor_vals[2] = 0;
						motor_vals[3] = 0;


						break;

					default:
						break;

				}
			}
		#endif // MOTOR_TEST

		#ifdef LED_DEBUG
			#ifdef LEDS
	  			toggle_led(GP_LED2);
	  			toggle_led(GP_LED1);
		  	#endif
  		#endif

		#ifdef PACKET_DEBUG
		  	#ifdef COM
		  		#ifdef UART
					receive_packet();
				#endif
		  		packet_send();
		  	#endif
		#endif // PACKET_DEBUG

		#ifdef IMU_DEBUG
			#ifdef GYRO
	  			Gyro_Update();
	  		#endif

	  		#ifdef ACCEL
	  			Accel_Update();
	  		#endif

		 	#ifdef GYRO
	  			memset(gbuffer, '\0', 15);
				UART_SendByte('\n');

				sprintf(gbuffer, "  G_X = %f\n", Gyro_GetX());
				UART_SendString(gbuffer);
				sprintf(gbuffer, " G_Y = %f\n", Gyro_GetY());
				UART_SendString(gbuffer);
				sprintf(gbuffer, " G_Z = %f\n", Gyro_GetZ());
				UART_SendString(gbuffer);

				UART_SendByte('\n');

		 	#endif

		 	#ifdef ACCEL
				memset(abuffer, '\0', 15);
				UART_SendByte('\n');

				sprintf(abuffer, "  A_X = %f\n", Accel_GetX());
				UART_SendString(abuffer);
				sprintf(abuffer, " A_Y = %f\n", Accel_GetY());
				UART_SendString(abuffer);
				sprintf(abuffer, " A_Z = %f\n", Accel_GetZ());
				UART_SendString(abuffer);

				UART_SendString("--------------------------");
		 	#endif // ACCEL
        	#endif // IMU_DEBUG

		#ifdef GPS_DEBUG
	        #ifdef GPS
	            NEO6M_GetChar();

	            if(GPS_NewDataReady()) {

				UART_SendString("-------***********-------");

	                memset(gpsbuffer, '\0', 50);
	                GPS_UpdateData();
	                struct tm time = GPS_GetTime();
	                sprintf(gpsbuffer, "T: %02d:%02d:%02d\nL: %.0f\nL: %.0f\nS: %.2f\nA: %.2f\n", time.tm_hour, time.tm_min, time.tm_sec, GPS_GetLatitude(), GPS_GetLongitude(), GPS_GetSpeed(), GPS_GetAltitude());

	                while (gpsbuffer[i] != '\0') {
	                    UART_SendByte(gpsbuffer[i]);
	                    i++;
	                }
	            }

	        #endif
	    	#endif



		#ifdef PID_DEBUG
			if(PIDGetFlag() == 1) {

				Gyro_Update();
				Accel_Update();

				cli();
				imu2euler(ypr, Accel_GetX(), Accel_GetY(), Accel_GetZ(), Mag_GetX(), Mag_GetY());
				sei();

				sprintf(testing, " \nY: %f ", ypr[0]);
				UART_SendString(testing);
				sprintf(testing, " P: %f ", ypr[1]);
				UART_SendString(testing);
				sprintf(testing, " R: %f\n", ypr[2]);
				UART_SendString(testing);


				AttituteAdjustUpdatePID(ypr[0], ypr[1], ypr[2]);

				// Update Motors
				AttitudeAdjustGetError(motor_delta);

				if (o == 1) {
					AttitudeAdjustSetActuation(motor_delta);
				}

				// Debugging
				sprintf(testing, " \nM1: %d | ", motor_get_speed(MOTOR_ONE));
				UART_SendString(testing);
				sprintf(testing, " M2: %d | ", motor_get_speed(MOTOR_TWO));
				UART_SendString(testing);
				sprintf(testing, " M3: %d | ", motor_get_speed(MOTOR_THREE));
				UART_SendString(testing);
				sprintf(testing, " M4: %d ", motor_get_speed(MOTOR_FOUR));
				UART_SendString(testing);

				PIDResetFlag();
			}
		#endif // PID_DEBUG

		#ifdef FILTER_DEBUG
			if(PIDGetFlag() == 1) {
				Gyro_Update();
				Accel_Update();

				CompAccelUpdate(Accel_GetX(), Accel_GetY(), Accel_GetZ());
				CompGyroUpdate(Gyro_GetX(), Gyro_GetY(), Gyro_GetZ());
				CompUpdate();
				CompAnglesGet(&compPitch, &compRoll);

				sprintf(testing, " \nP: %f ", compPitch);
				UART_SendString(testing);
				sprintf(testing, " R: %f", compRoll);
				UART_SendString(testing);
				PIDResetFlag();
			}
		#endif

		#ifdef YPR
			Gyro_Update();
			Accel_Update();

			cli();
			imu2euler(ypr, Accel_GetX(), Accel_GetY(), Accel_GetZ(), Mag_GetX(), Mag_GetY());
			sei();

			sprintf(testing, " \nY: %f ", ypr[0]);
			UART_SendString(testing);
			sprintf(testing, " P: %f ", ypr[1]);
			UART_SendString(testing);
			sprintf(testing, " R: %f\n", ypr[2]);
			UART_SendString(testing);
		#endif

	}

}
