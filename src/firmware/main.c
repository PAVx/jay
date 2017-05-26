#include "system.h"

#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define LED_DEBUG
//#define WAIT_FOR_SYSTEM_ACK
//#define PACKET_DEBUG
#ifdef BATTERY
	#define BATTERY_DEBUG
#endif
#define PID_DEBUG
#define KEYBOARD_DEBUG
//#define PID_PRINT_DEBUG
//#define PID_TIME_TEST
//static uint8_t roll_static_count = 0;
//static uint8_t pitch_static_count = 0;


//#define SEND_STATUS_PACKET

#ifdef SEND_STATUS_PACKET
	#ifdef IR_CAM
		#define IR_CAM_DEBUG (1)
	#endif
	static uint8_t battery_level = 0xFF;
	static uint16_t altitude = 420;
#endif

#ifdef KEYBOARD_DEBUG
	static uint8_t ref_init = 0;
#endif

char testing[30];
char sys_print[32];
char op_code;

static uint8_t o = 0;

#ifdef PID_DEBUG
	double ypr[3] = {0};
	double last_ypr[3] = {0};
	int motor_delta[4];

	#ifdef PID_PRINT_DEBUG
		static uint16_t pid_print_flag = 0;
	#endif
#endif


int main (void) {
	system_initialize();


	#ifdef WAIT_FOR_SYSTEM_ACK
		while(!system_acknowledged()) {
			send_initialization_packet();
		}
	#endif

	AttituteAdjustSetDesired(0, 0, 0); // testing this attitude
 	AttitudeSetThrottle(0);

 	#ifdef PID_TIME_TEST
 		leds_init(DIGITAL_PIN_1);
 		led_off(DIGITAL_PIN_1);
 	#endif

	for(;;) {

		#ifdef KEYBOARD_DEBUG
			if(!UART_IsEmpty()){
				op_code = UART_GetByte();

				if ((op_code == 's') && (o == 0) && battery_charged()) {
					AttitudeSetThrottle(4500);
					ref_init = 0;
					o = 1;
				}
				else if(op_code == 'k') {
					motor_set(MOTOR_ONE, 0);
					motor_set(MOTOR_TWO, 0);
					motor_set(MOTOR_THREE, 0);
					motor_set(MOTOR_FOUR, 0);
					o = 0;
				}
			}
		#endif

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

			if (tick_timer_flag(IMU_TIMER_ID)) {
				// #ifdef PID_TIME_TEST
 			// 		led_on(DIGITAL_PIN_1);
 			// 	#endif
				//
				// IMU_Update();
				// //Mag_Update();
				// //Gyro_Update();
				// //cli();
				//
				// //imu2euler(ypr, Accel_GetX(), Accel_GetY(), Accel_GetZ(), Gyro_GetX(), Gyro_GetY(), Gyro_GetZ(), Mag_GetX(), Mag_GetY());//Mag_GetX(), Mag_GetY());
				// //imu2euler_simple(ypr, Accel_GetX(), Accel_GetY(), Accel_GetZ(), 0, 0);
				// sensfusion6UpdateQ(Gyro_GetX(), Gyro_GetY(), Gyro_GetZ(), Accel_GetX(), Accel_GetY(), Accel_GetZ(), IMU_UPDATE_PERIOD_SECONDS*SYSTEM_TICK_OFFSET);
				// sensfusion6GetEulerRPY(&ypr[2], &ypr[1], &ypr[0]);

				clear_tick_timer_flag(IMU_TIMER_ID);

			}

			if(tick_timer_flag(PID_TIMER_ID)) {

				#ifdef LEDS
		  			toggle_led(GP_LED1);
			  	#endif

				//////////////////

				#ifdef PID_TIME_TEST
 					led_on(DIGITAL_PIN_1);
 				#endif

				IMU_Update();
				//Mag_Update();
				//Gyro_Update();
				//cli();

				//imu2euler(ypr, Accel_GetX(), Accel_GetY(), Accel_GetZ(), Gyro_GetX(), Gyro_GetY(), Gyro_GetZ(), Mag_GetX(), Mag_GetY());//Mag_GetX(), Mag_GetY());
				//imu2euler_simple(ypr, Accel_GetX(), Accel_GetY(), Accel_GetZ(), 0, 0);
				sensfusion6UpdateQ(Gyro_GetX(), Gyro_GetY(), Gyro_GetZ(), Accel_GetX(), Accel_GetY(), Accel_GetZ(), IMU_UPDATE_PERIOD_SECONDS*SYSTEM_TICK_OFFSET);
				sensfusion6GetEulerRPY(&ypr[2], &ypr[1], &ypr[0]);

				//////////////////

				AttituteAdjustUpdatePID(0, ypr[1], ypr[2]);

				// Update Motors
				AttitudeAdjustGetError(motor_delta);

				if (o == 1) {
					AttitudeAdjustSetActuation(motor_delta);
				}

				// Debugging
				/*if (abs(ypr[1]) > 45 || abs(ypr[2]) > 45) {
					motor_set(MOTOR_ONE, 0);
					motor_set(MOTOR_TWO, 0);
					motor_set(MOTOR_THREE, 0);
					motor_set(MOTOR_FOUR, 0);
					o = 0;
				}
				*/

				#ifdef PID_PRINT_DEBUG
					if(pid_print_flag == 10) {
						//sprintf(testing, " \nBATT: {%d} | ", (int)battery_get_voltage());
						//UART_SendString(testing);

						sprintf(testing, " \nY: {%lf} | ", ypr[0]);
						UART_SendString(testing);
						sprintf(testing, " P: {%lf} | ", ypr[1]);
						UART_SendString(testing);
						sprintf(testing, " R: {%lf}          ", ypr[2]);
						UART_SendString(testing);

						// sprintf(testing, "           M1: {%d} | ", (int)motor_get_speed(MOTOR_ONE));
						// UART_SendString(testing);
						// sprintf(testing, " M2: {%d} | ", (int)motor_get_speed(MOTOR_TWO));
						// UART_SendString(testing);
						// sprintf(testing, " M3: {%d} | ", (int)motor_get_speed(MOTOR_THREE));
						// UART_SendString(testing);
						// sprintf(testing, " M4: {%d}          ", (int)motor_get_speed(MOTOR_FOUR));
						// UART_SendString(testing);

						pid_print_flag = 0;

					}
					else pid_print_flag ++;
				#endif

				#ifdef PID_TIME_TEST
					led_off(DIGITAL_PIN_1);
				#endif

				clear_tick_timer_flag(PID_TIMER_ID);
			}

		#endif // PID_DEBUG

		#ifdef PACKET_DEBUG
			packet_receiver();

			if (tick_timer_flag(PACKET_TIMER_ID)) {
				// update IR cam data
				D6T8L_UpdateData();

				#ifdef LED_DEBUG
					#ifdef LEDS
		  				toggle_led(GP_LED2);
			  		#endif
	  			#endif

				status_update_time(420);
				status_update_longitude(-122.063338);
				status_update_latitude(37.000273);
				if (battery_charged()) {
					battery_level = 0xFF;
				} else {
					battery_level = 0x00;
				}

				status_update_status_vector(
					(uint64_t)(((uint64_t)altitude 		<< 48) 	& 0xFFFF000000000000) |
					(uint64_t)(((uint64_t)ypr[2] 		<< 32) 	& 0x0000FFFF00000000) |
					(uint64_t)(((uint64_t)ypr[1] 		<< 16) 	& 0x00000000FFFF0000) |
					(uint64_t)(((uint64_t)battery_level << 8) 	& 0x000000000000FF00) |
					(uint64_t)((D6T8L_GetAvgData() 		<< 0)	& 0x00000000000000FF)
					);

				//packet_send(STATUS_PACKET_TYPE);
				clear_tick_timer_flag(PACKET_TIMER_ID);
			}
		#endif

	}

}
