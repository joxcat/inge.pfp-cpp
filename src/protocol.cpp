#include "pfp/protocol.hpp"
#include "pfp/binarytools.hpp"

Packet::Packet() {}
Packet::~Packet() {}

uint8_t deserialize_uint8(uint8_t *buffer, int *offset) {
	int prev_offset = *offset;
	*offset += 1;
	return buffer[prev_offset];
}
uint32_t deserialize_uint32(uint8_t *buffer, int *offset) {
	int prev_offset = *offset;
	*offset += 4;
	return uint8_arr_to_uint32(buffer + prev_offset);
}

int Packet::deserialize(uint8_t *buffer, int buffer_length, Packet &packet) {
	if (buffer == nullptr) {
		return -1;
	} else if (buffer_length < PROTOCOL_MAX_PACKET_SIZE) {
		return -2;
	}

	int offset = 0;
	packet.command_id = (Command)deserialize_uint8(buffer, &offset);

	packet.hop_count = deserialize_uint8(buffer, &offset);
	packet.source_addr = deserialize_uint32(buffer, &offset);
	packet.dest_addr = deserialize_uint32(buffer, &offset);
	packet.request_id = deserialize_uint8(buffer, &offset);
	packet.request_part = deserialize_uint8(buffer, &offset);
	packet.request_count = deserialize_uint8(buffer, &offset);
	for (int i = 0; i < PROTOCOL_MAX_PAYLOAD_SIZE; i++) {
		packet.payload[i] = buffer[offset + i];
	}

	return 0;
}

void serialize_uint8(uint8_t *dest, int *offset, uint8_t src) {
	dest[*offset] = src;
	*offset += 1;
}
void serialize_uint32(uint8_t *dest, int *offset, uint32_t src) {
	uint32_to_uint8_arr(dest + *offset, src);
	*offset += 4;
}

int Packet::serialize(uint8_t *dest_buffer) {
	int offset = 0;
	
	serialize_uint8(dest_buffer, &offset, (uint8_t)this->command_id);
	serialize_uint8(dest_buffer, &offset, this->hop_count);
	serialize_uint32(dest_buffer, &offset, this->source_addr);
	serialize_uint32(dest_buffer, &offset, this->dest_addr);
	serialize_uint8(dest_buffer, &offset, this->request_id);
	serialize_uint8(dest_buffer, &offset, this->request_part);
	serialize_uint8(dest_buffer, &offset, this->request_count);
	for (int i = 0; i < PROTOCOL_MAX_PAYLOAD_SIZE; i++) {
		dest_buffer[offset + i] = this->payload[i];
	}

	return 0;
}