/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "command.h"

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

static CommandPacket_t command_packet;
// define packet and pos
#define COMMAND_PACKET_TYPE (2)
#define COMMAND_PACKET_DATA_LENGTH (34) // 1B (command type) + 4*16B (coordinates)

// Packet Receiver/Injection/Parsing Functions
void inititialize_command_packet(void) {
	command_packet.command_type = LAND_COMMAND;

	command_packet.p1.longitude = 0.0;
	command_packet.p1.latitude = 0.0;

	command_packet.p2.longitude = 0.0;
	command_packet.p2.latitude = 0.0;

	command_packet.p3.longitude = 0.0;
	command_packet.p3.latitude = 0.0;

	command_packet.p4.longitude = 0.0;
	command_packet.p4.latitude = 0.0;
}

uint8_t command_packet_handler(uint8_t *input_buffer) {
	parse_sub_packet(&(command_packet.command_type), 1, input_buffer, COMMAND_TYPE_DATA_POS);

	if (command_packet.command_type == LAND_COMMAND) {
		// if (flying()) {
			// go_home();
			// 
		// }
		return TRUE;
	}

	// parse_sub_packet(&(bit_buffer), sizeof(Coordinates_t), input_buffer, P1_DATA_POS);
	// parse_sub_packet(&(bit_buffer), sizeof(Coordinates_t), input_buffer, P2_DATA_POS);
	// parse_sub_packet(&(bit_buffer), sizeof(Coordinates_t), input_buffer, P3_DATA_POS);
	// parse_sub_packet(&(bit_buffer), sizeof(Coordinates_t), input_buffer, P4_DATA_POS);

	// if (input_buffer[command_type] == SCAN_COMMAND)
	// plan_route(command_packet)
	// else chill();
}

#endif