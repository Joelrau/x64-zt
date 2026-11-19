#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "havok.hpp"
#include "mapents.hpp"

#include <iw7/assets/clipmap.hpp>

namespace zonetool::iw8::converter::iw7::clipmap
{
	namespace
	{
		namespace target = zonetool::iw7;

		target::Stage* convert_stages(const clipMap_t* asset, utils::memory::allocator& allocator)
		{
			auto* stages = allocator.allocate_array<target::Stage>(asset->stageCount);
			for (auto i = 0; i < asset->stageCount; i++)
			{
				const auto& stage = asset->stages[i];
				stages[i].name = stage.name;
				std::memcpy(stages[i].origin, stage.origin, sizeof(stages[i].origin));
				stages[i].triggerIndex = stage.triggerIndex;
				stages[i].sunPrimaryLightIndex = stage.sunPrimaryLightIndex;
				stages[i].luminanceScale = 0.001f;
				stages[i].entityUID = stage.entityUID;
				std::memcpy(stages[i].skyRotationAngles, stage.skyRotationAngles, sizeof(stages[i].skyRotationAngles));
			}

			return stages;
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

		std::memcpy(clip->broadphaseMin, asset->broadphaseMin, sizeof(clip->broadphaseMin));
		std::memcpy(clip->broadphaseMax, asset->broadphaseMax, sizeof(clip->broadphaseMax));
		clip->physicsCapacities = std::bit_cast<target::PhysicsCapacities>(asset->physicsCapacities);

		clip->numCollisionHeatmapEntries = asset->numCollisionHeatmapEntries;
		clip->collisionHeatmap = allocator.allocate_array<target::CollisionHeatmapEntry>(asset->numCollisionHeatmapEntries);
		for (auto i = 0u; i < asset->numCollisionHeatmapEntries; i++)
		{
			clip->collisionHeatmap[i] = std::bit_cast<target::CollisionHeatmapEntry>(asset->collisionHeatmap[i]);
		}

		if (asset->topDownMapData)
		{
			clip->topDownMapData = allocator.allocate<target::TopDownMapData>();
			*clip->topDownMapData = std::bit_cast<target::TopDownMapData>(*asset->topDownMapData);
		}

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
