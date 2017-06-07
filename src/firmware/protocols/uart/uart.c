/*
    PAVx -- Pod-Based Autonomous Vehicles
    Library Created By: Niraj Raniga, Sargis S Yonan
    March 2017
*/

#include "uart.h"

#include <string.h>
#include <stdlib.h>

void InitializeUART(uint32_t baudrate)
{
    _uart_driver_Init(baudrate);
}

// only as fast as physical layer
void UART_SendByte(uint8_t data)
{
    _uart_driver_SendByte(data);
}

// returns NUL if no byte available
char UART_GetByte(void)
{
    uint8_t c = NUL;
    if (_uart_driver_ReceiveByte(&c) == TRUE)
    {
        return (char)c;
    }
    return NUL;
}

// returns TRUE if UART buffer is empty
bool UART_IsEmpty(void)
{
    return (_uart_driver_ReceiveBufferIsEmpty() == TRUE);
}

void UART_FlushReceiveBuffer(void)
{
    _uart_driver_FlushReceiveBuffer();
}

// send a buffer over UART TX
// Returns TRUE on success
bool UART_SendBuffer(char *buffer, uint16_t nBytes)
{
    return (_uart_driver_SendBuffer(buffer, nBytes) == TRUE);
}

// sends a string over UART
bool UART_SendString(char* string)
{
    uint16_t n = (uint16_t)strnlen(string, UART_TX_BUFFER_SIZE);
    return (_uart_driver_SendBuffer(string, n));
}
