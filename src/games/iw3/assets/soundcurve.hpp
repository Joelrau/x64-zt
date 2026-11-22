#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class sound_curve
	{
	public:
		static void read(zone_reader32& reader, SndCurve* asset);
	};
}
