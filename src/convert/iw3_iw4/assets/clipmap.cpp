#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "mapents.hpp"
#include "xmodel.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace clipmap
	{
		namespace
		{
			void compute_bounds(zonetool::iw4::Bounds& bounds, const float* mins, const float* maxs)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					bounds.midPoint[axis] = (mins[axis] + maxs[axis]) * 0.5f;
					bounds.halfSize[axis] = (maxs[axis] - mins[axis]) * 0.5f;
				}
			}

			void convert_leaf(zonetool::iw4::cLeaf_t& leaf, const zonetool::iw3::cLeaf_t& source)
			{
				leaf.firstCollAabbIndex = source.firstCollAabbIndex;
				leaf.collAabbCount = source.collAabbCount;
				leaf.brushContents = source.brushContents;
				leaf.terrainContents = source.terrainContents;
				compute_bounds(leaf.bounds, source.mins, source.maxs);
				leaf.leafBrushNode = source.leafBrushNode;
			}

			void build_smodel_nodes(zonetool::iw4::clipMap_t* clipmap, x86::allocator& allocator)
			{
				float mins[3]{};
				float maxs[3]{};

				for (unsigned int i = 0; i < clipmap->numStaticModels; i++)
				{
					const auto& bounds = clipmap->staticModelList[i].absBounds;
					for (auto axis = 0; axis < 3; axis++)
					{
						const auto low = bounds.midPoint[axis] - bounds.halfSize[axis];
						const auto high = bounds.midPoint[axis] + bounds.halfSize[axis];
						mins[axis] = i == 0 ? low : std::min(mins[axis], low);
						maxs[axis] = i == 0 ? high : std::max(maxs[axis], high);
					}
				}

				const auto node = allocator.allocate<zonetool::iw4::SModelAabbNode>();
				compute_bounds(node->bounds, mins, maxs);
				node->firstChild = 0;
				node->childCount = static_cast<short>(clipmap->numStaticModels);

				clipmap->smodelNodeCount = 1;
				clipmap->smodelNodes = node;
			}

			void add_triggers(zonetool::iw4::clipMap_t* clipmap, zonetool::iw4::MapEnts* map_ents, x86::allocator& allocator)
			{
				if (!clipmap->cmodels)
				{
					return;
				}

				std::vector<zonetool::iw4::TriggerHull> hulls;
				std::vector<zonetool::iw4::TriggerSlab> slabs;

				const auto models = allocator.allocate<zonetool::iw4::TriggerModel>(clipmap->numSubModels);

				for (unsigned int i = 0; i < clipmap->numSubModels; i++)
				{
					const auto& cmodel = clipmap->cmodels[i];
					const auto contents = cmodel.leaf.brushContents | cmodel.leaf.terrainContents;

					models[i].contents = contents;
					models[i].firstHull = static_cast<unsigned short>(hulls.size());

					const auto node_index = cmodel.leaf.leafBrushNode;
					const zonetool::iw4::cLeafBrushNode_s* node = node_index >= 0 &&
						static_cast<unsigned int>(node_index) < clipmap->leafbrushNodesCount
						? &clipmap->leafbrushNodes[node_index] : nullptr;

					if (node && node->leafBrushCount > 0)
					{
						const auto brush_indices = node->data.leaf.brushes.get();
						for (auto j = 0; j < node->leafBrushCount; j++)
						{
							const auto brush_index = brush_indices[j];
							if (brush_index >= clipmap->numBrushes)
							{
								continue;
							}

							const auto& brush = clipmap->brushes[brush_index];

							zonetool::iw4::TriggerHull hull{};
							hull.bounds = clipmap->brushBounds[brush_index];
							hull.contents = clipmap->brushContents[brush_index];
							hull.firstSlab = static_cast<unsigned short>(slabs.size());

							for (unsigned int side = 0; side < brush.numsides; side++)
							{
								const auto plane = brush.sides[side].plane.get();

								zonetool::iw4::TriggerSlab slab{};
								std::memcpy(slab.dir, plane->normal, sizeof(slab.dir));
								slab.halfSize = plane->dist;
								slab.midPoint = 0.0f;
								slabs.push_back(slab);
							}

							hull.slabCount = static_cast<unsigned short>(slabs.size() - hull.firstSlab);
							hulls.push_back(hull);
						}
					}

					if (hulls.size() == models[i].firstHull)
					{
						zonetool::iw4::TriggerHull hull{};
						hull.bounds = cmodel.bounds;
						hull.contents = contents;
						hulls.push_back(hull);
					}

					models[i].hullCount = static_cast<unsigned short>(hulls.size() - models[i].firstHull);
				}

				map_ents->trigger.count = static_cast<int>(clipmap->numSubModels);
				map_ents->trigger.models = models;

				map_ents->trigger.hullCount = static_cast<int>(hulls.size());
				const auto hull_array = allocator.allocate<zonetool::iw4::TriggerHull>(hulls.size());
				std::memcpy(hull_array, hulls.data(), sizeof(zonetool::iw4::TriggerHull) * hulls.size());
				map_ents->trigger.hulls = hull_array;

				map_ents->trigger.slabCount = static_cast<int>(slabs.size());
				const auto slab_array = allocator.allocate<zonetool::iw4::TriggerSlab>(slabs.size());
				std::memcpy(slab_array, slabs.data(), sizeof(zonetool::iw4::TriggerSlab) * slabs.size());
				map_ents->trigger.slabs = slab_array;
			}

			void add_stage(zonetool::iw4::MapEnts* map_ents, x86::allocator& allocator)
			{
				const auto stage = allocator.allocate<zonetool::iw4::Stage>();
				stage->name = allocator.duplicate_string("stage 0");
				stage->triggerIndex = 0x400;
				stage->sunPrimaryLightIndex = 1;

				map_ents->stageCount = 1;
				map_ents->stages = stage;
			}
		}

		zonetool::iw4::clipMap_t* convert(zonetool::iw3::clipMap_t* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::clipMap_t>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(isInUse);

			COPY_VALUE(planeCount);
			COPY_POINTER_RAW(planes);

			new_asset->numStaticModels = asset->numStaticModels;
			const auto static_models = allocator.allocate<zonetool::iw4::cStaticModel_t>(asset->numStaticModels);
			std::unordered_map<zonetool::iw3::XModel*, zonetool::iw4::XModel*> converted_models;
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
				compute_bounds(model.absBounds, source.absmin, source.absmax);
			}
			new_asset->staticModelList = static_models;

			new_asset->numMaterials = asset->numMaterials;
			const auto materials = allocator.allocate<zonetool::iw4::dmaterial_t>(asset->numMaterials);
			for (unsigned int i = 0; i < asset->numMaterials; i++)
			{
				const auto& source = asset->materials[i];
				materials[i].material = allocator.duplicate_string(source.material);
				materials[i].surfaceFlags = source.surfaceFlags;
				materials[i].contentFlags = source.contentFlags;
			}
			new_asset->materials = materials;

			new_asset->numBrushSides = asset->numBrushSides;
			const auto sides = allocator.allocate<zonetool::iw4::cbrushside_t>(asset->numBrushSides);
			for (unsigned int i = 0; i < asset->numBrushSides; i++)
			{
				const auto& source = asset->brushsides[i];
				sides[i].plane.value = source.plane.value;
				sides[i].materialNum = static_cast<unsigned short>(source.materialNum);
				sides[i].firstAdjacentSideOffset = static_cast<char>(source.firstAdjacentSideOffset);
				sides[i].edgeCount = source.edgeCount;
			}
			new_asset->brushsides = sides;

			COPY_VALUE(numBrushEdges);
			COPY_POINTER_RAW(brushEdges);

			COPY_VALUE(numNodes);
			COPY_POINTER_RAW(nodes);

			new_asset->numLeafs = asset->numLeafs;
			const auto leafs = allocator.allocate<zonetool::iw4::cLeaf_t>(asset->numLeafs);
			for (unsigned int i = 0; i < asset->numLeafs; i++)
			{
				convert_leaf(leafs[i], asset->leafs[i]);
			}
			new_asset->leafs = leafs;

			COPY_VALUE(leafbrushNodesCount);
			COPY_POINTER_RAW(leafbrushNodes);

			COPY_VALUE(numLeafBrushes);
			COPY_POINTER_RAW(leafbrushes);

			COPY_VALUE(numLeafSurfaces);
			COPY_POINTER_RAW(leafsurfaces);

			COPY_VALUE(vertCount);
			COPY_POINTER_RAW(verts);

			COPY_VALUE(triCount);
			COPY_POINTER_RAW(triIndices);
			COPY_POINTER_RAW(triEdgeIsWalkable);

			COPY_VALUE(borderCount);
			COPY_POINTER_RAW(borders);

			COPY_VALUE(partitionCount);
			COPY_POINTER_RAW(partitions);

			new_asset->aabbTreeCount = asset->aabbTreeCount;
			const auto trees = allocator.allocate<zonetool::iw4::CollisionAabbTree>(asset->aabbTreeCount);
			for (auto i = 0; i < asset->aabbTreeCount; i++)
			{
				const auto& source = asset->aabbTrees[i];
				std::memcpy(trees[i].midPoint, source.origin, sizeof(trees[i].midPoint));
				std::memcpy(trees[i].halfSize, source.halfSize, sizeof(trees[i].halfSize));
				trees[i].materialIndex = source.materialIndex;
				trees[i].childCount = source.childCount;
				static_assert(sizeof(trees[i].u) == sizeof(source.u));
				std::memcpy(&trees[i].u, &source.u, sizeof(source.u));
			}
			new_asset->aabbTrees = trees;

			new_asset->numSubModels = asset->numSubModels;
			const auto cmodels = allocator.allocate<zonetool::iw4::cmodel_t>(asset->numSubModels);
			for (unsigned int i = 0; i < asset->numSubModels; i++)
			{
				const auto& source = asset->cmodels[i];
				compute_bounds(cmodels[i].bounds, source.mins, source.maxs);
				cmodels[i].radius = source.radius;
				convert_leaf(cmodels[i].leaf, source.leaf);
			}
			new_asset->cmodels = cmodels;

			new_asset->numBrushes = asset->numBrushes;
			const auto brushes = allocator.allocate<zonetool::iw4::cbrush_t>(asset->numBrushes);
			const auto brush_bounds = allocator.allocate<zonetool::iw4::Bounds>(asset->numBrushes);
			const auto brush_contents = allocator.allocate<int>(asset->numBrushes);
			for (unsigned int i = 0; i < asset->numBrushes; i++)
			{
				const auto& source = asset->brushes[i];
				auto& brush = brushes[i];

				brush.numsides = static_cast<unsigned short>(source.numsides);
				brush.glassPieceIndex = 0;

				if (const auto source_sides = source.sides.get())
				{
					const auto side_index = source_sides - asset->brushsides.get();
					brush.sides = &sides[side_index];
				}

				brush.baseAdjacentSide.value = source.baseAdjacentSide.value;

				for (auto x = 0; x < 2; x++)
				{
					for (auto y = 0; y < 3; y++)
					{
						brush.axialMaterialNum[x][y] = source.axialMaterialNum[x][y];
						brush.firstAdjacentSideOffsets[x][y] = static_cast<unsigned char>(source.firstAdjacentSideOffsets[x][y]);
						brush.edgeCount[x][y] = static_cast<unsigned char>(source.edgeCount[x][y]);
					}
				}

				compute_bounds(brush_bounds[i], source.mins, source.maxs);
				brush_contents[i] = source.contents;
			}
			new_asset->brushes = brushes;
			new_asset->brushBounds = brush_bounds;
			new_asset->brushContents = brush_contents;

			const auto map_ents = mapents::convert(asset->mapEnts.get(), allocator);
			new_asset->mapEnts = map_ents;

			build_smodel_nodes(new_asset, allocator);

			for (auto list = 0; list < 2; list++)
			{
				const auto count = asset->dynEntCount[list];
				new_asset->dynEntCount[list] = count;

				const auto definitions = allocator.allocate<zonetool::iw4::DynEntityDef>(count);
				const auto poses = allocator.allocate<zonetool::iw4::DynEntityPose>(count);
				const auto clients = allocator.allocate<zonetool::iw4::DynEntityClient>(count);
				const auto collisions = allocator.allocate<zonetool::iw4::DynEntityColl>(count);

				for (unsigned int i = 0; i < count; i++)
				{
					const auto& source = asset->dynEntDefList[list][i];
					auto& definition = definitions[i];

					definition.type = static_cast<zonetool::iw4::DynEntityType>(source.type);
					std::memcpy(&definition.pose, &source.pose, sizeof(definition.pose));
					definition.xModel.value = source.xModel.value;
					definition.brushModel = source.brushModel;
					definition.physicsBrushModel = source.physicsBrushModel;
					definition.destroyFx.value = source.destroyFx.value;
					definition.physPreset.value = source.physPreset.value;
					definition.health = source.health;
					std::memcpy(&definition.mass, &source.mass, sizeof(definition.mass));
					definition.contents = source.contents;

					std::memcpy(&poses[i], &asset->dynEntPoseList[list][i], sizeof(poses[i]));

					const auto& source_client = asset->dynEntClientList[list][i];
					clients[i].physObjId = source_client.physObjId;
					clients[i].flags = source_client.flags;
					clients[i].lightingHandle = source_client.lightingHandle;
					clients[i].health = source_client.health;
					clients[i].contents = source.contents;

					std::memcpy(&collisions[i], &asset->dynEntCollList[list][i], sizeof(collisions[i]));
				}

				new_asset->dynEntDefList[list] = definitions;
				new_asset->dynEntPoseList[list] = poses;
				new_asset->dynEntClientList[list] = clients;
				new_asset->dynEntCollList[list] = collisions;
			}

			COPY_VALUE(checksum);

			add_triggers(new_asset, map_ents, allocator);
			add_stage(map_ents, allocator);

			return new_asset;
		}
	}
}
