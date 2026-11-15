#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class equipment_sound_table
	{
	public:
		static void read(zone_reader& reader, EquipmentSoundTable* asset);
	};
}
