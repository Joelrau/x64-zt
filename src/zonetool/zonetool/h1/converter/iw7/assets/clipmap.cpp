#include <std_include.hpp>

#include "zonetool/h1/converter/iw7/include.hpp"
#include "clipmap.hpp"
#ifdef EXPERIMENTAL_IW7

#include "zonetool/iw7/assets/clipmap.hpp"

#include "zonetool/utils/gsc.hpp"

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace
		{
			std::string convert_mapents_ids(const std::string& source)
			{
				std::string out_buffer;

				const auto lines = utils::string::split(source, '\n');

				for (auto i = 0; i < lines.size(); i++)
				{
					const auto _0 = gsl::finally([&]
					{
						out_buffer.append("\n");
					});

					const auto& line = lines[i];

					std::regex expr(R"~((.+) "(.*)")~");
					std::smatch match{};
					if (!line.starts_with("0 ") && std::regex_search(line, match, expr))
					{
						const auto id = std::atoi(match[1].str().data());
						const auto value = match[2].str();

						std::string key = gsc::h1::gsc_ctx->token_name(
							static_cast<std::uint16_t>(id));
						if (!key.starts_with("_id_"))
						{
							out_buffer.append(utils::string::va("\"%s\" \"%s\"", key.data(), value.data()));
							continue;
						}
					}
					out_buffer.append(line);
				}

				return out_buffer;
			}
		}

		namespace mapents
		{
			std::uint8_t convert_trigger_type(std::int32_t flags)
			{
				std::uint8_t new_flags = 0;
				auto convert = [&](zonetool::h1::clientTriggerType_t a, zonetool::iw7::clientTriggerType_t b)
				{
					if (flags & a)
						new_flags |= b;
				};

				convert(zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_VISIONSET, zonetool::iw7::clientTriggerType_t::CLIENT_TRIGGER_VISIONSET);
				convert(zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_AUDIO, zonetool::iw7::clientTriggerType_t::CLIENT_TRIGGER_AUDIO);
				convert(zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_BLEND_VISION, zonetool::iw7::clientTriggerType_t::CLIENT_TRIGGER_BLEND_VISION);
				convert(zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_BLEND_AUDIO, zonetool::iw7::clientTriggerType_t::CLIENT_TRIGGER_BLEND_AUDIO);
				convert(zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_NPC, zonetool::iw7::clientTriggerType_t::CLIENT_TRIGGER_AUDIO_NPC);
				return new_flags;
			}

			zonetool::iw7::MapEnts* generate_mapents(clipMap_t* clipmap, utils::memory::allocator& allocator)
			{
				const auto* asset = clipmap->mapEnts;
				auto* new_asset = allocator.allocate<zonetool::iw7::MapEnts>();
				COPY_VALUE(name);

				const auto str = convert_mapents_ids(
					std::string{ asset->entityString, static_cast<size_t>(asset->numEntityChars) });
				new_asset->entityString = allocator.duplicate_string(str);
				new_asset->numEntityChars = static_cast<int>(str.size());

				COPY_VALUE(trigger.count);
				new_asset->trigger.models = allocator.allocate_array<zonetool::iw7::TriggerModel>(asset->trigger.count);
				for (unsigned int i = 0; i < asset->trigger.count; i++)
				{
					new_asset->trigger.models[i].contents = asset->trigger.models[i].contents;
					new_asset->trigger.models[i].hullCount = asset->trigger.models[i].hullCount;
					new_asset->trigger.models[i].firstHull = asset->trigger.models[i].firstHull;
					new_asset->trigger.models[i].windingCount = 0;
					new_asset->trigger.models[i].firstWinding = 0;
					new_asset->trigger.models[i].flags = 0;
					new_asset->trigger.models[i].physicsAsset = nullptr;
					new_asset->trigger.models[i].physicsShapeOverrideIdx = 0xFFFF;
				}

				COPY_VALUE(trigger.hullCount);
				REINTERPRET_CAST_SAFE(trigger.hulls);
				COPY_VALUE(trigger.slabCount);
				REINTERPRET_CAST_SAFE(trigger.slabs);

				new_asset->clientTrigger.trigger.windingCount = 0;
				new_asset->clientTrigger.trigger.windings = nullptr;
				new_asset->clientTrigger.trigger.windingPointCount = 0;
				new_asset->clientTrigger.trigger.windingPoints = nullptr;

				COPY_VALUE(clientTrigger.trigger.count);
				new_asset->clientTrigger.trigger.models = allocator.allocate_array<zonetool::iw7::TriggerModel>(asset->clientTrigger.trigger.count);
				for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
				{
					new_asset->clientTrigger.trigger.models[i].contents = asset->clientTrigger.trigger.models[i].contents;
					new_asset->clientTrigger.trigger.models[i].hullCount = asset->clientTrigger.trigger.models[i].hullCount;
					new_asset->clientTrigger.trigger.models[i].firstHull = asset->clientTrigger.trigger.models[i].firstHull;
					new_asset->clientTrigger.trigger.models[i].windingCount = 0;
					new_asset->clientTrigger.trigger.models[i].firstWinding = 0;
					new_asset->clientTrigger.trigger.models[i].flags = 0;
					new_asset->clientTrigger.trigger.models[i].physicsAsset = nullptr;
					new_asset->clientTrigger.trigger.models[i].physicsShapeOverrideIdx = 0xFFFF;
				}

				COPY_VALUE(clientTrigger.trigger.hullCount);
				REINTERPRET_CAST_SAFE(clientTrigger.trigger.hulls);
				COPY_VALUE(clientTrigger.trigger.slabCount);
				REINTERPRET_CAST_SAFE(clientTrigger.trigger.slabs);

				new_asset->clientTrigger.trigger.windingCount = 0;
				new_asset->clientTrigger.trigger.windings = nullptr;
				new_asset->clientTrigger.trigger.windingPointCount = 0;
				new_asset->clientTrigger.trigger.windingPoints = nullptr;

				COPY_VALUE(clientTrigger.triggerStringLength);
				REINTERPRET_CAST_SAFE(clientTrigger.triggerString);

				REINTERPRET_CAST_SAFE(clientTrigger.visionSetTriggers); // convert?

				new_asset->clientTrigger.triggerType = allocator.allocate_array<unsigned char>(asset->clientTrigger.trigger.count);
				for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
				{
					new_asset->clientTrigger.triggerType[i] = convert_trigger_type(asset->clientTrigger.triggerType[i]);
				}

				REINTERPRET_CAST_SAFE(clientTrigger.origins);
				REINTERPRET_CAST_SAFE(clientTrigger.scriptDelay);
				REINTERPRET_CAST_SAFE(clientTrigger.audioTriggers);
				REINTERPRET_CAST_SAFE(clientTrigger.blendLookup);
				REINTERPRET_CAST_SAFE(clientTrigger.npcTriggers);
				new_asset->clientTrigger.audioStateIds = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				new_asset->clientTrigger.audioRvbPanInfo = allocator.allocate_array<zonetool::iw7::CTAudRvbPanInfo>(asset->clientTrigger.trigger.count);
				new_asset->clientTrigger.transientIndex = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
				{
					new_asset->clientTrigger.audioStateIds[i] = -1;

					new_asset->clientTrigger.audioRvbPanInfo[i].hasCustomPosition = false;
					// has some more data..

					new_asset->clientTrigger.transientIndex[i] = 0;
				}

				new_asset->clientTrigger.linkTo = allocator.allocate_array<zonetool::iw7::ClientEntityLinkToDef*>(asset->clientTrigger.trigger.count);
				for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
				{
					new_asset->clientTrigger.linkTo[i] = nullptr;
				}

				COPY_VALUE(clientTriggerBlend.numClientTriggerBlendNodes);
				REINTERPRET_CAST_SAFE(clientTriggerBlend.blendNodes);

				COPY_VALUE(spawnList.spawnsCount);
				new_asset->spawnList.spawns = allocator.allocate_array<zonetool::iw7::SpawnPointEntityRecord>(asset->spawnList.spawnsCount);
				for (unsigned short i = 0; i < asset->spawnList.spawnsCount; i++)
				{
					COPY_VALUE(spawnList.spawns[i].index);
					COPY_VALUE_CAST(spawnList.spawns[i].name);
					COPY_VALUE_CAST(spawnList.spawns[i].target);
					COPY_VALUE_CAST(spawnList.spawns[i].script_noteworthy);
					COPY_ARR(spawnList.spawns[i].origin);
					COPY_ARR(spawnList.spawns[i].angles);
				}

				COPY_VALUE(splineList.splineCount);
				REINTERPRET_CAST_SAFE(splineList.splines);

				new_asset->havokEntsShapeDataSize = 0;
				new_asset->havokEntsShapeData = nullptr;

				new_asset->numSubModels = clipmap->numSubModels;
				new_asset->cmodels = allocator.allocate_array<zonetool::iw7::cmodel_t>(clipmap->numSubModels);
				for (unsigned int i = 0; i < clipmap->numSubModels; i++)
				{
					memcpy(&new_asset->cmodels[i].bounds, &clipmap->cmodels[i].bounds, sizeof(Bounds));
					new_asset->cmodels[i].radius = clipmap->cmodels[i].radius;
					new_asset->cmodels[i].info = nullptr; //reinterpret_cast<zonetool::iw7::ClipInfo*>(clipmap->cmodels[i].info);
					new_asset->cmodels[i].physicsAsset = nullptr;
					new_asset->cmodels[i].physicsShapeOverrideIdx = 0xFFFF;
					new_asset->cmodels[i].navObstacleIdx = 0;
					//new_asset->cmodels[i].edgeFirstIndex = 0;
				}

				new_asset->dynEntCount[0] = clipmap->dynEntCount[0];
				new_asset->dynEntCount[1] = clipmap->dynEntCount[1];

				unsigned short reserved_dynents = 64;
				new_asset->dynEntCount[0] += reserved_dynents;

				unsigned short total_dynents = new_asset->dynEntCount[0] + new_asset->dynEntCount[1];
				new_asset->dynEntCountTotal = total_dynents;

				new_asset->dynEntDefList[0] = allocator.allocate_array<zonetool::iw7::DynEntityDef>(new_asset->dynEntCount[0]);
				new_asset->dynEntPoseList[0][0] = allocator.allocate_array<zonetool::iw7::DynEntityPose>(new_asset->dynEntCount[0]);
				new_asset->dynEntPoseList[1][0] = allocator.allocate_array<zonetool::iw7::DynEntityPose>(new_asset->dynEntCount[0]);
				new_asset->dynEntClientList[0][0] = allocator.allocate_array<zonetool::iw7::DynEntityClient>(new_asset->dynEntCount[0]);
				new_asset->dynEntClientList[1][0] = allocator.allocate_array<zonetool::iw7::DynEntityClient>(new_asset->dynEntCount[0]);

				new_asset->dynEntDefList[1] = allocator.allocate_array<zonetool::iw7::DynEntityDef>(new_asset->dynEntCount[1]);
				new_asset->dynEntPoseList[0][1] = allocator.allocate_array<zonetool::iw7::DynEntityPose>(new_asset->dynEntCount[1]);
				new_asset->dynEntPoseList[1][1] = allocator.allocate_array<zonetool::iw7::DynEntityPose>(new_asset->dynEntCount[1]);
				new_asset->dynEntClientList[0][1] = allocator.allocate_array<zonetool::iw7::DynEntityClient>(new_asset->dynEntCount[1]);
				new_asset->dynEntClientList[1][1] = allocator.allocate_array<zonetool::iw7::DynEntityClient>(new_asset->dynEntCount[1]);

				new_asset->dynEntGlobalIdList[0] = allocator.allocate_array<zonetool::iw7::DynEntityGlobalId>(new_asset->dynEntCountTotal);
				new_asset->dynEntGlobalIdList[1] = allocator.allocate_array<zonetool::iw7::DynEntityGlobalId>(new_asset->dynEntCountTotal);

				for (auto i = 0; i < reserved_dynents; i++)
				{
					auto* dyn = &new_asset->dynEntDefList[0][i];
					dyn->type = zonetool::iw7::DYNENT_TYPE_SCRIPTABLEINST;
					dyn->scriptableMapIndex = 500;
					dyn->unk2 = true;
				}

				const auto copy_dynents = [&](const auto index)
				{
					for (auto i = reserved_dynents; i < new_asset->dynEntCount[index]; i++)
					{
						const auto idx = i - reserved_dynents;

						{
							auto* new_dynent_def = &new_asset->dynEntDefList[index][i];
							auto* dynent_def = &clipmap->dynEntDefList[index][idx];

							const auto convert_type = [](DynEntityType type) -> zonetool::iw7::DynEntityType
							{
								switch (type)
								{
								case DYNENT_TYPE_INVALID:
									return zonetool::iw7::DYNENT_TYPE_INVALID;
									break;
								case DYNENT_TYPE_CLUTTER:
									return zonetool::iw7::DYNENT_TYPE_CLUTTER;
									break;
								case DYNENT_TYPE_DESTRUCT:
									return zonetool::iw7::DYNENT_TYPE_INVALID;
									break;
								case DYNENT_TYPE_HINGE:
									return zonetool::iw7::DYNENT_TYPE_HINGE;
									break;
								case DYNENT_TYPE_SCRIPTABLEINST:
									return zonetool::iw7::DYNENT_TYPE_INVALID; //zonetool::iw7::DYNENT_TYPE_SCRIPTABLEINST;
									break;
								case DYNENT_TYPE_SCRIPTABLEPHYSICS:
									return zonetool::iw7::DYNENT_TYPE_SCRIPTABLEPHYSICS;
									break;
								case DYNENT_TYPE_LINKED:
									return zonetool::iw7::DYNENT_TYPE_LINKED;
									break;
								case DYNENT_TYPE_LINKED_NOSHADOW:
									return zonetool::iw7::DYNENT_TYPE_LINKED_NOSHADOW;
									break;
								}
								return zonetool::iw7::DYNENT_TYPE_INVALID;
							};

							new_dynent_def->type = convert_type(dynent_def->type);
							memcpy(&new_dynent_def->pose, &dynent_def->pose, sizeof(GfxPlacement));
							new_dynent_def->baseModel = reinterpret_cast<zonetool::iw7::XModel*>(dynent_def->baseModel);
							new_dynent_def->brushModel = dynent_def->brushModel;
							new_dynent_def->linkTo = reinterpret_cast<zonetool::iw7::DynEntityLinkToDef*>(dynent_def->linkTo);
							new_dynent_def->scriptableMapIndex = dynent_def->scriptableIndex;
							new_dynent_def->unk2 = true;

							new_dynent_def->scriptableMapIndex = 0; // remove this for now
						}

						{
							auto* dynent_pose_model = &new_asset->dynEntPoseList[zonetool::iw7::DynEntityBasis::DYNENT_BASIS_MODEL][index][i];
							auto* dynent_pose_brush = &new_asset->dynEntPoseList[zonetool::iw7::DynEntityBasis::DYNENT_BASIS_BRUSH][index][i];
							auto* dynent_pose = &clipmap->dynEntPoseList[index][idx];

							// model
							memcpy(&dynent_pose_model->pose, &dynent_pose->pose, sizeof(zonetool::iw7::GfxPlacement));
							dynent_pose_model->numPoses = 1;
							dynent_pose_model->poses = allocator.allocate_array<zonetool::iw7::GfxPlacement>(1);
							memcpy(&dynent_pose_model->poses[0], &dynent_pose_model->pose, sizeof(zonetool::iw7::GfxPlacement));
							dynent_pose_model->radius = dynent_pose->radius;

							// brush
							memcpy(&dynent_pose_brush->pose, &dynent_pose->pose, sizeof(zonetool::iw7::GfxPlacement));
							dynent_pose_brush->numPoses = 1;
							dynent_pose_brush->poses = allocator.allocate_array<zonetool::iw7::GfxPlacement>(1);
							memcpy(&dynent_pose_brush->poses[0], &dynent_pose_brush->pose, sizeof(zonetool::iw7::GfxPlacement));
							dynent_pose_brush->radius = dynent_pose->radius;
						}
					}
				};
				copy_dynents(0);
				copy_dynents(1);

				for (auto i = 0; i < new_asset->dynEntCountTotal; i++)
				{
					new_asset->dynEntGlobalIdList[0][i].basis = 0;
					new_asset->dynEntGlobalIdList[0][i].id = i;

					new_asset->dynEntGlobalIdList[1][i].basis = 0;
					new_asset->dynEntGlobalIdList[1][i].id = i;
				}

				for (auto i = 0; i < 8; i++)
				{
					new_asset->unkIndexes[i] = -1;
				}

				new_asset->unk2Count = 0;
				new_asset->unk2 = nullptr;
				new_asset->unk2_1[0] = nullptr;
				new_asset->unk2_1[1] = nullptr;
				new_asset->unk2_2[0] = nullptr;
				new_asset->unk2_2[1] = nullptr;
				new_asset->unk3Count = 0;
				new_asset->unk3 = nullptr;

				new_asset->clientEntAnchorCount = clipmap->dynEntAnchorCount;
				new_asset->clientEntAnchors = allocator.allocate_array<zonetool::iw7::ClientEntAnchor>(new_asset->clientEntAnchorCount);
				for (unsigned int i = 0; i < new_asset->clientEntAnchorCount; i++)
				{
					new_asset->clientEntAnchors[i].entNum = 0; // runtime data
					new_asset->clientEntAnchors[i].name = static_cast<zonetool::iw7::scr_string_t>(clipmap->dynEntAnchorNames[i]);
				}

				new_asset->scriptableMapEnts.totalInstanceCount = 500;
				new_asset->scriptableMapEnts.runtimeInstanceCount = 500;
				new_asset->scriptableMapEnts.reservedInstanceCount = 500;

				new_asset->scriptableMapEnts.instances = allocator.allocate_array<zonetool::iw7::ScriptableInstance>(new_asset->scriptableMapEnts.totalInstanceCount);

				// these are runtime data
				new_asset->scriptableMapEnts.reservedDynents[0].numReservedDynents = reserved_dynents;
				new_asset->scriptableMapEnts.reservedDynents[0].reservedDynents = 
					allocator.allocate_array<zonetool::iw7::ScriptableReservedDynent>(new_asset->scriptableMapEnts.reservedDynents[0].numReservedDynents);

				// these are runtime data
				new_asset->scriptableMapEnts.reservedDynents[1].numReservedDynents = reserved_dynents;
				new_asset->scriptableMapEnts.reservedDynents[1].reservedDynents =
					allocator.allocate_array<zonetool::iw7::ScriptableReservedDynent>(new_asset->scriptableMapEnts.reservedDynents[1].numReservedDynents);

				new_asset->numMayhemScenes = 0;
				new_asset->mayhemScenes = nullptr;

				new_asset->spawners.spawnerCount = 0;
				new_asset->spawners.spawnerList = nullptr;

				new_asset->audioPASpeakerCount = 0;
				new_asset->audioPASpeakers = nullptr;

				return new_asset;
			}

			zonetool::iw7::MapEnts* convert(clipMap_t* asset, utils::memory::allocator& allocator)
			{
				return generate_mapents(asset, allocator);
			}
		}

		namespace clip_map
		{
			void generate_clip_info(zonetool::iw7::ClipInfo* info, clipMap_t* asset, utils::memory::allocator& allocator)
			{
				info->planeCount = asset->info.planeCount;
				REINTERPRET_CAST_SAFE_TO_FROM(info->planes, asset->info.planes);
			}

			zonetool::iw7::clipMap_t* generate_clipmap(clipMap_t* asset, utils::memory::allocator& allocator)
			{
				// allocate iw7 clipMap_t structure
				const auto iw7_asset = allocator.allocate<zonetool::iw7::clipMap_t>();

				iw7_asset->name = asset->name;
				iw7_asset->isInUse = asset->isInUse;

				generate_clip_info(&iw7_asset->info, asset, allocator);
				iw7_asset->pInfo = &iw7_asset->info;

				iw7_asset->numStaticModels = asset->info.sCollisionData.numStaticModels;
				iw7_asset->staticModelList = allocator.allocate_array<zonetool::iw7::cStaticModel_s>(asset->info.sCollisionData.numStaticModels);
				for (unsigned int i = 0; i < asset->info.sCollisionData.numStaticModels; i++)
				{
					iw7_asset->staticModelList[i].xmodel = reinterpret_cast<zonetool::iw7::XModel*>(asset->info.sCollisionData.staticModelList[i].xmodel);
					std::memcpy(&iw7_asset->staticModelList[i].origin, &asset->info.sCollisionData.staticModelList[i].origin, sizeof(float[3]));
					std::memcpy(&iw7_asset->staticModelList[i].invScaledAxis, &asset->info.sCollisionData.staticModelList[i].invScaledAxis, sizeof(float[3][3]));
					iw7_asset->staticModelList[i].unk1 = false;
					iw7_asset->staticModelList[i].unk2 = false;
					iw7_asset->staticModelList[i].hasTransientModel = false;
					iw7_asset->staticModelList[i].hasTransientPhysicsAsset = false;
				}

				iw7_asset->staticModelCollisionModelList.numModels = asset->info.sCollisionData.numStaticModels;
				iw7_asset->staticModelCollisionModelList.staticModelIndex = allocator.allocate_array<int>(asset->info.sCollisionData.numStaticModels);
				for (unsigned int i = 0; i < asset->info.sCollisionData.numStaticModels; i++)
				{
					iw7_asset->staticModelCollisionModelList.staticModelIndex[i] = i;
				}
				iw7_asset->numStaticModelCollisionModelLists = 0;
				iw7_asset->staticModelCollisionModelLists = nullptr;

				iw7_asset->mapEnts = allocator.allocate<zonetool::iw7::MapEnts>();
				iw7_asset->mapEnts->name = asset->mapEnts->name;

				iw7_asset->stageCount = asset->stageCount;
				iw7_asset->stages = allocator.allocate_array<zonetool::iw7::Stage>(iw7_asset->stageCount);
				for (unsigned int i = 0; i < iw7_asset->stageCount; i++)
				{
					iw7_asset->stages[i].name = asset->stages[i].name;
					memcpy(&iw7_asset->stages[i].origin, &asset->stages[i].origin, sizeof(float[3]));
					iw7_asset->stages[i].triggerIndex = asset->stages[i].triggerIndex;
					iw7_asset->stages[i].sunPrimaryLightIndex = asset->stages[i].sunPrimaryLightIndex;
					iw7_asset->stages[i].entityUID = 0x3A83126F;
				}

				//memcpy(&iw7_asset->stageTrigger, &mapents_converted_asset->trigger, sizeof(MapTriggers));

				iw7_asset->broadphaseMin[0] = -131072.f;
				iw7_asset->broadphaseMin[1] = -131072.f;
				iw7_asset->broadphaseMin[2] = -131072.f;
				iw7_asset->broadphaseMax[0] = 131072.f;
				iw7_asset->broadphaseMax[1] = 131072.f;
				iw7_asset->broadphaseMax[2] = 131072.f;

				iw7_asset->physicsCapacities;

				iw7_asset->havokWorldShapeDataSize = 0;
				iw7_asset->havokWorldShapeData = nullptr;

				iw7_asset->numCollisionHeatmapEntries = 0;
				iw7_asset->collisionHeatmap = nullptr;

				iw7_asset->topDownMapData = nullptr;

				iw7_asset->checksum = asset->checksum;

				return iw7_asset;
			}

			zonetool::iw7::clipMap_t* convert(clipMap_t* asset, utils::memory::allocator& allocator)
			{
				return generate_clipmap(asset, allocator);
			}

			void dump(clipMap_t* asset)
			{
				utils::memory::allocator allocator;

				const auto mapents_converted_asset = mapents::convert(asset, allocator);
				zonetool::iw7::map_ents::dump(mapents_converted_asset);

				const auto converted_asset = convert(asset, allocator);
				zonetool::iw7::clip_map::dump(converted_asset);
			}
		}
	}
}
#endif