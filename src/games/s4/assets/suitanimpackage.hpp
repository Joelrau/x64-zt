#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class suit_anim_package
	{
	public:
		static void read(zone_reader& reader, SuitAnimPackage* asset);
	};
}
