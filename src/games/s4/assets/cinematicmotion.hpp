#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class cinematic_motion
	{
	public:
		static void read(zone_reader& reader, CinematicMotionDef* asset);
	};
}
