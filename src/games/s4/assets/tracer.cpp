#include <std_include.hpp>
#include "tracer.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_tracer(Walk walk, TracerDef& asset)
		{
			for (auto& effect : asset.effects)
			{
				walk.asset(ASSET_TYPE_VFX, effect.particleSystemDef);
			}

			walk.asset(ASSET_TYPE_MATERIAL, asset.material);
		}
	}

	void tracer_def::read(zone_reader& reader, TracerDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_tracer(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void tracer_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_tracer(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void tracer_def::dump(TracerDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("tracer\\"s + asset->name + ".tracerb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_tracer(dump_walk{dump}, *asset);
		dump.close();
	}

	TracerDef* tracer_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("tracer\\"s + name + ".tracerb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing tracer \"%s\"...", name.data());

		auto* asset = read.read_single<TracerDef>();
		asset->name = read.read_string();
		walk_tracer(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void tracer_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<TracerDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void tracer_def::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_tracer(depend_walk{zone}, *this->asset_);
	}

	std::string tracer_def::name()
	{
		return this->name_;
	}

	std::int32_t tracer_def::type()
	{
		return ASSET_TYPE_TRACER;
	}
}
