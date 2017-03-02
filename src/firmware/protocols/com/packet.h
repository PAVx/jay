// PAVx jay packet protocol
// sargis s yonan -- jan. 26 2017
#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdint.h>
#include <stdbool.h>

#include "system.h"

struct rx_packet {
	uint8_t device_id;
	uint64_t longitude;
	uint64_t latitude;
	uint64_t time;
	uint64_t status;
};
typedef struct rx_packet rx_packet_t;
rx_packet_t rx_packets[DEFAULT_NUM_SIBLINGS + 1];

void receive_packet(void);
bool packet_send(void);
void packet_init(void);
void packet_update_device_id(uint8_t device_id);
void packet_update_longitude(uint64_t longitude);
void packet_update_latitude(uint64_t latitude);
void packet_update_time(uint64_t time);
void packet_update_status(uint64_t status);

#endif /* _PACKET_H_ */