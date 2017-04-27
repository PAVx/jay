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
			_uart_driver_FlushTransmitBuffer();
			UART_SendString("accel initialized...\n");
		#endif
	#endif


	#ifdef MOTORS
		motors_initialize();
		#ifdef SYSTEM_INIT_DEBUG_PRINTOUTS
			_uart_driver_FlushTransmitBuffer();
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
			_uart_driver_FlushTransmitBuffer();
			UART_SendString("system leds initialized...\n");
		#endif
	#endif

	sei();

	return TRUE;
}
