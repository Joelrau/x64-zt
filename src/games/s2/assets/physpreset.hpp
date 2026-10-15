#pragma once

namespace zonetool::s2
{
	class phys_preset : public asset_base<PhysPreset, ASSET_TYPE_PHYSPRESET>
	{
	public:
		static void read(zone_reader& reader, PhysPreset* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_string(asset->sndAliasPrefix);
			for (auto& sound : asset->sounds)
			{
				reader.read_string(sound.name);
			}
			reader.read_asset(ASSET_TYPE_SNDPHYSPRESET, asset->sndPhysPreset);
			reader.pop_stream();
		}

		PhysPreset* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_PHYSPRESET, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing physpreset \"%s\"...", name.data());

			auto* asset = read.read_single<PhysPreset>();
			asset->name = read.read_string();
			asset->sndAliasPrefix = read.read_string();
			for (auto& sound : asset->sounds)
			{
				sound.name = read.read_string();
			}
			asset->sndPhysPreset = read.read_asset<SndPhysPreset>();
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			add_dependency(zone, ASSET_TYPE_SNDPHYSPRESET, this->asset_->sndPhysPreset);
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (data->sndAliasPrefix)
			{
				dest->sndAliasPrefix = buf->write_str(data->sndAliasPrefix);
			}
			for (auto i = 0; i < 3; i++)
			{
				if (data->sounds[i].name)
				{
					dest->sounds[i].name = buf->write_str(data->sounds[i].name);
				}
			}
			dest->sndPhysPreset = asset_reference(zone, ASSET_TYPE_SNDPHYSPRESET, data->sndPhysPreset);

			buf->pop_stream();
		}

		static void dump(PhysPreset* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_PHYSPRESET, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_string(asset->sndAliasPrefix);
			for (auto& sound : asset->sounds)
			{
				write.dump_string(sound.name);
			}
			write.dump_asset(asset->sndPhysPreset);
			write.close();
		}
	};

	class phys_water_preset : public asset_base<PhysWaterPreset, ASSET_TYPE_PHYSWATERPRESET>
	{
	public:
		static void read(zone_reader& reader, PhysWaterPreset* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			for (auto& fx : asset->fx)
			{
				reader.read_asset(ASSET_TYPE_FX, fx);
			}
			reader.pop_stream();
		}

		PhysWaterPreset* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_PHYSWATERPRESET, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing physwaterpreset \"%s\"...", name.data());

			auto* asset = read.read_single<PhysWaterPreset>();
			asset->name = read.read_string();
			for (auto& fx : asset->fx)
			{
				fx = read.read_asset<FxEffectDef>();
			}
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			for (const auto* fx : this->asset_->fx)
			{
				add_dependency(zone, ASSET_TYPE_FX, fx);
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			for (auto i = 0; i < 7; i++)
			{
				dest->fx[i] = asset_reference(zone, ASSET_TYPE_FX, data->fx[i]);
			}

			buf->pop_stream();
		}

		static void dump(PhysWaterPreset* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_PHYSWATERPRESET, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			for (auto* fx : asset->fx)
			{
				write.dump_asset(fx);
			}
			write.close();
		}
	};

	class phys_cloth_tuning : public asset_base<PhysClothTuning, ASSET_TYPE_PHYS_CLOTH_TUNING>
	{
	public:
		static void read(zone_reader& reader, PhysClothTuning* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_single(asset->data, 15);
			reader.pop_stream();
		}

		PhysClothTuning* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_PHYS_CLOTH_TUNING, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing physclothtuning \"%s\"...", name.data());

			auto* asset = read.read_single<PhysClothTuning>();
			asset->name = read.read_string();
			asset->data = read.read_single<PhysClothTuningData>();
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (data->data)
			{
				dest->data = buf->write_s(15, data->data);
			}

			buf->pop_stream();
		}

		static void dump(PhysClothTuning* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_PHYS_CLOTH_TUNING, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_single(asset->data);
			write.close();
		}
	};
}
