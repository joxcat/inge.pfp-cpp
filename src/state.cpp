#include <array>

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

	this->current_device = new Device(false, atol(this->ubit.getSerial().toCharArray()), 0, 0, 0, this->ubit.systemTime());
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

void State::discover_network() {
	Packet packet = Packet::new_packet_helop(this->current_device->get_id());
	auto raw_packet = packet.to_bytes();
	this->ubit.radio.datagram.send(raw_packet.data(), raw_packet.size());
}

AddDeviceResult State::add_device(Device device) {
	int count = 0;
	for (const auto &device : this->devices) {
		if (device->get_id() == device->get_id()) {
			device->last_update = this->ubit.systemTime();
			return AddDeviceResult::create<AddDeviceCode::AlreadyExistTTLUpdated>(this);
		}
		count++;
	}
	
	if (count >= DEVICE_MAX_COUNT) {
		return AddDeviceResult::create<AddDeviceCode::DeviceListFull>(this);
	}

	this->devices.back() = &device;

	return AddDeviceResult::create<AddDeviceCode::Success>(this);
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
	auto raw_packet = packet.to_bytes();
	this->ubit.radio.datagram.send(raw_packet.data(), raw_packet.size());
	return *this;
}

State &State::broadcast_lost_device(uint32_t device_logical_id) {
	Packet packet = Packet::new_packet_del(this->current_device->get_id(), device_logical_id);
	auto raw_packet = packet.to_bytes();
	this->ubit.radio.datagram.send(raw_packet.data(), raw_packet.size());
	return *this;
}