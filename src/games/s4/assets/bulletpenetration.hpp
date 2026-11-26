#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class bullet_penetration
	{
	public:
		static void read(zone_reader& reader, BulletPenetration* asset);
	};
}
