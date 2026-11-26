#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "havok.hpp"

#include <iw8/assets/clipmap.hpp>
#include <iw8/assets/comworld.hpp>

namespace zonetool::s4::converter::iw8::clipmap
{
	namespace target = zonetool::iw8;

	namespace
	{
		std::vector<const CollisionTile*> collect_tiles()
		{
			std::vector<const CollisionTile*> tiles;
			db_enum_assets(ASSET_TYPE_COLLISION_TILE, [&](void* header)
			{
				tiles.push_back(static_cast<const CollisionTile*>(header));
			});
			return tiles;
		}

		havok::tile_shape_map get_tile_shapes(const std::vector<const CollisionTile*>& tiles)
		{
			havok::tile_shape_map shapes;
			for (const auto* tile : tiles)
			{
				if (tile->havokTileShapeData)
				{
					shapes.try_emplace(tile->index, tile->havokTileShapeData, tile->havokTileShapeDataSize);
				}
			}

			return shapes;
		}

		const char* get_static_model_name(const StaticModelCollisionCompressedModel& model)
		{
			const auto* name = model.detailCollision ? model.detailCollision->name : model.physicsAsset->name;
			return name[0] == ',' ? name + 1 : name;
		}

		target::StaticModelCollisionModelList* convert_static_models(const std::vector<const CollisionTile*>& tiles, utils::memory::allocator& allocator)
		{
			std::vector<const StaticModelCollisionCompressedModel*> models;
			std::vector<std::vector<target::StaticModelCollisionInstance>> instances;
			std::map<std::pair<const void*, const void*>, std::size_t> model_indices;
			std::size_t instance_count = 0;

			for (const auto* tile : tiles)
			{
				const auto* list = tile->staticModelCollision;
				for (auto i = 0u; list && list->models && i < list->numModels; i++)
				{
					const auto& model = list->models[i];
					if (!model.detailCollision && !model.physicsAsset)
					{
						continue;
					}

					const auto [entry, added] = model_indices.try_emplace({model.detailCollision, model.physicsAsset}, models.size());
					if (added)
					{
						models.push_back(&model);
						instances.emplace_back();
					}

					for (auto j = 0u; model.instances && j < model.numInstances; j++)
					{
						const auto& compressed = model.instances[j];
						auto& instance = instances[entry->second].emplace_back();
						for (auto k = 0; k < 3; k++)
						{
							instance.origin[k] = tile->origin[k] + half_float::half_to_float(compressed.origin[k]);
							instance.angles[k] = half_float::half_to_float(compressed.angles[k]);
						}

						instance.scale = half_float::half_to_float(compressed.scale);
						instance_count++;
					}
				}
			}

			if (models.empty())
			{
				return nullptr;
			}

			const auto list = allocator.allocate<target::StaticModelCollisionModelList>();
			list->numModels = static_cast<unsigned int>(models.size());
			list->structureMemoryCost = static_cast<unsigned int>(sizeof(target::StaticModelCollisionModelList)
				+ models.size() * sizeof(target::StaticModelCollisionModel) + instance_count * sizeof(target::StaticModelCollisionInstance));
			list->models = allocator.allocate_array<target::StaticModelCollisionModel>(models.size());
			for (auto i = 0u; i < models.size(); i++)
			{
				auto& model = list->models[i];
				model.name = static_cast<target::scr_string_t>(SL_GetString(get_static_model_name(*models[i])));
				model.physicsAsset = name_reference<target::PhysicsAsset>(models[i]->physicsAsset, allocator);
				model.detailCollision = name_reference<target::XModelDetailCollision>(models[i]->detailCollision, allocator);
				model.numInstances = static_cast<unsigned int>(instances[i].size());
				model.instances = allocator.allocate_array<target::StaticModelCollisionInstance>(instances[i].size());
				std::memcpy(model.instances, instances[i].data(), instances[i].size() * sizeof(target::StaticModelCollisionInstance));
			}

			return list;
		}
	}

	void dump(clipMap_t* asset)
	{
		static_assert(sizeof(target::MapTriggers) == sizeof(MapTriggers));
		static_assert(sizeof(target::PhysicsCapacities) == sizeof(PhysicsCapacities));
		static_assert(sizeof(target::CollisionHeatmapEntry) == sizeof(CollisionHeatmapEntry));
		static_assert(sizeof(target::TopDownMapDataHeader) == sizeof(TopDownMapDataHeader));

		utils::memory::allocator allocator;
		const auto clip = allocator.allocate<target::clipMap_t>();
		clip->name = bsp_name(asset->name, allocator);

		if (asset->mapEnts)
		{
			clip->mapEnts = allocator.allocate<target::MapEnts>();
			clip->mapEnts->name = bsp_name(asset->mapEnts->name, allocator);
		}

		clip->stageCount = asset->stageCount;
		clip->stages = allocator.allocate_array<target::Stage>(asset->stageCount);
		for (auto i = 0; asset->stages && i < asset->stageCount; i++)
		{
			const auto& stage = asset->stages[i];
			std::memcpy(clip->stages[i].origin, stage.origin, sizeof(stage.origin));
			clip->stages[i].triggerIndex = stage.triggerIndex;
			clip->stages[i].sunPrimaryLightIndex = stage.sunPrimaryLightIndex;
			std::memcpy(clip->stages[i].skyRotationAngles, stage.skyRotationAngles, sizeof(stage.skyRotationAngles));
		}

		std::memcpy(&clip->stageTrigger, &asset->stageTrigger, sizeof(clip->stageTrigger));
		std::memcpy(clip->broadphaseMin, asset->broadphaseMin, sizeof(clip->broadphaseMin));
		std::memcpy(clip->broadphaseMax, asset->broadphaseMax, sizeof(clip->broadphaseMax));
		clip->physicsCapacities = std::bit_cast<target::PhysicsCapacities>(asset->physicsCapacities);
		const auto tiles = collect_tiles();
		const auto tile_shapes = get_tile_shapes(tiles);
		const auto world_shape = havok::convert_world_shape_list(std::string_view(asset->havokWorldShapeData, asset->havokWorldShapeDataSize), tile_shapes, asset->name);
		if (world_shape)
		{
			clip->havokWorldShapeDataSize = static_cast<unsigned int>(world_shape->size());
			clip->havokWorldShapeData = allocator.allocate_array<char>(world_shape->size());
			std::memcpy(clip->havokWorldShapeData, world_shape->data(), world_shape->size());
		}
		if (const auto static_models = convert_static_models(tiles, allocator))
		{
			clip->numStaticModelCollisionModelLists = 1;
			clip->staticModelCollisionModelLists = static_models;
		}

		clip->numCollisionHeatmapEntries = asset->numCollisionHeatmapEntries;
		clip->collisionHeatmap = reinterpret_cast<target::CollisionHeatmapEntry*>(asset->collisionHeatmap);
		clip->totalGlassInitPieceCount = asset->totalGlassInitPieceCount;
		clip->totalGlassPieceLimit = asset->totalGlassPieceLimit;

		if (asset->topDownMapData)
		{
			clip->topDownMapData = allocator.allocate<target::TopDownMapData>();
			std::memcpy(clip->topDownMapData, asset->topDownMapData, sizeof(TopDownMapData));
		}

		clip->visionSetName = asset->visionSetName;
		clip->checksum = asset->checksum;

		target::clip_map::dump(clip);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_CLIPMAP), clip->name);
	}
}

namespace zonetool::s4::converter::iw8::comworld
{
	namespace target = zonetool::iw8;

	void dump(ComWorld* asset)
	{
		static_assert(sizeof(target::ComPrimaryLight) == sizeof(ComPrimaryLight));
		static_assert(sizeof(target::ComChangeListInfo) == sizeof(ComChangeListInfo));

		utils::memory::allocator allocator;
		const auto world = allocator.allocate<target::ComWorld>();
		world->name = bsp_name(asset->name, allocator);
		world->isInUse = asset->isInUse;
		world->useForwardPlus = asset->useForwardPlus;
		world->bakeQuality = asset->bakeQuality;
		world->proxyLODQuality = asset->proxyLODQuality;
		std::memcpy(world->trVisRadii, asset->trVisRadii, sizeof(world->trVisRadii));
		world->trVisFacingDistAdd = asset->trVisFacingDistAdd;
		world->primaryLightCount = asset->primaryLightCount;
		world->primaryLights = reinterpret_cast<target::ComPrimaryLight*>(asset->primaryLights);
		world->scriptablePrimaryLightCount = asset->scriptablePrimaryLightCount;
		world->firstScriptablePrimaryLight = asset->firstScriptablePrimaryLight;
		world->transientTableSize = asset->transientTableSize;
		world->transientTable = asset->transientTable;
		world->changeListInfo = std::bit_cast<target::ComChangeListInfo>(asset->changeListInfo);
		world->numUmbraGates = asset->numUmbraGates;
		world->umbraGateNames = asset->umbraGateNames;
		std::memcpy(world->umbraGateInitialStates, asset->umbraGateInitialStates, sizeof(world->umbraGateInitialStates));

		target::com_world::dump(world);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_COMWORLD), world->name);
	}
}
