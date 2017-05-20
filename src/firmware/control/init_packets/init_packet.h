#ifndef _INIT_PACKETS_H_
#define _INIT_PACKETS_H_

#include "packet.h"
#include <stdint.h>
#include <stdbool.h>

/* 
 * Each drone will send a NACK to the ground station
 * until an ACK is received from the ground station
 */

#define NACK_BYTE (0x12)
#define ACK_BYTE (0x77)

#define INIT_PACKET_TYPE (3)
#define INIT_PACKET_DATA_LENGTH (3) // 1B (ACK) + 2B (SOURCE/DESTINATION)


void inititialize_init_packet(void);
uint8_t init_packet_handler(uint8_t *input_buffer);

void send_initialization_packet(void);
void system_set_ack(void);
bool system_acknowledged(void);

#endif