#ifndef __PFP_PROTOCOL_HPP__
#define __PFP_PROTOCOL_HPP__

#include "MicroBit.h"

// Header constants
#define PROTOCOL_COMMAND_ID_SIZE 1
#define PROTOCOL_HOP_COUNT_SIZE 1
#define PROTOCOL_SOURCE_ADDR_SIZE 4
#define PROTOCOL_DEST_ADDR_SIZE 4
#define PROTOCOL_REQUEST_ID_SIZE 1
#define PROTOCOL_REQUEST_PART_SIZE 1
#define PROTOCOL_REQUEST_COUNT_SIZE 1

// Protocol constants
#define PROTOCOL_HEADERS_SIZE (PROTOCOL_COMMAND_ID_SIZE + PROTOCOL_HOP_COUNT_SIZE + PROTOCOL_SOURCE_ADDR_SIZE + PROTOCOL_DEST_ADDR_SIZE + PROTOCOL_REQUEST_ID_SIZE + PROTOCOL_REQUEST_PART_SIZE + PROTOCOL_REQUEST_COUNT_SIZE)
#define PROTOCOL_MAX_PAYLOAD_SIZE 0
#define PROTOCOL_MAX_PACKET_SIZE (PROTOCOL_HEADERS_SIZE + PROTOCOL_MAX_PAYLOAD_SIZE)

enum Command {
	HELOP = 0,
	OLEP,
	IDENT,
	TNEDI,
	TRUSTB,
	TRUSTT,
	HELOL,
	OLEHL,
	PUSH,
	PUSH_ACK,
	ADENY,
	UDENY,
	ADD,
	DEL
};

struct Packet {
	public:
		Command command_id;
		uint8_t hop_count;
		uint32_t source_addr;
		uint32_t dest_addr;
		uint8_t request_id;
		uint8_t request_part;
		uint8_t request_count;
		uint8_t payload[PROTOCOL_MAX_PAYLOAD_SIZE];

		Packet();
		~Packet();

		static int deserialize(uint8_t *buffer, Packet &packet);
		int serialize(uint8_t *dest_buffer);
};

#endif // __PFP_PROTOCOL_HPP__