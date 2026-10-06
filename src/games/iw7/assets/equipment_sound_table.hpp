#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class equipment_sound_table
	{
	public:
		static void read(zone_reader& reader, EquipmentSoundTable* asset);
	};
}
