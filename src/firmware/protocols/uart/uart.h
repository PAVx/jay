/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#include <stdbool.h>

#include "uart_driver.h"

#ifndef NUL
#define NUL ('\0')
#endif

void InitializeUART(uint32_t baudrate);

// only as fast as physical layer
void UART_SendByte(uint8_t data);

// returns NUL if no byte available
char UART_GetByte(void);

// returns TRUE if UART buffer is empty
bool UART_IsEmpty(void);

void UART_FlushReceiveBuffer(void);

// send a buffer over UART TX
// Returns TRUE on success
bool UART_SendBuffer(char *buffer, uint16_t nBytes);

// sends a string over UART
bool UART_SendString(char* string);
