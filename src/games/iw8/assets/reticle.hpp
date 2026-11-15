#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class reticle_def
	{
	public:
		static void read(zone_reader& reader, ReticleDef* asset);
	};
}
