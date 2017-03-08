/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdint.h>

#define SCAN_COMMAND (0x01)
#define LAND_COMMAND (0x02) 

struct _coordinates {
	double longitude;
	double latitude;
}; // 16B
typedef struct _coordinates Coordinates_t;

struct _command_packet {
	uint8_t command_type;

	// these coordinates describe the outer area to scan

/*	p1				p2
	  *------------*
	  |			   |
	  |	SCAN  AREA |
	  |			   |
	  *------------*
	p3				p4	
*/
	Coordinates_t p1; 
	Coordinates_t p2;
	Coordinates_t p3;
	Coordinates_t p4;
};
typedef struct _command_packet CommandPacket_t;


// define packet and pos
#define COMMAND_PACKET_TYPE (2)
#define COMMAND_PACKET_DATA_LENGTH (34) // 1B (command type) + 4*16B (coordinates)

// Packet Receiver/Injection/Parsing Functions
void inititialize_command_packet(void);
uint8_t command_packet_handler(uint8_t *input_buffer);

#endif