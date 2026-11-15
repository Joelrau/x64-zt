#include <std_include.hpp>
#include "fogspline.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_fog_spline(Walk walk, GfxFogSpline& asset)
		{
			walk.asset(ASSET_TYPE_IMAGE, asset.image);
		}
	}

	void fog_spline::read(zone_reader& reader, GfxFogSpline* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_fog_spline(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void fog_spline::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_fog_spline(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void fog_spline::dump(GfxFogSpline* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("fogspline\\"s + asset->name + ".fogsplineb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_fog_spline(dump_walk{dump}, *asset);
		dump.close();
	}

	GfxFogSpline* fog_spline::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("fogspline\\"s + name + ".fogsplineb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing fogspline \"%s\"...", name.data());

		auto* asset = read.read_single<GfxFogSpline>();
		asset->name = read.read_string();
		walk_fog_spline(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void fog_spline::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<GfxFogSpline>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void fog_spline::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_fog_spline(depend_walk{zone}, *this->asset_);
	}

	std::string fog_spline::name()
	{
		return this->name_;
	}

	std::int32_t fog_spline::type()
	{
		return ASSET_TYPE_FOG_SPLINE;
	}
}
