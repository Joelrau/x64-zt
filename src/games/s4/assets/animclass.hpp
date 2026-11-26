#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class anim_class
	{
	public:
		static void read(zone_reader& reader, AnimationClass* asset);
	};
}
