/* 
	PAVx -- Pod-Based Autonomous Vehicles 
	Library Created By: Sargis S Yonan
	March 2017
*/ 
// originally written by Sargis S Yonan -- jan. 26 2017

#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdint.h>

#include "system.h"

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

// type 0 - manual control packet type (control/manual_control.h)
// type 1 - status packets from other vehicles (control/status.h)
// type 3 - initialization packets -- need an ack from the ground to init (control/init_packet.h)
#define MAX_NUMBER_PACKET_TYPES (3)

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
void parse_sub_packet(uint64_t *data_out, uint8_t n, 
	uint8_t *data_buffer, uint8_t start_byte_index);
uint8_t packet_data_inject(uint8_t packet_opcode, uint8_t data_packet_position, uint8_t n, uint8_t* data);
uint8_t packet_send(uint8_t packet_opcode);

#define DATA_PACKET_START_POS (3)


#endif /* _PACKET_H_ */