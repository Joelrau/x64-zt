#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "mapents.hpp"
#include "xmodel.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace clipmap
	{
		namespace
		{
			template <typename Destination, typename Source>
			void copy_layout(Destination& destination, const Source& source)
			{
				static_assert(sizeof(Destination) == sizeof(Source));
				std::memcpy(&destination, &source, sizeof(Destination));
			}
		}

		zonetool::iw5::clipMap_t* convert(zonetool::iw4::clipMap_t* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw5::clipMap_t>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(isInUse);

			auto& info = new_asset->info;
			info.planeCount = asset->planeCount;
			info.planes.value = asset->planes.value;

			info.numMaterials = asset->numMaterials;
			static_assert(sizeof(zonetool::iw5::ClipMaterial) == sizeof(zonetool::iw4::dmaterial_t));
			info.materials.value = asset->materials.value;

			info.numBrushSides = asset->numBrushSides;
			static_assert(sizeof(zonetool::iw5::cbrushside_t) == sizeof(zonetool::iw4::cbrushside_t));
			info.brushsides.value = asset->brushsides.value;

			info.numBrushEdges = asset->numBrushEdges;
			info.brushEdges.value = asset->brushEdges.value;

			info.leafbrushNodesCount = asset->leafbrushNodesCount;
			static_assert(sizeof(zonetool::iw5::cLeafBrushNode_s) == sizeof(zonetool::iw4::cLeafBrushNode_s));
			info.leafbrushNodes.value = asset->leafbrushNodes.value;

			info.numLeafBrushes = asset->numLeafBrushes;
			info.leafbrushes.value = asset->leafbrushes.value;

			info.numBrushes = asset->numBrushes;
			static_assert(sizeof(zonetool::iw5::cbrush_t) == sizeof(zonetool::iw4::cbrush_t));
			info.brushes.value = asset->brushes.value;
			info.brushBounds.value = asset->brushBounds.value;
			info.brushContents.value = asset->brushContents.value;

			new_asset->pInfo = &new_asset->info;

			COPY_VALUE(numStaticModels);
			const auto static_models = allocator.allocate<zonetool::iw5::cStaticModel_s>(asset->numStaticModels);
			std::unordered_map<zonetool::iw4::XModel*, zonetool::iw5::XModel*> converted_models;
			for (unsigned int i = 0; i < asset->numStaticModels; i++)
			{
				const auto& source = asset->staticModelList[i];
				auto& model = static_models[i];

				if (const auto source_model = source.xmodel.get())
				{
					auto& converted = converted_models[source_model];
					if (!converted)
					{
						converted = xmodel::convert(source_model, allocator);
					}
					model.xmodel = converted;
				}

				std::memcpy(model.origin, source.origin, sizeof(model.origin));
				std::memcpy(model.invScaledAxis, source.invScaledAxis, sizeof(model.invScaledAxis));
				copy_layout(model.absBounds, source.absBounds);
			}
			new_asset->staticModelList = static_models;

			COPY_VALUE(numNodes);
			COPY_POINTER_RAW(nodes);

			COPY_VALUE(numLeafs);
			static_assert(sizeof(zonetool::iw5::cLeaf_t) == sizeof(zonetool::iw4::cLeaf_t));
			COPY_POINTER_RAW(leafs);

			COPY_VALUE(vertCount);
			COPY_POINTER_RAW(verts);
			COPY_VALUE(triCount);
			COPY_POINTER_RAW(triIndices);
			COPY_POINTER_RAW(triEdgeIsWalkable);
			COPY_VALUE(borderCount);
			COPY_POINTER_RAW(borders);

			COPY_VALUE(partitionCount);
			static_assert(sizeof(zonetool::iw5::CollisionPartition) == sizeof(zonetool::iw4::CollisionPartition));
			COPY_POINTER_RAW(partitions);

			COPY_VALUE(aabbTreeCount);
			static_assert(sizeof(zonetool::iw5::CollisionAabbTree) == sizeof(zonetool::iw4::CollisionAabbTree));
			COPY_POINTER_RAW(aabbTrees);

			new_asset->numSubModels = asset->numSubModels;
			const auto cmodels = allocator.allocate<zonetool::iw5::cmodel_t>(asset->numSubModels);
			for (unsigned int i = 0; i < asset->numSubModels; i++)
			{
				copy_layout(cmodels[i].bounds, asset->cmodels[i].bounds);
				cmodels[i].radius = asset->cmodels[i].radius;
				cmodels[i].info = &new_asset->info;
				copy_layout(cmodels[i].leaf, asset->cmodels[i].leaf);
			}
			new_asset->cmodels = cmodels;

			const auto map_ents = mapents::convert(asset->mapEnts.get(), allocator);
			new_asset->mapEnts = map_ents;

			new_asset->stageCount = static_cast<unsigned char>(asset->mapEnts->stageCount);
			const auto stages = allocator.allocate<zonetool::iw5::Stage>(new_asset->stageCount);
			for (auto i = 0; i < new_asset->stageCount; i++)
			{
				const auto& source = asset->mapEnts->stages[i];
				stages[i].name.value = source.name.value;
				std::memcpy(stages[i].origin, source.origin, sizeof(stages[i].origin));
				stages[i].triggerIndex = source.triggerIndex;
				stages[i].sunPrimaryLightIndex = static_cast<unsigned char>(source.sunPrimaryLightIndex);
			}
			new_asset->stages = stages;

			new_asset->smodelNodeCount = asset->smodelNodeCount;
			static_assert(sizeof(zonetool::iw5::SModelAabbNode) == sizeof(zonetool::iw4::SModelAabbNode));
			COPY_POINTER_RAW(smodelNodes);

			for (auto list = 0; list < 2; list++)
			{
				const auto count = asset->dynEntCount[list];
				new_asset->dynEntCount[list] = count;

				const auto definitions = allocator.allocate<zonetool::iw5::DynEntityDef>(count);
				const auto poses = allocator.allocate<zonetool::iw5::DynEntityPose>(count);
				const auto clients = allocator.allocate<zonetool::iw5::DynEntityClient>(count);
				const auto collisions = allocator.allocate<zonetool::iw5::DynEntityColl>(count);

				for (unsigned int i = 0; i < count; i++)
				{
					const auto& source = asset->dynEntDefList[list][i];
					auto& definition = definitions[i];

					definition.type = static_cast<zonetool::iw5::DynEntityType>(source.type);
					copy_layout(definition.pose, source.pose);
					definition.xModel.value = source.xModel.value;
					definition.brushModel = source.brushModel;
					definition.physicsBrushModel = source.physicsBrushModel;
					definition.destroyFx.value = source.destroyFx.value;
					definition.physPreset.value = source.physPreset.value;
					definition.health = source.health;
					copy_layout(definition.mass, source.mass);
					definition.contents = source.contents;

					copy_layout(poses[i], asset->dynEntPoseList[list][i]);

					const auto& source_client = asset->dynEntClientList[list][i];
					clients[i].physObjId = source_client.physObjId;
					clients[i].flags = source_client.flags;
					clients[i].lightingHandle = source_client.lightingHandle;
					clients[i].health = source_client.health;

					copy_layout(collisions[i], asset->dynEntCollList[list][i]);
				}

				new_asset->dynEntDefList[list] = definitions;
				new_asset->dynEntPoseList[list] = poses;
				new_asset->dynEntClientList[list] = clients;
				new_asset->dynEntCollList[list] = collisions;
			}

			COPY_VALUE(checksum);

			return new_asset;
		}
	}
}
