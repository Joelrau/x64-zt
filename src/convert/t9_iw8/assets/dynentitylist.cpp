#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "dynentitylist.hpp"

#include <iw8/assets/dynentitylist.hpp>

namespace zonetool::t9::converter::iw8::dynentitylist
{
	namespace
	{
		namespace target = zonetool::iw8;

		unsigned int converted_count = 0;

		template <typename T>
		T* make_array(const unsigned int count, utils::memory::allocator& allocator)
		{
			return count ? allocator.allocate_array<T>(count) : nullptr;
		}

		const DynModel* resolve_dyn_model(const DynModel* asset)
		{
			if (asset->model1)
			{
				return asset;
			}

			const auto* loaded = static_cast<const DynModel*>(db_find_asset(ASSET_TYPE_DYNMODEL, xhash_name(asset->name)));
			return loaded ? loaded : asset;
		}

		XModel* resolve_model(XModel* asset)
		{
			auto* loaded = static_cast<XModel*>(db_find_asset(ASSET_TYPE_XMODEL, xhash_name(asset->name)));
			return loaded ? loaded : asset;
		}

		void copy_pose(const ClipMapDynModel& placement, target::GfxPlacement& pose)
		{
			const auto length = std::sqrt(placement.quat[0] * placement.quat[0] + placement.quat[1] * placement.quat[1]
				+ placement.quat[2] * placement.quat[2] + placement.quat[3] * placement.quat[3]);
			for (auto i = 0; i < 4; i++)
			{
				pose.quat[i] = length > 0.0f ? placement.quat[i] / length : i == 3 ? 1.0f : 0.0f;
			}

			for (auto i = 0; i < 3; i++)
			{
				pose.origin[i] = placement.origin[i];
			}
		}

		target::DynEntityDef make_def(const char* model_name, const ClipMapDynModel& placement, const unsigned int index,
			utils::memory::allocator& allocator)
		{
			auto* model = allocator.allocate<target::XModel>();
			model->name = allocator.duplicate_string(model_name);

			target::DynEntityDef def{};
			def.baseModel = model;
			copy_pose(placement, def.initialPose);
			def.spatialNode.spatialNodeNext = index ? index - 1 : 0xFFFFFFFF;
			def.type = target::DYNENT_TYPE_CLUTTER;
			def.clientId[0] = 0xFFFF;
			def.clientId[1] = 0xFFFF;
			def.physicsAssetRigidBodiesCount = 1;
			def.noPhysics = true;
			def.collisionFlags = 2;
			return def;
		}

		target::SpatialPartition_TransientPopulation* make_transient_population(const unsigned int count,
			utils::memory::allocator& allocator)
		{
			auto* population = allocator.allocate<target::SpatialPartition_TransientPopulation>();
			if (!count)
			{
				return population;
			}

			population->partitionCount = 1;
			population->currentPopulation = count;
			population->partitionIds = allocator.allocate<unsigned int>();
			population->buckets = allocator.allocate<target::SpatialPartition_Population_Node>();
			population->buckets->spatialNodeNext = count - 1;
			population->bucketTails = allocator.allocate<target::SpatialPartition_Population_Node>();
			population->bucketTails->spatialNodeNext = 0;
			return population;
		}

		target::SpatialPartition_Population* make_population(const target::Bounds& bounds, utils::memory::allocator& allocator)
		{
			auto* tree = allocator.allocate<target::SpatialPartition_Population_Tree>();
			tree->partitionCount = 1;
			tree->partitions = allocator.allocate<target::SpatialPartition_Population_Partition>();
			tree->partitions->axis = 3; // leaf
			tree->extents = allocator.allocate<target::ExtentBounds>();
			for (auto i = 0; i < 3; i++)
			{
				tree->extents->mins[i] = bounds.midPoint[i] - bounds.halfSize[i];
				tree->extents->maxs[i] = bounds.midPoint[i] + bounds.halfSize[i];
			}

			auto* population = allocator.allocate<target::SpatialPartition_Population>();
			population->tree = tree;
			population->partitionCount = 1;
			population->buckets = allocator.allocate<target::SpatialPartition_Population_Node>();
			population->buckets->spatialNodeNext = 0xFFFFFFFF;
			return population;
		}
	}

	unsigned int dump(const ClipMap* asset)
	{
		converted_count = 0;
		if (!asset || !asset->dynModels || !asset->dynModelCount)
		{
			return 0;
		}

		utils::memory::allocator allocator;
		const auto clip_name = xhash_name(asset->name);
		std::vector<target::DynEntityDef> defs;
		auto placements = 0u;
		auto fx_only = 0u;
		auto unconverted = 0u;
		for (auto i = 0u; i < asset->dynModelCount; i++)
		{
			const auto& placement = asset->dynModels[i];
			if (!placement.dynModel)
			{
				continue;
			}

			placements++;
			const auto* dyn_model = resolve_dyn_model(placement.dynModel);
			if (!dyn_model->model1)
			{
				fx_only++;
				continue;
			}

			auto* model = resolve_model(dyn_model->model1);
			const auto* info = xmodel::dump_static(model);
			if (!info)
			{
				unconverted++;
				continue;
			}

			if (placement.scale != 1.0f)
			{
				ZONETOOL_WARNING("dynmodel \"%s\" (%s): IW8 dynents have no scale, scale %g is dropped", xhash_name(dyn_model->name).data(),
					info->name.data(), placement.scale);
			}

			defs.emplace_back(make_def(info->name.data(), placement, static_cast<unsigned int>(defs.size()), allocator));
		}

		ZONETOOL_INFO("clipmap \"%s\": %u dynmodel placements in %u slots, %zu dynent defs, %u without a model, %u without a converted xmodel",
			clip_name.data(), placements, static_cast<unsigned>(asset->dynModelCount), defs.size(), fx_only, unconverted);

		if (defs.empty())
		{
			return 0;
		}

		const auto count = static_cast<unsigned int>(defs.size());
		auto* list = allocator.allocate<target::DynEntityList>();
		list->name = allocator.duplicate_string("dynentitylist0");
		list->dynEntCount[0] = count;
		list->dynEntCountTotal = count;
		list->dynEntDefList[0] = allocator.allocate_array<target::DynEntityDef>(count);
		std::ranges::copy(defs, list->dynEntDefList[0]);
		list->dynEntSpatialPopulation[0] = make_transient_population(count, allocator);
		list->dynEntSpatialPopulation[1] = make_transient_population(0, allocator);
		list->dynEntNoSpatialList.spatialNodeNext = 0xFFFFFFFF;
		list->dynEntNoSpatialListTail.spatialNodeNext = 0xFFFFFFFF;

		target::dyn_entity_list::dump(list);
		add_csv_row("dynentitylist", list->name);

		converted_count = count;
		return count;
	}

	void fill_map_ents(target::MapEnts* map_ents, const unsigned int count, const target::Bounds& world_bounds,
		utils::memory::allocator& allocator)
	{
		for (auto& population : map_ents->dynEntSpatialPopulation)
		{
			population = make_population(world_bounds, allocator);
		}

		map_ents->dynEntNoSpatialList.spatialNodeNext = 0xFFFFFFFF;
		if (!count)
		{
			return;
		}

		map_ents->dynEntCount[0] = count;
		map_ents->dynEntCountTotal = count;
		map_ents->dynEntityListsCount = 1;
		map_ents->dynEntListIds = allocator.allocate<target::DynEntityListId>();
		map_ents->dynEntMaxClientHistoryCount = 10000;
		map_ents->dynEntMaxPosePartCount = 1;
	}

	void fill_world(target::GfxWorld* world, utils::memory::allocator& allocator)
	{
		if (!converted_count)
		{
			return;
		}

		const unsigned int client_counts[2]{converted_count + 10, 10};
		auto& dpvs = world->dpvsDyn;
		for (auto i = 0; i < 2; i++)
		{
			const auto count = client_counts[i];
			const auto word_count = 2 * ((count + 31) >> 5);
			dpvs.dynEntClientCount[i] = count;
			dpvs.dynEntClientWordCount[i] = word_count;
			dpvs.dynEntCellBits[i] = make_array<unsigned int>(word_count * world->dpvsPlanes.cellCount, allocator);
			for (auto slot = 0; slot < 12; slot++)
			{
				if (slot != 3)
				{
					dpvs.dynEntVisData[i][slot] = make_array<unsigned char>(32 * word_count, allocator);
				}
			}

			world->dynEntMotionBitsEntries[i] = word_count;
			world->dynEntMotionBits[i] = make_array<unsigned int>(word_count, allocator);
			world->numPrimaryLightDynEntShadowVisEntries[i] = (world->shadowBitsArrayPitch * count) >> 4;
			world->primaryLightDynEntShadowVis[i] = make_array<unsigned int>(world->numPrimaryLightDynEntShadowVisEntries[i], allocator);
		}

		world->sceneDynModel = make_array<target::GfxSceneDynModel>(client_counts[0], allocator);
		world->sceneDynBrush = make_array<target::GfxSceneDynBrush>(client_counts[1], allocator);
	}
}
