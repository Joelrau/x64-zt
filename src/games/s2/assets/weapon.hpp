#pragma once

namespace zonetool::s2
{
	class weapon_def : public asset_base<WeaponCompleteDef, ASSET_TYPE_WEAPON>
	{
	public:
		static void read(zone_reader& reader, WeaponCompleteDef* asset);

		WeaponCompleteDef* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(WeaponCompleteDef* asset);
	};

	std::optional<std::string> weapon_field_name(std::uint16_t offset);
	std::optional<std::uint16_t> weapon_field_offset(const std::string& name);

	class laser_def : public asset_base<LaserDef, ASSET_TYPE_LASER>
	{
	public:
		static void read(zone_reader& reader, LaserDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->laserMaterial);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->laserLightMaterial);
			reader.read_asset(ASSET_TYPE_FX, asset->effect);
			reader.read_asset(ASSET_TYPE_LASER, asset->altLaser);
			reader.read_script_string(asset->ownerTag);
			reader.pop_stream();
		}

		LaserDef* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_LASER, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing laser \"%s\"...", name.data());

			auto* asset = read.read_single<LaserDef>();
			asset->name = read.read_string();
			asset->laserMaterial = read.read_asset<Material>();
			asset->laserLightMaterial = read.read_asset<Material>();
			asset->effect = read.read_asset<FxEffectDef>();
			asset->altLaser = read.read_asset<LaserDef>();
			asset->ownerTag = parse_script_string(read);
			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			script_string_preparer prepare(buf);
			prepare(this->asset_->ownerTag);
		}

		void load_depending(zone_base* zone) override
		{
			add_dependency(zone, ASSET_TYPE_MATERIAL, this->asset_->laserMaterial);
			add_dependency(zone, ASSET_TYPE_MATERIAL, this->asset_->laserLightMaterial);
			add_dependency(zone, ASSET_TYPE_FX, this->asset_->effect);
			static thread_local std::unordered_set<std::string> loading;
			const auto* alt_laser = this->asset_->altLaser;
			if (alt_laser && alt_laser->name && !loading.contains(alt_laser->name))
			{
				loading.insert(this->name());
				add_dependency(zone, ASSET_TYPE_LASER, alt_laser);
				loading.erase(this->name());
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);
			dest->laserMaterial = asset_reference(zone, ASSET_TYPE_MATERIAL, data->laserMaterial);
			dest->laserLightMaterial = asset_reference(zone, ASSET_TYPE_MATERIAL, data->laserLightMaterial);
			dest->effect = asset_reference(zone, ASSET_TYPE_FX, data->effect);
			dest->altLaser = asset_reference(zone, ASSET_TYPE_LASER, data->altLaser);
			buf->pop_stream();
		}

		static void dump(LaserDef* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_LASER, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_asset(asset->laserMaterial);
			write.dump_asset(asset->laserLightMaterial);
			write.dump_asset(asset->effect);
			write.dump_asset(asset->altLaser);
			dump_script_string(write, asset->ownerTag);
			write.close();
		}
	};

	class beam_def : public asset_base<BeamDef, ASSET_TYPE_BEAM>
	{
	public:
		static void read(zone_reader& reader, BeamDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->beamMaterial);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->beamMaterial2);
			for (auto& effect : asset->effects)
			{
				reader.read_asset(ASSET_TYPE_FX, effect);
			}
			for (auto& model : asset->models)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, model);
			}
			reader.read_string(asset->string664);
			reader.read_string(asset->string672);
			for (auto& sound : asset->sounds)
			{
				reader.read_name_reference(sound);
			}
			for (auto& sound : asset->sounds2)
			{
				reader.read_name_reference(sound);
			}
			reader.pop_stream();
		}

		BeamDef* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_BEAM, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing beam \"%s\"...", name.data());

			auto* asset = read.read_single<BeamDef>();
			asset->name = read.read_string();
			asset->beamMaterial = read.read_asset<Material>();
			asset->beamMaterial2 = read.read_asset<Material>();
			for (auto& effect : asset->effects)
			{
				effect = read.read_asset<FxEffectDef>();
			}
			for (auto& model : asset->models)
			{
				model = read.read_asset<XModel>();
			}
			asset->string664 = read.read_string();
			asset->string672 = read.read_string();
			for (auto& sound : asset->sounds)
			{
				sound = read.read_asset<snd_alias_list_t>();
			}
			for (auto& sound : asset->sounds2)
			{
				sound = read.read_asset<snd_alias_list_t>();
			}
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			auto* data = this->asset_;
			add_dependency(zone, ASSET_TYPE_MATERIAL, data->beamMaterial);
			add_dependency(zone, ASSET_TYPE_MATERIAL, data->beamMaterial2);
			add_dependencies(zone, ASSET_TYPE_FX, data->effects, std::size(data->effects));
			add_dependencies(zone, ASSET_TYPE_XMODEL, data->models, std::size(data->models));
			add_dependencies(zone, ASSET_TYPE_SOUND, data->sounds, std::size(data->sounds));
			add_dependencies(zone, ASSET_TYPE_SOUND, data->sounds2, std::size(data->sounds2));
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);
			dest->beamMaterial = asset_reference(zone, ASSET_TYPE_MATERIAL, data->beamMaterial);
			dest->beamMaterial2 = asset_reference(zone, ASSET_TYPE_MATERIAL, data->beamMaterial2);
			for (auto i = 0u; i < std::size(data->effects); i++)
			{
				dest->effects[i] = asset_reference(zone, ASSET_TYPE_FX, data->effects[i]);
			}
			for (auto i = 0u; i < std::size(data->models); i++)
			{
				dest->models[i] = asset_reference(zone, ASSET_TYPE_XMODEL, data->models[i]);
			}
			write_string(buf, data->string664, dest->string664);
			write_string(buf, data->string672, dest->string672);
			for (auto i = 0u; i < std::size(data->sounds); i++)
			{
				write_sound(buf, data->sounds[i], dest->sounds[i]);
			}
			for (auto i = 0u; i < std::size(data->sounds2); i++)
			{
				write_sound(buf, data->sounds2[i], dest->sounds2[i]);
			}
			buf->pop_stream();
		}

		static void dump(BeamDef* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_BEAM, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_asset(asset->beamMaterial);
			write.dump_asset(asset->beamMaterial2);
			for (auto* effect : asset->effects)
			{
				write.dump_asset(effect);
			}
			for (auto* model : asset->models)
			{
				write.dump_asset(model);
			}
			write.dump_string(asset->string664);
			write.dump_string(asset->string672);
			for (auto* sound : asset->sounds)
			{
				write.dump_asset(sound);
			}
			for (auto* sound : asset->sounds2)
			{
				write.dump_asset(sound);
			}
			write.close();
		}
	};
}
