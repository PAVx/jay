#include "system_init.h"
#include "system.h"

//#define GYRO (1)
#define COM (1)
#define MOTORS (1)
#define SYSTEM_TICK (1)

uint8_t system_initialize(void) {
	// initialize protcols 
	#ifdef GYRO
		InitializeGyro();
	#endif
	
	#ifdef COM
		#define UART (1)
		InitializeUART(HW_UART_BAUD);
		packet_init();
	#endif

	#ifdef MOTORS
		motors_initialize();
	#endif

	// initialize system components
	#ifdef SYSTEM_TICK
		clock_init();
	#endif

	#ifdef LEDS
		led_init();
	#endif

	#ifdef UART
		UART_SendString("PAVx Jay UAV initialized\n\n");
	#endif 

	return TRUE;
}
