/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef _STATUS_H_
#define _STATUS_H_

#include <stdint.h>

struct _status_packet {
	double longitude;
	double latitude;
	uint64_t time;
	uint64_t status;
};
typedef struct _status_packet StatusPacket_t;


// define packet and pos
#define STATUS_PACKET_TYPE (1)
#define STATUS_PACKET_DATA_LENGTH (26) // 2*4 (GPS) + 2*8 (STATUS AND TIME DATA) + 1 (SOURCE) + 1 (DESTINATION) 

void GetSiblingStatus(StatusPacket_t *data, uint8_t sibling_device_address);

// Packet Receiver/Injection/Parsing Functions
void inititialize_status_packet(void);
uint8_t status_packet_handler(uint8_t *input_buffer);

// use destination_address = BROADCAST_ALL_ADDRESS to send to entire pod
void send_status_packet(uint8_t destination_address);

void status_update_longitude(double longitude);
void status_update_latitude(double latitude);
void status_update_time(uint64_t time);
void status_update_status_vector(uint64_t status);

#endif