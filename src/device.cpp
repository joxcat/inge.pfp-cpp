#include "pfp/device.hpp"

Device::~Device() {}

DeviceState Device::get_device_state() {
	if (this->relay_distance == 0) {
		return DeviceState::Alone;
	} else if (this->relay_distance == 1) {
		return DeviceState::Relay;
	} else if (this->relay_distance > 1) {
		return DeviceState::IndirectlyConnected;
	}

	return DeviceState::Alone;
}

Device::Device() {
	this->logical_id = 0;
	this->device_distance = 0;
	this->relay_distance = DEVICE_IS_ALONE_DISTANCE;
	this->load = 0;
	this->last_update = 0;
}

uint32_t Device::get_id() { return this->logical_id; }