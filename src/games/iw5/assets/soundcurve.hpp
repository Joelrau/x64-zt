#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class sound_curve
	{
	public:
		static void read(zone_reader32& reader, SndCurve* asset);
	};
}
