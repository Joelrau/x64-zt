#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class suit_anim_package
	{
	public:
		static void read(zone_reader& reader, SuitAnimPackage* asset);
	};
}
