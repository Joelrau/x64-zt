#include <std_include.hpp>
#include "comworld.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_com_world(Walk walk, ComWorld& asset)
		{
			if (const auto lights = walk.array(asset.primaryLights, asset.primaryLightCount, 7))
			{
				for (auto i = 0u; i < asset.primaryLightCount; i++)
				{
					walk.string(lights[i].defName);
				}
			}

			walk.array(asset.transientTable, asset.transientTableSize, 1);
			walk.string(asset.changeListInfo.userName);

			if (const auto names = walk.array(asset.umbraGateNames, asset.numUmbraGates, 7))
			{
				for (auto i = 0u; i < asset.numUmbraGates; i++)
				{
					walk.string(names[i]);
				}
			}
		}
	}

	void com_world::read(zone_reader& reader, ComWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_com_world(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void com_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_com_world(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void com_world::dump(ComWorld* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".commap"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_com_world(dump_walk{dump}, *asset);
		dump.close();
	}

	ComWorld* com_world::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".commap"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing comworld \"%s\"...", name.data());

		auto* asset = read.read_single<ComWorld>();
		asset->name = read.read_string();
		walk_com_world(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void com_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<ComWorld>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string com_world::name()
	{
		return this->name_;
	}

	std::int32_t com_world::type()
	{
		return ASSET_TYPE_COMWORLD;
	}
}
