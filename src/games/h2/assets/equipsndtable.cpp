#include <std_include.hpp>
#include "equipsndtable.hpp"

namespace zonetool::h2
{
	EquipmentSoundTable* equip_snd_table::parse(const std::string& name, zone_memory* mem)
	{
		// equipsndtable,soundaliases/equipment_snd.def is only asset in h1 and it doesn't have anything in it.
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
		void read_chances(zone_reader& reader, EquipmentChanceRattleTypes*& field, const EquipmentSoundTable* asset)
		{
			const auto rattle_types = reader.read_array(field, 3, asset->numClothTypes);
			if (!rattle_types)
			{
				return;
			}

			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				const auto move_types = reader.read_array(rattle_types[i].chances, 3, asset->numWeaponRattleTypes);
				if (!move_types)
				{
					continue;
				}

				for (auto j = 0u; j < asset->numWeaponRattleTypes; j++)
				{
					reader.read_array(move_types[j].chances, 3, asset->numMoveTypes);
				}
			}
		}

		void read_sound_set(zone_reader& reader, EquipmentSoundSet& set)
		{
			reader.read_name_reference(set.soundPLR);
			reader.read_name_reference(set.soundNPC);
		}

		void read_move_types(zone_reader& reader, EquipSoundSetMoveTypes& move_types, const unsigned int count)
		{
			if (const auto sets = reader.read_array(move_types.soundSets, 3, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					read_sound_set(reader, sets[i]);
				}
			}
		}

		void read_move_types_array(zone_reader& reader, EquipSoundSetMoveTypes*& field, const unsigned int count,
			const unsigned int move_count)
		{
			if (const auto move_types = reader.read_array(field, 3, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					read_move_types(reader, move_types[i], move_count);
				}
			}
		}

		void write_chances(zone_buffer* buf, EquipmentChanceRattleTypes* data, EquipmentChanceRattleTypes*& field,
			const EquipmentSoundTable* asset)
		{
			if (!data)
			{
				return;
			}

			buf->align(3);
			const auto rattle_types = buf->write(data, asset->numClothTypes);
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				if (!data[i].chances)
				{
					continue;
				}

				buf->align(3);
				const auto move_types = buf->write(data[i].chances, asset->numWeaponRattleTypes);
				for (auto j = 0u; j < asset->numWeaponRattleTypes; j++)
				{
					if (data[i].chances[j].chances)
					{
						buf->align(3);
						buf->write(data[i].chances[j].chances, asset->numMoveTypes);
						buf->clear_pointer(&move_types[j].chances);
					}
				}

				buf->clear_pointer(&rattle_types[i].chances);
			}

			buf->clear_pointer(&field);
		}

		void write_sound_alias_name(zone_buffer* buf, const snd_alias_list_t* data, snd_alias_list_t*& field)
		{
			if (!data)
			{
				return;
			}

			buf->align(7);
			const auto ptr = reinterpret_cast<const char**>(buf->write(&buf->data_following));
			*ptr = buf->write_str(data->name);
			buf->clear_pointer(&field);
		}

		void write_sound_set(zone_buffer* buf, EquipmentSoundSet& data, EquipmentSoundSet& dest)
		{
			write_sound_alias_name(buf, data.soundPLR, dest.soundPLR);
			write_sound_alias_name(buf, data.soundNPC, dest.soundNPC);
		}

		void write_move_types(zone_buffer* buf, EquipSoundSetMoveTypes& data, EquipSoundSetMoveTypes& dest,
			const unsigned int count)
		{
			if (!data.soundSets)
			{
				return;
			}

			buf->align(3);
			const auto sets = buf->write(data.soundSets, count);
			for (auto i = 0u; i < count; i++)
			{
				write_sound_set(buf, data.soundSets[i], sets[i]);
			}

			buf->clear_pointer(&dest.soundSets);
		}

		void write_move_types_array(zone_buffer* buf, EquipSoundSetMoveTypes* data, EquipSoundSetMoveTypes*& field,
			const unsigned int count, const unsigned int move_count)
		{
			if (!data)
			{
				return;
			}

			buf->align(3);
			const auto move_types = buf->write(data, count);
			for (auto i = 0u; i < count; i++)
			{
				write_move_types(buf, data[i], move_types[i], move_count);
			}

			buf->clear_pointer(&field);
		}
	}

	void equip_snd_table::read(zone_reader& reader, EquipmentSoundTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto cloth_types = reader.read_array(asset->clothTypes, 3, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				reader.read_string(cloth_types[i].szName);
			}
		}

		if (const auto rattle_types = reader.read_array(asset->weaponRattleTypes, 3, asset->numWeaponRattleTypes))
		{
			for (auto i = 0u; i < asset->numWeaponRattleTypes; i++)
			{
				reader.read_string(rattle_types[i].szName);
			}
		}

		read_chances(reader, asset->chancesPLR, asset);
		read_chances(reader, asset->chancesNPC, asset);
		read_move_types_array(reader, asset->mvmtClothSoundSets, asset->numClothTypes, asset->numMoveTypes);
		read_move_types_array(reader, asset->mvmtRattleSoundSets, asset->numWeaponRattleTypes, asset->numMoveTypes);
		read_move_types(reader, asset->mvmtAccentSoundSets, asset->numMoveTypes);

		if (const auto mantle_types = reader.read_array(asset->mvmtMantleSoundSets, 3, asset->numClothTypes))
		{
			for (auto i = 0u; i < asset->numClothTypes; i++)
			{
				for (auto& set : mantle_types[i].soundSets)
				{
					read_sound_set(reader, set);
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

		if (data->clothTypes)
		{
			buf->align(3);
			const auto cloth_types = buf->write(data->clothTypes, data->numClothTypes);
			for (auto i = 0u; i < data->numClothTypes; i++)
			{
				cloth_types[i].szName = buf->write_str(data->clothTypes[i].szName);
			}
			buf->clear_pointer(&dest->clothTypes);
		}

		if (data->weaponRattleTypes)
		{
			buf->align(3);
			const auto rattle_types = buf->write(data->weaponRattleTypes, data->numWeaponRattleTypes);
			for (auto i = 0u; i < data->numWeaponRattleTypes; i++)
			{
				rattle_types[i].szName = buf->write_str(data->weaponRattleTypes[i].szName);
			}
			buf->clear_pointer(&dest->weaponRattleTypes);
		}

		write_chances(buf, data->chancesPLR, dest->chancesPLR, data);
		write_chances(buf, data->chancesNPC, dest->chancesNPC, data);
		write_move_types_array(buf, data->mvmtClothSoundSets, dest->mvmtClothSoundSets, data->numClothTypes, data->numMoveTypes);
		write_move_types_array(buf, data->mvmtRattleSoundSets, dest->mvmtRattleSoundSets, data->numWeaponRattleTypes, data->numMoveTypes);
		write_move_types(buf, data->mvmtAccentSoundSets, dest->mvmtAccentSoundSets, data->numMoveTypes);

		if (data->mvmtMantleSoundSets)
		{
			buf->align(3);
			const auto mantle_types = buf->write(data->mvmtMantleSoundSets, data->numClothTypes);
			for (auto i = 0u; i < data->numClothTypes; i++)
			{
				for (auto j = 0u; j < std::size(mantle_types[i].soundSets); j++)
				{
					write_sound_set(buf, data->mvmtMantleSoundSets[i].soundSets[j], mantle_types[i].soundSets[j]);
				}
			}
			buf->clear_pointer(&dest->mvmtMantleSoundSets);
		}

		buf->pop_stream();
	}

	void equip_snd_table::dump(EquipmentSoundTable* asset)
	{
	}
}
