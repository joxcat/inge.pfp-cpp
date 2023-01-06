#ifndef __PFP_STATE_HPP__
#define __PFP_STATE_HPP__

#include <array>

#include "MicroBit.h"
#include "pfp/device.hpp"
#include "asenum.h"

enum class AddDeviceCode {
	Success,
	AlreadyExistTTLUpdated,
	DeviceListFull,
};

class State;
using AddDeviceResult = asenum::AsEnum<
	asenum::Case11<AddDeviceCode, AddDeviceCode::Success, State*>,
	asenum::Case11<AddDeviceCode, AddDeviceCode::AlreadyExistTTLUpdated, State*>,
	asenum::Case11<AddDeviceCode, AddDeviceCode::DeviceListFull, State*>
>;

class State {
	private:
		State &remove_device(uint32_t device_logical_id);
		State &broadcast_lost_device(uint32_t device_logical_id);
		State &broadcast_new_device(uint32_t device_logical_id);
		std::array<Device *, DEVICE_MAX_COUNT> devices;
		Device * gateway;

	public:
		MicroBit ubit;
		MicroBitSerial * serial;
		Device * current_device;
		uint32_t last_ttl_check;

		State();
		~State();
		void init();
		void check_ttl();
		void discover_network();
		AddDeviceResult add_device(Device device);
};

#endif // __PFP_STATE_HPP__