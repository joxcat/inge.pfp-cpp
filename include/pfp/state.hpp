#ifndef __PFP_STATE_HPP__
#define __PFP_STATE_HPP__

#include <array>

#include "MicroBit.h"
#include "pfp/device.hpp"
#include "asenum.h"

enum class UpsertDeviceCode {
	Success,
	AlreadyExistTTLUpdated,
	DeviceListFull,
};

class State;
using UpsertDeviceResult = asenum::AsEnum<
	asenum::Case11<UpsertDeviceCode, UpsertDeviceCode::Success, State*>,
	asenum::Case11<UpsertDeviceCode, UpsertDeviceCode::AlreadyExistTTLUpdated, State*>,
	asenum::Case11<UpsertDeviceCode, UpsertDeviceCode::DeviceListFull, State*>
>;

class State {
	private:
		void send_packet(Packet packet);
		State &remove_device(uint32_t device_logical_id);
		State &broadcast_lost_device(uint32_t device_logical_id);
		State &broadcast_new_device(uint32_t device_logical_id);
		std::array<Device *, DEVICE_MAX_COUNT> devices;
		Device * gateway;
		uint32_t last_assigned_logical_id = 1;

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
		void forward(Packet packet);
		UpsertDeviceResult upsert_device(Device device);
		
		void handle_helop(Packet packet);
		void handle_olehl(Packet packet);
		void handle_alive(Packet packet);
};

#endif // __PFP_STATE_HPP__