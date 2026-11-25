#pragma once
#include "../iw5.hpp"

namespace zonetool::iw5
{
	class attachment
	{
	public:
		static void read(zone_reader32& reader, WeaponAttachment* asset);
	};
}
