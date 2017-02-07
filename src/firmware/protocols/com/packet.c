// PAVx jay packet protocol
// Sargis S Yonan -- jan. 26 2017

#include <string.h>

#include "packet.h"
#include "uart.h"

#define SOH (0x01) // start of transmission byte
#define STX (0x02) // start of text byte
#define EOT (0x04) // end of transmission byte

#define PACKET_SIZE_BYTES (40)
#define BYTE_RECEIVE_SIZEOUT (80)

#define SOH_POS (0)
#define STX_POS (1)
#define DEVICE_ID_POS (3)
#define LONGITUDE_BYTE_START_POS (4)
#define LATITUDE_BYTE_START_POS (12)
#define TIME_BYTE_START_POS (20)
#define STATUS_BYTE_START_POS (28)
#define EOT_POS (36)

#ifndef TRUE
	#define TRUE (1)
#endif 

#ifndef FALSE
	#define FALSE (0)
#endif

static uint8_t __new_tx = FALSE;

uint8_t __tx_packet[PACKET_SIZE_BYTES];
uint8_t __rx_packet[PACKET_SIZE_BYTES];

void receive_packet(void) {
	uint8_t received_byte;
	uint8_t state = 0;
	uint8_t inner_state = 0;
	uint8_t bytes_received = 0;

	if (!UART_IsEmpty()) {
		memset(__rx_packet, 0, PACKET_SIZE_BYTES);

		for(bytes_received = 0; (received_byte = (uint8_t)UART_GetByte()) != NUL; bytes_received++) {

			// waiting for SOH
			if (state == 0) {
				if (received_byte == SOH) {
					__rx_packet[SOH_POS] = SOH;
					state = 1;
				}
			}
			// waiting for STX
			else if (state == 1) {
				if (received_byte == STX) {
					__rx_packet[STX_POS] = STX;
					state = 2;
				} else {
					state = 1;
				}
			}
			// receiving device id
			else if (state == 2) {
				__rx_packet[DEVICE_ID_POS] = received_byte;
				state = 3;
			}
			// receiving longitude
			else if (state == 3) {
				__rx_packet[LONGITUDE_BYTE_START_POS + inner_state] = received_byte;
				inner_state++;
				if (inner_state == 8) {
					inner_state = 0;
					state++;
				}
			}
			// receiving latitude
			else if (state == 4) {
				__rx_packet[LATITUDE_BYTE_START_POS + inner_state] = received_byte;
				inner_state++;
				if (inner_state == 8) {
					inner_state = 0;
					state++;
				}
			}
			// receiving time
			else if (state == 5) {
				__rx_packet[TIME_BYTE_START_POS + inner_state] = received_byte;
				inner_state++;
				if (inner_state == 8) {
					inner_state = 0;
					state++;
				}
			}
			// receiving status
			else if (state == 6) {
				__rx_packet[STATUS_BYTE_START_POS + inner_state] = received_byte;
				inner_state++;
				if (inner_state == 8) {
					inner_state = 0;
					state++;
				}
			}
			else if (state == 7) {
				if (received_byte == EOT) {
					__rx_packet[EOT_POS] = EOT;
					break;
				} else {
					state = 0;
				}
			}

			if (bytes_received > BYTE_RECEIVE_SIZEOUT) {
				memset(__rx_packet, 0, PACKET_SIZE_BYTES);
				break;
			}

		}
	}
}

bool packet_send(void) {

	if (__new_tx == TRUE) {
		return UART_SendBuffer((char*)__tx_packet, PACKET_SIZE_BYTES);
		__new_tx = FALSE;
	}
	return true;
}

void packet_init(void) {
	memset(__tx_packet, 0, PACKET_SIZE_BYTES);
	__tx_packet[SOH_POS] = SOH;
	__tx_packet[STX_POS] = STX;
	__tx_packet[EOT_POS] = EOT;
}

void packet_update_device_id(uint8_t device_id) {
	__tx_packet[DEVICE_ID_POS] = device_id;

	__new_tx = TRUE;
}

void packet_update_longitude(uint64_t longitude) {
	uint8_t i = 0;
	for (i = 0; i < 8; i++) {
		__tx_packet[LONGITUDE_BYTE_START_POS + i] = 
		(uint8_t)((uint64_t)(longitude & 0x00000000000000FF));
		longitude = (uint64_t)longitude >> 8;
	}
	__new_tx = TRUE;
}

void packet_update_latitude(uint64_t latitude) {
	uint8_t i = 0;
	for (i = 0; i < 8; i++) {
		__tx_packet[LATITUDE_BYTE_START_POS + i] = 
		(uint8_t)((uint64_t)latitude & 0x00000000000000FF);
		latitude = (uint64_t)latitude >> 8;
	}
	__new_tx = TRUE;
}

void packet_update_time(uint64_t time) {
	uint8_t i = 0;
	for (i = 0; i < 8; i++) {
		__tx_packet[TIME_BYTE_START_POS + i] = 
		(uint8_t)((uint64_t)time & 0x00000000000000FF);
		time = (uint64_t)time >> 8;
	}
	__new_tx = TRUE;
}

void packet_update_status(uint64_t status) {
	uint8_t i = 0;
	for (i = 0; i < 8; i++) {
		__tx_packet[STATUS_BYTE_START_POS + i] = 
		(uint8_t)((uint64_t)status & 0x00000000000000FF);
		status = (uint64_t)status >> 8;
	}
	__new_tx = TRUE;
}
