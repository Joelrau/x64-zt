#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "clipmap.hpp"

#include "zonetool/h1/assets/clipmap.hpp"
#include "zonetool/h1/assets/physworld.hpp"

#include "zonetool/iw6/functions.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace clipmap
		{
			namespace
			{
				/*zonetool::h1::CSurfaceFlags surf_flags_conversion_table[31]
				{
					zonetool::h1::SURF_FLAG_DEFAULT,
					zonetool::h1::SURF_FLAG_BARK,
					zonetool::h1::SURF_FLAG_BRICK,
					zonetool::h1::SURF_FLAG_CARPET,
					zonetool::h1::SURF_FLAG_CLOTH,
					zonetool::h1::SURF_FLAG_CONCRETE,
					zonetool::h1::SURF_FLAG_DIRT,
					zonetool::h1::SURF_FLAG_FLESH,
					zonetool::h1::SURF_FLAG_FOLIAGE_DEBRIS,
					zonetool::h1::SURF_FLAG_GLASS,
					zonetool::h1::SURF_FLAG_GRASS,
					zonetool::h1::SURF_FLAG_GRAVEL,
					zonetool::h1::SURF_FLAG_ICE,
					zonetool::h1::SURF_FLAG_METAL_SOLID,
					zonetool::h1::SURF_FLAG_MUD,
					zonetool::h1::SURF_FLAG_PAPER,
					zonetool::h1::SURF_FLAG_PLASTER,
					zonetool::h1::SURF_FLAG_ROCK,
					zonetool::h1::SURF_FLAG_SAND,
					zonetool::h1::SURF_FLAG_SNOW,
					zonetool::h1::SURF_FLAG_WATER_WAIST,
					zonetool::h1::SURF_FLAG_WOOD_SOLID,
					zonetool::h1::SURF_FLAG_ASPHALT,
					zonetool::h1::SURF_FLAG_CERAMIC,
					zonetool::h1::SURF_FLAG_PLASTIC_SOLID,
					zonetool::h1::SURF_FLAG_RUBBER,
					zonetool::h1::SURF_FLAG_CUSHION,
					zonetool::h1::SURF_FLAG_FRUIT,
					zonetool::h1::SURF_FLAG_PAINTEDMETAL,
					zonetool::h1::SURF_FLAG_RIOTSHIELD,
					zonetool::h1::SURF_FLAG_SLUSH,
				}; zonetool::iw6::CSurfaceFlags;

				int convert_surf_flags(int flags)
				{
					int new_flags = surf_flags_conversion_table[flags >> 20];
					auto convert = [&](zonetool::iw6::CSurfaceFlags a, zonetool::h1::CSurfaceFlags b)
					{
						new_flags |= ((flags & a) == a) ? b : 0;
					};
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_OPAQUEGLASS, zonetool::h1::CSurfaceFlags::SURF_FLAG_DEFAULT);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_CLIPMISSILE, zonetool::h1::CSurfaceFlags::SURF_FLAG_CLIPMISSILE);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT, zonetool::h1::CSurfaceFlags::SURF_FLAG_AI_NOSIGHT);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_CLIPSHOT, zonetool::h1::CSurfaceFlags::SURF_FLAG_CLIPSHOT);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_PLAYERCLIP, zonetool::h1::CSurfaceFlags::SURF_FLAG_PLAYERCLIP);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_MONSTERCLIP, zonetool::h1::CSurfaceFlags::SURF_FLAG_MONSTERCLIP);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH, zonetool::h1::CSurfaceFlags::SURF_FLAG_AICLIPALLOWDEATH);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_VEHICLECLIP, zonetool::h1::CSurfaceFlags::SURF_FLAG_VEHICLECLIP);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_ITEMCLIP, zonetool::h1::CSurfaceFlags::SURF_FLAG_ITEMCLIP);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NODROP, zonetool::h1::CSurfaceFlags::SURF_FLAG_NODROP);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NONSOLID, zonetool::h1::CSurfaceFlags::SURF_FLAG_NONSOLID);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_DETAIL, zonetool::h1::CSurfaceFlags::SURF_FLAG_DETAIL);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_STRUCTURAL, zonetool::h1::CSurfaceFlags::SURF_FLAG_STRUCTURAL);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_PORTAL, zonetool::h1::CSurfaceFlags::SURF_FLAG_PORTAL);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP, zonetool::h1::CSurfaceFlags::SURF_FLAG_CANSHOOTCLIP);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_ORIGIN, zonetool::h1::CSurfaceFlags::SURF_FLAG_ORIGIN);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_SKY, zonetool::h1::CSurfaceFlags::SURF_FLAG_SKY);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW, zonetool::h1::CSurfaceFlags::SURF_FLAG_NOCASTSHADOW);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM, zonetool::h1::CSurfaceFlags::SURF_FLAG_PHYSICSGEOM);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL, zonetool::h1::CSurfaceFlags::SURF_FLAG_LIGHTPORTAL);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS, zonetool::h1::CSurfaceFlags::SURF_FLAG_OUTDOORBOUNDS);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_SLICK, zonetool::h1::CSurfaceFlags::SURF_FLAG_SLICK);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NOIMPACT, zonetool::h1::CSurfaceFlags::SURF_FLAG_NOIMPACT);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NOMARKS, zonetool::h1::CSurfaceFlags::SURF_FLAG_NOMARKS);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NOPENETRATE, zonetool::h1::CSurfaceFlags::SURF_FLAG_NOPENETRATE);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_LADDER, zonetool::h1::CSurfaceFlags::SURF_FLAG_LADDER);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NODAMAGE, zonetool::h1::CSurfaceFlags::SURF_FLAG_NODAMAGE);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_MANTLEON, zonetool::h1::CSurfaceFlags::SURF_FLAG_MANTLEON);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_MANTLEOVER, zonetool::h1::CSurfaceFlags::SURF_FLAG_MANTLEOVER);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_STAIRS, zonetool::h1::CSurfaceFlags::SURF_FLAG_STAIRS);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_SOFT, zonetool::h1::CSurfaceFlags::SURF_FLAG_SOFT);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NOSTEPS, zonetool::h1::CSurfaceFlags::SURF_FLAG_NOSTEPS);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NODRAW, zonetool::h1::CSurfaceFlags::SURF_FLAG_NODRAW);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP, zonetool::h1::CSurfaceFlags::SURF_FLAG_NOLIGHTMAP);
					convert(zonetool::iw6::CSurfaceFlags::SURF_FLAG_NODLIGHT, zonetool::h1::CSurfaceFlags::SURF_FLAG_NODLIGHT);
					return new_flags;
				}*/
			}

			void generate_clip_info(zonetool::h1::ClipInfo* info, clipMap_t* asset, zone_memory* mem)
			{
				info->planeCount = asset->info.planeCount;
				REINTERPRET_CAST_SAFE_TO_FROM(info->planes, asset->info.planes);

				info->numMaterials = asset->info.numMaterials;
				info->materials = mem->allocate<zonetool::h1::ClipMaterial>(info->numMaterials);
				for (unsigned int i = 0; i < info->numMaterials; i++)
				{
					info->materials[i].name = asset->info.materials[i].name;
					info->materials[i].surfaceFlags = asset->info.materials[i].surfaceFlags; // todo://convert_surf_flags(asset->info.materials[i].surfaceFlags);
					info->materials[i].contents = asset->info.materials[i].contents;
				}

				info->bCollisionTree.leafbrushNodesCount = asset->info.leafbrushNodesCount;
				info->bCollisionTree.leafbrushNodes = mem->allocate<zonetool::h1::cLeafBrushNode_s>(info->bCollisionTree.leafbrushNodesCount);
				for (unsigned int i = 0; i < info->bCollisionTree.leafbrushNodesCount; i++)
				{
					info->bCollisionTree.leafbrushNodes[i].axis = asset->info.leafbrushNodes[i].axis;
					info->bCollisionTree.leafbrushNodes[i].leafBrushCount = asset->info.leafbrushNodes[i].leafBrushCount;
					info->bCollisionTree.leafbrushNodes[i].contents = asset->info.leafbrushNodes[i].contents;
					if (info->bCollisionTree.leafbrushNodes[i].leafBrushCount > 0)
					{
						REINTERPRET_CAST_SAFE_TO_FROM(info->bCollisionTree.leafbrushNodes[i].data.leaf.brushes, asset->info.leafbrushNodes[i].data.leaf.brushes);
					}
					else
					{
						info->bCollisionTree.leafbrushNodes[i].data.children.dist = asset->info.leafbrushNodes[i].data.children.dist;
						info->bCollisionTree.leafbrushNodes[i].data.children.range = asset->info.leafbrushNodes[i].data.children.range;

						info->bCollisionTree.leafbrushNodes[i].data.children.childOffset[0] = asset->info.leafbrushNodes[i].data.children.childOffset[0];
						info->bCollisionTree.leafbrushNodes[i].data.children.childOffset[1] = asset->info.leafbrushNodes[i].data.children.childOffset[1];
					}
				}

				info->bCollisionTree.numLeafBrushes = asset->info.numLeafBrushes;
				REINTERPRET_CAST_SAFE_TO_FROM(info->bCollisionTree.leafbrushes, asset->info.leafbrushes);

				info->pCollisionTree.aabbTreeCount = asset->aabbTreeCount;
				info->pCollisionTree.aabbTrees = mem->allocate<zonetool::h1::CollisionAabbTree>(info->pCollisionTree.aabbTreeCount);
				for (int i = 0; i < info->pCollisionTree.aabbTreeCount; i++)
				{
					std::memcpy(info->pCollisionTree.aabbTrees[i].midPoint, asset->aabbTrees[i].midPoint, sizeof(float[3]));
					std::memcpy(info->pCollisionTree.aabbTrees[i].halfSize, asset->aabbTrees[i].halfSize, sizeof(float[3]));
					info->pCollisionTree.aabbTrees[i].materialIndex = asset->aabbTrees[i].materialIndex;
					info->pCollisionTree.aabbTrees[i].childCount = asset->aabbTrees[i].childCount;

					info->pCollisionTree.aabbTrees[i].u.firstChildIndex = asset->aabbTrees[i].u.firstChildIndex;
					//info->pCollisionTree.aabbTrees[i].u.partitionIndex = asset->aabbTrees[i].u.partitionIndex;
				}

				info->sCollisionTree.numStaticModels = static_cast<unsigned short>(asset->numStaticModels);
				info->sCollisionTree.smodelNodeCount = asset->smodelNodeCount;
				REINTERPRET_CAST_SAFE_TO_FROM(info->sCollisionTree.smodelNodes, asset->smodelNodes);

				std::unordered_map<cbrushside_t*, zonetool::h1::cbrushside_t*> mapped_brush_sides;

				info->bCollisionData.numBrushSides = asset->info.numBrushSides;
				info->bCollisionData.brushSides = mem->allocate<zonetool::h1::cbrushside_t>(info->bCollisionData.numBrushSides);
				for (unsigned int i = 0; i < info->bCollisionData.numBrushSides; i++)
				{
					mapped_brush_sides[&asset->info.brushsides[i]] = &info->bCollisionData.brushSides[i];

					// check
					int index = static_cast<int>(
						(reinterpret_cast<std::uintptr_t>(asset->info.brushsides[i].plane) - 
							reinterpret_cast<std::uintptr_t>(asset->info.planes)) / sizeof(cplane_s));
					assert(index <= info->planeCount);

					info->bCollisionData.brushSides[i].planeIndex = index;

					info->bCollisionData.brushSides[i].materialNum = asset->info.brushsides[i].materialNum;
					info->bCollisionData.brushSides[i].firstAdjacentSideOffset = asset->info.brushsides[i].firstAdjacentSideOffset;
					info->bCollisionData.brushSides[i].edgeCount = asset->info.brushsides[i].edgeCount;
				}

				info->bCollisionData.numBrushEdges = asset->info.numBrushEdges;
				REINTERPRET_CAST_SAFE_TO_FROM(info->bCollisionData.brushEdges, asset->info.brushEdges);

				info->bCollisionData.numBrushes = asset->info.numBrushes;
				info->bCollisionData.brushes = mem->allocate<zonetool::h1::cbrush_t>(asset->info.numBrushes);
				for (unsigned int i = 0; i < info->bCollisionData.numBrushes; i++)
				{
					info->bCollisionData.brushes[i].numsides = asset->info.brushes[i].numsides;
					info->bCollisionData.brushes[i].glassPieceIndex = asset->info.brushes[i].glassPieceIndex;

					if (asset->info.brushes[i].sides)
						info->bCollisionData.brushes[i].sides = mapped_brush_sides.find(asset->info.brushes[i].sides)->second;

					REINTERPRET_CAST_SAFE_TO_FROM(info->bCollisionData.brushes[i].baseAdjacentSide, asset->info.brushes[i].baseAdjacentSide);

					memcpy(&info->bCollisionData.brushes[i].axialMaterialNum, &asset->info.brushes[i].axialMaterialNum, sizeof(short[2][3]));
					memcpy(&info->bCollisionData.brushes[i].firstAdjacentSideOffsets, &asset->info.brushes[i].firstAdjacentSideOffsets, sizeof(char[2][3]));
					memcpy(&info->bCollisionData.brushes[i].edgeCount, &asset->info.brushes[i].edgeCount, sizeof(char[2][3]));
				}

				REINTERPRET_CAST_SAFE_TO_FROM(info->bCollisionData.brushBounds, asset->info.brushBounds);
				REINTERPRET_CAST_SAFE_TO_FROM(info->bCollisionData.brushContents, asset->info.brushContents);

				info->pCollisionData.vertCount = asset->vertCount;
				REINTERPRET_CAST_SAFE_TO_FROM(info->pCollisionData.verts, asset->verts);
				info->pCollisionData.triCount = asset->triCount;
				REINTERPRET_CAST_SAFE_TO_FROM(info->pCollisionData.triIndices, asset->triIndices);
				REINTERPRET_CAST_SAFE_TO_FROM(info->pCollisionData.triEdgeIsWalkable, asset->triEdgeIsWalkable);
				info->pCollisionData.borderCount = asset->borderCount;
				REINTERPRET_CAST_SAFE_TO_FROM(info->pCollisionData.borders, asset->borders);
				info->pCollisionData.partitionCount = asset->partitionCount;
				info->pCollisionData.partitions = mem->allocate<zonetool::h1::CollisionPartition>(info->pCollisionData.partitionCount);
				for (int i = 0; i < info->pCollisionData.partitionCount; i++)
				{
					info->pCollisionData.partitions[i].triCount = asset->partitions[i].triCount;
					info->pCollisionData.partitions[i].borderCount = asset->partitions[i].borderCount;
					info->pCollisionData.partitions[i].firstVertSegment = asset->partitions[i].firstVertSegment;
					info->pCollisionData.partitions[i].firstTri = asset->partitions[i].firstTri;
					REINTERPRET_CAST_SAFE_TO_FROM(info->pCollisionData.partitions[i].borders, asset->partitions[i].borders);
				}

				info->sCollisionData.numStaticModels = asset->numStaticModels;
				info->sCollisionData.staticModelList = mem->allocate<zonetool::h1::cStaticModel_s>(info->sCollisionData.numStaticModels);
				for (unsigned int i = 0; i < info->sCollisionData.numStaticModels; i++)
				{
					info->sCollisionData.staticModelList[i].xmodel = reinterpret_cast<zonetool::h1::XModel*>(asset->staticModelList[i].xmodel);

					memcpy(&info->sCollisionData.staticModelList[i].origin, &asset->staticModelList[i].origin, sizeof(float[3]));
					memcpy(&info->sCollisionData.staticModelList[i].invScaledAxis, &asset->staticModelList[i].invScaledAxis, sizeof(float[3][3]));
					memcpy(&info->sCollisionData.staticModelList[i].absBounds, &asset->staticModelList[i].absBounds, sizeof(float[2][3]));
					info->sCollisionData.staticModelList[i].lightingHandle = 0;
				}
			}

			zonetool::h1::clipMap_t* generate_clipmap(clipMap_t* asset, zone_memory* mem)
			{
				// allocate H1 clipMap_t structure
				const auto h1_asset = mem->allocate<zonetool::h1::clipMap_t>();

				h1_asset->name = asset->name;
				h1_asset->isInUse = asset->isInUse;

				generate_clip_info(&h1_asset->info, asset, mem);
				h1_asset->pInfo = &h1_asset->info;

				h1_asset->numNodes = asset->numNodes;
				h1_asset->nodes = mem->allocate<zonetool::h1::cNode_t>(h1_asset->numNodes);
				for (unsigned int i = 0; i < h1_asset->numNodes; i++)
				{
					REINTERPRET_CAST_SAFE_TO_FROM(h1_asset->nodes[i].plane, asset->nodes[i].plane);
					h1_asset->nodes[i].children[0] = asset->nodes[i].children[0];
					h1_asset->nodes[i].children[1] = asset->nodes[i].children[1];
				}

				h1_asset->numLeafs = asset->numLeafs;
				h1_asset->leafs = mem->allocate<zonetool::h1::cLeaf_t>(h1_asset->numLeafs);
				for (unsigned int i = 0; i < h1_asset->numLeafs; i++)
				{
					h1_asset->leafs[i].firstCollAabbIndex = asset->leafs[i].firstCollAabbIndex;
					h1_asset->leafs[i].collAabbCount = asset->leafs[i].collAabbCount;
					h1_asset->leafs[i].brushContents = asset->leafs[i].brushContents;
					h1_asset->leafs[i].terrainContents = asset->leafs[i].terrainContents;
					memcpy(&h1_asset->leafs[i].bounds, &asset->leafs[i].bounds, sizeof(Bounds));
					h1_asset->leafs[i].leafBrushNode = asset->leafs[i].leafBrushNode;
				}

				h1_asset->numSubModels = asset->numSubModels;
				h1_asset->cmodels = mem->allocate<zonetool::h1::cmodel_t>(h1_asset->numSubModels);
				for (unsigned int i = 0; i < h1_asset->numSubModels; i++)
				{
					memcpy(&h1_asset->cmodels[i].bounds, &asset->cmodels[i].bounds, sizeof(Bounds));
					h1_asset->cmodels[i].radius = asset->cmodels[i].radius;

					h1_asset->cmodels[i].info = nullptr; // mapped in h1

					h1_asset->cmodels[i].leaf.firstCollAabbIndex = asset->cmodels[i].leaf.firstCollAabbIndex;
					h1_asset->cmodels[i].leaf.collAabbCount = asset->cmodels[i].leaf.collAabbCount;
					h1_asset->cmodels[i].leaf.brushContents = asset->cmodels[i].leaf.brushContents;
					h1_asset->cmodels[i].leaf.terrainContents = asset->cmodels[i].leaf.terrainContents;
					memcpy(&h1_asset->cmodels[i].leaf.bounds, &asset->cmodels[i].leaf.bounds, sizeof(Bounds));
					h1_asset->cmodels[i].leaf.leafBrushNode = asset->cmodels[i].leaf.leafBrushNode;
				}

				h1_asset->mapEnts = mem->allocate<zonetool::h1::MapEnts>();
				h1_asset->mapEnts->name = asset->mapEnts->name;

				h1_asset->stageCount = asset->stageCount;
				h1_asset->stages = mem->allocate<zonetool::h1::Stage>(h1_asset->stageCount);
				for (unsigned int i = 0; i < h1_asset->stageCount; i++)
				{
					h1_asset->stages[i].name = reinterpret_cast<const char*>(asset->stages[i].name);
					memcpy(&h1_asset->stages[i].origin, &asset->stages[i].origin, sizeof(float[3]));
					h1_asset->stages[i].triggerIndex = asset->stages[i].triggerIndex;
					h1_asset->stages[i].sunPrimaryLightIndex = asset->stages[i].sunPrimaryLightIndex;
					h1_asset->stages[i].unk = 0x3A83126F;
				}
				h1_asset->stageTrigger.count = 0;
				h1_asset->stageTrigger.models = nullptr;
				h1_asset->stageTrigger.hullCount = 0;
				h1_asset->stageTrigger.hulls = nullptr;
				h1_asset->stageTrigger.slabCount = 0;
				h1_asset->stageTrigger.slabs = nullptr;

				for (unsigned char i = 0; i < 2; i++)
				{
					h1_asset->dynEntCount[i] = asset->dynEntCount[i];
					h1_asset->dynEntDefList[i] = mem->allocate<zonetool::h1::DynEntityDef>(h1_asset->dynEntCount[i]);
					h1_asset->dynEntPoseList[i] = mem->allocate<zonetool::h1::DynEntityPose>(h1_asset->dynEntCount[i]);
					h1_asset->dynEntClientList[i] = mem->allocate<zonetool::h1::DynEntityClient>(h1_asset->dynEntCount[i]);
					h1_asset->dynEntCollList[i] = reinterpret_cast<zonetool::h1::DynEntityColl*>(asset->dynEntCollList[i]);
					for (unsigned short j = 0; j < h1_asset->dynEntCount[i]; j++)
					{
						h1_asset->dynEntDefList[i][j].type = static_cast<zonetool::h1::DynEntityType>(asset->dynEntDefList[i][j].type);
						memcpy(&h1_asset->dynEntDefList[i][j].pose, &asset->dynEntDefList[i][j].pose, sizeof(GfxPlacement));
						h1_asset->dynEntDefList[i][j].baseModel = reinterpret_cast<zonetool::h1::XModel*>(asset->dynEntDefList[i][j].baseModel);
						h1_asset->dynEntDefList[i][j].brushModel = asset->dynEntDefList[i][j].brushModel;
						h1_asset->dynEntDefList[i][j].physicsBrushModel = asset->dynEntDefList[i][j].physicsBrushModel;
						h1_asset->dynEntDefList[i][j].health = asset->dynEntDefList[i][j].health;
						h1_asset->dynEntDefList[i][j].destroyFx = reinterpret_cast<zonetool::h1::FxEffectDef*>(asset->dynEntDefList[i][j].destroyFx);
						h1_asset->dynEntDefList[i][j].sound = nullptr;
						h1_asset->dynEntDefList[i][j].physPreset = reinterpret_cast<zonetool::h1::PhysPreset*>(asset->dynEntDefList[i][j].physPreset);
						h1_asset->dynEntDefList[i][j].hinge = reinterpret_cast<zonetool::h1::DynEntityHingeDef*>(asset->dynEntDefList[i][j].hinge);
						h1_asset->dynEntDefList[i][j].linkTo = nullptr;
						memcpy(&h1_asset->dynEntDefList[i][j].mass, &asset->dynEntDefList[i][j].mass, sizeof(PhysMass));
						h1_asset->dynEntDefList[i][j].contents = asset->dynEntDefList[i][j].contents;
						if (1) // check
						{
							h1_asset->dynEntDefList[i][j].unk[0] = 2500.00000f;
							h1_asset->dynEntDefList[i][j].unk[1] = 0.0199999996f;
						}

						if (h1_asset->dynEntDefList[i][j].type == zonetool::h1::DYNENT_TYPE_SCRIPTABLEINST)
						{
							//h1_asset->dynEntDefList[i][j].scriptableIndex = asset->dynEntDefList[i][j].scriptableIndex;
							h1_asset->dynEntDefList[i][j].type = zonetool::h1::DYNENT_TYPE_INVALID;
						}

						memcpy(&h1_asset->dynEntPoseList[i][j].pose, &asset->dynEntPoseList[i][j].pose, sizeof(GfxPlacement));
						h1_asset->dynEntPoseList[i][j].radius = asset->dynEntPoseList[i][j].radius;
						h1_asset->dynEntPoseList[i][j].__pad0; // check

						h1_asset->dynEntClientList[i][j].physObjId = asset->dynEntClientList[i]->physObjId;
						h1_asset->dynEntClientList[i][j].flags = asset->dynEntClientList[i]->flags;
						h1_asset->dynEntClientList[i][j].lightingHandle = asset->dynEntClientList[i]->lightingHandle;
						h1_asset->dynEntClientList[i][j].health = asset->dynEntClientList[i]->health;
						h1_asset->dynEntClientList[i][j].hinge = nullptr;
						h1_asset->dynEntClientList[i][j].activeModel = nullptr;
						h1_asset->dynEntClientList[i][j].contents = asset->dynEntClientList[i]->contents;
					}
				}

				h1_asset->dynEntAnchorCount = 0;
				h1_asset->dynEntAnchorNames = nullptr;

				h1_asset->scriptableMapEnts;

				h1_asset->grappleData;

				h1_asset->checksum = asset->checksum;

				return h1_asset;
			}

			zonetool::h1::clipMap_t* convert(clipMap_t* asset, zone_memory* mem)
			{
				return generate_clipmap(asset, mem);
			}

			void dump(clipMap_t* asset, zone_memory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::clip_map::dump(converted_asset);

				// dump physmap here too i guess, since it's needed.
				auto* physmap = mem->allocate<zonetool::h1::PhysWorld>();
				physmap->name = asset->name;

				physmap->brushModelCount = converted_asset->numSubModels;
				physmap->brushModels = mem->allocate<zonetool::h1::PhysBrushModel>(physmap->brushModelCount);
				for (unsigned int i = 0; i < physmap->brushModelCount; i++)
				{
					physmap->brushModels[i].fields.polytopeIndex = -1;
					physmap->brushModels[i].fields.unk = -1;
					physmap->brushModels[i].fields.worldIndex = 0;
					physmap->brushModels[i].fields.meshIndex = -1;
				}

				// todo: polytope data
				physmap->polytopeCount = 0;
				physmap->polytopeDatas = nullptr;

				// todo: mesh data
				physmap->meshDataCount = 0;
				physmap->meshDatas = nullptr;

				// todo?
				physmap->waterVolumeDefCount = 0;
				physmap->waterVolumeDefs = nullptr;

				zonetool::h1::phys_world::dump(physmap);
			}
		}
	}
}