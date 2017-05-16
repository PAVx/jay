/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "system_init.h"
#include "system.h"

uint8_t system_initialize(void) {
	// initialize protcols

	char print_buffer[30];

	#ifdef SYSTEM_TICK
	#endif

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
	//	packet_init();

		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			_uart_driver_FlushTransmitBuffer();
			UART_SendString(" communication substrate initialized...\n");
		#endif

	#endif


	#ifdef GYRO
		InitializeGyro();
		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			_uart_driver_FlushTransmitBuffer();
			UART_SendString(" i2c initialized...\n");
			_uart_driver_FlushTransmitBuffer();
			UART_SendString(" gyroscope initialized...\n");
			_uart_driver_FlushTransmitBuffer();
		#endif
	#endif

	#ifdef ACCEL
		InitializeAccel();
		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			#ifndef GYRO
				_uart_driver_FlushTransmitBuffer();
				UART_SendString(" i2c initialized...\n");
			#endif
			UART_SendString("accel initialized...\n");
		#endif
	#endif

	#ifdef MAGNOMETER
		InitializeMag();
		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			#ifndef GYRO
				_uart_driver_FlushTransmitBuffer();
				UART_SendString("i2c initialized...\n");
			#endif
			UART_SendString("magnometer initialized...\n");
		#endif
	#endif

	// initialize system components
	#ifdef LEDS
		leds_init(SYSTEM_LED);
		leds_init(GP_LED1);
		leds_init(GP_LED2);

		led_on(SYSTEM_LED);
		led_on(GP_LED1);
		led_off(GP_LED2);

		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			_uart_driver_FlushTransmitBuffer();
			UART_SendString("system leds initialized...\n");
		#endif
	#endif

	// PID initialize
	#ifdef PID_CONTROLLER
		InitializeAttitudeAdjust();
	#endif

	sei();

	#ifdef UART
		UART_SendString("\n\nPAVx Jay UAV System v3.0\n\n");
		sprintf(print_buffer, " DEVICE ADDRESS %d IS ONLINE\n\n", DEVICE_ADDRESS);
		UART_SendString(print_buffer);
	#endif

	return TRUE;
}
