#include <std_include.hpp>
#include "gradingclut.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_grading_clut(Walk walk, GfxGradingClut& asset)
		{
			walk.asset(ASSET_TYPE_IMAGE, asset.hdrClutImage);
		}
	}

	void grading_clut::read(zone_reader& reader, GfxGradingClut* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_grading_clut(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void grading_clut::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_grading_clut(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void grading_clut::dump(GfxGradingClut* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("gradingclut\\"s + asset->name + ".gradingclutb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_grading_clut(dump_walk{dump}, *asset);
		dump.close();
	}

	GfxGradingClut* grading_clut::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("gradingclut\\"s + name + ".gradingclutb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gradingclut \"%s\"...", name.data());

		auto* asset = read.read_single<GfxGradingClut>();
		asset->name = read.read_string();
		walk_grading_clut(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void grading_clut::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<GfxGradingClut>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void grading_clut::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_grading_clut(depend_walk{zone}, *this->asset_);
	}

	std::string grading_clut::name()
	{
		return this->name_;
	}

	std::int32_t grading_clut::type()
	{
		return ASSET_TYPE_GRADING_CLUT;
	}
}
