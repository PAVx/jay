/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Sargis S Yonan
	March 2013
*/

#include "ctrl_pkt.h"
#include "packet.h"
#include "system.h"

#define SOURCE_ADDRESS_DATA_POS (0)
#define DESTINATION_ADDRESS_DATA_POS (1)
#define COMMAND_BYTE_POS (2)
#define LEFT_DATA_POS (3)
#define RIGHT_DATA_POS (LEFT_DATA_POS + 4)
#define TOP_DATA_POS (RIGHT_DATA_POS + 4)
#define BOTTOM_DATA_POS (TOP_DATA_POS + 4)

void inititialize_control_packet(void) {
	uint8_t val = 0;

	input_packet_type(CONTROL_PACKET_TYPE,
		CONTROL_PACKET_DATA_LENGTH, TRUE, &control_packet_handler);

	val = DEVICE_ADDRESS;
	packet_data_inject(CONTROL_PACKET_TYPE, SOURCE_ADDRESS_DATA_POS, 1, &val);

	val = BROADCAST_ALL_DESTINATION_ADDRESS;
	packet_data_inject(CONTROL_PACKET_TYPE, DESTINATION_ADDRESS_DATA_POS, 1, &val);
}

uint8_t control_packet_handler(uint8_t *input_buffer) {
	uint8_t _source_address = 0;
	//uint64_t bit_buffer = 0;
	double parsed_val = 0;

	if ((input_buffer[DESTINATION_ADDRESS_DATA_POS] == BROADCAST_ALL_DESTINATION_ADDRESS) ||
		(input_buffer[DESTINATION_ADDRESS_DATA_POS] == DEVICE_ADDRESS)) {

		_source_address = input_buffer[SOURCE_ADDRESS_DATA_POS];
		if (_source_address > DEFAULT_NUM_SIBLINGS) {
			return TRUE; // packet received successfully, but its irrelevent to this device
		}

		//controls[_source_address].command = input_buffer[COMMAND_BYTE_POS];

		memcpy(&(parsed_val), &input_buffer[LEFT_DATA_POS], 4);
		memcpy(&(parsed_val), &input_buffer[RIGHT_DATA_POS], 4);
		memcpy(&(parsed_val), &input_buffer[TOP_DATA_POS], 4);
		memcpy(&(parsed_val), &input_buffer[BOTTOM_DATA_POS], 4);
	}
	return TRUE;
}

/*
void send_control_packet(uint8_t destination_address) {
	packet_data_inject(CONTROL_PACKET_TYPE, DESTINATION_ADDRESS_DATA_POS, 1, &destination_address);
	packet_send(CONTROL_PACKET_TYPE);
}

void control_update_command(uint8_t command) {

    packet_data_inject(CONTROL_PACKET_TYPE, COMMAND_BYTE_POS, 1, &command);
}

void control_update_left(double left) {
	uint8_t buff[4];
	uint64_t binary_double = 0;
	uint8_t i = 0;

	memcpy(&binary_double, &left, sizeof(double));


	for (i = 0; i < 4; i++) {
		buff[i] = (uint8_t)((binary_double >> (i * 8)) & 0x000000FF);
	}

	packet_data_inject(CONTROL_PACKET_TYPE, LEFT_DATA_POS, 4, buff);
}

void control_update_right(double right) {
	uint8_t buff[4];
	uint64_t binary_double = 0;
	uint8_t i = 0;

	memcpy(&binary_double, &right, sizeof(double));

	for (i = 0; i < 4; i++) {
		buff[i] = (uint8_t)((binary_double >> (i * 8)) & 0x000000FF);
	}

	packet_data_inject(CONTROL_PACKET_TYPE, RIGHT_DATA_POS, 4, buff);
}
void control_update_top(double top) {
	uint8_t buff[4];
	uint64_t binary_double = 0;
	uint8_t i = 0;

	memcpy(&binary_double, &top, sizeof(double));
	for (i = 0; i < 4; i++) {
		buff[i] = (uint8_t)((binary_double >> (i * 8)) & 0x000000FF);
	}

	packet_data_inject(CONTROL_PACKET_TYPE, TOP_DATA_POS, 4, buff);
}

void control_update_bottom(double bottom) {
	uint8_t buff[4];
	uint64_t binary_double = 0;
	uint8_t i = 0;

	memcpy(&binary_double, &bottom, sizeof(double));
	for (i = 0; i < 4; i++) {
		buff[i] = (uint8_t)((binary_double >> (i * 8)) & 0x000000FF);
	}

	packet_data_inject(CONTROL_PACKET_TYPE, BOTTOM_DATA_POS, 4, buff);
}

*/
// packet handler/injection/parsing
