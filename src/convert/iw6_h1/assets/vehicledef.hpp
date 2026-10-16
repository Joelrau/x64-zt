#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace vehicledef
		{
			zonetool::h1::VehicleDef* convert(VehicleDef* asset, utils::memory::allocator& allocator);
			void dump(VehicleDef* asset);
		}
	}
}