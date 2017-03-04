/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#include <stdio.h>
#include <stdlib.h>

#include "packet.h"
#include "uart.h"
#include "system.h"
#include "types.h"

#ifndef TRUE
	#define TRUE (1)
#endif

#ifndef FALSE
	#define FALSE
#endif

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


void parse_sub_packet(uint64_t *data_out, uint8_t n, 
	uint8_t *data_buffer, uint8_t start_byte_index) {

	uint8_t parse_count = 0;
	
	*data_out = 0x0000000000000000;

	for (parse_count = 0; parse_count < n; parse_count++) {
		*data_out |= (uint64_t)(data_buffer[start_byte_index + parse_count])  << (8 * parse_count);	
	}
}

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

void packet_receiver(void) {
	static PacketReceiverStates_t receiver_state = SOH_STATE;
	static uint8_t byte_index = 0;
	static uint8_t this_opcode = 0;
	
	if (UART_IsEmpty()) return;

	uint8_t byte_in = UART_GetByte();

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
				if (byte_in == EOT_BYTE) {
					(*(packet_opcodes[this_opcode].packet_handler_ptr))(rx_packet);
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

uint8_t packet_data_inject(uint8_t packet_opcode, uint8_t data_packet_position, uint8_t n, uint8_t* data) {
	uint8_t i = 0;

	if (packet_opcode < MAX_NUMBER_PACKET_TYPES) {
			for (i = 0; i < n; i++) {
				packet_opcodes[packet_opcode].tx_buffer[DATA_PACKET_START_POS + data_packet_position + i] = data[i];
			}

			return TRUE;
	}

	return FALSE;
}

uint8_t packet_send(uint8_t packet_opcode) {
	if (packet_opcode < MAX_NUMBER_PACKET_TYPES) {
		if (packet_opcodes[packet_opcode].transmit_enabled) {
			UART_SendBuffer((char*)packet_opcodes[packet_opcode].tx_buffer, packet_opcodes[packet_opcode].constant_packet_data_length + CONSTANT_PACKET_BYTE_LENGTH);
		}

		return TRUE;
	}

	return FALSE;
}
