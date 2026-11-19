#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class anim_class
	{
	public:
		static void read(zone_reader& reader, AnimationClass* asset);
	};
}
