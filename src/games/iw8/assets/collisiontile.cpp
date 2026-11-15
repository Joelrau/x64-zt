#include <std_include.hpp>
#include "collisiontile.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_static_model_collision(zone_reader& reader, StaticModelCollisionCompressedModelList& list)
		{
			const auto models = reader.read_array(list.models, 7, list.numModels);
			if (!models)
			{
				return;
			}

			for (auto i = 0u; i < list.numModels; i++)
			{
				auto& model = models[i];
				reader.read_script_string(model.name);
				reader.read_asset(ASSET_TYPE_PHYSICSASSET, model.physicsAsset);
				reader.read_asset(ASSET_TYPE_XMODEL_DETAIL_COLLISION, model.detailCollision);
				reader.read_array(model.instances, 1, model.numInstances);
			}
		}
	}

	void collision_tile::read(zone_reader& reader, CollisionTile* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto list = reader.read_single(asset->staticModelCollision, 7))
		{
			read_static_model_collision(reader, *list);
		}
		reader.read_array(asset->havokTileShapeData, 15, asset->havokTileShapeDataSize);
		reader.read_array(asset->havokTileShapePointer, 0, 1);
		reader.pop_stream();
	}
}
