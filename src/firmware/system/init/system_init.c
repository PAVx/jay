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
		//clock_init();
	#endif

	#ifdef GYRO
		InitializeGyro();
	#endif

	#ifdef ACCEL
		InitializeAccel();
	#endif

	#ifdef COM
		#ifdef SW_UART
			softuart_init();
		#endif

		#ifdef UART
			InitializeUART(HW_UART_BAUD);
		#endif
	//	packet_init();
	#endif

	#ifdef MOTORS
		motors_initialize();
	#endif

	// initialize system components
	#ifdef LEDS
		leds_init(SYSTEM_LED);
	#endif

	sei();

	return TRUE;
}
