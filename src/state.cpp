#include <array>
#include <vector>

#include "pfp/state.hpp"
#include "pfp/protocol.hpp"
#include "pfp/binarytools.hpp"
#include "MicroBit.h"

State::State() {
	this->serial = nullptr;
	this->current_device = nullptr;
	this->gateway = nullptr;
}

State::~State() {
	delete this->serial;
	delete this->current_device;
}

void State::init() {
	this->serial = new MicroBitSerial(USBTX, USBRX);

	this->ubit.init();
  this->ubit.seedRandom();
  this->ubit.radio.enable();

	this->current_device = new Device();
	this->current_device->logical_id = atol(this->ubit.getSerial().toCharArray());
}

void State::send_packet(Packet packet) {
	std::array<uint8_t, PROTOCOL_MAX_PACKET_SIZE> raw_packet = packet.to_bytes();
	this->ubit.radio.datagram.send(raw_packet.data(), raw_packet.size());
}

void State::check_ttl() {
	uint32_t now = this->ubit.systemTime();
	if (now - this->last_ttl_check > DEVICE_DEFAULT_TTL * 1000) {
		for (const auto &device : this->devices) {
			if (device->last_update < now) {
				this->broadcast_lost_device(device->get_id())
					.remove_device(device->get_id());
			}
		}
		this->last_ttl_check = now;
	}
}

void State::forward(Packet packet) {
	for (const auto &device : this->devices) {
		if (device->get_id() == packet.dest_addr) {
			packet.forwarded_by_addr = device->through_device;
			break;
		}
	}

	packet.hop_count++;

	std::array<uint8_t, PROTOCOL_MAX_PACKET_SIZE> raw_packet = packet.to_bytes();
	this->ubit.radio.datagram.send(raw_packet.data(), raw_packet.size());
}

void State::discover_network() {
	Packet packet = Packet::new_packet_helop(this->current_device->get_id());
	std::array<uint8_t, PROTOCOL_MAX_PACKET_SIZE> raw_packet = packet.to_bytes();
	this->ubit.radio.datagram.send(raw_packet.data(), raw_packet.size());
}

UpsertDeviceResult State::upsert_device(Device device) {
	int count = 0;
	for (const auto &dev : this->devices) {
		if (dev->get_id() == device.get_id()) {
			if (dev->device_distance > device.device_distance) {
				dev->through_device = device.through_device;
				dev->device_distance = device.device_distance;
				dev->relay_distance = device.relay_distance;
				dev->load = device.load;
			}

			dev->last_update = this->ubit.systemTime();
			return UpsertDeviceResult::create<UpsertDeviceCode::AlreadyExistTTLUpdated>(this);
		}
		count++;
	}
	
	if (count >= DEVICE_MAX_COUNT) {
		return UpsertDeviceResult::create<UpsertDeviceCode::DeviceListFull>(this);
	}

	this->devices.back() = &device;

	return UpsertDeviceResult::create<UpsertDeviceCode::Success>(this);
}

State &State::remove_device(uint32_t device_logical_id) {
	for (const auto &device : this->devices) {
		if (device->get_id() == device_logical_id) {
			std::remove(this->devices.begin(), this->devices.end(), device);
		}
	}
	return *this;
}

State &State::broadcast_new_device(uint32_t device_logical_id) {
	Packet packet = Packet::new_packet_add(this->current_device->get_id(), device_logical_id);
	this->send_packet(packet);
	return *this;
}

State &State::broadcast_lost_device(uint32_t device_logical_id) {
	Packet packet = Packet::new_packet_del(this->current_device->get_id(), device_logical_id);
	this->send_packet(packet);
	return *this;
}

void State::handle_helop(Packet packet) {
	if (this->current_device->get_device_state() == DeviceState::Alone) {
		if (this->current_device->get_id() < packet.source_addr) {
			this->gateway = this->current_device;
			this->current_device->logical_id = this->last_assigned_logical_id;
			this->last_assigned_logical_id++;
			this->current_device->relay_distance = 0;
		} else {
			// NOTE: Else we let the other device initialize the network
			return;
		}
	}

	std::vector<Packet> responses = Packet::new_packet_olehl(this->current_device->get_id(), packet.source_addr, this->devices);
	for (const auto &response : responses) {
		this->send_packet(response);
	}
}

void State::handle_olehl(Packet packet) {
		IncrementalReader payload_reader(packet.payload, 0);

		while (true) {
			Device dev;
			dev.through_device = packet.source_addr;
			
			uint32_t device_logical_id;
			payload_reader.read(&device_logical_id);

			if (device_logical_id == 0) {
				break;
			}

			dev.logical_id = device_logical_id;
			payload_reader
				.read(&dev.device_distance)
				.read(&dev.relay_distance)
				.read(&dev.load);
			
			if (dev.logical_id == this->current_device->get_id()) {
				// NOTE: We don't add ourselves to the list
				continue;
			}

			// NOTE: Because we are one more hop away from the device, we need to add one to the device distance
			dev.device_distance++;

			this->upsert_device(dev);
		}
}

void State::handle_alive(Packet packet) {
	for (auto device : this->devices) {
		if (device->get_id() == packet.source_addr) {
			uint32_t packet_timestamp = read_uint32(packet.payload, 0);
			if (packet_timestamp > device->last_update) {
				// NOTE: We only update the device if the timestamp is newer than the last update
				// NOTE: Rebroadcast it because it's the first time we receive it
				device->last_update = packet_timestamp;
				this->send_packet(packet);
			}
			break;
		}
	}
}