#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class equipment_sound_table
	{
	public:
		static void read(zone_reader& reader, EquipmentSoundTable* asset);
	};
}
