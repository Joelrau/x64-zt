#include <std_include.hpp>
#include "watersystem.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_water_system(Walk walk, WaterSystem& asset)
		{
			walk.asset(ASSET_TYPE_MATERIAL, asset.material);
			walk.asset(ASSET_TYPE_IMAGE, asset.image);

			auto& data = asset.data;
			if (const auto sets = walk.array(data.imageSets, data.count, 7))
			{
				for (auto i = 0u; i < data.count; i++)
				{
					for (auto& image : sets[i].images)
					{
						walk.asset(ASSET_TYPE_IMAGE, image);
					}
				}
			}

			walk.array(data.grid, data.countB * data.countA, 3);
			walk.array(data.data, data.count, 15);
			walk.array(data.data2, data.count, 3);
		}
	}

	void water_system::read(zone_reader& reader, WaterSystem* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_water_system(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void water_system::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_water_system(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void water_system::dump(WaterSystem* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("watersystem\\"s + asset->name + ".watersystemb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_water_system(dump_walk{dump}, *asset);
		dump.close();
	}

	WaterSystem* water_system::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("watersystem\\"s + name + ".watersystemb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing watersystem \"%s\"...", name.data());

		auto* asset = read.read_single<WaterSystem>();
		asset->name = read.read_string();
		walk_water_system(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void water_system::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WaterSystem>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void water_system::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_water_system(depend_walk{zone}, *this->asset_);
	}

	std::string water_system::name()
	{
		return this->name_;
	}

	std::int32_t water_system::type()
	{
		return ASSET_TYPE_WATER_SYSTEM;
	}
}
