#include "pfp/protocol.hpp"
#include "pfp/binarytools.hpp"

Packet::Packet() {
	this->hop_count = 0;
	this->source_addr = 0;
	this->dest_addr = 0;
	this->forwarded_by_addr = 0;
	this->request_id = 0;
	this->request_part = 0;
	this->request_count = 1;
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
	if ((uint8_t)Command::HELOP > command_id || command_id > (uint8_t)Command::ALIVE) {
		return DeserializeResult::create<DeserializeCode::InvalidCommand>(command_id);
	}
	packet.command_id = (Command)command_id;

	reader
		.read(&packet.hop_count)
		.read(&packet.source_addr)
		.read(&packet.dest_addr)
		.read(&packet.forwarded_by_addr)
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
		.write(this->forwarded_by_addr)
		.write(this->request_id)
		.write(this->request_part)
		.write(this->request_count)
		.write(this->payload, PROTOCOL_MAX_PAYLOAD_SIZE);

	return SerializeResult::create<SerializeCode::Success>();
}

Packet Packet::new_packet_add(uint32_t source_addr, uint32_t new_device_addr) {
	Packet packet;
	packet.command_id = Command::ADD;
	packet.source_addr = source_addr;
	packet.dest_addr = 0;
	packet.request_id = 1; // TODO: store request till ack received

	write_uint32(packet.payload, 0, new_device_addr);
	return packet;
}

Packet Packet::new_packet_del(uint32_t source_addr, uint32_t lost_device_addr) {
	Packet packet;
	packet.command_id = Command::DEL;
	packet.source_addr = source_addr;
	packet.dest_addr = 0;
	packet.request_id = 1; // TODO: store request till ack received

	write_uint32(packet.payload, 0, lost_device_addr);
	return packet;
}

Packet Packet::new_packet_helop(uint32_t source_addr) {
	Packet packet;
	packet.command_id = Command::HELOP;
	packet.source_addr = source_addr;
	packet.dest_addr = 0;
	packet.request_id = 1; // TODO: store request till ack received

	return packet;
}

std::vector<Packet> Packet::new_packet_olehl(uint32_t source_addr, uint32_t dest_addr, std::array<Device *, DEVICE_MAX_COUNT> devices) {
	std::vector<Packet> packets;
	int packet_id = 1;
	int device_count = 0;
	Packet packet;
	IncrementalWriter devices_writer(packet.payload, 0);

	for (const auto &device : devices) {
		if (device != nullptr) {
			device_count++;
		} else {
			break;
		}
		if (device_count > 4) {
			packets.push_back(packet);
			packet = Packet();
			devices_writer = IncrementalWriter(packet.payload, 0);

			device_count = 1;
		}
		if (device_count == 1) {
			packet.command_id = Command::OLEHL;
			packet.source_addr = source_addr;
			packet.dest_addr = dest_addr;
			packet.request_id = 1; // TODO: store request till ack received
			packet.request_part = 1;
		}

		devices_writer.write(device->get_id())
			.write(device->device_distance)
			.write(device->relay_distance)
			.write(device->load);
	}

	for (auto &packet : packets) {
		packet.request_count = packets.size();
	}

	return packets;
}

Packet Packet::new_packet_alive(uint32_t source_addr, uint32_t timestamp) {
	Packet packet;
	packet.command_id = Command::ALIVE;
	packet.source_addr = source_addr;
	packet.dest_addr = 0;
	packet.request_id = 1; // TODO: store request till ack received

	write_uint32(packet.payload, 0, timestamp);

	return packet;
}