#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class fx
	{
	public:
		static void read(zone_reader32& reader, FxEffectDef* asset);
	};
}
