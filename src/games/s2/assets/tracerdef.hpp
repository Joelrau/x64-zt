#pragma once

namespace zonetool::s2
{
	class tracer_def : public asset_base<TracerDef, ASSET_TYPE_TRACER>
	{
	public:
		static void read(zone_reader& reader, TracerDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
			reader.read_asset(ASSET_TYPE_FX, asset->effect);
			reader.pop_stream();
		}

		TracerDef* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_TRACER, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing tracer \"%s\"...", name.data());

			auto* asset = read.read_single<TracerDef>();
			asset->name = read.read_string();
			asset->material = read.read_asset<Material>();
			asset->effect = read.read_asset<FxEffectDef>();
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			add_dependency(zone, ASSET_TYPE_MATERIAL, this->asset_->material);
			add_dependency(zone, ASSET_TYPE_FX, this->asset_->effect);
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			dest->material = asset_reference(zone, ASSET_TYPE_MATERIAL, data->material);
			dest->effect = asset_reference(zone, ASSET_TYPE_FX, data->effect);

			buf->pop_stream();
		}

		static void dump(TracerDef* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_TRACER, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_asset(asset->material);
			write.dump_asset(asset->effect);
			write.close();
		}
	};
}
