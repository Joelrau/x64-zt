#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class bullet_penetration
	{
	public:
		static void read(zone_reader& reader, BulletPenetration* asset);
	};
}
