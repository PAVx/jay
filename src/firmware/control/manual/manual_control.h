#ifndef _MANUAL_CONTROL_H_
#define _MANUAL_CONTROL_H_

#include <stdint.h>

#ifndef TRUE
	#define TRUE (1)
#endif

#ifndef FALSE
	#define FALSE (0)
#endif

// data packet
struct _manual_control_packet {
	int16_t yaw;
	int16_t pitch;
	int16_t roll;
	uint8_t throttle;
};
typedef struct _manual_control_packet ManualControlPacket_t;

uint8_t reference_given = FALSE;

// define packet and pos
#define MANUAL_CONTROL_PACKET_TYPE (0)
#define MANUAL_CONTROL_PACKET_DATA_LENGTH (9) // 7 (attitude + throttle) + 1 (SOURCE) + 1 (DESTINATION)

// Packet Receiver/Injection/Parsing Functions
void inititialize_manual_control_packet(void);
uint8_t manual_control_packet_handler(uint8_t *input_buffer);

#endif
