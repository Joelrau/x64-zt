#pragma once

namespace zonetool::s2
{
	struct weapon_layout
	{
		template <typename V>
		static void visit(const V& w);
	};

	class weapon_def : public field_layout_asset<WeaponCompleteDef, ASSET_TYPE_WEAPON, weapon_layout>
	{
	};

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

	struct beam_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(BeamDef, name));
			w.asset(ASSET_TYPE_MATERIAL, offsetof(BeamDef, beamMaterial));
			w.asset(ASSET_TYPE_MATERIAL, offsetof(BeamDef, beamMaterial2));
			w.assets(ASSET_TYPE_FX, offsetof(BeamDef, effects), std::size(BeamDef{}.effects));
			w.assets(ASSET_TYPE_XMODEL, offsetof(BeamDef, models), std::size(BeamDef{}.models));
			w.strings({offsetof(BeamDef, string664), offsetof(BeamDef, string672)});
			w.sounds(offsetof(BeamDef, sounds), std::size(BeamDef{}.sounds));
			w.sounds(offsetof(BeamDef, sounds2), std::size(BeamDef{}.sounds2));
		}
	};

	class beam_def : public field_layout_asset<BeamDef, ASSET_TYPE_BEAM, beam_layout>
	{
	};
}
