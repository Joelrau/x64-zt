#include <std_include.hpp>
#include "equipsndtable.hpp"

namespace zonetool::s4
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
				const auto rattle_types = reader.read_array(cloth_types[i].chances, 7, asset->numWeaponRattleTypes);
				if (!rattle_types)
				{
					continue;
				}

				for (auto j = 0u; j < asset->numWeaponRattleTypes; j++)
				{
					reader.read_array(rattle_types[j].chances, 3, asset->numMoveTypes);
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
		reader.read_array(asset->mvmtClothFootstepSoundSets, 7, asset->numClothTypes);
		reader.read_array(asset->mvmtClothFootstepCeilingSoundSets, 7, asset->numClothTypes);

		if (const auto foley = reader.read_array(asset->mvmtClothFoleySoundSets, 7, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				reader.read_array(foley[i].soundSetsPre, 7, asset->numMoveTypes);
				reader.read_array(foley[i].soundSetsPost, 7, asset->numMoveTypes);
			}
		}

		if (const auto rattle = reader.read_array(asset->mvmtRattleSoundSets, 7, asset->numWeaponRattleTypes))
		{
			for (auto i = 0u; i < asset->numWeaponRattleTypes; i++)
			{
				reader.read_array(rattle[i].soundSets, 7, asset->numMoveTypes);
			}
		}

		reader.read_array(asset->mvmtAccentSoundSets.soundSets, 7, asset->numMoveTypes);
		reader.read_array(asset->mvmtMantleSoundSets, 7, asset->numClothTypes);
		reader.read_array(asset->mvmtStanceSoundSets, 7, asset->numClothTypes);
		reader.read_array(asset->meleeAttackVMSoundSets, 7, asset->numClothTypes);
		reader.read_array(asset->meleeAttackWMSoundSets, 7, asset->numClothTypes);
		reader.read_array(asset->meleeCharacterSoundSets, 7, asset->numClothTypes);
		reader.pop_stream();
	}
}
