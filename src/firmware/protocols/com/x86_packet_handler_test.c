// packet_handler_test.c
#define TRUE (1)
#define FALSE (0)

#include <stdint.h>
#include <stdio.h>

uint64_t double_to_binary(double val) {
	unsigned long long *double_as_bin = (unsigned long long *)&val;

	return ((uint64_t)*double_as_bin);
}

double binary_to_double(uint64_t bin) {
	double *bin_as_double = (double *)&bin;

	return ((double)*bin_as_double);
}

#define PRINT_STATE(state, byte_in) printf("\n\nSTATE: %d\nbyte_in: 0x%X\n", state, byte_in)

#define MAX_PACKET_DATA_LENGTH_BYTES (256)

struct _variable_packet_descriptor {
	uint8_t _opcode_set;
	uint8_t constant_packet_data_length;

	// the packet handler function must always 
	// take a uint8_t* argument representing the
	// packet accepted by the packet receiver.
	// MUST return TRUE or FALSE representing
	// the error state of the packet/parser
	uint8_t (*packet_handler_ptr)(uint8_t*);
};

#define MAX_NUMBER_PACKET_TYPES (8)

typedef struct _variable_packet_descriptor PacketDescriptor_t;
PacketDescriptor_t packet_opcodes[MAX_NUMBER_PACKET_TYPES];

void initialize_packet_handler(void);
/*
 * input_packet_type
 * packet_opcode - the number to assign to the opcode. Every time this opcode
 					is detected, this corresponding packet handler function
 					will be called
 * constant_packet_data_length - the const size of the data section 
  					of the packet of this opcode
 * packet_handler_ptr - a function pointer that will handle this packet opcode
 * returns TRUE/FALSE as error state
*/
uint8_t input_packet_type(uint8_t packet_opcode, uint8_t constant_packet_data_length, 
						uint8_t (*packet_handler_ptr)(uint8_t*));

// autonomous function that uses UART to receive one byte at a time,
// determines the packet type, and passes it to the correct handler
void packet_receiver(void);



//////////////////////////////////////////////////////////////
// system.h
#define DEVICE_ADDRESS (0x01)
//////////////////////////////////////////////////////////////
// random_library_h
#define BROADCAST_ALL_ADDRESS (0xAA)

#define SOURCE_ADDRESS_START_POS (0)
#define DESTINATION_ADDRESS_START_POS (1)
#define LONGITUDE_BYTE_START_POS (2)
#define LATITUDE_BYTE_START_POS (LONGITUDE_BYTE_START_POS + 8)
#define TIME_BYTE_START_POS (LATITUDE_BYTE_START_POS + 8)
#define STATUS_BYTE_START_POS (TIME_BYTE_START_POS + 8)

void _parse_sub_packet(uint64_t *data_out, uint8_t n, 
	uint8_t *data_buffer, uint8_t start_byte_index) {

	uint8_t parse_count = 0;
	
	*data_out = 0x0000000000000000;

	for (parse_count = 0; parse_count < n; parse_count++) {
		*data_out |= (uint64_t)(data_buffer[start_byte_index + parse_count]) << (((n - 1)	- parse_count) * 8);	

		printf("BYTE %d: 0x%X\n0x%llX\n", parse_count, data_buffer[start_byte_index + parse_count], *data_out);
	}
}

uint8_t gps_packet_parser(uint8_t *input_buffer) {
	uint8_t _source_address = 0;
	uint64_t _longitude = 0;
	uint64_t _latitude = 0;
	uint64_t _time = 0;
	uint64_t _status = 0;


	if ((input_buffer[DESTINATION_ADDRESS_START_POS] != BROADCAST_ALL_ADDRESS) &&
		(input_buffer[DESTINATION_ADDRESS_START_POS] != DEVICE_ADDRESS)) {
		puts("INCORRECT DESTINATION ADDRESS");
		return TRUE; // packet received successfully, but its irrelevent to this device
	}

	_source_address = input_buffer[SOURCE_ADDRESS_START_POS];
	_parse_sub_packet(&_longitude, 8, input_buffer, LONGITUDE_BYTE_START_POS);
	_parse_sub_packet(&_latitude, 8, input_buffer, LATITUDE_BYTE_START_POS);
	_parse_sub_packet(&_time, 8, input_buffer, TIME_BYTE_START_POS);
	_parse_sub_packet(&_status, 8, input_buffer, STATUS_BYTE_START_POS);


	printf("PARSED::\nSOURCE:%d\nLONGITUDE: %lf\nLATITUDE: %llu\nTIME: %llu\nSTATUS: %llu\n", _source_address, binary_to_double(_longitude), _latitude, _time, _status);

	return TRUE;
}


//////////////////////////////////////////////////////////////
#define SOH_BYTE (0x01)
#define STX_BYTE (0x02)
#define EOT_BYTE (0x04)

typedef enum {
	SOH_STATE = 0x00,
	STX_STATE,
	OPCODE_STATE,
	DATA_STATE,
	EOT_STATE 
} PacketReceiverStates_t;

static uint8_t _packet_handler_initialized = FALSE;

// must be handed to specific function 
// parser immediately after retrieval
// NOT thread safe since the pointer will
// likley be set to the persisently used
// accepting buffer by the packet handler
static uint8_t rx_packet[MAX_PACKET_DATA_LENGTH_BYTES];


void initialize_packet_handler(void) {
	uint8_t i = 0;

	// initializes all empty opcodes
	for(i = 0; i < MAX_NUMBER_PACKET_TYPES; i++) {
		packet_opcodes[i]._opcode_set = FALSE;
		packet_opcodes[i].constant_packet_data_length = 0;
		packet_opcodes[i].packet_handler_ptr = NULL;	
	}

	_packet_handler_initialized = FALSE;
}

uint8_t input_packet_type(uint8_t packet_opcode, uint8_t constant_packet_data_length, 
	uint8_t (*packet_handler_ptr)(uint8_t*)) {


	if (!_packet_handler_initialized) {
		initialize_packet_handler();
	}

	if ((packet_opcode < MAX_NUMBER_PACKET_TYPES) &&
		(constant_packet_data_length > 0) &&
		(packet_handler_ptr)) {

		if (packet_opcodes[packet_opcode]._opcode_set) {
			return FALSE;
		}

		packet_opcodes[packet_opcode].constant_packet_data_length = constant_packet_data_length;
		packet_opcodes[packet_opcode].packet_handler_ptr = packet_handler_ptr;
		packet_opcodes[packet_opcode]._opcode_set = TRUE;

		return TRUE;
	}

	return FALSE;
}

uint8_t _test_byte;

void packet_receiver(void) {
	static PacketReceiverStates_t receiver_state = SOH_STATE;
	static uint8_t byte_index = 0;
	static uint8_t this_opcode = 0;

	static uint8_t error_state = FALSE;
	
	uint8_t byte_in = _test_byte;

	switch (receiver_state) {
		case SOH_STATE:
			if (byte_in == SOH_BYTE) {
				receiver_state = STX_STATE;
			}

			break;

		case STX_STATE:
			if (byte_in == STX_BYTE) {
				receiver_state = OPCODE_STATE;
			} else {
				receiver_state = SOH_STATE;
			}

			break;

		case OPCODE_STATE:
			if (byte_in < MAX_NUMBER_PACKET_TYPES) {
				this_opcode = byte_in;
				receiver_state = DATA_STATE;
			} else {
				receiver_state = SOH_STATE;
			}

			break;

		case DATA_STATE:
			if (byte_index < (MAX_PACKET_DATA_LENGTH_BYTES - 1)) {
				rx_packet[byte_index++] = byte_in;
				
				if (byte_index >= packet_opcodes[this_opcode].constant_packet_data_length) {
					receiver_state = EOT_STATE;

					// reset buffer index
					byte_index = 0;
				}
			}

			break;

		case EOT_STATE:

				if ((((*(packet_opcodes[this_opcode].packet_handler_ptr))(rx_packet))) &&
					(byte_in == EOT_BYTE)) {
					error_state = FALSE;
				} else {
					error_state = TRUE;
				}

				receiver_state = SOH_STATE;
				this_opcode = 0;
			break;

		default:
			receiver_state = SOH_STATE;
			this_opcode = 0;
			byte_index = 0;

			break;
	} 
}




int main (void) {
	uint8_t tx_buffer[38] = {
		0x01, 
		0x02, 
		0x02, 
		0x02, // source
		0x01, // destination

		0xC0, 0x5E, 0x81, 0x47, 0xAE, 0x14, 0x7A, 0xE1, // -122.020000
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, // 8
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, // 8
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, // 8
		0x04
	};

	double longi = 36.9711913;
	double lat = -122.02;
	uint64_t time = 29238112912812;
	uint64_t status = 0b00010101101001;

	input_packet_type(2, 34, &gps_packet_parser);



	for(int i = 0; i < 38; i++) {
		_test_byte = tx_buffer[i];
		packet_receiver();
	}

}
