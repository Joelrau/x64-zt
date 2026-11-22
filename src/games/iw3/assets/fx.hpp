#pragma once
#include "../iw3.hpp"

namespace zonetool::iw3
{
	class fx
	{
	public:
		static void read(zone_reader32& reader, FxEffectDef* asset);
	};
}
