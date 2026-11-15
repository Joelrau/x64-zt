#include <std_include.hpp>
#include "clipmap.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_static_model_collision(zone_reader& reader, clipMap_t* asset)
		{
			const auto lists = reader.read_array(asset->staticModelCollisionModelLists, 7, asset->numStaticModelCollisionModelLists);
			if (!lists)
			{
				return;
			}

			for (auto i = 0u; i < asset->numStaticModelCollisionModelLists; i++)
			{
				const auto models = reader.read_array(lists[i].models, 7, lists[i].numModels);
				if (!models)
				{
					continue;
				}

				for (auto j = 0u; j < lists[i].numModels; j++)
				{
					auto& model = models[j];
					reader.read_script_string(model.name);
					reader.read_asset(ASSET_TYPE_PHYSICSASSET, model.physicsAsset);
					reader.read_asset(ASSET_TYPE_XMODEL_DETAIL_COLLISION, model.detailCollision);
					reader.read_array(model.instances, 3, model.numInstances);
				}
			}
		}
	}

	void clip_map::read(zone_reader& reader, clipMap_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_static_model_collision(reader, asset);
		reader.read_asset(ASSET_TYPE_MAP_ENTS, asset->mapEnts);

		if (const auto stages = reader.read_array(asset->stages, 7, asset->stageCount))
		{
			for (auto i = 0u; i < asset->stageCount; i++)
			{
				reader.read_string(stages[i].name);
			}
		}

		read_map_triggers(reader, asset->stageTrigger);
		reader.read_array(asset->havokWorldShapeData, 15, asset->havokWorldShapeDataSize);
		reader.read_array(asset->collisionHeatmap, 3, asset->numCollisionHeatmapEntries);
		reader.read_single(asset->topDownMapData, 3);
		reader.read_string(asset->visionSetName);
		reader.pop_stream();
	}
}
