#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class vehicle_sound_def
	{
	public:
		static void read(auto& reader, VehicleSoundDef* asset);
	};

	class vehicle_assembly
	{
	public:
		static void read(auto& reader, VehicleAssembly* asset);
	};

	class vehicle
	{
	public:
		static void read(auto& reader, Vehicle* asset);
	};
}
