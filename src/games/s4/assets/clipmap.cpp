#include <std_include.hpp>
#include "clipmap.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_clip_map(Walk walk, clipMap_t& asset)
		{
			walk.asset(ASSET_TYPE_MAP_ENTS, asset.mapEnts);
			walk.array(asset.stages, asset.stageCount, 3);
			walk_map_triggers(walk, asset.stageTrigger);
			walk.array(asset.havokWorldShapeData, asset.havokWorldShapeDataSize, 15);
			walk.array(asset.collisionHeatmap, asset.numCollisionHeatmapEntries, 3);
			walk.array(asset.topDownMapData, 1, 3);
			walk.string(asset.visionSetName);
		}
	}

	void clip_map::read(zone_reader& reader, clipMap_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_clip_map(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void clip_map::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_clip_map(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void clip_map::dump(clipMap_t* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".colmap"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_clip_map(dump_walk{dump}, *asset);
		dump.close();
	}

	clipMap_t* clip_map::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".colmap"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing clipmap \"%s\"...", name.data());

		auto* asset = read.read_single<clipMap_t>();
		asset->name = read.read_string();
		walk_clip_map(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void clip_map::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<clipMap_t>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void clip_map::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_clip_map(depend_walk{zone}, *this->asset_);
	}

	void clip_map::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_clip_map(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string clip_map::name()
	{
		return this->name_;
	}

	std::int32_t clip_map::type()
	{
		return ASSET_TYPE_CLIPMAP;
	}
}
