/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#include "status.h"
#include "packet.h"
#include "system.h"

static StatusPacket_t statuses[DEFAULT_NUM_SIBLINGS + 1];
static StatusPacket_t this_status_packet;

#define SOURCE_ADDRESS_DATA_POS (0)
#define DESTINATION_ADDRESS_DATA_POS (1)
#define LONGITUDE_DATA_POS (2)
#define LATITUDE_DATA_POS (10)
#define TIME_DATA_POS (18)
#define STATUS_DATA_POS (26)

void GetSiblingStatus(StatusPacket_t *data, uint8_t sibling_device_address) {
	if ((sibling_device_address < DEFAULT_NUM_SIBLINGS) &&
		(data != NULL)) {
		*data = statuses[sibling_device_address]; // copy to make memory coherent
	}
}

void inititialize_status_packet(void) {
	uint8_t val = 0;

	this_status_packet.longitude = 0.0;
	this_status_packet.latitude = 0.0;
	this_status_packet.time = 0;
	this_status_packet.status = 0;

	input_packet_type(STATUS_PACKET_TYPE, 
		STATUS_PACKET_DATA_LENGTH, TRUE, &status_packet_handler);

	val = DEVICE_ADDRESS;
	packet_data_inject(STATUS_PACKET_TYPE, SOURCE_ADDRESS_DATA_POS, 1, &val);

	val = BROADCAST_ALL_DESTINATION_ADDRESS;
	packet_data_inject(STATUS_PACKET_TYPE, DESTINATION_ADDRESS_DATA_POS, 1, &val);
}	

uint8_t status_packet_handler(uint8_t *input_buffer) {
	uint8_t _source_address = 0;
	uint64_t bit_buffer = 0x0000000000000000;

	if ((input_buffer[DESTINATION_ADDRESS_DATA_POS] == BROADCAST_ALL_DESTINATION_ADDRESS) ||
		(input_buffer[DESTINATION_ADDRESS_DATA_POS] == DEVICE_ADDRESS)) {
		
		_source_address = input_buffer[SOURCE_ADDRESS_DATA_POS];
		if (_source_address > DEFAULT_NUM_SIBLINGS) {
			return TRUE; // packet received successfully, but its irrelevent to this device
		}

		parse_sub_packet(&bit_buffer, 8, input_buffer, LONGITUDE_DATA_POS);
		statuses[_source_address].longitude = binary_to_double(bit_buffer);

		parse_sub_packet(&bit_buffer, 8, input_buffer, LATITUDE_DATA_POS);
		statuses[_source_address].latitude = binary_to_double(bit_buffer);

		parse_sub_packet(&(statuses[_source_address].time), 8, input_buffer, TIME_DATA_POS);
		parse_sub_packet(&(statuses[_source_address].status), 8, input_buffer, STATUS_DATA_POS);
	}
	return TRUE;
}

void send_status_packet(uint8_t destination_address) {
	packet_data_inject(STATUS_PACKET_TYPE, DESTINATION_ADDRESS_DATA_POS, 1, &destination_address);
	
	packet_send(STATUS_PACKET_TYPE);
}

void status_update_longitude(double longitude) {
	uint8_t buff[8];
	uint64_t binary_double = 0;
	uint8_t i = 0;

	memcpy(&binary_double, &longitude, sizeof(double));


	for (i = 0; i < 8; i++) {
		buff[7 - i] = (uint8_t)((binary_double >> (i * 8)) & 0x00000000000000FF);
	}

	packet_data_inject(STATUS_PACKET_TYPE, LONGITUDE_DATA_POS, 8, buff);
}

void status_update_latitude(double latitude) {
	uint8_t buff[8];
	uint64_t binary_double = 0;
	uint8_t i = 0;

	memcpy(&binary_double, &latitude, sizeof(double));

	for (i = 0; i < 8; i++) {
		buff[7 - i] = (uint8_t)((binary_double >> (i * 8)) & 0x00000000000000FF);
	}

	packet_data_inject(STATUS_PACKET_TYPE, LATITUDE_DATA_POS, 8, buff);
}
void status_update_time(uint64_t time) {
	uint8_t buff[8];
	uint8_t i = 0;

	for (i = 0; i < 8; i++) {
		buff[7 - i] = (uint8_t)((time >> (i * 8)) & 0x00000000000000FF);
	}
	
	packet_data_inject(STATUS_PACKET_TYPE, TIME_DATA_POS, 8, buff);
}

void status_update_status_vector(uint64_t status) {
	uint8_t buff[8];
	uint8_t i = 0;

	for (i = 0; i < 8; i++) {
		buff[7 - i] = (uint8_t)((status >> (i * 8)) & 0x00000000000000FF);
	}

	packet_data_inject(STATUS_PACKET_TYPE, STATUS_DATA_POS, 8, buff);
}


// packet handler/injection/parsing