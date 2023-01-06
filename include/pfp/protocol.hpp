#ifndef __PFP_PROTOCOL_HPP__
#define __PFP_PROTOCOL_HPP__

#include "MicroBit.h"
#include "asenum.h"

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
#define PROTOCOL_MAX_PAYLOAD_SIZE 32
#define PROTOCOL_MAX_PACKET_SIZE (PROTOCOL_HEADERS_SIZE + PROTOCOL_MAX_PAYLOAD_SIZE)

enum class Command {
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


enum class SerializeCode {
	Success,
	NotEnoughSpace,
};

using SerializeResult = asenum::AsEnum<
	asenum::Case11<SerializeCode, SerializeCode::Success, void>,
	asenum::Case11<SerializeCode, SerializeCode::NotEnoughSpace, int>
>;

enum class DeserializeCode {
	Success,
	NotEnoughData,
	InvalidCommand
};

using DeserializeResult = asenum::AsEnum<
	asenum::Case11<DeserializeCode, DeserializeCode::Success, void>,
	asenum::Case11<DeserializeCode, DeserializeCode::NotEnoughData, int>,
	asenum::Case11<DeserializeCode, DeserializeCode::InvalidCommand, uint8_t>
>;

struct Packet {
	private:
		uint8_t hop_count;
		uint32_t source_addr;
		uint8_t request_id;
		uint8_t request_part;
		uint8_t request_count;
		uint8_t payload[PROTOCOL_MAX_PAYLOAD_SIZE];

	public:
		Command command_id;
		uint32_t dest_addr;
		std::array<uint8_t, PROTOCOL_MAX_PACKET_SIZE> to_bytes();

		Packet();
		~Packet();

		static DeserializeResult deserialize(uint8_t *buffer, int buffer_length, Packet &packet);
		SerializeResult serialize(uint8_t *dest_buffer);

		static Packet new_packet_add(uint32_t source_addr, uint32_t new_device_addr);
		static Packet new_packet_del(uint32_t source_addr, uint32_t lost_device_addr);
		static Packet new_packet_helop(uint32_t source_addr);
};

#endif // __PFP_PROTOCOL_HPP__