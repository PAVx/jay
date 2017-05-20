#include "init_packet.h"

/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	May 2017
*/ 

#include "packet.h"
#include "system.h"

#define SOURCE_ADDRESS_DATA_POS (0)
#define DESTINATION_ADDRESS_DATA_POS (1)
#define ACK_DATA_POS (2)

#define NACK_BYTE (0x12)
#define ACK_BYTE (0x77)

static bool _system_acknowledged_by_ground_station = false;

void inititialize_init_packet(void) {
	uint8_t val = 0;

	_system_acknowledged_by_ground_station = false;

	input_packet_type(INIT_PACKET_TYPE, 
		INIT_PACKET_DATA_LENGTH, TRUE, &init_packet_handler);

	val = DEVICE_ADDRESS;
	packet_data_inject(INIT_PACKET_TYPE, SOURCE_ADDRESS_DATA_POS, 1, &val);

	val = BROADCAST_ALL_DESTINATION_ADDRESS;
	packet_data_inject(INIT_PACKET_TYPE, DESTINATION_ADDRESS_DATA_POS, 1, &val);

	val = NACK_BYTE;
	packet_data_inject(INIT_PACKET_TYPE, ACK_DATA_POS, 1, &val);

}	

uint8_t init_packet_handler(uint8_t *input_buffer) {
	if ((input_buffer[DESTINATION_ADDRESS_DATA_POS] == BROADCAST_ALL_DESTINATION_ADDRESS) ||
		(input_buffer[DESTINATION_ADDRESS_DATA_POS] == DEVICE_ADDRESS)) {
		
		if (input_buffer[SOURCE_ADDRESS_DATA_POS] != GROUND_STATION_ADDRESS) {
			return TRUE; // packet received successfully, but its irrelevent to this device
		}

		if (input_buffer[ACK_DATA_POS] == ACK_BYTE) {
			system_set_ack();
		}		
	}
	return TRUE;
}

void send_initialization_packet(void) {
	packet_send(INIT_PACKET_TYPE);
}

void system_set_ack(void) {
	_system_acknowledged_by_ground_station = true;
}

bool system_acknowledged(void) {
	return _system_acknowledged_by_ground_station;
}
