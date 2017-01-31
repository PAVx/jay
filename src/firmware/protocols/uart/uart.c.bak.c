// UART PROTOCOL DEFINITIONS
// WRITTEN BY: SARGIS S YONAN

#include "uart.h"

#include <string.h>
#include <stdlib.h>

void InitializeUART(uint32_t baudrate, softUART *softUART)
{
  if(softUART == HW)
  {
    _uart_driver_Init(baudrate);
  }
  else
  {
    _softuart_init(softUART);
  }
}

// only as fast as physical layer
void UART_SendByte(uint8_t data, softUART *softUART)
{
  if(softUART == HW)
  {
    _uart_driver_SendByte(data);
  }
  else
  {
    _softuart_putchar((char)data, softUART);
  }
}

// returns NUL if no byte available
char UART_GetByte(softUART *softUART)
{
  uint8_t c = NUL;

  if(softUART == HW)
  {
    if (_uart_driver_ReceiveByte(&c) == TRUE)
    {
        return (char)c;
    }
    return NUL;
  }
  else
  {
    return _softuart_getchar( softUART );
  }
}

// returns TRUE if UART buffer is empty
bool UART_IsEmpty(softUART *softUART)
{
  if(softUART == HW)
  {
    return (_uart_driver_ReceiveBufferIsEmpty() == TRUE);
  }
  else
  {
    return (_softuart_transmit_busy(softUART) == FALSE);
  }
}

void UART_FlushReceiveBuffer(softUART *softUART)
{
  if(softUART == HW)
  {
    _uart_driver_FlushReceiveBuffer();
  }
  else
  {
    _softuart_flush_input_buffer(softUART);
  }
}

// send a buffer over UART TX
// Returns TRUE on success
bool UART_SendBuffer(char *buffer, uint16_t nBytes, softUART *softUART)
{
  if(softUART == HW)
  {
    return (_uart_driver_SendBuffer(buffer, nBytes) == TRUE);
  }
  else
  {
    _softuart_puts(buffer, softUART);
    return TRUE;
  }
}

// sends a string over UART
bool UART_SendString(char* string, softUART *softUART)
{
    if(softUART == HW)
    {
      uint16_t n = (uint16_t)strnlen(string, UART_TX_BUFFER_SIZE);
      return (_uart_driver_SendBuffer(string, n));
    }
    else
    {
      _softuart_puts(string, softUART);
      return TRUE;
    }
}
