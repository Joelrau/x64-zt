#include <std_include.hpp>
#include "clothasset.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_cloth(Walk walk, ClothAsset& asset)
		{
			walk.array(asset.havokData, asset.havokDataSize, 15);

			if (const auto sets = walk.array(asset.boneMappingSets, asset.boneMappingSetCount, 7))
			{
				for (auto i = 0u; i < asset.boneMappingSetCount; i++)
				{
					if (const auto mappings = walk.array(sets[i].boneMappings, sets[i].boneMappingCount, 7))
					{
						for (auto j = 0u; j < sets[i].boneMappingCount; j++)
						{
							walk.script_strings(mappings[j].scrTagNames, mappings[j].scrTagCount);
						}
					}
				}
			}

			walk.string(asset.animationStateName);

			if (const auto lods = walk.array(asset.simulationLods, asset.simulationLodCount, 7))
			{
				for (auto i = 0u; i < asset.simulationLodCount; i++)
				{
					walk.string(lods[i].stateName);
				}
			}

			if (const auto sounds = walk.array(asset.sounds, asset.soundCount, 7))
			{
				for (auto i = 0u; i < asset.soundCount; i++)
				{
					walk.string(sounds[i].soundAlias);
				}
			}
		}
	}

	void cloth_asset::read(zone_reader& reader, ClothAsset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_cloth(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void cloth_asset::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_cloth(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void cloth_asset::dump(ClothAsset* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("clothasset\\"s + asset->name + ".clothassetb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_cloth(dump_walk{dump}, *asset);
		dump.close();
	}

	ClothAsset* cloth_asset::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("clothasset\\"s + name + ".clothassetb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing clothasset \"%s\"...", name.data());

		auto* asset = read.read_single<ClothAsset>();
		asset->name = read.read_string();
		walk_cloth(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void cloth_asset::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<ClothAsset>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void cloth_asset::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_cloth(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string cloth_asset::name()
	{
		return this->name_;
	}

	std::int32_t cloth_asset::type()
	{
		return ASSET_TYPE_CLOTH_ASSET;
	}
}
