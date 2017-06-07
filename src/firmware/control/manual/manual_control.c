/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga, Sargis S Yonan
	March 2017
*/

#include "manual_control.h"
#include "packet.h"

static ManualControlPacket_t _references;
static uint8_t reference_given = FALSE;


#define SOURCE_ADDRESS_DATA_POS (0)
#define DESTINATION_ADDRESS_DATA_POS (1)
#define YAW_DATA_POS (2)
#define PITCH_DATA_POS (4)
#define ROLL_DATA_POS (6)
#define THROTTLE_DATA_POS (8)

#define VALID_SOURCE_ADDRESS (0x01)

// Packet Receiver/Injection/Parsing Functions
void inititialize_manual_control_packet(void) {
	reference_given = FALSE;

	_references.yaw = 0;
	_references.pitch = 0;
	_references.roll = 0;
	_references.throttle = 0;
}
uint8_t manual_control_packet_handler(uint8_t *input_buffer) {
	uint8_t _source_address = 0;
	uint64_t data_out = 0x0000000000000000;

	if ((input_buffer[DESTINATION_ADDRESS_DATA_POS] == BROADCAST_ALL_DESTINATION_ADDRESS) ||
		(input_buffer[DESTINATION_ADDRESS_DATA_POS] == DEVICE_ADDRESS)) {

		_source_address = input_buffer[SOURCE_ADDRESS_DATA_POS];
		if (_source_address != VALID_SOURCE_ADDRESS) {
			return TRUE; // packet received successfully, but its irrelevent to this device
		}

		parse_sub_packet(&data_out, 2, input_buffer, YAW_DATA_POS);
		_references.yaw = (int16_t)data_out;

		parse_sub_packet(&data_out, 2, input_buffer, PITCH_DATA_POS);
		_references.pitch = (int16_t)data_out;

		parse_sub_packet(&data_out, 2, input_buffer, ROLL_DATA_POS);
		_references.roll = (int16_t)data_out;

		parse_sub_packet(&data_out, 1, input_buffer, THROTTLE_DATA_POS);
		_references.throttle = (uint8_t)data_out;

		// PID SET WITH _references struct
		reference_given = TRUE;
	}
	return TRUE;
}
