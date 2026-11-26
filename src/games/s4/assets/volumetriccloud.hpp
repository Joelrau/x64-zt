#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class volumetric_cloud
	{
	public:
		static void read(zone_reader& reader, VolumetricCloud* asset);
	};
}
