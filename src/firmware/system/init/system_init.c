/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga, Sargis S Yonan
	March 2017
*/

#include "system_init.h"
#include "system.h"

uint8_t system_initialize(void) {
	char print_buffer[30] = {0};

	// initialize protcols
	#ifdef MOTORS
		motors_initialize();
	#endif

	#ifdef BATTERY
		battery_initialize();
	#endif

	#ifdef COM
		#ifdef SW_UART
			softuart_init();
		#endif

		#ifdef UART
			InitializeUART(HW_UART_BAUD);
		#endif

		#ifdef PACKET
			initialize_packet_handler();
			inititialize_status_packet();
			inititialize_init_packet();
            inititialize_control_packet();

			set_tick_period(PACKET_TIMER_ID, PACKET_UPDATE_TIME_MS);
		#endif

	#endif

	#ifdef IMU

		#if defined(GYRO) || defined(ACCEL) || defined(MAGNOMETER)
			IMU_Init();
			#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
				_uart_driver_FlushTransmitBuffer();
				UART_SendString(" i2c initialized...\n");
				_uart_driver_FlushTransmitBuffer();
				UART_SendString(" imu initialized...\n");
				_uart_driver_FlushTransmitBuffer();
			#endif
		#endif

		set_tick_period(IMU_TIMER_ID, IMU_UPDATE_TIME_US);
	#endif

	// initialize system components
	#ifdef LEDS
		leds_init(SYSTEM_LED);
		leds_init(GP_LED1);
		leds_init(GP_LED2);


		led_on(GP_LED1);
		led_off(GP_LED2);
	#endif

	// PID initialize
	#ifdef PID_CONTROLLER
		InitializeAttitudeAdjust();
		// initialize timer
		set_tick_period(PID_TIMER_ID, PID_TIMER_UPDATE_TIME_US);
	#endif

	#ifdef IR_CAM
		InitializeD6T8L();
	#endif

	clock_start();
	sei();


	#ifdef UART
		UART_SendString("\n\nPAVx Jay UAV System v3.0\n\n");
		sprintf(print_buffer, " DEVICE ADDRESS %d IS ONLINE\n\n", DEVICE_ADDRESS);
		UART_SendString(print_buffer);
	#endif



	return TRUE;
}
