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
#ifdef BATTERY
	#define BATTERY_DEBUG
	char batt_debug[30];
#endif
#define PID_DEBUG
#define PID_PRINT_DEBUG
#define PID_TIME_TEST
static uint8_t roll_static_count = 0;
static uint8_t pitch_static_count = 0;

//#define FILTER_DEBUG
//#define YPR

static uint8_t ref_init = 0;

char testing[10];
char sys_print[32];
char op_code;

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
	uint32_t motor_vals[4] = {0};
	uint8_t motor = 0;
#endif

#if defined(PID_DEBUG) || defined(YPR)
	double ypr[3] = {0};
	double last_ypr[3] = {0};
	int motor_delta[4];

	static uint8_t o = 0;
	#ifdef PID_PRINT_DEBUG
		static uint16_t pid_print_flag = 0;
	#endif
#endif

#ifdef FILTER_DEBUG
	double compPitch, compRoll;
#endif

int main (void) {
	system_initialize();

	sensfusion6Init();
	AttituteAdjustSetDesired(0, 0, 0); // testing this attitude
 	AttitudeSetThrottle(0);

 	#ifdef PID_TIME_TEST
 		leds_init(DIGITAL_PIN_1);
 		led_off(DIGITAL_PIN_1);
 	#endif

	for(;;) {

		#ifdef UART
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

		#ifdef BATTERY_DEBUG
			battery_charged();
			//if (!battery_charged()) {
				/*
				motor_set(MOTOR_ONE, 0);
				motor_set(MOTOR_TWO, 0);
				motor_set(MOTOR_THREE, 0);
				motor_set(MOTOR_FOUR, 0);

				o = 0;
				*/
			//}
		#endif

		#ifdef LED_DEBUG
			#ifdef LEDS
	  			toggle_led(GP_LED2);
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
				#ifdef PID_TIME_TEST
 					led_on(DIGITAL_PIN_1);
 				#endif
				Accel_Update();
				//Mag_Update();

				cli();
				sensfusion6UpdateQ(Gyro_GetX(), Gyro_GetY(), Gyro_GetZ(), Accel_GetX(), Accel_GetY(), Accel_GetZ(), IMU_UPDATE_PERIOD_SECONDS);
				sensfusion6GetEulerRPY(&ypr[2], &ypr[1], &ypr[0]);
				
				if (((abs(ypr[1]) - abs(last_ypr[1])) / IMU_UPDATE_PERIOD_SECONDS) > 400) {
					pitch_static_count++;

					if (pitch_static_count > (int)IMU_UPDATE_RATE * .5) {
						last_ypr[1] = ypr[1];
						pitch_static_count = 0;
					} else {
						ypr[1] = last_ypr[1];
						pitch_static_count = 0;
					}

				} else {
					last_ypr[1] = ypr[1];
					pitch_static_count = 0;
				}

				if (((abs(ypr[2]) - abs(last_ypr[2])) / IMU_UPDATE_PERIOD_SECONDS) > 400) {
					roll_static_count++;

					if (roll_static_count > (int)IMU_UPDATE_RATE * .1) {
						last_ypr[2] = ypr[2];
						roll_static_count = 0;
					} else {
						ypr[2] = last_ypr[2];
						roll_static_count = 0;
					}
				} else {
					last_ypr[2] = ypr[2];
					roll_static_count = 0;
				}

				sei();

				clear_tick_timer_flag(IMU_TIMER_ID);
			} else {
				#ifdef PID_TIME_TEST
 					led_off(DIGITAL_PIN_1);
 				#endif
			}
		

			if(tick_timer_flag(PID_TIMER_ID)) {

				#ifdef LEDS
		  			toggle_led(GP_LED1);
			  	#endif



				if (ref_init == 0) {
					AttituteAdjustSetDesired(0, 0, 0); // testing this attitude
					ref_init = 1;
				}

				AttituteAdjustUpdatePID(0, ypr[1], 0);

				// Update Motors
				AttitudeAdjustGetError(motor_delta);

				if (o == 1) {
					AttitudeAdjustSetActuation(motor_delta);
				}

				// Debugging
				if (abs(ypr[1]) > 45 || abs(ypr[2]) > 45) {
					motor_set(MOTOR_ONE, 0);
					motor_set(MOTOR_TWO, 0);
					motor_set(MOTOR_THREE, 0);
					motor_set(MOTOR_FOUR, 0);
					o = 0;
				}

				#ifdef PID_PRINT_DEBUG
					if(pid_print_flag == 10){
						sprintf(testing, " \nBATT: {%d} | ", (int)battery_get_voltage());
						UART_SendString(testing);
						sprintf(testing, " TEMP: {%lf}\n", Temperature_Get());
						UART_SendString(testing);
						
						sprintf(testing, " \nY: {%lf} | ", ypr[0]);
						UART_SendString(testing);
						sprintf(testing, " P: {%lf} | ", ypr[1]);
						UART_SendString(testing);
						sprintf(testing, " R: {%lf}          ", ypr[2]);
						UART_SendString(testing);

						sprintf(testing, "           M1: {%d} | ", (int)motor_get_speed(MOTOR_ONE));
						UART_SendString(testing);
						sprintf(testing, " M2: {%d} | ", (int)motor_get_speed(MOTOR_TWO));
						UART_SendString(testing);
						sprintf(testing, " M3: {%d} | ", (int)motor_get_speed(MOTOR_THREE));
						UART_SendString(testing);
						sprintf(testing, " M4: {%d}          ", (int)motor_get_speed(MOTOR_FOUR));
						UART_SendString(testing);
					/*
						sprintf(testing, "           M1: {%d} | ", (int)motor_delta[0]);
						UART_SendString(testing);
						sprintf(testing, " M2: {%d} | ", (int)motor_delta[1]);
						UART_SendString(testing);
						sprintf(testing, " M3: {%d} | ", (int)motor_delta[2]);
						UART_SendString(testing);
						sprintf(testing, " M4: {%d}          ", (int)motor_delta[3]);
						UART_SendString(testing);
					*/
						pid_print_flag = 0;
					
					}
					else pid_print_flag ++;
				#endif

				clear_tick_timer_flag(PID_TIMER_ID);
			}
			#ifdef PID_TIME_TEST
					led_off(DIGITAL_PIN_1);
			#endif
		#endif // PID_DEBUG

		#ifdef FILTER_DEBUG
			if(PIDGetFlag() == 1) {
				//Gyro_Update();
				Accel_Update();

				CompAccelUpdate(Accel_GetX(), Accel_GetY(), Accel_GetZ());
				//CompGyroUpdate(Gyro_GetX(), Gyro_GetY(), Gyro_GetZ());
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
				//Mag_Update();


				#ifdef LEDS
					toggle_led(GP_LED1);
				#endif

				cli();
				sensfusion6UpdateQ(Gyro_GetX(), Gyro_GetY(), Gyro_GetZ(), Accel_GetX(), Accel_GetY(), Accel_GetZ(), PID_UPDATE_PERIOD_SECONDS);
				sensfusion6GetEulerRPY(&ypr[2], &ypr[1], &ypr[0]);
				//imu2euler_simple(ypr, Accel_GetX(), Accel_GetY(), Accel_GetZ(), 0, 0);
				sei();

				// Debugging
				if(pid_print_flag == 1){
					sprintf(testing, " \nYaw: {%lf} | ", ypr[0]);
					UART_SendString(testing);
					sprintf(testing, " P: {%lf} | ", ypr[1]);
					UART_SendString(testing);
					sprintf(testing, " R: {%lf}          ", ypr[2]);
					UART_SendString(testing);
					pid_print_flag = 0;
				}
				else pid_print_flag ++;

		#endif

	}

}
