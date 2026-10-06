#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class animation_class
	{
	public:
		static void read(zone_reader& reader, AnimationClass* asset);
	};
}
