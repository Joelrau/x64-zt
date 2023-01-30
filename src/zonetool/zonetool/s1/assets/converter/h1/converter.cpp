#include <std_include.hpp>
#include "converter.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
#define COPY_VALUE(name) \
		static_assert(sizeof(new_asset->name) == sizeof(asset->name)); \
		new_asset->name = asset->name;

#define COPY_VALUE_CAST(name) \
		static_assert(sizeof(new_asset->name) == sizeof(asset->name)); \
		new_asset->name = *reinterpret_cast<decltype(new_asset->name)*>(&asset->name);

#define COPY_ARR(name) \
		static_assert(sizeof(new_asset->name) == sizeof(asset->name)); \
		std::memcpy(&new_asset->name, &asset->name, sizeof(new_asset->name));

#define REINTERPRET_CAST_SAFE(name) \
		static_assert(sizeof(*new_asset->name) == sizeof(*asset->name)); \
		new_asset->name = reinterpret_cast<decltype(new_asset->name)>(asset->name);

#define REINTERPRET_CAST_SAFE_ARR(name, count) \
		for (auto i_ = 0; i_ < count; i_++) \
		{ \
			REINTERPRET_CAST_SAFE(name[i_]); \
		} \

		zonetool::h1::PathData* convert_path_data(PathData* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::PathData>();

			REINTERPRET_CAST_SAFE(name);

			new_asset->nodeCount = asset->nodeCount;
			new_asset->nodes = mem->Alloc<zonetool::h1::pathnode_t>(asset->nodeCount);
			for (unsigned int i = 0; i < asset->nodeCount; i++)
			{
				// something is wrong in these structs
				auto* node = &asset->nodes[i];
				auto* new_node = &new_asset->nodes[i];

				new_node->constant.type = node->constant.type; // convert?
				new_node->constant.spawnflags = node->constant.spawnflags;
				std::memcpy(&new_node->constant.targetname, &node->constant.targetname, sizeof(scr_string_t) * 5); // copy strings
				new_node->constant.animscriptfunc = node->constant.animscriptfunc;
				std::memcpy(&new_node->constant.vLocalOrigin, &node->constant.vLocalOrigin, sizeof(float[3]));
				std::memcpy(&new_node->constant.___u9, &node->constant.___u9, 12);
				std::memcpy(&new_node->constant.parent, &node->constant.parent, 4);
				std::memcpy(&new_node->constant.___u11, &node->constant.___u11, 4);
				std::memcpy(&new_node->constant.wOverlapNode, &node->constant.wOverlapNode, sizeof(short[2]));
				new_node->constant.totalLinkCount = node->constant.totalLinkCount;
				new_node->constant.Links = reinterpret_cast<zonetool::h1::pathlink_s*>(node->constant.Links);
				new_node->constant.unk = zonetool::h1::scr_string_t(0);

				// pad0, pad1

				std::memcpy(&new_node->dynamic, &node->dynamic, sizeof(pathnode_dynamic_t));
				std::memcpy(&new_node->transient, &node->transient, sizeof(pathnode_transient_t));
			}

			new_asset->parentIndexResolved = asset->parentIndexResolved;
			new_asset->version = 26;
			new_asset->visBytes = asset->visBytes;
			new_asset->pathVis = asset->pathVis;
			new_asset->nodeTreeCount = asset->nodeTreeCount;
			new_asset->nodeTree = reinterpret_cast<zonetool::h1::pathnode_tree_t*>(asset->nodeTree);
			new_asset->dynamicNodeGroupCount = asset->dynamicNodeGroupCount;
			new_asset->dynamicNodeGroups = reinterpret_cast<zonetool::h1::PathDynamicNodeGroup*>(asset->dynamicNodeGroups);
			new_asset->exposureBytes = asset->exposureBytes;
			new_asset->pathExposure = asset->pathExposure;
			new_asset->noPeekVisBytes = asset->noPeekVisBytes;
			new_asset->pathNoPeekVis = asset->pathNoPeekVis;
			new_asset->zoneCount = asset->zoneCount;
			new_asset->zonesBytes = asset->zonesBytes;
			new_asset->pathZones = asset->pathZones;
			new_asset->dynStatesBytes = asset->dynStatesBytes;
			new_asset->pathDynStates = asset->pathDynStates;

			return new_asset;
		}

		zonetool::h1::FxEffectDef* convert_fx_effect_def(FxEffectDef* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::FxEffectDef>();

			memcpy(new_asset, asset, sizeof(zonetool::h1::FxEffectDef));

			auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
			new_asset->elemDefs = mem->Alloc<zonetool::h1::FxElemDef>(count);
			for (auto i = 0; i < count; i++)
			{
				auto* elem = &asset->elemDefs[i];
				auto* new_elem = &new_asset->elemDefs[i];

				memcpy(new_elem, elem, sizeof(zonetool::h1::FxElemDef));

				if (elem->elemType == FX_ELEM_TYPE_DECAL)
				{
					if (elem->visuals.markArray)
					{
						new_elem->visuals.markArray = mem->Alloc<zonetool::h1::FxElemMarkVisuals>(elem->visualCount);

						for (unsigned char j = 0; j < elem->visualCount; j++)
						{
							new_elem->visuals.markArray[j].materials[0] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[0]); // mc
							new_elem->visuals.markArray[j].materials[1] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[1]); // wc
							new_elem->visuals.markArray[j].materials[2] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[1]); // wc displacement
						}
					}
				}
			}

			return new_asset;
		}

		zonetool::h1::GfxWorld* convert_gfx_world(GfxWorld* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::GfxWorld>();

			REINTERPRET_CAST_SAFE(name);
			REINTERPRET_CAST_SAFE(baseName);
			new_asset->bspVersion = 111;
			COPY_VALUE(planeCount);
			COPY_VALUE(nodeCount);
			COPY_VALUE(surfaceCount);
			COPY_VALUE(skyCount);
			REINTERPRET_CAST_SAFE(skies);
			COPY_VALUE(portalGroupCount);
			COPY_VALUE(lastSunPrimaryLightIndex);
			COPY_VALUE(primaryLightCount);
			COPY_VALUE(primaryLightEnvCount);
			new_asset->sortKeyLitDecal = 7;
			new_asset->sortKeyEffectDecal = 43;
			new_asset->sortKeyTopDecal = 17;
			new_asset->sortKeyEffectAuto = 53;
			new_asset->sortKeyDistortion = 48;
			new_asset->sortKeyHair = 18;
			new_asset->sortKeyEffectBlend = 33;

			COPY_VALUE(dpvsPlanes.cellCount);
			REINTERPRET_CAST_SAFE(dpvsPlanes.planes);
			REINTERPRET_CAST_SAFE(dpvsPlanes.nodes);
			REINTERPRET_CAST_SAFE(dpvsPlanes.sceneEntCellBits);

			REINTERPRET_CAST_SAFE(skies);
			REINTERPRET_CAST_SAFE(aabbTreeCounts);
			REINTERPRET_CAST_SAFE(aabbTrees);
			REINTERPRET_CAST_SAFE(cells);

			REINTERPRET_CAST_SAFE(portalGroup);

			COPY_VALUE(unk_vec4_count_0);
			REINTERPRET_CAST_SAFE(unk_vec4_0);

			COPY_VALUE(draw.reflectionProbeCount);
			REINTERPRET_CAST_SAFE(draw.reflectionProbes);
			REINTERPRET_CAST_SAFE(draw.reflectionProbeOrigins);
			REINTERPRET_CAST_SAFE(draw.reflectionProbeTextures);
			COPY_VALUE(draw.reflectionProbeReferenceCount);
			REINTERPRET_CAST_SAFE(draw.reflectionProbeReferenceOrigins);
			REINTERPRET_CAST_SAFE(draw.reflectionProbeReferences);
			COPY_VALUE(draw.lightmapCount);
			REINTERPRET_CAST_SAFE(draw.lightmaps);
			REINTERPRET_CAST_SAFE(draw.lightmapPrimaryTextures);
			REINTERPRET_CAST_SAFE(draw.lightmapSecondaryTextures);
			REINTERPRET_CAST_SAFE(draw.lightmapOverridePrimary);
			REINTERPRET_CAST_SAFE(draw.lightmapOverrideSecondary);
			COPY_ARR(draw.u1);
			COPY_ARR(draw.u2);
			COPY_VALUE(draw.u3);
			COPY_VALUE(draw.trisType);
			COPY_VALUE(draw.vertexCount);
			REINTERPRET_CAST_SAFE(draw.vd.vertices);
			COPY_VALUE(draw.vertexLayerDataSize);
			REINTERPRET_CAST_SAFE(draw.vld.data);
			COPY_VALUE(draw.indexCount);
			REINTERPRET_CAST_SAFE(draw.indices);
			new_asset->draw.displacementParmsCount = 0;
			new_asset->draw.displacementParms = nullptr;

			COPY_VALUE_CAST(lightGrid);
			COPY_VALUE(modelCount);
			REINTERPRET_CAST_SAFE(models);
			COPY_ARR(unkBounds);
			COPY_ARR(shadowBounds);
			COPY_VALUE(checksum);
			COPY_VALUE(materialMemoryCount);
			REINTERPRET_CAST_SAFE(materialMemory);
			COPY_VALUE_CAST(sun);
			COPY_ARR(outdoorLookupMatrix);
			REINTERPRET_CAST_SAFE(outdoorImage);
			REINTERPRET_CAST_SAFE(cellCasterBits);
			REINTERPRET_CAST_SAFE(cellHasSunLitSurfsBits);
			REINTERPRET_CAST_SAFE(sceneDynModel);
			REINTERPRET_CAST_SAFE(sceneDynBrush);
			REINTERPRET_CAST_SAFE(primaryLightEntityShadowVis);
			REINTERPRET_CAST_SAFE_ARR(primaryLightDynEntShadowVis, 2);
			REINTERPRET_CAST_SAFE(nonSunPrimaryLightForModelDynEnt);
			REINTERPRET_CAST_SAFE(shadowGeom);
			REINTERPRET_CAST_SAFE(shadowGeomOptimized);
			REINTERPRET_CAST_SAFE(lightRegion);

			// dpvs
			{
				COPY_VALUE(dpvs.smodelCount);
				COPY_VALUE(dpvs.subdivVertexLightingInfoCount);
				COPY_VALUE(dpvs.staticSurfaceCount);
				COPY_VALUE(dpvs.litOpaqueSurfsBegin);
				COPY_VALUE(dpvs.litOpaqueSurfsEnd);
				COPY_VALUE(dpvs.unkSurfsBegin);
				COPY_VALUE(dpvs.unkSurfsEnd);
				COPY_VALUE(dpvs.litDecalSurfsBegin);
				COPY_VALUE(dpvs.litDecalSurfsEnd);
				COPY_VALUE(dpvs.litTransSurfsBegin);
				COPY_VALUE(dpvs.litTransSurfsEnd);
				COPY_VALUE(dpvs.shadowCasterSurfsBegin);
				COPY_VALUE(dpvs.shadowCasterSurfsEnd);
				COPY_VALUE(dpvs.emissiveSurfsBegin);
				COPY_VALUE(dpvs.emissiveSurfsEnd);
				COPY_VALUE(dpvs.smodelVisDataCount);
				COPY_VALUE(dpvs.surfaceVisDataCount);
				REINTERPRET_CAST_SAFE_ARR(dpvs.smodelVisData, 4);
				REINTERPRET_CAST_SAFE_ARR(dpvs.smodelUnknownVisData, 27);
				REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceVisData, 4);
				REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceUnknownVisData, 27);
				REINTERPRET_CAST_SAFE_ARR(dpvs.smodelUmbraVisData, 4);
				REINTERPRET_CAST_SAFE_ARR(dpvs.surfaceUmbraVisData, 4);
				REINTERPRET_CAST_SAFE(dpvs.lodData);
				new_asset->dpvs.tessellationCutoffVisData = mem->Alloc<unsigned int>(asset->dpvs.surfaceVisDataCount);
				REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
				REINTERPRET_CAST_SAFE(dpvs.smodelInsts);
				REINTERPRET_CAST_SAFE(dpvs.surfaces);

				new_asset->dpvs.surfaces = mem->Alloc<zonetool::h1::GfxSurface>(asset->surfaceCount);
				for (unsigned int i = 0; i < asset->surfaceCount; i++)
				{
					COPY_VALUE(dpvs.surfaces[i].tris.vertexLayerData);
					COPY_VALUE(dpvs.surfaces[i].tris.firstVertex);
					COPY_VALUE(dpvs.surfaces[i].tris.maxEdgeLength);
					new_asset->dpvs.surfaces[i].tris.unk = -1;
					COPY_VALUE(dpvs.surfaces[i].tris.vertexCount);
					COPY_VALUE(dpvs.surfaces[i].tris.triCount);
					COPY_VALUE(dpvs.surfaces[i].tris.baseIndex);
					new_asset->dpvs.surfaces[i].material = reinterpret_cast<zonetool::h1::Material*>(asset->dpvs.surfaces[i].material);
					COPY_ARR(dpvs.surfaces[i].laf);
				}

				new_asset->dpvs.surfacesBounds = mem->Alloc<zonetool::h1::GfxSurfaceBounds>(asset->surfaceCount);
				for (unsigned int i = 0; i < asset->surfaceCount; i++)
				{
					COPY_ARR(dpvs.surfacesBounds[i].bounds);
				}

				REINTERPRET_CAST_SAFE(dpvs.smodelDrawInsts);

				REINTERPRET_CAST_SAFE(dpvs.unknownSModelVisData1);
				REINTERPRET_CAST_SAFE(dpvs.unknownSModelVisData2);
				REINTERPRET_CAST_SAFE(dpvs.smodelLighting);
				REINTERPRET_CAST_SAFE(dpvs.subdivVertexLighting);
				REINTERPRET_CAST_SAFE(dpvs.surfaceMaterials);
				REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
				COPY_VALUE(dpvs.sunShadowOptCount);
				COPY_VALUE(dpvs.sunSurfVisDataCount);
				REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
				REINTERPRET_CAST_SAFE(dpvs.surfaceDeptAndSurf);
				REINTERPRET_CAST_SAFE(dpvs.constantBuffersLit);
				REINTERPRET_CAST_SAFE(dpvs.constantBuffersAmbient);
				COPY_VALUE(dpvs.usageCount);
			}

			COPY_VALUE_CAST(dpvsDyn);
			COPY_VALUE(mapVtxChecksum);
			COPY_VALUE(heroOnlyLightCount);
			REINTERPRET_CAST_SAFE(heroOnlyLights);
			COPY_VALUE(fogTypesAllowed);
			COPY_VALUE(umbraTomeSize);
			REINTERPRET_CAST_SAFE(umbraTomeData);
			COPY_VALUE_CAST(umbraTomePtr);
			COPY_VALUE(mdaoVolumesCount);
			REINTERPRET_CAST_SAFE(mdaoVolumes);

			COPY_VALUE(unk1);
			COPY_ARR(unk2);
			COPY_VALUE(unk3);

			COPY_VALUE_CAST(buildInfo);

			return new_asset;
		}

		zonetool::h1::MapEnts* convert_map_ents(MapEnts* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::MapEnts>();

			REINTERPRET_CAST_SAFE(name);

			// todo:
			REINTERPRET_CAST_SAFE(entityString);
			COPY_VALUE(numEntityChars);

			COPY_VALUE_CAST(trigger);

			COPY_VALUE_CAST(clientTrigger.trigger);
			COPY_VALUE(clientTrigger.numClientTriggerNodes);
			COPY_VALUE_CAST(clientTrigger.clientTriggerAabbTree);
			COPY_VALUE(clientTrigger.triggerStringLength);
			REINTERPRET_CAST_SAFE(clientTrigger.triggerString);
			REINTERPRET_CAST_SAFE(clientTrigger.visionSetTriggers);
			REINTERPRET_CAST_SAFE(clientTrigger.blendLookup);
			new_asset->clientTrigger.unk1 = asset->clientTrigger.unk1;
			REINTERPRET_CAST_SAFE(clientTrigger.triggerType); // convert?
			REINTERPRET_CAST_SAFE(clientTrigger.origins);
			REINTERPRET_CAST_SAFE(clientTrigger.scriptDelay);
			REINTERPRET_CAST_SAFE(clientTrigger.audioTriggers);
			new_asset->clientTrigger.unk2 = asset->clientTrigger.unk2;
			new_asset->clientTrigger.unk3 = asset->clientTrigger.unk3;
			new_asset->clientTrigger.unk4 = asset->clientTrigger.unk4;
			new_asset->clientTrigger.unk5 = asset->clientTrigger.unk5;
			new_asset->clientTrigger.unk6 = mem->Alloc<short>(asset->clientTrigger.trigger.count);

			COPY_VALUE_CAST(clientTriggerBlend);
			COPY_VALUE_CAST(spawnList);
			COPY_VALUE_CAST(splineList);

			return new_asset;
		}

		zonetool::h1::Material* convert_material(Material* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::Material>();

			REINTERPRET_CAST_SAFE(name);

			new_asset->info.gameFlags = asset->info.gameFlags; // convert?
			new_asset->info.sortKey = asset->info.sortKey; // convert?
			new_asset->info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
			new_asset->info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
			new_asset->info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
			new_asset->info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
			new_asset->info.renderFlags = asset->info.renderFlags; // convert?
			COPY_VALUE_CAST(info.drawSurf); // todo?
			COPY_VALUE_CAST(info.surfaceTypeBits); // todo?
			new_asset->info.hashIndex = asset->info.hashIndex;

			std::memset(new_asset->stateBitsEntry, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
			std::memcpy(new_asset->stateBitsEntry, asset->stateBitsEntry, MaterialTechniqueType::TECHNIQUE_COUNT);

			new_asset->textureCount = asset->textureCount;
			new_asset->constantCount = asset->constantCount;
			new_asset->stateBitsCount = asset->stateBitsCount;
			new_asset->stateFlags = asset->stateFlags; // convert?
			new_asset->cameraRegion = asset->cameraRegion; // convert?
			new_asset->materialType = asset->materialType; // convert?
			new_asset->layerCount = asset->layerCount;
			new_asset->assetFlags = asset->assetFlags; // convert?

			new_asset->techniqueSet = convert_techset(asset->techniqueSet, mem);
			REINTERPRET_CAST_SAFE(textureTable);
			REINTERPRET_CAST_SAFE(constantTable);
			REINTERPRET_CAST_SAFE(stateBitsTable);

			std::memset(new_asset->constantBufferIndex, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
			std::memcpy(new_asset->constantBufferIndex, asset->constantBufferIndex, MaterialTechniqueType::TECHNIQUE_COUNT);

			REINTERPRET_CAST_SAFE(constantBufferTable);
			new_asset->constantBufferCount = asset->constantBufferCount;

			REINTERPRET_CAST_SAFE(subMaterials);

			return new_asset;
		}

		zonetool::h1::snd_alias_list_t* convert_sound(snd_alias_list_t* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::snd_alias_list_t>();

			std::memcpy(new_asset, asset, sizeof(snd_alias_list_t));
			new_asset->head = mem->Alloc<zonetool::h1::snd_alias_t>(asset->count);
			for (unsigned char i = 0; i < asset->count; i++)
			{
				auto* head = &asset->head[i];
				auto* new_head = &new_asset->head[i];

				new_head->aliasName = head->aliasName;
				new_head->subtitle = head->subtitle;
				new_head->secondaryAliasName = head->secondaryAliasName;
				new_head->chainAliasName = head->chainAliasName;
				new_head->soundFile = reinterpret_cast<zonetool::h1::SoundFile*>(head->soundFile);
				new_head->mixerGroup = head->chainAliasName;
				new_head->poly = head->poly;
				new_head->polyGlobal = 174;
				new_head->polyEntityType = 0;
				new_head->polyGlobalType = 0;
				new_head->dspBusIndex = head->dspBusIndex > 8 ? head->dspBusIndex + 1 : head->dspBusIndex;
				new_head->priority = head->priority;
				// pad/unknown
				new_head->volMin = head->volMin;
				new_head->volMax = head->volMax;
				new_head->volModIndex = head->volModIndex; // should be the same
				new_head->pitchMin = head->pitchMin;
				new_head->pitchMax = head->pitchMax;
				new_head->distMin = head->distMin;
				new_head->distMax = head->distMax;
				new_head->velocityMin = head->velocityMin;
				new_head->flags = head->flags; // convert?
				new_head->masterPriority = head->masterPriority;
				new_head->masterPercentage = head->masterPercentage;
				new_head->slavePercentage = head->slavePercentage;
				new_head->playbackPercentage = head->playbackPercentage;
				new_head->probability = head->probability;
				new_head->u1 = head->u1;
				new_head->sndContext = reinterpret_cast<zonetool::h1::SndContext*>(head->sndContext);
				new_head->sequence = head->sequence;
				new_head->lfePercentage = head->lfePercentage;
				new_head->centerPercentage = head->centerPercentage;
				new_head->startDelay = head->startDelay;
				new_head->sndCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->sndCurve);
				new_head->envelopMin = head->envelopMin;
				new_head->envelopMax = head->envelopMax;
				new_head->lpfCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->lpfCurve);
				new_head->reverbSendCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->reverbSendCurve);
				new_head->speakerMap = reinterpret_cast<zonetool::h1::SpeakerMap*>(head->speakerMap);
				new_head->reverbWetMixOverride = head->reverbWetMixOverride;
				new_head->reverbMultiplier = head->focusPercentage; // should be same
				new_head->smartPanDistance2d = head->smartPanDistance2d;
				new_head->smartPanDistance3d = head->smartPanDistance3d;
				new_head->smartPanAttenuation3d = head->smartPanAttenuation3d;
				new_head->envelopPercentage = head->envelopPercentage;
				new_head->stereo3dAngle = head->stereo3dAngle;
				new_head->stereo3dStart = head->stereo3dStart;
				new_head->stereo3dEnd = head->stereo3dEnd;
				new_head->allowDoppler = head->allowDoppler;
				new_head->dopplerPreset = reinterpret_cast<zonetool::h1::DopplerPreset*>(head->dopplerPreset);
				new_head->u2 = head->u2;

				if (head->soundFile)
				{
					if (head->soundFile->type == SAT_STREAMED)
					{
						new_head->soundFile = mem->Alloc<zonetool::h1::SoundFile>();
						new_head->soundFile->exists = false;
						new_head->soundFile->type = zonetool::h1::SAT_STREAMED;
						new_head->soundFile->u.streamSnd.filename.info.raw.dir = "";
						new_head->soundFile->u.streamSnd.filename.info.raw.name = "";
					}
				}
			}

			return new_asset;
		}

		const std::unordered_map <std::uint16_t, std::uint16_t> const_src_code_map =
		{
			{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION1, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE1, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE1, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET1, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT1, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM1, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION2, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE2, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE2, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET2, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT2, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM2, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION3, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE3, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE3, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET3, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT3, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM3, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DYN_COUNT, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_COUNT},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DYN_TYPES, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_TYPES},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_DYN_SHADOW_TYPES, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_SHADOW_TYPES},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_COLOR, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_COLOR},
			{zonetool::s1::CONST_SRC_CODE_GAMETIME, zonetool::h1::CONST_SRC_CODE_GAMETIME},
			{zonetool::s1::CONST_SRC_CODE_GENERIC_MATERIAL_DATA, zonetool::h1::CONST_SRC_CODE_GENERIC_MATERIAL_DATA},
			{zonetool::s1::CONST_SRC_CODE_EYEOFFSET, zonetool::h1::CONST_SRC_CODE_EYEOFFSET},
			{zonetool::s1::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET},
			{zonetool::s1::CONST_SRC_CODE_EMBLEM_LUT_SELECTOR, zonetool::h1::CONST_SRC_CODE_EMBLEM_LUT_SELECTOR},
			{zonetool::s1::CONST_SRC_CODE_MODEL_VELOCITY_PARMS, zonetool::h1::CONST_SRC_CODE_MODEL_VELOCITY_PARMS},
			{zonetool::s1::CONST_SRC_CODE_MODEL_VELOCITY_SKINNED_PARMS, zonetool::h1::CONST_SRC_CODE_MODEL_VELOCITY_SKINNED_PARMS},
			//{zonetool::s1::CONST_SRC_CODE_HUD_OUTLINE_PARMS, zonetool::h1::CONST_SRC_CODE_HUD_OUTLINE_PARMS},
			//{zonetool::s1::CONST_SRC_CODE_CHROMATIC_ABERRATION_PARMS, zonetool::h1::CONST_SRC_CODE_CHROMATIC_ABERRATION_PARMS},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL0, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL0},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL1, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL1},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL2, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL2},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL3, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL3},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL4, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL4},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL5, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL5},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL6, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL6},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL7, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL7},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL8, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL8},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL9, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL9},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLA, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLA},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLB, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLB},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLC, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLC},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLD, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLD},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLE, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLE},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLF, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLF},
			{zonetool::s1::CONST_SRC_CODE_SCRIPT_PARMS, zonetool::h1::CONST_SRC_CODE_SCRIPT_PARMS},
			{zonetool::s1::CONST_SRC_CODE_EFFECT_MODEL_COLOR, zonetool::h1::CONST_SRC_CODE_EFFECT_MODEL_COLOR},
			{zonetool::s1::CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE, zonetool::h1::CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_0, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_0},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_1, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_1},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_2, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_2},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_3, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_3},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_4, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_4},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_5, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_5},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_6, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_6},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_7, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_7},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_8, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_8},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_9, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_9},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_10, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_10},
			{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_11, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_11},
			{zonetool::s1::CONST_SRC_CODE_COLOR_MATRIX_R, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_R},
			{zonetool::s1::CONST_SRC_CODE_COLOR_MATRIX_G, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_G},
			{zonetool::s1::CONST_SRC_CODE_COLOR_MATRIX_B, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_B},
			{zonetool::s1::CONST_SRC_CODE_RENDER_TARGET_SIZE, zonetool::h1::CONST_SRC_CODE_RENDER_TARGET_SIZE},
			{zonetool::s1::CONST_SRC_CODE_RENDER_SOURCE_SIZE, zonetool::h1::CONST_SRC_CODE_RENDER_SOURCE_SIZE},
			{zonetool::s1::CONST_SRC_CODE_VEIL_PARAMS, zonetool::h1::CONST_SRC_CODE_VEIL_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_FXAA_RCPFRAMEOPT, zonetool::h1::CONST_SRC_CODE_FXAA_RCPFRAMEOPT},
			{zonetool::s1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_1, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_1},
			{zonetool::s1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_2, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_2},
			{zonetool::s1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_3, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_3},
			{zonetool::s1::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS, zonetool::h1::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_NEARPLANE_ORG, zonetool::h1::CONST_SRC_CODE_NEARPLANE_ORG},
			{zonetool::s1::CONST_SRC_CODE_NEARPLANE_DX, zonetool::h1::CONST_SRC_CODE_NEARPLANE_DX},
			{zonetool::s1::CONST_SRC_CODE_NEARPLANE_DY, zonetool::h1::CONST_SRC_CODE_NEARPLANE_DY},
			{zonetool::s1::CONST_SRC_CODE_SSAO_POWER_BB_SHARPNESS_STEP, zonetool::h1::CONST_SRC_CODE_SSAO_POWER_BB_SHARPNESS_STEP},
			{zonetool::s1::CONST_SRC_CODE_SSAO_COLOR_COEFF, zonetool::h1::CONST_SRC_CODE_SSAO_COLOR_COEFF},
			{zonetool::s1::CONST_SRC_CODE_SSAO_FALLOFF_DEPTH_SCALE, zonetool::h1::CONST_SRC_CODE_SSAO_FALLOFF_DEPTH_SCALE},
			{zonetool::s1::CONST_SRC_CODE_SSAO_UV_TO_ROT_SCALE_AND_FADE, zonetool::h1::CONST_SRC_CODE_SSAO_UV_TO_ROT_SCALE_AND_FADE},
			{zonetool::s1::CONST_SRC_CODE_SSAO_SAMPLE_MAT_SCALE, zonetool::h1::CONST_SRC_CODE_SSAO_SAMPLE_MAT_SCALE},
			{zonetool::s1::CONST_SRC_CODE_SSAO_DEPTH_OFFSET_REJECT, zonetool::h1::CONST_SRC_CODE_SSAO_DEPTH_OFFSET_REJECT},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_LIGHTDIRX2, zonetool::h1::CONST_SRC_CODE_LIT2D_LIGHTDIRX2},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_SPECHALFANGLEDIRX2, zonetool::h1::CONST_SRC_CODE_LIT2D_SPECHALFANGLEDIRX2},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_AMBIENTCOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_AMBIENTCOLOR},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_DIFFUSECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_DIFFUSECOLOR},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_SPECCOLOR_SPECEXPONENT, zonetool::h1::CONST_SRC_CODE_LIT2D_SPECCOLOR_SPECEXPONENT},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_ADDITIVECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_ADDITIVECOLOR},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_AMBIENTCOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_AMBIENTCOLOR},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_DIFFUSECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_DIFFUSECOLOR},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_SPECCOLOR_SPECEXPONENT, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_SPECCOLOR_SPECEXPONENT},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_ADDITIVECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_ADDITIVECOLOR},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_HUDBLOOD_COLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_HUDBLOOD_COLOR},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_HUDBLOOD_COLOR_THIN, zonetool::h1::CONST_SRC_CODE_LIT2D_HUDBLOOD_COLOR_THIN},
			{zonetool::s1::CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR, zonetool::h1::CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR},
			{zonetool::s1::CONST_SRC_CODE_DOF_EQUATION_SCENE, zonetool::h1::CONST_SRC_CODE_DOF_EQUATION_SCENE},
			{zonetool::s1::CONST_SRC_CODE_DOF_LERP_SCALE, zonetool::h1::CONST_SRC_CODE_DOF_LERP_SCALE},
			{zonetool::s1::CONST_SRC_CODE_DOF_LERP_BIAS, zonetool::h1::CONST_SRC_CODE_DOF_LERP_BIAS},
			{zonetool::s1::CONST_SRC_CODE_DOF_ROW_DELTA, zonetool::h1::CONST_SRC_CODE_DOF_ROW_DELTA},
			{zonetool::s1::CONST_SRC_CODE_DOF_EQUATION, zonetool::h1::CONST_SRC_CODE_DOF_EQUATION},
			{zonetool::s1::CONST_SRC_CODE_DOF_PARAMS, zonetool::h1::CONST_SRC_CODE_DOF_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_DOF_TILE_PARAMS, zonetool::h1::CONST_SRC_CODE_DOF_TILE_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_FRAME_COUNT_PARAMS, zonetool::h1::CONST_SRC_CODE_FRAME_COUNT_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_MOTION_MATRIX_X, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_X},
			{zonetool::s1::CONST_SRC_CODE_MOTION_MATRIX_Y, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_Y},
			{zonetool::s1::CONST_SRC_CODE_MOTION_MATRIX_W, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_W},
			{zonetool::s1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
			{zonetool::s1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
			{zonetool::s1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
			{zonetool::s1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
			{zonetool::s1::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM, zonetool::h1::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM},
			{zonetool::s1::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS, zonetool::h1::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS},
			{zonetool::s1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET},
			{zonetool::s1::CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS, zonetool::h1::CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS},
			{zonetool::s1::CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF, zonetool::h1::CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF},
			{zonetool::s1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R0},
			{zonetool::s1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R1},
			{zonetool::s1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R2},
			{zonetool::s1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R3},
			{zonetool::s1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R0},
			{zonetool::s1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R1},
			{zonetool::s1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R2},
			{zonetool::s1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R3},
			{zonetool::s1::CONST_SRC_CODE_MDAO_SCREEN_PARAMS, zonetool::h1::CONST_SRC_CODE_MDAO_SCREEN_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_MDAO_CAMERA_PARAMS, zonetool::h1::CONST_SRC_CODE_MDAO_CAMERA_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_MDAO_LOOKUP_PARAMS, zonetool::h1::CONST_SRC_CODE_MDAO_LOOKUP_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_MDAO_VOLUME_PARAMS, zonetool::h1::CONST_SRC_CODE_MDAO_VOLUME_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS, zonetool::h1::CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS},
			{zonetool::s1::CONST_SRC_CODE_SCOPE_SAMPLE_LIMITS_PS, zonetool::h1::CONST_SRC_CODE_SCOPE_SAMPLE_LIMITS_PS},
			{zonetool::s1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS, zonetool::h1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS},
			{zonetool::s1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS, zonetool::h1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS},
			{zonetool::s1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS, zonetool::h1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET},
			{zonetool::s1::CONST_SRC_CODE_STATIC_VELOCITY_PARMS, zonetool::h1::CONST_SRC_CODE_STATIC_VELOCITY_PARMS},
			{zonetool::s1::CONST_SRC_CODE_MOTION_BLUR_HQ_PARAMS, zonetool::h1::CONST_SRC_CODE_MOTION_BLUR_HQ_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_MOTION_BLUR_HQ_TILE_MAX_PARAMS, zonetool::h1::CONST_SRC_CODE_MOTION_BLUR_HQ_TILE_MAX_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS},
			{zonetool::s1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS},
			{zonetool::s1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS},
			{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BIAS, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BIAS},
			{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_SCALE, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SCALE},
			{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_PARTITION_UV_OFFSET, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_PARTITION_UV_OFFSET},
			{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_CASCADE_MASK, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_CASCADE_MASK},
			{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BLEND, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BLEND},
			{zonetool::s1::CONST_SRC_CODE_ZNEAR, zonetool::h1::CONST_SRC_CODE_ZNEAR},
			{zonetool::s1::CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE, zonetool::h1::CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE},
			{zonetool::s1::CONST_SRC_CODE_INV_SCENE_PROJECTION, zonetool::h1::CONST_SRC_CODE_INV_SCENE_PROJECTION},
			{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_0_DIR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_0_DIR},
			{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_0_COLOR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_0_COLOR},
			{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_1_DIR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_1_DIR},
			{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_1_COLOR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_1_COLOR},
			{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_TECHNIQUE, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_TECHNIQUE},
			{zonetool::s1::CONST_SRC_CODE_DEBUG_BUMPMAP, zonetool::h1::CONST_SRC_CODE_DEBUG_BUMPMAP},
			{zonetool::s1::CONST_SRC_CODE_MATERIAL_COLOR, zonetool::h1::CONST_SRC_CODE_MATERIAL_COLOR},
			{zonetool::s1::CONST_SRC_CODE_FOG, zonetool::h1::CONST_SRC_CODE_FOG},
			{zonetool::s1::CONST_SRC_CODE_FOG_COLOR_LINEAR, zonetool::h1::CONST_SRC_CODE_FOG_COLOR_LINEAR},
			{zonetool::s1::CONST_SRC_CODE_FOG_COLOR_GAMMA, zonetool::h1::CONST_SRC_CODE_FOG_COLOR_GAMMA},
			{zonetool::s1::CONST_SRC_CODE_FOG_SUN_CONSTS, zonetool::h1::CONST_SRC_CODE_FOG_SUN_CONSTS},
			{zonetool::s1::CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR, zonetool::h1::CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR},
			{zonetool::s1::CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA, zonetool::h1::CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA},
			{zonetool::s1::CONST_SRC_CODE_FOG_SUN_DIR, zonetool::h1::CONST_SRC_CODE_FOG_SUN_DIR},
			{zonetool::s1::CONST_SRC_CODE_FOG_SKY_FOG, zonetool::h1::CONST_SRC_CODE_FOG_SKY_FOG},
			{zonetool::s1::CONST_SRC_CODE_FOG_HEIGHT_FOG, zonetool::h1::CONST_SRC_CODE_FOG_HEIGHT_FOG},
			{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_0, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_0},
			{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_1, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_1},
			{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_2, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_2},
			{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_3, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_3},
			{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_4, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_4},
			{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_5, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_5},
			{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_6, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_6},
			{zonetool::s1::CONST_SRC_CODE_FOG_SKY_DIR, zonetool::h1::CONST_SRC_CODE_FOG_SKY_DIR},
			{zonetool::s1::CONST_SRC_CODE_FOG_DEPTHHACK, zonetool::h1::CONST_SRC_CODE_FOG_DEPTHHACK},
			{zonetool::s1::CONST_SRC_CODE_GLOW_SETUP, zonetool::h1::CONST_SRC_CODE_GLOW_SETUP},
			{zonetool::s1::CONST_SRC_CODE_GLOW_APPLY, zonetool::h1::CONST_SRC_CODE_GLOW_APPLY},
			{zonetool::s1::CONST_SRC_CODE_GLOW_SETUP_ALT_COLOR_SCALE, zonetool::h1::CONST_SRC_CODE_GLOW_SETUP_ALT_COLOR_SCALE},
			{zonetool::s1::CONST_SRC_CODE_COLOR_BIAS, zonetool::h1::CONST_SRC_CODE_COLOR_BIAS},
			{zonetool::s1::CONST_SRC_CODE_COLOR_TINT_BASE, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_BASE},
			{zonetool::s1::CONST_SRC_CODE_COLOR_TINT_DELTA, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_DELTA},
			{zonetool::s1::CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA},
			{zonetool::s1::CONST_SRC_CODE_PERCEPTUAL_PARMS, zonetool::h1::CONST_SRC_CODE_PERCEPTUAL_PARMS},
			{zonetool::s1::CONST_SRC_CODE_FILMIC_PARMS, zonetool::h1::CONST_SRC_CODE_FILMIC_PARMS},
			{zonetool::s1::CONST_SRC_CODE_FILMIC_SHOULDER_PARMS, zonetool::h1::CONST_SRC_CODE_FILMIC_SHOULDER_PARMS},
			{zonetool::s1::CONST_SRC_CODE_FILMIC_TOE_PARMS, zonetool::h1::CONST_SRC_CODE_FILMIC_TOE_PARMS},
			{zonetool::s1::CONST_SRC_CODE_TONEMAP_PARMS, zonetool::h1::CONST_SRC_CODE_TONEMAP_PARMS},
			{zonetool::s1::CONST_SRC_CODE_HDR_STAGE_PARMS, zonetool::h1::CONST_SRC_CODE_HDR_STAGE_PARMS},
			{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_0, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_0},
			{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_1, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_1},
			{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_2, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_2},
			{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_3, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_3},
			{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_4, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_4},
			{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_5, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_5},
			{zonetool::s1::CONST_SRC_CODE_HUDFX_PARMS, zonetool::h1::CONST_SRC_CODE_HUDFX_PARMS},
			{zonetool::s1::CONST_SRC_CODE_HUDFX_PARMS2, zonetool::h1::CONST_SRC_CODE_HUDFX_PARMS2},
			{zonetool::s1::CONST_SRC_CODE_SSSS_PARAMS, zonetool::h1::CONST_SRC_CODE_SSSS_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_SSSS_SCALE, zonetool::h1::CONST_SRC_CODE_SSSS_SCALE},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_0, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_0},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_1, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_1},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_2, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_2},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_3, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_3},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_4, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_4},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_5, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_5},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_6, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_6},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_7, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_7},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_8, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_8},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_9, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_9},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_10, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_10},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_11, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_11},
			{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_12, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_12},
			{zonetool::s1::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS, zonetool::h1::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS},
			{zonetool::s1::CONST_SRC_CODE_ENVMAP_PARMS, zonetool::h1::CONST_SRC_CODE_ENVMAP_PARMS},
			{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_2, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_CASCADE_V_CLAMP, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_CASCADE_V_CLAMP},
			{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_NEAR_FAR_PLANE, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_NEAR_FAR_PLANE},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_1},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_2},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_3},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_1},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_2},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_3},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_0},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_1},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_2},
			{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_3},
			{zonetool::s1::CONST_SRC_CODE_COMPOSITE_FX_DISTORTION, zonetool::h1::CONST_SRC_CODE_COMPOSITE_FX_DISTORTION},
			{zonetool::s1::CONST_SRC_CODE_POSTFX_FADE_EFFECT, zonetool::h1::CONST_SRC_CODE_POSTFX_FADE_EFFECT},
			{zonetool::s1::CONST_SRC_CODE_SCENE_UV_MULT, zonetool::h1::CONST_SRC_CODE_SCENE_UV_MULT},
			{zonetool::s1::CONST_SRC_CODE_SCENE_VIEW_MULT, zonetool::h1::CONST_SRC_CODE_SCENE_VIEW_MULT},
			{zonetool::s1::CONST_SRC_CODE_TEXCOORD_CLAMP_0, zonetool::h1::CONST_SRC_CODE_TEXCOORD_CLAMP_0},
			{zonetool::s1::CONST_SRC_CODE_TEXCOORD_CLAMP_1, zonetool::h1::CONST_SRC_CODE_TEXCOORD_CLAMP_1},
			{zonetool::s1::CONST_SRC_CODE_VIEWPORT_DIMENSIONS, zonetool::h1::CONST_SRC_CODE_VIEWPORT_DIMENSIONS},
			{zonetool::s1::CONST_SRC_CODE_FRAMEBUFFER_READ, zonetool::h1::CONST_SRC_CODE_FRAMEBUFFER_READ},
			{zonetool::s1::CONST_SRC_CODE_THERMAL_COLOR_OFFSET, zonetool::h1::CONST_SRC_CODE_THERMAL_COLOR_OFFSET},
			{zonetool::s1::CONST_SRC_CODE_THERMAL_FADE_CONTROL, zonetool::h1::CONST_SRC_CODE_THERMAL_FADE_CONTROL},
			{zonetool::s1::CONST_SRC_CODE_THERMAL_FADE_COLOR, zonetool::h1::CONST_SRC_CODE_THERMAL_FADE_COLOR},
			{zonetool::s1::CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS, zonetool::h1::CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS},
			{zonetool::s1::CONST_SRC_CODE_TESSELLATION_PARMS, zonetool::h1::CONST_SRC_CODE_TESSELLATION_PARMS},
			{zonetool::s1::CONST_SRC_CODE_TESSELLATION_PARMS2, zonetool::h1::CONST_SRC_CODE_TESSELLATION_PARMS2},
			{zonetool::s1::CONST_SRC_CODE_EYE_PARAMETER_DVAR, zonetool::h1::CONST_SRC_CODE_EYE_PARAMETER_DVAR},
			{zonetool::s1::CONST_SRC_CODE_MP_RIM_PARAMETER_DVAR1, zonetool::h1::CONST_SRC_CODE_MP_RIM_PARAMETER_DVAR1},
			{zonetool::s1::CONST_SRC_CODE_MP_RIM_PARAMETER_DVAR2, zonetool::h1::CONST_SRC_CODE_MP_RIM_PARAMETER_DVAR2},
			{zonetool::s1::CONST_SRC_CODE_HEAD_ROT_MAT_R0, zonetool::h1::CONST_SRC_CODE_HEAD_ROT_MAT_R0},
			{zonetool::s1::CONST_SRC_CODE_HEAD_ROT_MAT_R1, zonetool::h1::CONST_SRC_CODE_HEAD_ROT_MAT_R1},
			{zonetool::s1::CONST_SRC_CODE_HEAD_ROT_MAT_R2, zonetool::h1::CONST_SRC_CODE_HEAD_ROT_MAT_R2},
			{zonetool::s1::CONST_SRC_CODE_GUN_SIGHT_COLOR, zonetool::h1::CONST_SRC_CODE_GUN_SIGHT_COLOR},
			{zonetool::s1::CONST_SRC_CODE_GUN_RETICLE_COLOR, zonetool::h1::CONST_SRC_CODE_GUN_RETICLE_COLOR},
			{zonetool::s1::CONST_SRC_CODE_BASE_LIGHTING_COORDS, zonetool::h1::CONST_SRC_CODE_BASE_LIGHTING_COORDS},
			{zonetool::s1::CONST_SRC_CODE_LIGHT_PROBE_AMBIENT, zonetool::h1::CONST_SRC_CODE_LIGHT_PROBE_AMBIENT},
			{zonetool::s1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE},
			{zonetool::s1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_TEXTURE_ATLAS_SETTINGS, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_TEXTURE_ATLAS_SETTINGS},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_VEL_WORLD, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_VEL_WORLD},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0, zonetool::h1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0},
			{zonetool::s1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1, zonetool::h1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1},
			{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_FADESHARPNESS, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_FADESHARPNESS},
			{zonetool::s1::CONST_SRC_CODE_BLUR_SCENE_PIXEL_POS_TO_UV, zonetool::h1::CONST_SRC_CODE_BLUR_SCENE_PIXEL_POS_TO_UV},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_0, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_0},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_1, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_1},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_2, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_2},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_3, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_3},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_4, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_4},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_5, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_5},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_6, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_6},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_7, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_7},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_8, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_8},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_9, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_9},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_10, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_10},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_11, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_11},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_12, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_12},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_13, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_13},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_14, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_14},
			{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_15, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_15},
			{zonetool::s1::CONST_SRC_CODE_DEPTH_FROM_CLIP, zonetool::h1::CONST_SRC_CODE_DEPTH_FROM_CLIP},
			{zonetool::s1::CONST_SRC_CODE_CODE_MESH_ARG_0, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_0},
			{zonetool::s1::CONST_SRC_CODE_CODE_MESH_ARG_1, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_1},
			{zonetool::s1::CONST_SRC_CODE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_VIEW_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_VIEW_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_PROJECTION_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX3},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX3},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3},
			{zonetool::s1::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_INVERSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_INVERSE_WORLD_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_TRANSPOSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_TRANSPOSE_WORLD_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_INVERSE_TRANSPOSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_INVERSE_TRANSPOSE_WORLD_MATRIX},
			{zonetool::s1::CONST_SRC_CODE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0},
			{zonetool::s1::CONST_SRC_CODE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1},
			{zonetool::s1::CONST_SRC_CODE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2},
			{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2},
		};

		const std::unordered_map <std::uint16_t, std::uint16_t> texture_src_code_map =
		{
			{zonetool::s1::TEXTURE_SRC_CODE_BLACK, zonetool::h1::TEXTURE_SRC_CODE_BLACK},
			{zonetool::s1::TEXTURE_SRC_CODE_WHITE, zonetool::h1::TEXTURE_SRC_CODE_WHITE},
			{zonetool::s1::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP, zonetool::h1::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP},
			{zonetool::s1::TEXTURE_SRC_CODE_MODEL_LIGHTING, zonetool::h1::TEXTURE_SRC_CODE_MODEL_LIGHTING},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY, zonetool::h1::TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY, zonetool::h1::TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY},
			{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SUN, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SUN},
			{zonetool::s1::TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN, zonetool::h1::TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN},
			{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT},
			{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1},
			{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2},
			{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3},
			{zonetool::s1::TEXTURE_SRC_CODE_FEEDBACK, zonetool::h1::TEXTURE_SRC_CODE_FEEDBACK},
			{zonetool::s1::TEXTURE_SRC_CODE_RESOLVED_POST_SUN, zonetool::h1::TEXTURE_SRC_CODE_RESOLVED_POST_SUN},
			{zonetool::s1::TEXTURE_SRC_CODE_SSR_BUFFER, zonetool::h1::TEXTURE_SRC_CODE_SSR_BUFFER},
			{zonetool::s1::TEXTURE_SRC_CODE_HQ_SSR_DEPTH, zonetool::h1::TEXTURE_SRC_CODE_HQ_SSR_DEPTH},
			{zonetool::s1::TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER, zonetool::h1::TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER},
			{zonetool::s1::TEXTURE_SRC_CODE_RESOLVED_SCENE, zonetool::h1::TEXTURE_SRC_CODE_RESOLVED_SCENE},
			{zonetool::s1::TEXTURE_SRC_CODE_POST_EFFECT_0, zonetool::h1::TEXTURE_SRC_CODE_POST_EFFECT_0},
			{zonetool::s1::TEXTURE_SRC_CODE_POST_EFFECT_1, zonetool::h1::TEXTURE_SRC_CODE_POST_EFFECT_1},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION1, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION1},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS1, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS1},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION2, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION2},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS2, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS2},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION3, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION3},
			{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS3, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS3},
			{zonetool::s1::TEXTURE_SRC_CODE_OUTDOOR, zonetool::h1::TEXTURE_SRC_CODE_OUTDOOR},
			{zonetool::s1::TEXTURE_SRC_CODE_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_FLOATZ},
			{zonetool::s1::TEXTURE_SRC_CODE_PROCESSED_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_PROCESSED_FLOATZ},
			{zonetool::s1::TEXTURE_SRC_CODE_RAW_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_RAW_FLOATZ},
			{zonetool::s1::TEXTURE_SRC_CODE_FLOATZ_STENCIL, zonetool::h1::TEXTURE_SRC_CODE_FLOATZ_STENCIL},
			{zonetool::s1::TEXTURE_SRC_CODE_RAW_STENCIL, zonetool::h1::TEXTURE_SRC_CODE_RAW_STENCIL},
			{zonetool::s1::TEXTURE_SRC_CODE_HALF_PARTICLES, zonetool::h1::TEXTURE_SRC_CODE_HALF_PARTICLES},
			{zonetool::s1::TEXTURE_SRC_CODE_HALF_PARTICLES_Z, zonetool::h1::TEXTURE_SRC_CODE_HALF_PARTICLES_Z},
			{zonetool::s1::TEXTURE_SRC_CODE_CASE_TEXTURE, zonetool::h1::TEXTURE_SRC_CODE_CASE_TEXTURE},
			{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_Y, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_Y},
			{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_CR, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CR},
			{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_CB, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CB},
			{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_A, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_A},
			//{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_CBCR, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CBCR},
			{zonetool::s1::TEXTURE_SRC_CODE_REFLECTION_PROBE, zonetool::h1::TEXTURE_SRC_CODE_REFLECTION_PROBE},
			{zonetool::s1::TEXTURE_SRC_CODE_DALTONIZE_CLUT, zonetool::h1::TEXTURE_SRC_CODE_DALTONIZE_CLUT},
			{zonetool::s1::TEXTURE_SRC_CODE_CLUT, zonetool::h1::TEXTURE_SRC_CODE_CLUT},
			{zonetool::s1::TEXTURE_SRC_CODE_LINEAR_CLAMP, zonetool::h1::TEXTURE_SRC_CODE_LINEAR_CLAMP},
			{zonetool::s1::TEXTURE_SRC_CODE_POINT_CLAMP, zonetool::h1::TEXTURE_SRC_CODE_POINT_CLAMP},
			{zonetool::s1::TEXTURE_SRC_CODE_BLUR_SCENE_SAVE, zonetool::h1::TEXTURE_SRC_CODE_BLUR_SCENE_SAVE},
			{zonetool::s1::TEXTURE_SRC_CODE_RANDOM_ROTATIONS, zonetool::h1::TEXTURE_SRC_CODE_RANDOM_ROTATIONS},
			{zonetool::s1::TEXTURE_SRC_CODE_SSAO, zonetool::h1::TEXTURE_SRC_CODE_SSAO},
			{zonetool::s1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_0, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_0},
			{zonetool::s1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_1, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_1},
			{zonetool::s1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_2, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_2},
			{zonetool::s1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_3, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_3},
			{zonetool::s1::TEXTURE_SRC_CODE_MSAA_SCENE, zonetool::h1::TEXTURE_SRC_CODE_MSAA_SCENE},
			{zonetool::s1::TEXTURE_SRC_CODE_DEPTH_BLUR_0, zonetool::h1::TEXTURE_SRC_CODE_DEPTH_BLUR_0},
			{zonetool::s1::TEXTURE_SRC_CODE_DEPTH_BLUR_1, zonetool::h1::TEXTURE_SRC_CODE_DEPTH_BLUR_1},
			{zonetool::s1::TEXTURES_SRC_CODE_DEFAULT_DEPTH, zonetool::h1::TEXTURES_SRC_CODE_DEFAULT_DEPTH},
			{zonetool::s1::TEXTURE_SRC_CODE_SCENE_VELOCITY, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY},
			{zonetool::s1::TEXTURE_SRC_CODE_SCENE_VELOCITY_TILE0, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY_TILE0},
			{zonetool::s1::TEXTURE_SRC_CODE_SCENE_VELOCITY_TILE1, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY_TILE1},
			{zonetool::s1::TEXTURE_SRC_CODE_DOF_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_DOF_FLOATZ},
			{zonetool::s1::TEXTURE_SRC_CODE_DOF_HALF_PREPASS, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_PREPASS},
			{zonetool::s1::TEXTURE_SRC_CODE_DOF_HALF_COLOR, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_COLOR},
			{zonetool::s1::TEXTURE_SRC_CODE_DOF_HALF_ALPHA, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_ALPHA},
			{zonetool::s1::TEXTURE_SRC_CODE_DOF_TILE0, zonetool::h1::TEXTURE_SRC_CODE_DOF_TILE0},
			{zonetool::s1::TEXTURE_SRC_CODE_DOF_TILE1, zonetool::h1::TEXTURE_SRC_CODE_DOF_TILE1},
			{zonetool::s1::TEXTURE_SRC_CODE_SCENE_SPECULAR, zonetool::h1::TEXTURE_SRC_CODE_SCENE_SPECULAR},
			{zonetool::s1::TEXTURE_SRC_CODE_SCENE_ALBEDO, zonetool::h1::TEXTURE_SRC_CODE_SCENE_ALBEDO},
			{zonetool::s1::TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA, zonetool::h1::TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA},
			{zonetool::s1::TEXTURE_SRC_CODE_UI3D, zonetool::h1::TEXTURE_SRC_CODE_UI3D},
			{zonetool::s1::TEXTURE_SRC_CODE_STREAMING_LOADING, zonetool::h1::TEXTURE_SRC_CODE_STREAMING_LOADING},
			{zonetool::s1::TEXTURE_SRC_CODE_MDAO_HDIR, zonetool::h1::TEXTURE_SRC_CODE_MDAO_HDIR},
			{zonetool::s1::TEXTURE_SRC_CODE_MDAO_DEPTH, zonetool::h1::TEXTURE_SRC_CODE_MDAO_DEPTH},
			{zonetool::s1::TEXTURE_SRC_CODE_MDAO_OCCLUSION_VOLUME, zonetool::h1::TEXTURE_SRC_CODE_MDAO_OCCLUSION_VOLUME},
			{zonetool::s1::TEXTURE_SRC_CODE_MDAO_LOOKUP, zonetool::h1::TEXTURE_SRC_CODE_MDAO_LOOKUP},
			{zonetool::s1::TEXTURE_SRC_CODE_TEXTURE_0, zonetool::h1::TEXTURE_SRC_CODE_TEXTURE_0},
			{zonetool::s1::TEXTURE_SRC_CODE_COMPOSITE_RESULT, zonetool::h1::TEXTURE_SRC_CODE_COMPOSITE_RESULT},
			//{zonetool::s1::TEXTURE_SRC_CODE_FONT_CACHE, zonetool::h1::TEXTURE_SRC_CODE_FONT_CACHE},
		};

		const std::unordered_map <std::uint8_t, std::uint8_t> worldvertexformat_map =
		{
			{0, 0},
			{1, 1},
			{2, 4},
			{3, 7},
			{4, 11},
			{5, 15},
			{6, 19},
			{7, 24},
			{8, 29},
			{9, 34},
			{10, 39},
			{11, 45},
			{12, 51},
			{13, 57},
			{14, 63},
		};

		std::unordered_map<std::string, zonetool::h1::MaterialTechniqueSet*> converted_techset_assets;
		zonetool::h1::MaterialTechniqueSet* convert_techset(MaterialTechniqueSet* asset, ZoneMemory* mem)
		{
			if (converted_techset_assets.contains(asset->name + "_s1"s))
			{
				return converted_techset_assets[asset->name];
			}

			auto* new_asset = mem->Alloc<zonetool::h1::MaterialTechniqueSet>();

			new_asset->name = mem->StrDup(asset->name + "_s1"s);
			new_asset->flags = asset->flags; // convert?

			if (worldvertexformat_map.contains(asset->worldVertFormat))
			{
				new_asset->worldVertFormat = worldvertexformat_map.at(asset->worldVertFormat);
			}
			else
			{
				ZONETOOL_ERROR("Unable to map worldVertFormat %d for technique '%s'!\n", asset->worldVertFormat, asset->name);
				new_asset->worldVertFormat = 0;
			}

			new_asset->preDisplacementOnlyCount = asset->preDisplacementOnlyCount;

			std::unordered_map<std::uintptr_t, zonetool::h1::MaterialTechnique*> converted_asset_techniques;
			for (std::int32_t i = 0; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
			{
				if (asset->techniques[i])
				{
					if (converted_asset_techniques.contains(reinterpret_cast<std::uintptr_t>(asset->techniques[i])))
					{
						new_asset->techniques[i] = converted_asset_techniques[reinterpret_cast<std::uintptr_t>(asset->techniques[i])];
					}
					else
					{
						const auto size = sizeof(MaterialTechniqueHeader) + sizeof(MaterialPass) * asset->techniques[i]->hdr.passCount;
						new_asset->techniques[i] = mem->ManualAlloc<zonetool::h1::MaterialTechnique>(size); // no need to change index, it's the same

						auto* technique = asset->techniques[i];
						auto* new_technique = new_asset->techniques[i];
						
						std::memcpy(new_technique, technique, size); // same struct

						new_technique->hdr.name = mem->StrDup(technique->hdr.name + "_s1"s);

						for (unsigned short pass_index = 0; pass_index < technique->hdr.passCount; pass_index++)
						{
							auto* pass = &technique->passArray[pass_index];
							auto* new_pass = &new_technique->passArray[pass_index];

							if (pass->vertexShader)
							{
								new_pass->vertexShader = convert_vertexshader(pass->vertexShader, mem);
							}
							if (pass->vertexDecl)
							{
								new_pass->vertexDecl = convert_vertexdecl(pass->vertexDecl, mem);
							}
							if (pass->hullShader)
							{
								new_pass->hullShader = convert_hullshader(pass->hullShader, mem);
							}
							if (pass->domainShader)
							{
								new_pass->domainShader = convert_domainshader(pass->domainShader, mem);
							}
							if (pass->pixelShader)
							{
								new_pass->pixelShader = convert_pixelshader(pass->pixelShader, mem);
							}

							new_pass->stableArgSize += 16;

							if (pass->args)
							{
								const auto arg_count = pass->perObjArgCount + pass->perPrimArgCount + pass->stableArgCount;
								new_pass->args = mem->Alloc<zonetool::h1::MaterialShaderArgument>(arg_count);
								std::memcpy(new_pass->args, pass->args, sizeof(MaterialShaderArgument) * arg_count); // same struct
								for (auto arg_index = 0; arg_index < arg_count; arg_index++)
								{
									auto* arg = &pass->args[arg_index];
									auto* new_arg = &new_pass->args[arg_index];

									if (arg->type == MTL_ARG_CODE_CONST)
									{
										if (const_src_code_map.contains(arg->u.codeConst.index))
										{
											auto new_index = const_src_code_map.at(arg->u.codeConst.index);
											new_arg->u.codeConst.index = new_index;

											assert(new_arg->u.codeConst.index < zonetool::h1::CONST_SRC_TOTAL_COUNT);
										}
										else
										{
											ZONETOOL_ERROR("Unable to map code constant %d for technique '%s'!\n", arg->u.codeConst.index, asset->name);
											new_arg->u.codeConst.index = zonetool::h1::CONST_SRC_NONE;
										}
									}
									else if (arg->type == MTL_ARG_CODE_TEXTURE)
									{
										if (texture_src_code_map.contains(arg->u.codeConst.index))
										{
											auto new_index = texture_src_code_map.at(arg->u.codeConst.index);
											new_arg->u.codeConst.index = new_index;

											assert(new_arg->u.codeConst.index < zonetool::h1::TEXTURE_SRC_CODE_COUNT);
										}
										else
										{
											ZONETOOL_ERROR("Unable to map code sampler %d for technique '%s'!\n", arg->u.codeSampler, asset->name);
											new_arg->u.codeConst.index = zonetool::h1::TEXTURE_SRC_CODE_BLACK;
										}
									}
								}
							}
						}
						converted_asset_techniques[reinterpret_cast<std::uintptr_t>(asset->techniques[i])] = new_technique;
					}
				}
			}

			converted_techset_assets[asset->name] = new_asset;
			return new_asset;
		}

		zonetool::h1::ComputeShader* convert_computeshader(ComputeShader* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::ComputeShader>();

			std::memcpy(new_asset, asset, sizeof(ComputeShader));
			new_asset->name = mem->StrDup(asset->name + "_s1"s);

			return new_asset;
		}

		zonetool::h1::MaterialDomainShader* convert_domainshader(MaterialDomainShader* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::MaterialDomainShader>();

			std::memcpy(new_asset, asset, sizeof(MaterialDomainShader));
			new_asset->name = mem->StrDup(asset->name + "_s1"s);

			return new_asset;
		}

		zonetool::h1::MaterialHullShader* convert_hullshader(MaterialHullShader* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::MaterialHullShader>();

			std::memcpy(new_asset, asset, sizeof(MaterialHullShader));
			new_asset->name = mem->StrDup(asset->name + "_s1"s);

			return new_asset;
		}

		zonetool::h1::MaterialPixelShader* convert_pixelshader(MaterialPixelShader* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::MaterialPixelShader>();

			std::memcpy(new_asset, asset, sizeof(MaterialPixelShader));
			new_asset->name = mem->StrDup(asset->name + "_s1"s);

			return new_asset;
		}

		zonetool::h1::MaterialVertexDeclaration* convert_vertexdecl(MaterialVertexDeclaration* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::MaterialVertexDeclaration>();

			new_asset->streamCount = asset->streamCount;
			new_asset->hasOptionalSource = asset->hasOptionalSource;

			for (auto i = 0; i < 32; i++)
			{
				// maybe need to edit these?
				new_asset->routing.data[i].source = asset->routing.data[i].source;
				new_asset->routing.data[i].dest = asset->routing.data[i].dest;
				new_asset->routing.data[i].mask = asset->routing.data[i].mask;

				if (asset->routing.data[i].source > 7)
				{
					new_asset->routing.data[i].source += 2;
				}
			}

			new_asset->name = mem->StrDup(asset->name + "_s1"s);

			return new_asset;
		}

		zonetool::h1::MaterialVertexShader* convert_vertexshader(MaterialVertexShader* asset, ZoneMemory* mem)
		{
			auto* new_asset = mem->Alloc<zonetool::h1::MaterialVertexShader>();

			std::memcpy(new_asset, asset, sizeof(MaterialVertexShader));
			new_asset->name = mem->StrDup(asset->name + "_s1"s);

			return new_asset;
		}

		#undef COPY_VALUE
		#undef COPY_VALUE_CAST
		#undef COPY_ARR
		#undef REINTERPRET_CAST_SAFE
		#undef REINTERPRET_CAST_SAFE_ARR
	}
}