#ifndef __PFP_DEVICE_HPP__
#define __PFP_DEVICE_HPP__

#include <sys/_stdint.h>

#define DEVICE_MAX_COUNT 125
#define DEVICE_DEFAULT_TTL 60

enum class DeviceState {
	DirectlyConnected,
	IndirectlyConnected,
	Alone,
};

struct Device {
	private:
		bool relay;
		uint32_t logical_id;
		uint8_t device_distance;
		uint8_t relay_distance;
		uint8_t load;
	public:
		uint32_t get_id();
		DeviceState get_device_state();
		uint32_t last_update;

		Device(bool relay, uint32_t logical_id, uint32_t device_distance, uint32_t relay_distance, uint8_t load, uint32_t last_update);
		~Device();
};

#endif // __PFP_DEVICE_HPP__