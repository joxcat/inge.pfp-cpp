#include "pfp/protocol.hpp"

const uint8_t DISCOVER_PACKET[PROTOCOL_MAX_PACKET_SIZE] = { 
	0x00,
	0x00,
	0xDE, 0xAD, 0xBE, 0xEF,
	0xBE, 0xEF, 0xDE, 0xAD,
	0x00, 0x00, 0x00, 0x00,
	0x01,
	0x00,
	0x01,

	// 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	// 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	// 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	// 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 };