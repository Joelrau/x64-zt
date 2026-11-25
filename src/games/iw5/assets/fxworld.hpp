#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class fx_world
	{
	public:
		static void read(zone_reader32& reader, FxWorld* asset);
	};
}
