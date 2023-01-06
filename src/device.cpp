#include "pfp/device.hpp"

Device::~Device() {}

DeviceState Device::get_device_state() {
	if (this->relay_distance == 1) {
		return DeviceState::DirectlyConnected;
	} else if (this->relay_distance > 1) {
		return DeviceState::IndirectlyConnected;
	} else {
		return DeviceState::Alone;
	}
}

Device::Device(bool relay, uint32_t logical_id, uint32_t device_distance, uint32_t relay_distance, uint8_t load, uint32_t last_update) {
	this->relay = relay;
	this->logical_id = logical_id;
	this->device_distance = device_distance;
	this->relay_distance = relay_distance;
	this->load = load;
	this->last_update = last_update;
}

uint32_t Device::get_id() { return this->logical_id; }