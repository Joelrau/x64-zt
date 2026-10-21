#pragma once

namespace zonetool::s2
{
	class vehicle_def : public asset_base<VehicleDef, ASSET_TYPE_VEHICLE>
	{
	public:
		static void read(zone_reader& reader, VehicleDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);

			reader.read_string(asset->name);
			reader.read_string(asset->unk_0x10);
			reader.read_string(asset->unk_0x20);
			for (auto& id : asset->unk_0x154)
			{
				reader.read_script_string(id);
			}

			auto& phys = asset->vehPhysDef;
			reader.read_asset(ASSET_TYPE_PHYSPRESET, phys.physPreset);
			reader.read_string(phys.unk_0x10);
			reader.read_array(phys.physMass, 3, 1);
			reader.read_string(phys.unk_0x20);

			read_assets(reader, ASSET_TYPE_FX, asset->unk_0x348);
			read_assets(reader, ASSET_TYPE_FX, asset->unk_0x418);
			reader.read_string(asset->unk_0x560);
			reader.read_string(asset->unk_0x7F0);
			reader.read_asset(ASSET_TYPE_WEAPON, asset->unk_0x7F8);
			read_sounds(reader, asset->unk_0x868);
			for (auto& id : asset->unk_0x88C)
			{
				reader.read_script_string(id);
			}
			read_assets(reader, ASSET_TYPE_FX, asset->unk_0x8A0);
			read_assets(reader, ASSET_TYPE_MATERIAL, asset->unk_0x8B0);
			read_assets(reader, ASSET_TYPE_XANIM, asset->unk_0x8C8);
			read_sounds(reader, asset->unk_0x8E0);
			reader.read_script_string(asset->unk_0x90C);
			read_sounds(reader, asset->unk_0x910);
			reader.read_script_string(asset->unk_0x934);
			read_sounds(reader, asset->unk_0x938);
			read_sounds(reader, asset->unk_0x958);
			for (auto& entry : asset->unk_0x980)
			{
				reader.read_name_reference(entry.sound);
			}
			reader.read_string(asset->unk_0x9D0);
			read_sounds(reader, asset->unk_0x9D8);
			reader.read_string(asset->unk_0xBD8);

			reader.pop_stream();
		}

		VehicleDef* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_VEHICLE, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing vehicle \"%s\"...", name.data());

			auto* asset = read.read_single<VehicleDef>();
			asset->name = read.read_string();
			asset->unk_0x10 = read.read_string();
			asset->unk_0x20 = read.read_string();
			for (auto& id : asset->unk_0x154)
			{
				id = parse_script_string(read);
			}

			auto& phys = asset->vehPhysDef;
			phys.physPreset = read.read_asset<PhysPreset>();
			phys.unk_0x10 = read.read_string();
			parse_array(read, mem, phys.physMass, 1);
			phys.unk_0x20 = read.read_string();

			parse_assets(read, asset->unk_0x348);
			parse_assets(read, asset->unk_0x418);
			asset->unk_0x560 = read.read_string();
			asset->unk_0x7F0 = read.read_string();
			asset->unk_0x7F8 = nullptr;
			if (const auto* weapon_name = read.read_string())
			{
				asset->unk_0x7F8 = mem->allocate<WeaponCompleteDef>();
				asset->unk_0x7F8->szInternalName = weapon_name;
			}
			parse_assets(read, asset->unk_0x868);
			for (auto& id : asset->unk_0x88C)
			{
				id = parse_script_string(read);
			}
			parse_assets(read, asset->unk_0x8A0);
			parse_assets(read, asset->unk_0x8B0);
			parse_assets(read, asset->unk_0x8C8);
			parse_assets(read, asset->unk_0x8E0);
			asset->unk_0x90C = parse_script_string(read);
			parse_assets(read, asset->unk_0x910);
			asset->unk_0x934 = parse_script_string(read);
			parse_assets(read, asset->unk_0x938);
			parse_assets(read, asset->unk_0x958);
			for (auto& entry : asset->unk_0x980)
			{
				entry.sound = read.read_asset<snd_alias_list_t>();
			}
			asset->unk_0x9D0 = read.read_string();
			parse_assets(read, asset->unk_0x9D8);
			asset->unk_0xBD8 = read.read_string();

			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			auto* data = this->asset_;
			script_string_preparer prepare(buf);
			prepare(data->unk_0x154, std::size(data->unk_0x154));
			prepare(data->unk_0x88C, std::size(data->unk_0x88C));
			prepare(data->unk_0x90C);
			prepare(data->unk_0x934);
		}

		void load_depending(zone_base* zone) override
		{
			auto* data = this->asset_;
			add_dependency(zone, ASSET_TYPE_PHYSPRESET, data->vehPhysDef.physPreset);
			add_dependencies(zone, ASSET_TYPE_FX, data->unk_0x348, std::size(data->unk_0x348));
			add_dependencies(zone, ASSET_TYPE_FX, data->unk_0x418, std::size(data->unk_0x418));
			add_dependency(zone, ASSET_TYPE_WEAPON, data->unk_0x7F8);
			add_dependencies(zone, ASSET_TYPE_SOUND, data->unk_0x868, std::size(data->unk_0x868));
			add_dependencies(zone, ASSET_TYPE_FX, data->unk_0x8A0, std::size(data->unk_0x8A0));
			add_dependencies(zone, ASSET_TYPE_MATERIAL, data->unk_0x8B0, std::size(data->unk_0x8B0));
			add_dependencies(zone, ASSET_TYPE_XANIM, data->unk_0x8C8, std::size(data->unk_0x8C8));
			add_dependencies(zone, ASSET_TYPE_SOUND, data->unk_0x8E0, std::size(data->unk_0x8E0));
			add_dependencies(zone, ASSET_TYPE_SOUND, data->unk_0x910, std::size(data->unk_0x910));
			add_dependencies(zone, ASSET_TYPE_SOUND, data->unk_0x938, std::size(data->unk_0x938));
			add_dependencies(zone, ASSET_TYPE_SOUND, data->unk_0x958, std::size(data->unk_0x958));
			for (const auto& entry : data->unk_0x980)
			{
				add_dependency(zone, ASSET_TYPE_SOUND, entry.sound);
			}
			add_dependencies(zone, ASSET_TYPE_SOUND, data->unk_0x9D8, std::size(data->unk_0x9D8));
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			write_string(buf, data->unk_0x10, dest->unk_0x10);
			write_string(buf, data->unk_0x20, dest->unk_0x20);

			const auto& phys = data->vehPhysDef;
			auto& dest_phys = dest->vehPhysDef;
			dest_phys.physPreset = asset_reference(zone, ASSET_TYPE_PHYSPRESET, phys.physPreset);
			write_string(buf, phys.unk_0x10, dest_phys.unk_0x10);
			write_array(buf, 3, phys.physMass, 1, dest_phys.physMass);
			write_string(buf, phys.unk_0x20, dest_phys.unk_0x20);

			write_assets(zone, ASSET_TYPE_FX, data->unk_0x348, dest->unk_0x348);
			write_assets(zone, ASSET_TYPE_FX, data->unk_0x418, dest->unk_0x418);
			write_string(buf, data->unk_0x560, dest->unk_0x560);
			write_string(buf, data->unk_0x7F0, dest->unk_0x7F0);
			dest->unk_0x7F8 = asset_reference(zone, ASSET_TYPE_WEAPON, data->unk_0x7F8);
			write_sounds(buf, data->unk_0x868, dest->unk_0x868);
			write_assets(zone, ASSET_TYPE_FX, data->unk_0x8A0, dest->unk_0x8A0);
			write_assets(zone, ASSET_TYPE_MATERIAL, data->unk_0x8B0, dest->unk_0x8B0);
			write_assets(zone, ASSET_TYPE_XANIM, data->unk_0x8C8, dest->unk_0x8C8);
			write_sounds(buf, data->unk_0x8E0, dest->unk_0x8E0);
			write_sounds(buf, data->unk_0x910, dest->unk_0x910);
			write_sounds(buf, data->unk_0x938, dest->unk_0x938);
			write_sounds(buf, data->unk_0x958, dest->unk_0x958);
			for (auto i = 0u; i < std::size(data->unk_0x980); i++)
			{
				write_sound(buf, data->unk_0x980[i].sound, dest->unk_0x980[i].sound);
			}
			write_string(buf, data->unk_0x9D0, dest->unk_0x9D0);
			write_sounds(buf, data->unk_0x9D8, dest->unk_0x9D8);
			write_string(buf, data->unk_0xBD8, dest->unk_0xBD8);

			buf->pop_stream();
		}

		static void dump(VehicleDef* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_VEHICLE, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_string(asset->unk_0x10);
			write.dump_string(asset->unk_0x20);
			dump_script_strings(write, asset->unk_0x154, std::size(asset->unk_0x154));

			const auto& phys = asset->vehPhysDef;
			write.dump_asset(phys.physPreset);
			write.dump_string(phys.unk_0x10);
			write.dump_array(phys.physMass, 1);
			write.dump_string(phys.unk_0x20);

			dump_assets(write, asset->unk_0x348);
			dump_assets(write, asset->unk_0x418);
			write.dump_string(asset->unk_0x560);
			write.dump_string(asset->unk_0x7F0);
			write.dump_string(asset->unk_0x7F8 ? asset->unk_0x7F8->szInternalName : nullptr);
			dump_assets(write, asset->unk_0x868);
			dump_script_strings(write, asset->unk_0x88C, std::size(asset->unk_0x88C));
			dump_assets(write, asset->unk_0x8A0);
			dump_assets(write, asset->unk_0x8B0);
			dump_assets(write, asset->unk_0x8C8);
			dump_assets(write, asset->unk_0x8E0);
			dump_script_string(write, asset->unk_0x90C);
			dump_assets(write, asset->unk_0x910);
			dump_script_string(write, asset->unk_0x934);
			dump_assets(write, asset->unk_0x938);
			dump_assets(write, asset->unk_0x958);
			for (const auto& entry : asset->unk_0x980)
			{
				write.dump_asset(entry.sound);
			}
			write.dump_string(asset->unk_0x9D0);
			dump_assets(write, asset->unk_0x9D8);
			write.dump_string(asset->unk_0xBD8);

			write.close();
		}

	private:
		template <typename T, std::size_t N>
		static void read_assets(zone_reader& reader, const XAssetType type, T* (&assets)[N])
		{
			for (auto& asset : assets)
			{
				reader.read_asset(type, asset);
			}
		}

		template <std::size_t N>
		static void read_sounds(zone_reader& reader, snd_alias_list_t* (&sounds)[N])
		{
			for (auto& sound : sounds)
			{
				reader.read_name_reference(sound);
			}
		}

		template <typename T, std::size_t N>
		static void parse_assets(assetmanager::reader& read, T* (&assets)[N])
		{
			for (auto& asset : assets)
			{
				asset = read.read_asset<T>();
			}
		}

		template <typename T, std::size_t N>
		static void write_assets(zone_base* zone, const XAssetType type, T* const (&assets)[N], T* (&dest)[N])
		{
			for (auto i = 0u; i < N; i++)
			{
				dest[i] = asset_reference(zone, type, assets[i]);
			}
		}

		template <std::size_t N>
		static void write_sounds(zone_buffer* buf, snd_alias_list_t* const (&sounds)[N], snd_alias_list_t* (&dest)[N])
		{
			for (auto i = 0u; i < N; i++)
			{
				write_sound(buf, sounds[i], dest[i]);
			}
		}

		template <typename T, std::size_t N>
		static void dump_assets(assetmanager::dumper& write, T* const (&assets)[N])
		{
			for (auto* asset : assets)
			{
				write.dump_asset(asset);
			}
		}
	};
}
