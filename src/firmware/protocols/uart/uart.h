// UART PROTOCOL DECLARATIONS
// WRITTEN BY: SARGIS S YONAN

#include <stdbool.h>

#include "uart_driver.h"
#include "softuart.h"

#ifndef NUL
#define NUL ('\0')
#endif

#define NOT_INITIALIZED 0
#define HW NUL
#define SW 2

void InitializeUART(uint32_t baudrate, softUART *softUART);

// only as fast as physical layer
void UART_SendByte(uint8_t data, softUART *softUART);

// returns NUL if no byte available
char UART_GetByte(softUART *softUART);

// returns TRUE if UART buffer is empty
bool UART_IsEmpty(softUART *softUART);

void UART_FlushReceiveBuffer(softUART *softUART);

// send a buffer over UART TX
// Returns TRUE on success
bool UART_SendBuffer(char *buffer, uint16_t nBytes, softUART *softUART);

// sends a string over UART
bool UART_SendString(char* string, softUART *softUART);
