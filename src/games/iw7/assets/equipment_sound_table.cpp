#include <std_include.hpp>
#include "equipment_sound_table.hpp"

namespace zonetool::iw7
{
	namespace
	{
		void read_chances(zone_reader& reader, EquipmentChanceRattleTypes*& field, const EquipmentSoundTable* asset)
		{
			const auto cloth_types = reader.read_array(field, 7, asset->numClothTypes);
			if (!cloth_types)
			{
				return;
			}

			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				if (const auto rattle_types = reader.read_array(cloth_types[i].chances, 7, asset->numWeaponRattleTypes))
				{
					for (auto j = 0u; j < asset->numWeaponRattleTypes; j++)
					{
						reader.read_array(rattle_types[j].chances, 3, asset->numMoveTypes);
					}
				}
			}
		}

		void read_sound_sets(zone_reader& reader, EquipSoundSetMoveTypes*& field, const std::size_t count,
			const EquipmentSoundTable* asset)
		{
			if (const auto sets = reader.read_array(field, 7, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					reader.read_array(sets[i].soundSets, 7, asset->numMoveTypes);
				}
			}
		}
	}

	void equipment_sound_table::read(zone_reader& reader, EquipmentSoundTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szName);
		if (const auto cloth_types = reader.read_array(asset->clothTypes, 7, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				reader.read_string(cloth_types[i].foleyName);
				reader.read_string(cloth_types[i].footstepName);
			}
		}
		if (const auto rattle_types = reader.read_array(asset->weaponRattleTypes, 7, asset->numWeaponRattleTypes))
		{
			for (auto i = 0u; i < asset->numWeaponRattleTypes; i++)
			{
				reader.read_string(rattle_types[i].szName);
			}
		}
		read_chances(reader, asset->chancesPLR, asset);
		read_chances(reader, asset->chancesNPC, asset);
		read_sound_sets(reader, asset->mvmtClothFootstepCeilingSoundSets, asset->numClothTypes, asset);
		read_sound_sets(reader, asset->mvmtClothFoleySoundSets, asset->numClothTypes, asset);
		read_sound_sets(reader, asset->mvmtRattleSoundSets, asset->numWeaponRattleTypes, asset);
		reader.read_array(asset->mvmtAccentSoundSets.soundSets, 7, asset->numMoveTypes);
		reader.read_array(asset->mvmtMantleSoundSets, 7, asset->numClothTypes);
		reader.read_array(asset->mvmtStanceSoundSets, 7, asset->numClothTypes);
		reader.read_array(asset->meleeAttackVMSoundSets, 7, asset->numClothTypes);
		reader.pop_stream();
	}
}
