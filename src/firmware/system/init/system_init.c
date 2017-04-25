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
	
		#ifdef UART
				UART_SendString("system clock initialized...\n");
				UART_SendString("HW UART communication substrate initialized...\n");
				UART_SendString("SW UART communication substrate initialized...\n");
		#endif

	#endif


	#ifdef GYRO
		InitializeGyro();
		#ifdef UART
			UART_SendString("i2c initialized...\n");
			UART_SendString("gyroscope initialized...\n");
		#endif
	#endif

	#ifdef ACCEL
		InitializeAccel();
		#ifdef UART
			#ifndef GYRO
				UART_SendString("i2c initialized...\n");
			#endif
			UART_SendString("accel initialized...\n");
		#endif
	#endif


	#ifdef MOTORS
		motors_initialize();
		#ifdef UART
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
		
		#ifdef UART
			UART_SendString("system leds initialized...\n");
		#endif
	#endif

	sei();

	#ifdef UART
		UART_SendString("system interrupts initialized...\n");
	#endif

	#ifdef UART
		UART_SendString("system bootloader initialized...\n");
		UART_SendString("system starting up...\n");
	#endif
	return TRUE;
}
