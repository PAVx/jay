// PAVx jay packet protocol
// Sargis S Yonan -- jan. 26 2017

#include <string.h>
#include <stdio.h>

#include "packet.h"
#include "uart.h"

#ifdef UART
	#define RX_PACKET_TEST (1)
#endif

#ifdef RX_PACKET_TEST
	char test_out_buffer[2];
#endif

#define SOH (0x01) // start of transmission byte
#define STX (0x02) // start of text byte
#define EOT (0x04) // end of transmission byte

#define PACKET_SIZE_BYTES (40)
#define BYTE_RECEIVE_SIZEOUT (80)

#define SUB_PACKET_SIZE_BYTES (8)

#define SOH_POS (0)
#define STX_POS (1)
#define DEVICE_ID_POS (3)
#define LONGITUDE_BYTE_START_POS (4)
#define LATITUDE_BYTE_START_POS (LONGITUDE_BYTE_START_POS + SUB_PACKET_SIZE_BYTES)
#define TIME_BYTE_START_POS (LATITUDE_BYTE_START_POS + SUB_PACKET_SIZE_BYTES)
#define STATUS_BYTE_START_POS (TIME_BYTE_START_POS + SUB_PACKET_SIZE_BYTES)
#define EOT_POS (STATUS_BYTE_START_POS + SUB_PACKET_SIZE_BYTES)


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

	static uint8_t inner_state = 0;
	static uint8_t state = 0;
	static uint8_t this_device_id = 0;
	uint8_t parse_count = 0;

	#ifdef UART
		return;
	#endif

	if (!UART_IsEmpty()) {

		received_byte = (uint8_t)UART_GetByte();

			// waiting for SOH
			if (state == 0) {
				this_device_id = 0;

				if (received_byte == SOH) {
					__rx_packet[SOH_POS] = SOH;
					state = 1;

					UART_SendByte('A');
				}


			}
			// waiting for STX
			else if (state == 1) {
				if (received_byte == STX) {
					__rx_packet[STX_POS] = STX;
					state = 2;

					UART_SendByte('B');
				} else {
					state = 1;
				}
			}
			// receiving device id
			else if (state == 2) {
				__rx_packet[DEVICE_ID_POS] = received_byte;
				this_device_id = received_byte;

				rx_packets[this_device_id].device_id = this_device_id;
				state = 3;
			}
			// receiving longitude
			else if (state == 3) {
				__rx_packet[LONGITUDE_BYTE_START_POS + inner_state] = received_byte;
				inner_state++;
				if (inner_state == 8) {
					inner_state = 0;
					state++;

					UART_SendByte('C');

					rx_packets[this_device_id].longitude &= 0x0000000000000000;
					for (parse_count = 0; parse_count < SUB_PACKET_SIZE_BYTES; parse_count++) {
						rx_packets[this_device_id].longitude |=
						(__rx_packet[LONGITUDE_BYTE_START_POS + (8 - parse_count)]) << (parse_count * 8);
					}

				}
			}
			// receiving latitude
			else if (state == 4) {
				__rx_packet[LATITUDE_BYTE_START_POS + inner_state] = received_byte;
				inner_state++;
				if (inner_state == 8) {
					inner_state = 0;
					state++;

					UART_SendByte('D');

					rx_packets[this_device_id].latitude &= 0x0000000000000000;
					for (parse_count = 0; parse_count < SUB_PACKET_SIZE_BYTES; parse_count++) {
						rx_packets[this_device_id].latitude |=
						(__rx_packet[LATITUDE_BYTE_START_POS + (8 - parse_count)]) << (parse_count * 8);
					}
				}
			}
			// receiving time
			else if (state == 5) {
				__rx_packet[TIME_BYTE_START_POS + inner_state] = received_byte;
				inner_state++;
				if (inner_state == 8) {
					inner_state = 0;
					state++;

					UART_SendByte('E');

					rx_packets[this_device_id].time &= 0x0000000000000000;
					for (parse_count = 0; parse_count < SUB_PACKET_SIZE_BYTES; parse_count++) {
						rx_packets[this_device_id].time |=
						(__rx_packet[TIME_BYTE_START_POS + (8 - parse_count)]) << (parse_count * 8);
					}
				}
			}
			// receiving status
			else if (state == 6) {
				__rx_packet[STATUS_BYTE_START_POS + inner_state] = received_byte;
				inner_state++;
				if (inner_state == 8) {
					inner_state = 0;
					state++;

					UART_SendByte('F');

					rx_packets[this_device_id].status &= 0x0000000000000000;
					for (parse_count = 0; parse_count < SUB_PACKET_SIZE_BYTES; parse_count++) {
						rx_packets[this_device_id].status |=
						(__rx_packet[STATUS_BYTE_START_POS + (8 - parse_count)]) << (parse_count * 8);
					}
				}
			}
			else if (state == 7) {
				if (received_byte == EOT) {
					__rx_packet[EOT_POS] = EOT;

					UART_SendByte('G');

				#ifdef RX_PACKET_TEST

					sprintf(test_out_buffer, "DID:0x%X\nLONGITUDE:0x%X%X\nLATITUDE:0x%X%X\nTIME:0x%X%X\nSTATUS:0x%X%X\n",
						rx_packets[this_device_id].device_id,
						(unsigned int)(rx_packets[this_device_id].longitude >> 32),
						(unsigned int)(rx_packets[this_device_id].longitude & 0x0000000011111111),
						(unsigned int)(rx_packets[this_device_id].latitude >> 32),
						(unsigned int)(rx_packets[this_device_id].latitude & 0x0000000011111111),
						(unsigned int)(rx_packets[this_device_id].time >> 32),
						(unsigned int)(rx_packets[this_device_id].time & 0x0000000011111111),
						(unsigned int)(rx_packets[this_device_id].status >> 32),
						(unsigned int)(rx_packets[this_device_id].status & 0x0000000011111111)
						);
					UART_SendString(test_out_buffer);
					state = 0;
					inner_state = 0;

					memset(__rx_packet, 0, PACKET_SIZE_BYTES);
				#endif

				} else {
					state = 0;
				}
			}
		}
//	}
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
	for (i = 0; i < SUB_PACKET_SIZE_BYTES; i++) {
		__tx_packet[LATITUDE_BYTE_START_POS + i] =
		(uint8_t)((uint64_t)latitude & 0x00000000000000FF);
		latitude = (uint64_t)latitude >> 8;
	}
	__new_tx = TRUE;
}

void packet_update_time(uint64_t time) {
	uint8_t i = 0;
	for (i = 0; i < SUB_PACKET_SIZE_BYTES; i++) {
		__tx_packet[TIME_BYTE_START_POS + i] =
		(uint8_t)((uint64_t)time & 0x00000000000000FF);
		time = (uint64_t)time >> 8;
	}
	__new_tx = TRUE;
}

void packet_update_status(uint64_t status) {
	uint8_t i = 0;
	for (i = 0; i < SUB_PACKET_SIZE_BYTES; i++) {
		__tx_packet[STATUS_BYTE_START_POS + i] =
		(uint8_t)((uint64_t)status & 0x00000000000000FF);
		status = (uint64_t)status >> 8;
	}
	__new_tx = TRUE;
}
