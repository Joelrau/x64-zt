#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class cinematic_motion
	{
	public:
		static void read(zone_reader& reader, CinematicMotionDef* asset);
	};
}
