#include "system.h"

#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#define SOURCE_ADDRESS_START_POS (0)
#define DESTINATION_ADDRESS_START_POS (1)
#define LONGITUDE_BYTE_START_POS (2)
#define LATITUDE_BYTE_START_POS (LONGITUDE_BYTE_START_POS + 1)
#define TIME_BYTE_START_POS (LATITUDE_BYTE_START_POS + 1)
#define STATUS_BYTE_START_POS (TIME_BYTE_START_POS + 1)

#define BROADCAST_ALL_ADDRESS (0xAA)

char gbuffer[128];
char abuffer[128];
char gpsbuffer[50];
void _parse_sub_packet(uint64_t *data_out, uint8_t n, 
	uint8_t *data_buffer, uint8_t start_byte_index) {

	uint8_t parse_count = 0;
	
	*data_out = 0x0000000000000000;

	for (parse_count = 0; parse_count < n; parse_count++) {
		*data_out |= (uint64_t)(data_buffer[start_byte_index + parse_count])  << (8 * parse_count);	

		//printf("BYTE %d: 0x%X\n0x%llX\n", parse_count, data_buffer[start_byte_index + parse_count], *data_out);
	}
}

uint8_t gps_packet_parser(uint8_t *input_buffer) {
	//uint8_t _source_address = 0;
	uint64_t _longitude = 0;
	uint64_t _latitude = 0;
	uint64_t _time = 0;
	uint64_t _status = 0;


	if ((input_buffer[DESTINATION_ADDRESS_START_POS] != BROADCAST_ALL_ADDRESS) &&
		(input_buffer[DESTINATION_ADDRESS_START_POS] != DEVICE_ADDRESS)) {
		//puts("INCORRECT DESTINATION ADDRESS");
		//return TRUE; // packet received successfully, but its irrelevent to this device
	}

	//_source_address = input_buffer[SOURCE_ADDRESS_START_POS];
	_parse_sub_packet(&_longitude, 8, input_buffer, LONGITUDE_BYTE_START_POS);
	_parse_sub_packet(&_latitude, 8, input_buffer, LATITUDE_BYTE_START_POS);
	_parse_sub_packet(&_time, 8, input_buffer, TIME_BYTE_START_POS);
	_parse_sub_packet(&_status, 8, input_buffer, STATUS_BYTE_START_POS);
    
    UART_SendString("PARSED\n");
    
	//printf("PARSED::\nSOURCE:%d\nLONGITUDE: %lf\nLATITUDE: %llu\nTIME: %llu\nSTATUS: %llu\n", _source_address, binary_to_double(_longitude), _latitude, _time, _status);

	return TRUE;
}



int main (void) {
	system_initialize();
	softuart_init();
	led_off(SYSTEM_LED);
    //need to set compare values first to enable
    set_tick_period(0, 1000);
    set_tick_period(1, 200);
    //then start the timer
    clock_start(); 	
	input_packet_type(1, 8, TRUE, &gps_packet_parser);
    #ifdef UART
		UART_SendString("\nPAVx Jay UAV initialized\n\n");
	#endif
	#ifdef SW_UART
		//softuart_puts("\nPavx", 0);
	#endif

	while(1) {

  	#ifdef COM
  		#ifdef UART
			packet_receiver();
			#endif
  	// packet_send();
  	#endif
        #ifdef GPS
            NEO6M_GetChar();
        #endif



		if (tick_timer_flag(1)) { //read imu every 200ms

            clear_tick_timer_flag(1);
			#ifdef GYRO
  			Gyro_Update();
  		#endif

  		#ifdef ACCEL
  			Accel_Update();
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
                   /* 
                    struct tm time = GPS_GetTime();
                    sprintf(gpsbuffer, "T: %02d:%02d:%02d\nL: %.0f\nL: %.0f\nS: %.2f\nA: %.2f\n", time.tm_hour, time.tm_min, time.tm_sec, GPS_GetLatitude(), GPS_GetLongitude(), GPS_GetSpeed(), GPS_GetAltitude());
                    int i = 0;
                    while (gpsbuffer[i] != '\0') {
                        _uart_driver_SendByte(gpsbuffer[i]);
                        i++;
                    }
                 */   
                }

        }
               
        if(tick_timer_flag(0)) { // Do something every 1sec
            clear_tick_timer_flag(0);
            UART_SendByte('1');
            /*
            struct tm time = GPS_GetTime();
            sprintf(gpsbuffer, "T: %02d:%02d:%02d\nL: %.0f\nL: %.0f\nS: %c\nA: %.2f\n", time.tm_hour, time.tm_min, time.tm_sec, GPS_GetLatitude(), GPS_GetLongitude(), GPS_GetStatus(), GPS_GetAltitude());
            int i = 0;
            while (gpsbuffer[i] != '\0') {
                _uart_driver_SendByte(gpsbuffer[i]);
                i++;
            }
            */
            //char buffer[20];
            //sprintf(buffer, "Hi:%f\n", (double)clock_time());
            //UART_SendString(buffer);

            //UART_SendString("Testing\n");
        }
                    
            #endif

			#ifdef UART
  		 		//UART_SendString("\nHW_UART PRINT\n\r");
  		#endif

			#ifdef SW_UART
		 		//softuart_puts("S\n",0);
			#endif

		}

		//softuart_puts("sup\n",0);
		//UART_SendString("\nHW_UART PRINT\n\r");
		// todo:
		// update accel registers
		// update gps registers
		// update PID controller for stabality
	}
}
