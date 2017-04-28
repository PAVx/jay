/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2017
*/

#include "system_init.h"
#include "system.h"

uint8_t system_initialize(void) {
	// initialize protcols
	#ifdef SYSTEM_TICK
		clock_init();
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
				UART_SendString("system clock initialized...\n");
				UART_SendString("HW UART communication substrate initialized...\n");
				UART_SendString("SW UART communication substrate initialized...\n");
		#endif

	#endif


	#ifdef GYRO
		InitializeGyro();
		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			UART_SendString("i2c initialized...\n");
			UART_SendString("gyroscope initialized...\n");
		#endif
	#endif

	#ifdef ACCEL
		InitializeAccel();
		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			#ifndef GYRO
				UART_SendString("i2c initialized...\n");
			#endif
			//UART_SendString("accel initialized...\n");
		#endif
	#endif

	#ifdef MAG
		InitializeMag();
		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			#ifndef GYRO
				UART_SendString("i2c initialized...\n");
			#endif
			UART_SendString("magnometer initialized...\n");
		#endif
	#endif


	#ifdef MOTORS
		motors_initialize();
		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			UART_SendString("motor drivers initialized...\n");
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
			UART_SendString("system leds initialized...\n");
		#endif
	#endif

	// PID initialize
	#ifdef PID_CONTROLLER
		InitializeAttitudeAdjust();
	#endif

	sei();

	#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
		UART_SendString("system interrupts initialized...\n");
	#endif

	#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
		UART_SendString("system bootloader initialized...\n");
		UART_SendString("system starting up...\n");
	#endif

	#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
		UART_SendString("\n\nPAVx Jay UAV System v3.0\n\n");
		UART_SendString("\n\n--Device Address: ");
		UART_SendByte(DEVICE_ADDRESS + ASCII_NUMBER_OFFSET);
		UART_SendString("is online\n\n");
	#endif

	return TRUE;
}
