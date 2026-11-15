#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class xmodel_detail_collision
	{
	public:
		static void read(zone_reader& reader, XModelDetailCollision* asset);
	};
}
