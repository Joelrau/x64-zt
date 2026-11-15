#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class weapon_anim_package
	{
	public:
		static void read(zone_reader& reader, WeaponAnimPackage* asset);
	};
}
