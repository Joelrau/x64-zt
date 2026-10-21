#include <std_include.hpp>
#include "equipsndtable.hpp"

namespace zonetool::iw6
{
	EquipmentSoundTable* equip_snd_table::parse(const std::string& name, zone_memory* mem)
	{
		auto* asset = mem->allocate<EquipmentSoundTable>();
		asset->name = mem->duplicate_string(name);
		return asset;
	}

	void equip_snd_table::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Missing equipsndtable asset...");
		}
	}

	void equip_snd_table::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void equip_snd_table::load_depending(zone_base* zone)
	{
	}

	std::string equip_snd_table::name()
	{
		return this->name_;
	}

	std::int32_t equip_snd_table::type()
	{
		return ASSET_TYPE_EQUIPMENT_SND_TABLE;
	}

	namespace
	{
		void read_sound_sets(zone_reader& reader, EquipSoundSetMoveTypes& move_types, const unsigned int count)
		{
			if (const auto sets = reader.read_array(move_types.soundSets, 3, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_name_reference(sets[i].soundPLR);
					reader.read_name_reference(sets[i].soundNPC);
				}
			}
		}

		void read_sound_set_array(zone_reader& reader, EquipSoundSetMoveTypes*& field, const unsigned int count,
			const unsigned int move_type_count)
		{
			if (const auto move_types = reader.read_array(field, 3, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					read_sound_sets(reader, move_types[i], move_type_count);
				}
			}
		}

		void read_chances(zone_reader& reader, EquipmentChanceRattleTypes*& field, const EquipmentSoundTable* asset)
		{
			const auto rattle_types = reader.read_array(field, 3, asset->numClothTypes);
			for (auto i = 0u; rattle_types && i < asset->numClothTypes; i++)
			{
				const auto move_types = reader.read_array(rattle_types[i].chances, 3, asset->numWeaponRattleTypes);
				for (auto j = 0u; move_types && j < asset->numWeaponRattleTypes; j++)
				{
					reader.read_array(move_types[j].chances, 3, asset->numMoveTypes);
				}
			}
		}
	}

	void equip_snd_table::read(zone_reader& reader, EquipmentSoundTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto cloth = reader.read_array(asset->clothTypes, 3, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				reader.read_string(cloth[i].szName);
			}
		}

		if (const auto rattles = reader.read_array(asset->weaponRattleTypes, 3, asset->numWeaponRattleTypes))
		{
			for (auto i = 0u; i < asset->numWeaponRattleTypes; i++)
			{
				reader.read_string(rattles[i].szName);
			}
		}

		read_chances(reader, asset->chancesPLR, asset);
		read_chances(reader, asset->chancesNPC, asset);
		read_sound_set_array(reader, asset->mvmtClothSoundSets, asset->numClothTypes, asset->numMoveTypes);
		read_sound_set_array(reader, asset->mvmtRattleSoundSets, asset->numWeaponRattleTypes, asset->numMoveTypes);
		read_sound_sets(reader, asset->mvmtAccentSoundSets, asset->numMoveTypes);

		if (const auto mantle = reader.read_array(asset->mvmtMantleSoundSets, 3, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				for (auto& set : mantle[i].soundSets)
				{
					reader.read_name_reference(set.soundPLR);
					reader.read_name_reference(set.soundNPC);
				}
			}
		}

		reader.pop_stream();
	}

	void equip_snd_table::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		// won't bother adding this, not used.

		buf->pop_stream();
	}

	void equip_snd_table::dump(EquipmentSoundTable* asset)
	{
	}
}
