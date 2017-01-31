// PAVx jay packet protocol
// sargis s yonan -- jan. 26 2017

#include <stdint.h>
#include <stdbool.h>

void receive_packet(void);
bool packet_send(void);
void packet_init(void);
void packet_update_device_id(uint8_t device_id);
void packet_update_longitude(uint64_t longitude);
void packet_update_latitude(uint64_t latitude);
void packet_update_time(uint64_t time);
void packet_update_status(uint64_t status);