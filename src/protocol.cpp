#include "pfp/protocol.hpp"
#include "pfp/binarytools.hpp"

Packet::Packet() {
	this->hop_count = 0;
	this->source_addr = 0;
	this->dest_addr = 0;
	this->request_id = 0;
	this->request_part = 0;
	this->request_count = 0;
	std::fill_n(this->payload, PROTOCOL_MAX_PAYLOAD_SIZE, 0);
}
Packet::~Packet() {}

std::array<uint8_t, PROTOCOL_MAX_PACKET_SIZE> Packet::to_bytes() {
	auto raw_packet = std::array<uint8_t, PROTOCOL_MAX_PACKET_SIZE>();
	this->serialize(raw_packet.data());
	return raw_packet;
}

DeserializeResult Packet::deserialize(uint8_t *buffer, int buffer_length, Packet &packet) {
	if (buffer_length < PROTOCOL_MAX_PACKET_SIZE) {
		return DeserializeResult::create<DeserializeCode::NotEnoughData>(buffer_length);
	}

	IncrementalReader reader(buffer, 0);
	uint8_t command_id; 
	reader.read(&command_id);
	if ((uint8_t)Command::HELOP > command_id || command_id > (uint8_t)Command::DEL) {
		return DeserializeResult::create<DeserializeCode::InvalidCommand>(command_id);
	}
	packet.command_id = (Command)command_id;

	reader
		.read(&packet.hop_count)
		.read(&packet.source_addr)
		.read(&packet.dest_addr)
		.read(&packet.request_id)
		.read(&packet.request_part)
		.read(&packet.request_count)
		.read((uint8_t*)&packet.payload, PROTOCOL_MAX_PAYLOAD_SIZE);

	return DeserializeResult::create<DeserializeCode::Success>();
}

SerializeResult Packet::serialize(uint8_t *dest_buffer) {
	if (!check_arr_size(dest_buffer, PROTOCOL_MAX_PACKET_SIZE)) {
		return SerializeResult::create<SerializeCode::NotEnoughSpace>(PROTOCOL_MAX_PACKET_SIZE);
	}
	
	write_uint8(dest_buffer, 0, (uint8_t)this->command_id)
		.write(this->hop_count)
		.write(this->source_addr)
		.write(this->dest_addr)
		.write(this->request_id)
		.write(this->request_part)
		.write(this->request_count)
		.write(this->payload, PROTOCOL_MAX_PAYLOAD_SIZE);

	return SerializeResult::create<SerializeCode::Success>();
}

Packet Packet::new_packet_add(uint32_t source_addr, uint32_t new_device_addr) {
	Packet packet;
	packet.command_id = Command::ADD;
	packet.hop_count = 0;
	packet.source_addr = source_addr;
	packet.dest_addr = 0;
	packet.request_id = 1; // TODO: store request till ack received
	packet.request_part = 0;
	packet.request_count = 1;

	write_uint32(packet.payload, 0, new_device_addr);
	return packet;
}

Packet Packet::new_packet_del(uint32_t source_addr, uint32_t lost_device_addr) {
	Packet packet;
	packet.command_id = Command::DEL;
	packet.hop_count = 0;
	packet.source_addr = source_addr;
	packet.dest_addr = 0;
	packet.request_id = 1; // TODO: store request till ack received
	packet.request_part = 0;
	packet.request_count = 1;

	write_uint32(packet.payload, 0, lost_device_addr);
	return packet;
}

Packet Packet::new_packet_helop(uint32_t source_addr) {
	Packet packet;
	packet.command_id = Command::HELOP;
	packet.hop_count = 0;
	packet.source_addr = source_addr;
	packet.dest_addr = 0;
	packet.request_id = 1; // TODO: store request till ack received
	packet.request_part = 0;
	packet.request_count = 1;

	return packet;
}