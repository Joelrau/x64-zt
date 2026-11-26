#include <std_include.hpp>
#include "collisiontile.hpp"
#include "asset_util.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_static_model_collision(Walk walk, StaticModelCollisionCompressedModelList& list)
		{
			const auto models = walk.array(list.models, list.numModels, 7);
			if (!models)
			{
				return;
			}

			for (auto i = 0u; i < list.numModels; i++)
			{
				auto& model = models[i];
				walk.script_string(model.name);
				walk.asset(ASSET_TYPE_PHYSICSASSET, model.physicsAsset);
				walk.asset(ASSET_TYPE_XMODEL_DETAIL_COLLISION, model.detailCollision);
				walk.array(model.instances, model.numInstances, 1);
			}
		}

		template <typename Walk>
		void walk_collision_tile(Walk walk, CollisionTile& asset)
		{
			if (const auto list = walk.array(asset.staticModelCollision, 1, 7))
			{
				walk_static_model_collision(walk, *list);
			}

			walk.array(asset.havokTileShapeData, asset.havokTileShapeDataSize, 15);
			walk.array(asset.havokTileShapePointer, 1, 0);
		}
	}

	void collision_tile::read(zone_reader& reader, CollisionTile* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_collision_tile(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void collision_tile::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_collision_tile(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void collision_tile::dump(CollisionTile* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("collisiontile\\"s + asset->name + ".collisiontileb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_collision_tile(dump_walk{dump}, *asset);
		dump.close();
	}

	CollisionTile* collision_tile::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("collisiontile\\"s + name + ".collisiontileb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing collisiontile \"%s\"...", name.data());

		auto* asset = read.read_single<CollisionTile>();
		asset->name = read.read_string();
		walk_collision_tile(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void collision_tile::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<CollisionTile>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void collision_tile::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_collision_tile(depend_walk{zone}, *this->asset_);
	}

	void collision_tile::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_collision_tile(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string collision_tile::name()
	{
		return this->name_;
	}

	std::int32_t collision_tile::type()
	{
		return ASSET_TYPE_COLLISION_TILE;
	}
}
