/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef _CONTROL_PACKET_H_
#define _CONTROL_PACKET_H_

#include <stdint.h>

struct _control_packet {
	uint8_t command;
	double left;
	double right;
	double top;
    double bottom;
};
typedef struct _control_packet ControlPacket_t;


// define packet and pos
#define CONTROL_PACKET_TYPE (2)
#define CONTROL_PACKET_DATA_LENGTH (3 + 16) // (src, dest, command) + (border) 


// Packet Receiver/Injection/Parsing Functions
void inititialize_control_packet(void);
uint8_t control_packet_handler(uint8_t *input_buffer);

// use destination_address = BROADCAST_ALL_ADDRESS to send to entire pod
void send_control_packet(uint8_t destination_address);

void control_update_command(uint8_t command);
void control_update_left(double left);
void control_update_right(double right);
void control_update_top(double top);
void control_update_bottom(double bottom);

#endif
