// packet_handler_test.c
#define TRUE (1)
#define FALSE (0)

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

	// malloced to a size of constant_packet_data_length
	// this is the transmit buffer for this packet opcode
	// the space for this buffer is only malloced if 
	uint8_t *tx_buffer;
	uint8_t transmit_enabled;

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
 * transmit_enable - TRUE or FALSE. Does this packet type (opcode) transmit packets
 					from this device?
 * returns TRUE/FALSE as error state
*/
uint8_t input_packet_type(uint8_t packet_opcode, uint8_t constant_packet_data_length, 
						uint8_t transmit_enable, uint8_t (*packet_handler_ptr)(uint8_t*));

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
		*data_out |= (uint64_t)(data_buffer[start_byte_index + parse_count])  << (8 * parse_count);	

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

#define CONSTANT_PACKET_BYTE_LENGTH (4) // sizeof SOH_BYTE + STX_BYTE + OPCODE_BYTE + EOT_BYTE
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
		packet_opcodes[i].transmit_enabled = FALSE;
	}

	_packet_handler_initialized = FALSE;
}

uint8_t input_packet_type(uint8_t packet_opcode, uint8_t constant_packet_data_length, 
	uint8_t transmit_enable, uint8_t (*packet_handler_ptr)(uint8_t*)) {


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
		packet_opcodes[packet_opcode].transmit_enabled = transmit_enable;

		if (transmit_enable) {
			packet_opcodes[packet_opcode].tx_buffer = calloc(constant_packet_data_length + CONSTANT_PACKET_BYTE_LENGTH, sizeof(uint8_t));
			if (!packet_opcodes[packet_opcode].tx_buffer) {
				return FALSE;
			} else {
				packet_opcodes[packet_opcode].tx_buffer[0] = SOH_BYTE;
				packet_opcodes[packet_opcode].tx_buffer[1] = STX_BYTE;
				packet_opcodes[packet_opcode].tx_buffer[2] = packet_opcode;
				packet_opcodes[packet_opcode].tx_buffer[3 + constant_packet_data_length] = EOT_BYTE;	
			}
		}
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

void double_to_byte_array(double val, uint8_t buff[8]) {
	uint64_t bin = double_to_binary(val);
	uint8_t i = 0;

	for (i = 0; i < 8; i++) {
		buff[i] = (bin >> 8 * (i + 1)) & 0x00000000000000FF;
	}

}

uint8_t packet_data_inject(uint8_t packet_opcode, uint8_t data_packet_position, uint8_t n, uint8_t* data) {
	uint8_t i = 0;

	if (packet_opcode < MAX_NUMBER_PACKET_TYPES) {
			for (i = 0; i < n; i++) {
				packet_opcodes[packet_opcode].tx_buffer[3 + data_packet_position + i] = data[i];
			}

			return TRUE;
	}

	return FALSE;
}

void UART_Send(uint8_t *data, uint8_t n) {
	while(data) {
		if (*data == EOT_BYTE) break;
		printf("0x%X\n", *(data++));
	}
}

double byte_array_to_double(uint8_t *data, uint8_t pos) {
	uint64_t bin = 0;
	_parse_sub_packet(&bin, 8, data, pos);

	return binary_to_double(bin);
}

uint8_t packet_send(uint8_t packet_opcode) {
	if (packet_opcode < MAX_NUMBER_PACKET_TYPES) {
		if (packet_opcodes[packet_opcode].transmit_enabled) {
			UART_Send(packet_opcodes[packet_opcode].tx_buffer, packet_opcodes[packet_opcode].constant_packet_data_length + CONSTANT_PACKET_BYTE_LENGTH);
		}

		return TRUE;
	}

	return FALSE;
}

int main (void) {

	uint8_t source = 0x02;
	uint8_t destination = 0x01;
	double longi = 36.9711913;
	double lat = -122.02;
	uint64_t time = 29238112912812;
	uint64_t status = 0xFF00FF00FF00FF00;
	uint8_t byte_array[8];

	input_packet_type(2, 34, TRUE, &gps_packet_parser);

	// send the tx_buffer pointer to test out!!!!
	packet_data_inject(2, 0, 1, &source);
	packet_data_inject(2, 1, 1, &destination);

	double_to_byte_array(longi, byte_array);
	packet_data_inject(2, 2, 8, byte_array);

	double_to_byte_array(lat, byte_array);
	packet_data_inject(2, 10, 8, byte_array);

	double_to_byte_array(time, byte_array);
	packet_data_inject(2, 18, 8, byte_array);

	double_to_byte_array(status, byte_array);
	packet_data_inject(2, 26, 8, byte_array);

	packet_send(2);

	printf("\nLONG TO DUB TEST: %lf\n", byte_array_to_double(packet_opcodes[2].tx_buffer, 4));

}
