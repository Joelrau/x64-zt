#include <std_include.hpp>
#include "glassworld.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_glass_world(Walk walk, GlassWorld& asset)
		{
			if (const auto data = walk.array(asset.g_glassData, 1, 7))
			{
				walk.array(data->glassPieces, data->pieceCount, 3);

				if (const auto names = walk.array(data->glassNames, data->glassNameCount, 7))
				{
					for (auto i = 0u; i < data->glassNameCount; i++)
					{
						walk.string(names[i].nameStr);
						walk.script_string(names[i].name);
						walk.array(names[i].pieceIndices, names[i].pieceCount, 1);
					}
				}

				walk_spatial_tree(walk, data->spatialTree);
			}
		}
	}

	void glass_world::read(zone_reader& reader, GlassWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_glass_world(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void glass_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_glass_world(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void glass_world::dump(GlassWorld* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".glassmap"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_glass_world(dump_walk{dump}, *asset);
		dump.close();
	}

	GlassWorld* glass_world::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".glassmap"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing glassworld \"%s\"...", name.data());

		auto* asset = read.read_single<GlassWorld>();
		asset->name = read.read_string();
		walk_glass_world(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void glass_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<GlassWorld>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void glass_world::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_glass_world(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string glass_world::name()
	{
		return this->name_;
	}

	std::int32_t glass_world::type()
	{
		return ASSET_TYPE_GLASSWORLD;
	}
}
