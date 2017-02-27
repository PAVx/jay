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
char shit_buff[2];

typedef enum {
		SOP,
		MOTOR_NUMBER,
		DUTY_CYCLE,
		EOP = 0x04 // ASCII EOT
} MotorPacketFSM_t;
MotorPacketFSM_t m_state = SOP;
char current_mbyte = '\0';
uint8_t manipulate_motor_number = 0;
uint8_t manipulate_duty_cycle = 0;

int main (void) {
	system_initialize();
	

	#ifdef UART
		UART_SendString("\nPAVx Jay UAV initialized\n\n");
	#endif
	#ifdef SW_UART
		softuart_puts("\nPAVx Jay UAV initialized -- SWUART", 0);
	#endif
	//sprintf(gbuffer, "T0P = %f", (double)TIMER0_PERIOD);
	//softuart_puts(gbuffer, 0);



  	while(1) {

    	#ifdef COM
    		#if SW_UART && UART
  				if (!UART_IsEmpty()) {
  					current_mbyte = UART_GetByte();
  					sprintf(shit_buff, "%c", current_mbyte);

  					if (current_mbyte == '1') manipulate_motor_number = 1;
  					if (current_mbyte == '2') manipulate_motor_number = 2;
  					if (current_mbyte == '3') manipulate_motor_number = 3;
  					if (current_mbyte == '4') manipulate_motor_number = 4;
  					if (current_mbyte == '5') manipulate_motor_number = 5;
  					
  					if (current_mbyte == 'k') {
  						manipulate_duty_cycle = 0;
  				  		motor_set(1, manipulate_duty_cycle);
  						motor_set(2, manipulate_duty_cycle);
  						motor_set(3, manipulate_duty_cycle);
  						motor_set(4, manipulate_duty_cycle);  	
  					}

  					if (current_mbyte == 'u') {
  						if (manipulate_duty_cycle >= 250) manipulate_duty_cycle = 250;
  					  	manipulate_duty_cycle += 5;
  					  	if (manipulate_motor_number == 5) {
  					  		motor_set(1, manipulate_duty_cycle);
  							motor_set(2, manipulate_duty_cycle);
  							motor_set(3, manipulate_duty_cycle);
  							motor_set(4, manipulate_duty_cycle);  		
  					  	} else {
							motor_set(manipulate_motor_number, manipulate_duty_cycle);
  						}
  					}
  					else if (current_mbyte == 'd') {
  						if (manipulate_duty_cycle <= 6) manipulate_duty_cycle = 5;
  						manipulate_duty_cycle -= 5;
  					  	if (manipulate_motor_number == 5) {
  					  		motor_set(1, manipulate_duty_cycle);
  							motor_set(2, manipulate_duty_cycle);
  							motor_set(3, manipulate_duty_cycle);
  							motor_set(4, manipulate_duty_cycle);  		
  					  	} else {
							motor_set(manipulate_motor_number, manipulate_duty_cycle);
  						}  					}
#if 0
  					switch(m_state) {
  						case SOP:
  							if (current_mbyte <= 5) {
  								manipulate_motor_number = (uint8_t)current_mbyte;
  								m_state = MOTOR_NUMBER;
  							}
  							break;

  						case MOTOR_NUMBER:
  							manipulate_duty_cycle = (uint8_t)current_mbyte;
  							m_state = DUTY_CYCLE;
  							break;

  						case DUTY_CYCLE:
  							if (current_mbyte != (char)EOP) {
  								m_state = SOP;
  							} else {
  								m_state = EOP;
  							}
  							break;

  						case EOP:
  							if (manipulate_motor_number == 5) {
  								motor_set(1, manipulate_duty_cycle);
  								motor_set(2, manipulate_duty_cycle);
  								motor_set(3, manipulate_duty_cycle);
  								motor_set(4, manipulate_duty_cycle);
  							} else {
  								motor_set(manipulate_motor_number, manipulate_duty_cycle);
  							}

  							m_state = SOP;
  							manipulate_motor_number = 0;
  							manipulate_duty_cycle = 0;
  							break;

  						default:
  							m_state = SOP;
  							break;
  					}
  	#endif
  				}
			#endif
	    	// receive_packet();
    		// packet_send();
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
  		 		//UART_SendString("\nHW_UART PRINT\n\r");
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
