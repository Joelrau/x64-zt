#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class weapon
	{
	public:
		static void read(zone_reader& reader, WeaponCompleteDef* asset);
	};

	class weapon_attachment
	{
	public:
		static void read(zone_reader& reader, WeaponAttachment* asset);
	};

	class vfx_package
	{
	public:
		static void read(zone_reader& reader, WeaponVFXPackage* asset);
	};
}
