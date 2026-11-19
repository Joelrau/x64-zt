#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "havok.hpp"
#include "mapents.hpp"

#include <iw8/assets/clipmap.hpp>

namespace zonetool::iw7::converter::iw8::clipmap
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::Stage* convert_stages(const clipMap_t* asset, utils::memory::allocator& allocator)
		{
			auto* stages = allocator.allocate_array<target::Stage>(asset->stageCount);
			for (auto i = 0; i < asset->stageCount; i++)
			{
				const auto& stage = asset->stages[i];
				auto& converted = stages[i];
				converted.name = stage.name;
				converted.origin[0] = stage.origin[0];
				converted.origin[1] = stage.origin[1];
				converted.origin[2] = stage.origin[2];
				converted.triggerIndex = stage.triggerIndex;
				converted.sunPrimaryLightIndex = static_cast<unsigned char>(stage.sunPrimaryLightIndex);
				converted.entityUID = stage.entityUID;
				converted.skyRotationAngles[0] = stage.skyRotationAngles[0];
				converted.skyRotationAngles[1] = stage.skyRotationAngles[1];
				converted.skyRotationAngles[2] = stage.skyRotationAngles[2];
			}

			return stages;
		}

		target::CollisionHeatmapEntry* convert_heatmap(const clipMap_t* asset, utils::memory::allocator& allocator)
		{
			auto* entries = allocator.allocate_array<target::CollisionHeatmapEntry>(asset->numCollisionHeatmapEntries);
			for (auto i = 0u; i < asset->numCollisionHeatmapEntries; i++)
			{
				const auto& source = asset->collisionHeatmap[i];
				auto& converted = entries[i];
				converted.vertexCount = source.vertexCount;
				converted.vertexDensity = source.vertexDensity;
				for (auto axis = 0; axis < 3; axis++)
				{
					converted.minExtent[axis] = source.minExtent[axis];
					converted.maxExtent[axis] = source.maxExtent[axis];
				}
			}

			return entries;
		}
	}

	void dump(clipMap_t* asset)
	{
		utils::memory::allocator allocator;

		auto* clip = allocator.allocate<target::clipMap_t>();
		clip->name = asset->name;
		clip->isInUse = asset->isInUse;

		if (asset->mapEnts)
		{
			clip->mapEnts = allocator.allocate<target::MapEnts>();
			clip->mapEnts->name = asset->mapEnts->name;
		}

		clip->stageCount = asset->stageCount;
		clip->stages = convert_stages(asset, allocator);
		mapents::convert_triggers(asset->stageTrigger, clip->stageTrigger, allocator);

		clip->broadphaseMin[0] = asset->broadphaseMin[0];
		clip->broadphaseMin[1] = asset->broadphaseMin[1];
		clip->broadphaseMin[2] = asset->broadphaseMin[2];
		clip->broadphaseMax[0] = asset->broadphaseMax[0];
		clip->broadphaseMax[1] = asset->broadphaseMax[1];
		clip->broadphaseMax[2] = asset->broadphaseMax[2];
		clip->physicsCapacities = std::bit_cast<target::PhysicsCapacities>(asset->physicsCapacities);

		clip->numCollisionHeatmapEntries = asset->numCollisionHeatmapEntries;
		clip->collisionHeatmap = convert_heatmap(asset, allocator);

		if (asset->topDownMapData)
		{
			clip->topDownMapData = allocator.allocate<target::TopDownMapData>();
			std::memcpy(clip->topDownMapData, asset->topDownMapData, sizeof(target::TopDownMapData));
		}

		clip->totalGlassInitPieceCount = 0;
		clip->totalGlassPieceLimit = 0;
		clip->visionSetName = nullptr;
		clip->checksum = asset->checksum;

		std::string world_shape;
		try
		{
			world_shape = havok::convert_world_shape(std::string_view(asset->havokWorldShapeData, asset->havokWorldShapeDataSize));
		}
		catch (const std::exception& e)
		{
			ZONETOOL_ERROR("%s: %s", asset->name, e.what());
		}

		clip->havokWorldShapeDataSize = static_cast<unsigned int>(world_shape.size());
		clip->havokWorldShapeData = world_shape.data();

		target::clip_map::dump(clip);
		add_csv_row("col_map", asset->name);
	}
}
