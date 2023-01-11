#ifndef __PFP_DEVICE_HPP__
#define __PFP_DEVICE_HPP__

#include <sys/_stdint.h>

#define DEVICE_MAX_COUNT 125
#define DEVICE_DEFAULT_TTL 60
#define DEVICE_IS_ALONE_DISTANCE 0
#define DEVICE_IS_RELAY_DISTANCE 1

enum class DeviceState {
	Relay,
	IndirectlyConnected,
	Alone,
};

// Device size 8 bytes
struct Device {
	public:
		uint32_t logical_id; // 4 bytes
		uint8_t device_distance; // 1 byte
		uint8_t relay_distance; // 1 byte
		uint8_t load; // 1 byte
		uint32_t through_device;
		uint32_t get_id();
		DeviceState get_device_state();
		uint32_t last_update;

		Device();
		~Device();
};

#endif // __PFP_DEVICE_HPP__