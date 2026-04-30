#include <std_include.hpp>
#include "gfxworld.hpp"

namespace zonetool::s1
{
	namespace
	{
		template <typename T>
		T* get_asset(XAssetType type, const char* name, zone_base* zone)
		{
			T* asset = nullptr;
			auto* asset_interface = zone->find_asset(type, name);
			if (!asset_interface)
			{
				asset = reinterpret_cast<T*>(db_find_x_asset_header(type, name, 1).data);
			}
			else
			{
				asset = reinterpret_cast<T*>(asset_interface->pointer());
			}
			return asset;
		}

		MaterialTechnique* Material_GetTechnique(const Material* material, int techType, zone_base* zone)
		{
			return get_asset<MaterialTechniqueSet>(ASSET_TYPE_TECHNIQUE_SET, material->techniqueSet->name, zone)->techniques[techType];
		}

		bool Material_IsEmissive(Material* material, zone_base* zone)
		{
			return material->techniqueSet &&
				Material_GetTechnique(material, TECHNIQUE_EMISSIVE, zone);
		}

		bool Material_IsShadowCaster(Material* material, zone_base* zone)
		{
			return material->techniqueSet &&
				Material_GetTechnique(material, TECHNIQUE_BUILD_SHADOWMAP_DEPTH, zone) &&
				material->info.sortKey == 38;
		}

		bool Material_IsTransparent(Material* material, zone_base* zone)
		{
			return material->techniqueSet &&
				Material_GetTechnique(material, TECHNIQUE_LIT, zone) &&
				material->info.sortKey >= 18 && material->info.sortKey <= 33;
		}

		bool Material_IsDecal(Material* material, zone_base* zone)
		{
			return material->techniqueSet &&
				Material_GetTechnique(material, TECHNIQUE_LIT, zone) &&
				material->info.sortKey >= 7 && material->info.sortKey <= 17;
		}

		bool Material_IsOpaque(Material* material, zone_base* zone)
		{
			return material->techniqueSet &&
				(Material_GetTechnique(material, TECHNIQUE_LIT, zone) ||
					Material_GetTechnique(material, TECHNIQUE_BUILD_SHADOWMAP_DEPTH, zone)) &&
				material->info.sortKey < 7;
		}

		void sort_dpvs_surfaces(GfxWorld* asset, zone_base* zone)
		{
			try
			{
				auto mem = utils::memory::get_allocator();
				const unsigned int surfaceCount = asset->models->surfaceCount;

				// Categorize surfaces into different types
				std::vector<unsigned int> opaque, decal, trans, shadow_caster, emissive;

				for (unsigned int surf_idx = 0; surf_idx < surfaceCount; ++surf_idx)
				{
					auto& surf = asset->dpvs.surfaces[surf_idx];
					auto* surf_material = get_asset<Material>(ASSET_TYPE_MATERIAL, surf.material->name, zone);

					if (Material_IsOpaque(surf_material, zone))
						opaque.push_back(surf_idx);
					else if (Material_IsDecal(surf_material, zone))
						decal.push_back(surf_idx);
					else if (Material_IsTransparent(surf_material, zone))
						trans.push_back(surf_idx);
					else if (Material_IsShadowCaster(surf_material, zone))
						shadow_caster.push_back(surf_idx);
					else if (Material_IsEmissive(surf_material, zone))
						emissive.push_back(surf_idx);
					else
						opaque.push_back(surf_idx); // this is most likely "missing" material
				}

				// Allocate arrays for sorted surfaces and their bounds
				auto surfaces_sorted = mem->allocate_array<GfxSurface>(surfaceCount);
				auto surface_bounds_sorted = mem->allocate_array<GfxSurfaceBounds>(surfaceCount);

				std::unordered_map<unsigned int, unsigned int> old_to_new_surface_index;
				unsigned int index = 0;

				// Lambda to append surfaces and update the index mapping
				const auto append_surfaces = [&](const std::vector<unsigned int>& surfaces)
				{
					for (const auto surf_idx : surfaces)
					{
						old_to_new_surface_index[surf_idx] = index;
						surfaces_sorted[index] = asset->dpvs.surfaces[surf_idx];
						surface_bounds_sorted[index] = asset->dpvs.surfacesBounds[surf_idx];
						++index;
					}
				};

				append_surfaces(opaque);
				append_surfaces(decal);
				append_surfaces(trans);
				append_surfaces(shadow_caster);
				append_surfaces(emissive);

				// Exception check if index doesn't match the surface count
				if (index != surfaceCount)
				{
					throw std::runtime_error("Index count mismatch with surface count.");
				}

				// Replace original surface and bounds arrays with sorted ones
				std::memcpy(asset->dpvs.surfaces, surfaces_sorted, sizeof(GfxSurface) * surfaceCount);
				std::memcpy(asset->dpvs.surfacesBounds, surface_bounds_sorted, sizeof(GfxSurfaceBounds) * surfaceCount);

				// Remap sorted surface indices based on new order
				std::unordered_map<unsigned int*, bool> replaced_addresses;
				const auto replace_index_at_address = [&](unsigned int* address, unsigned int val)
				{
					if (replaced_addresses.find(address) != replaced_addresses.end())
						return;

					*address = val;
					replaced_addresses[address] = true;
				};

				// Now remap the sortedSurfIndex based on the new surface positions
				for (unsigned int i = 0; i < asset->dpvs.staticSurfaceCount; ++i)
				{
					const auto old_index = asset->dpvs.sortedSurfIndex[i];
					const auto new_index = old_to_new_surface_index.at(old_index);
					replace_index_at_address(&asset->dpvs.sortedSurfIndex[i], new_index);
				}

				// Remap shadow geometry if present
				if (asset->shadowGeom)
				{
					for (unsigned int s = 0; s < asset->primaryLightCount; ++s)
					{
						for (unsigned int i = 0; i < asset->shadowGeom[s].surfaceCount; ++i)
						{
							const auto old_index = asset->shadowGeom[s].sortedSurfIndex[i];
							const auto new_index = old_to_new_surface_index.at(old_index);
							replace_index_at_address(&asset->shadowGeom[s].sortedSurfIndex[i], new_index);
						}
					}
				}

				if (asset->shadowGeomOptimized)
				{
					for (unsigned int s = 0; s < asset->primaryLightCount; ++s)
					{
						for (unsigned int i = 0; i < asset->shadowGeomOptimized[s].surfaceCount; ++i)
						{
							const auto old_index = asset->shadowGeomOptimized[s].sortedSurfIndex[i];
							const auto new_index = old_to_new_surface_index.at(old_index);
							replace_index_at_address(&asset->shadowGeomOptimized[s].sortedSurfIndex[i], new_index);
						}
					}
				}

				// Update surface range information
				asset->dpvs.litOpaqueSurfsBegin = 0;
				asset->dpvs.litOpaqueSurfsEnd = static_cast<unsigned int>(opaque.size());
				asset->dpvs.litDecalSurfsBegin = asset->dpvs.litOpaqueSurfsEnd;
				asset->dpvs.litDecalSurfsEnd = asset->dpvs.litDecalSurfsBegin + static_cast<unsigned int>(decal.size());
				asset->dpvs.litTransSurfsBegin = asset->dpvs.litDecalSurfsEnd;
				asset->dpvs.litTransSurfsEnd = asset->dpvs.litTransSurfsBegin + static_cast<unsigned int>(trans.size());
				asset->dpvs.shadowCasterSurfsBegin = asset->dpvs.litTransSurfsEnd;
				asset->dpvs.shadowCasterSurfsEnd = asset->dpvs.shadowCasterSurfsBegin + static_cast<unsigned int>(shadow_caster.size());
				asset->dpvs.emissiveSurfsBegin = asset->dpvs.shadowCasterSurfsEnd;
				asset->dpvs.emissiveSurfsEnd = asset->dpvs.emissiveSurfsBegin + static_cast<unsigned int>(emissive.size());

				// Free allocated memory
				mem->free(surfaces_sorted);
				mem->free(surface_bounds_sorted);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Could not sort dpvs surfaces!\n%s", e.what());
			}
		}

		void re_calc_lightmap_params(GfxLightmapArray* array, GfxLightmapParameters* params, zone_base* zone)
		{
			array->primary = get_asset<GfxImage>(ASSET_TYPE_IMAGE, array->primary->name, zone);
			array->secondary = get_asset<GfxImage>(ASSET_TYPE_IMAGE, array->secondary->name, zone);

			params->lightmapWidthPrimary = array->primary->width;
			params->lightmapHeightPrimary = array->primary->height;

			params->lightmapWidthSecondary = array->secondary->width;
			params->lightmapHeightSecondary = array->secondary->height;

			params->lightmapModelUnitsPerTexel = 8;
		}

		void fixup_static_models(GfxWorld* world, zone_base* zone)
		{
			for (auto i = 0u; i < world->dpvs.smodelCount; i++)
			{
				auto* smodel_draw_inst = &world->dpvs.smodelDrawInsts[i];
				auto* smodel_inst = &world->dpvs.smodelInsts[i];
				if (smodel_draw_inst->model)
				{
					constexpr unsigned char SURF_PER_LOD_HARD_LIMIT = 16;
					bool model_has_more_surfs_than_allowed = false;
					auto* model = get_asset<XModel>(ASSET_TYPE_XMODEL, smodel_draw_inst->model->name, zone);
					for (auto lod_index = 0; lod_index < model->numLods; lod_index++)
					{
						auto* lod = &model->lodInfo[lod_index];
						if (lod->numsurfs >= SURF_PER_LOD_HARD_LIMIT)
						{
							model_has_more_surfs_than_allowed = true;
							break;
						}
					}

					if (model_has_more_surfs_than_allowed)
					{
						ZONETOOL_INFO("Converting static model %s to script model since it uses more than %d surfs...", model->name, SURF_PER_LOD_HARD_LIMIT);

						float angles[3]{};
						AxisToAngles(smodel_draw_inst->placement.axis, angles);

						map_ents::add_entity_string("{\n");
						map_ents::add_entity_string("\"classname\" \"script_model\"\n");
						map_ents::add_entity_string(utils::string::va("\"model\" \"%s\"\n", model->name));
						map_ents::add_entity_string(utils::string::va("\"origin\" \"%f %f %f\"\n",
							smodel_draw_inst->placement.origin[0],
							smodel_draw_inst->placement.origin[1],
							smodel_draw_inst->placement.origin[2]
						));
						map_ents::add_entity_string(utils::string::va("\"angles\" \"%f %f %f\"\n",
							angles[0],
							angles[1],
							angles[2]
						));
						if (smodel_draw_inst->placement.scale != 1.0f)
						{
							ZONETOOL_WARNING("Lost model scale(%f) for model %s while converting from static model to script model...", smodel_draw_inst->placement.scale, model->name);
							//map_ents::add_entity_string(utils::string::va("\"modelscale\" \"%f\"\n", smodel_draw_inst->placement.scale));
						}
						map_ents::add_entity_string("}\n");

						static XModel empty_model{};
						empty_model.name = "tag_origin";

						memset(smodel_inst->mins, 0, sizeof(smodel_inst->mins));
						memset(smodel_inst->maxs, 0, sizeof(smodel_inst->maxs));

						smodel_draw_inst->model = &empty_model;
						zone->add_asset_of_type(ASSET_TYPE_XMODEL, ","s + empty_model.name);
					}
				}
			}
		}
	}

	GfxWorld* gfx_world::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".gfxmap";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gfxmap \"%s\"...", name.data());

		GfxWorld* asset = read.read_single<GfxWorld>();
		asset->name = read.read_string();
		asset->baseName = read.read_string();

		asset->skies = read.read_array<GfxSky>();
		for (int i = 0; i < asset->skyCount; i++)
		{
			asset->skies[i].skyStartSurfs = read.read_array<int>();
			asset->skies[i].skyImage = read.read_asset<GfxImage>();
		}

		asset->dpvsPlanes.planes = read.read_array<cplane_s>();
		asset->dpvsPlanes.nodes = read.read_array<unsigned short>();
		asset->dpvsPlanes.sceneEntCellBits = read.read_array<unsigned int>();

		asset->aabbTreeCounts = read.read_array<GfxCellTreeCount>();
		asset->aabbTrees = read.read_array<GfxCellTree>();
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			asset->aabbTrees[i].aabbTree = read.read_array<GfxAabbTree>();
			for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
			{
				asset->aabbTrees[i].aabbTree[j].smodelIndexes = read.read_array<unsigned short>();
			}
		}

		asset->cells = read.read_array<GfxCell>();
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			asset->cells[i].portals = read.read_array<GfxPortal>();
			for (int j = 0; j < asset->cells[i].portalCount; j++)
			{
				asset->cells[i].portals[j].vertices = read.read_array<float[3]>();
			}
			asset->cells[i].reflectionProbes = read.read_array<unsigned char>();
			asset->cells[i].reflectionProbeReferences = read.read_array<unsigned char>();
		}

		asset->portalGroup = read.read_array<GfxPortalGroup>();
		for (unsigned int i = 0; i < asset->portalGroupCount; i++)
		{
			if (asset->portalGroup[i].targetName)
			{
				asset->portalGroup[i].targetName = read.read_string();
			}
			asset->portalGroup[i].gfxPortalArray = read.read_array<GfxPortalGroupInfo>();
		}

		asset->portalDistanceAnchorsAndCloseDistSquared = read.read_array<vec4_t>();

		asset->draw.reflectionProbes = read.read_array<GfxImage*>();
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			asset->draw.reflectionProbes[i] = read.read_asset<GfxImage>();
		}

		asset->draw.reflectionProbeOrigins = read.read_array<GfxReflectionProbe>();
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			asset->draw.reflectionProbeOrigins[i].probeVolumes = read.read_array<GfxReflectionProbeVolume>();
			for (unsigned int j = 0; j < asset->draw.reflectionProbeOrigins[i].probeVolumeCount; j++)
			{
				asset->draw.reflectionProbeOrigins[i].probeVolumes[j].probeVolumePlanes = read.read_array<unsigned short>();
			}
		}

		asset->draw.reflectionProbeTextures = read.read_array<GfxRawTexture>();

		asset->draw.reflectionProbeReferenceOrigins = read.read_array<GfxReflectionProbeReferenceOrigin>();
		asset->draw.reflectionProbeReferences = read.read_array<GfxReflectionProbeReference>();

		asset->draw.lightmaps = read.read_array<GfxLightmapArray>();
		for (int i = 0; i < asset->draw.lightmapCount; i++)
		{
			asset->draw.lightmaps[i].primary = read.read_asset<GfxImage>();
			asset->draw.lightmaps[i].secondary = read.read_asset<GfxImage>();
		}

		asset->draw.lightmapPrimaryTextures = read.read_array<GfxRawTexture>();
		asset->draw.lightmapSecondaryTextures = read.read_array<GfxRawTexture>();
		asset->draw.lightmapOverridePrimary = read.read_asset<GfxImage>();
		asset->draw.lightmapOverrideSecondary = read.read_asset<GfxImage>();

		asset->draw.vd.vertices = read.read_array<GfxWorldVertex>();
		asset->draw.vld.data = read.read_array<unsigned char>();
		asset->draw.indices = read.read_array<unsigned short>();

		asset->lightGrid.rowDataStart = read.read_array<unsigned short>();
		asset->lightGrid.rawRowData = read.read_array<unsigned char>();
		asset->lightGrid.entries = read.read_array<GfxLightGridEntry>();
		asset->lightGrid.colors = read.read_array<GfxLightGridColors>();

		asset->lightGrid.stageLightingContrastGain = read.read_array<float>();
		asset->lightGrid.paletteEntryAddress = read.read_array<int>();
		asset->lightGrid.paletteBitstream = read.read_array<unsigned char>();

		for (auto i = 0; i < 3; i++)
		{
			asset->lightGrid.tree[i].p_nodeTable = read.read_array<unsigned int>();
			asset->lightGrid.tree[i].p_leafTable = read.read_array<unsigned char>();
		}

		asset->models = read.read_array<GfxBrushModel>();

		asset->materialMemory = read.read_array<MaterialMemory>();
		for (int i = 0; i < asset->materialMemoryCount; i++)
		{
			asset->materialMemory[i].material = read.read_asset<Material>();
		}

		asset->sun.spriteMaterial = read.read_asset<Material>();
		asset->sun.flareMaterial = read.read_asset<Material>();

		asset->outdoorImage = read.read_asset<GfxImage>();

		asset->cellCasterBits = read.read_array<unsigned int>();
		asset->cellHasSunLitSurfsBits = read.read_array<unsigned int>();

		asset->sceneDynModel = read.read_array<GfxSceneDynModel>();
		asset->sceneDynBrush = read.read_array<GfxSceneDynBrush>();

		asset->primaryLightEntityShadowVis = read.read_array<unsigned int>();

		asset->primaryLightDynEntShadowVis[0] = read.read_array<unsigned int>();
		asset->primaryLightDynEntShadowVis[1] = read.read_array<unsigned int>();

		asset->nonSunPrimaryLightForModelDynEnt = read.read_array<unsigned short>();

		asset->shadowGeom = read.read_array<GfxShadowGeometry>();
		if (asset->shadowGeom)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->shadowGeom[i].sortedSurfIndex = read.read_array<unsigned int>();
				asset->shadowGeom[i].smodelIndex = read.read_array<unsigned short>();
			}
		}
		asset->shadowGeomOptimized = read.read_array<GfxShadowGeometry>();
		if (asset->shadowGeomOptimized)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				asset->shadowGeomOptimized[i].sortedSurfIndex = read.read_array<unsigned int>();
				asset->shadowGeomOptimized[i].smodelIndex = read.read_array<unsigned short>();
			}
		}

		asset->lightRegion = read.read_array<GfxLightRegion>();
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			asset->lightRegion[i].hulls = read.read_array<GfxLightRegionHull>();
			for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
			{
				asset->lightRegion[i].hulls[j].axis = read.read_array<GfxLightRegionAxis>();
			}
		}

		for (auto i = 0; i < 4; i++)
		{
			asset->dpvs.smodelVisData[i] = read.read_array<unsigned int>();
		}

		for (auto i = 0; i < 27; i++)
		{
			asset->dpvs.smodelUnknownVisData[i] = read.read_array<unsigned int>();
		}

		for (auto i = 0; i < 4; i++)
		{
			asset->dpvs.surfaceVisData[i] = read.read_array<unsigned int>();
		}

		for (auto i = 0; i < 27; i++)
		{
			asset->dpvs.surfaceUnknownVisData[i] = read.read_array<unsigned int>();
		}

		for (auto i = 0; i < 4; i++)
		{
			asset->dpvs.smodelUmbraVisData[i] = read.read_array<unsigned int>();
		}

		for (auto i = 0; i < 4; i++)
		{
			asset->dpvs.surfaceUmbraVisData[i] = read.read_array<unsigned int>();
		}

		asset->dpvs.unknownSModelVisData1 = read.read_array<unsigned int>();
		asset->dpvs.unknownSModelVisData2 = read.read_array<unsigned int>();

		asset->dpvs.lodData = read.read_array<unsigned int>();
		asset->dpvs.sortedSurfIndex = read.read_array<unsigned int>();

		asset->dpvs.smodelInsts = read.read_array<GfxStaticModelInst>();
		asset->dpvs.surfaces = read.read_array<GfxSurface>();
		for (unsigned int i = 0; i < asset->surfaceCount; i++)
		{
			asset->dpvs.surfaces[i].material = read.read_asset<Material>();
		}
		asset->dpvs.surfacesBounds = read.read_array<GfxSurfaceBounds>();
		asset->dpvs.smodelDrawInsts = read.read_array<GfxStaticModelDrawInst>();
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			asset->dpvs.smodelDrawInsts[i].model = read.read_asset<XModel>();
		}
		asset->dpvs.smodelLightingInsts = read.read_array<GfxStaticModelLighting>();
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			auto flags = asset->dpvs.smodelDrawInsts[i].flags;
			if ((flags & 0x180) != 0 && (flags & 0x80) != 0 && asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.lightingValues)
			{
				asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.lightingValues = read.read_array<GfxStaticModelVertexLighting>();
			}
		}

		asset->dpvs.subdivVertexLighting = read.read_array<GfxSubdivVertexLightingInfo>();

		asset->dpvs.surfaceMaterials = read.read_array<GfxDrawSurf>();
		asset->dpvs.surfaceCastsSunShadow = read.read_array<unsigned int>();
		asset->dpvs.surfaceCastsSunShadowOpt = read.read_array<unsigned int>();

		asset->dpvs.surfaceDeptAndSurf = read.read_array<GfxDepthAndSurf>();

		asset->dpvs.constantBuffersLit = read.read_array<GfxWorldDpvsVoid>();
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			asset->dpvs.constantBuffersLit[i] = read.read_raw<char>();
		}
		asset->dpvs.constantBuffersAmbient = read.read_array<GfxWorldDpvsVoid>();
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			asset->dpvs.constantBuffersAmbient[i] = read.read_raw<char>();
		}

		asset->dpvsDyn.dynEntCellBits[0] = read.read_array<unsigned int>();
		asset->dpvsDyn.dynEntCellBits[1] = read.read_array<unsigned int>();

		asset->dpvsDyn.dynEntVisData[0][0] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[0][1] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[0][2] = read.read_array<unsigned char>();

		asset->dpvsDyn.dynEntVisData[1][0] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[1][1] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[1][2] = read.read_array<unsigned char>();

		asset->dpvsDyn.dynEntVisData[0][3] = read.read_array<unsigned char>();
		asset->dpvsDyn.dynEntVisData[1][3] = read.read_array<unsigned char>();

		asset->heroOnlyLights = read.read_array<GfxHeroOnlyLight>();

		asset->umbraTomeData = read.read_array<char>();

		asset->mdaoVolumes = read.read_array<MdaoVolume>();
		for (unsigned int i = 0; i < asset->mdaoVolumesCount; i++)
		{
			asset->mdaoVolumes[i].volumeData = read.read_asset<GfxImage>();
		}

		asset->buildInfo.bspCommandline = read.read_string();
		asset->buildInfo.lightCommandline = read.read_string();
		asset->buildInfo.bspTimestamp = read.read_string();
		asset->buildInfo.lightTimestamp = read.read_string();

		read.close();

		return asset;
	}

	void gfx_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
		this->asset_ = this->parse(name, mem);

		this->base_name_ = filesystem::get_fastfile();

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse gfxmap \"%s\"", name.data());
		}
	}

	void gfx_world::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_world::load_depending(zone_base* zone)
	{
		auto data = this->asset_;

		// Skies
		if (data->skyCount)
		{
			for (int i = 0; i < data->skyCount; i++)
			{
				if (data->skies[i].skyImage)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->skies[i].skyImage->name);
				}
			}
		}

		// ReflectionImages
		if (data->draw.reflectionProbes)
		{
			for (unsigned int i = 0; i < data->draw.reflectionProbeCount; i++)
			{
				if (data->draw.reflectionProbes[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.reflectionProbes[i]->name);
				}
			}
		}

		// Lightmaps
		if (data->draw.lightmaps)
		{
			for (int i = 0; i < data->draw.lightmapCount; i++)
			{
				if (data->draw.lightmaps[i].primary)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmaps[i].primary->name);
				}

				if (data->draw.lightmaps[i].secondary)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmaps[i].secondary->name);
				}
			}
		}

		// LightmapOverridePrimary
		if (data->draw.lightmapOverridePrimary)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmapOverridePrimary->name);
		}

		// LightmapOverrideSecondary
		if (data->draw.lightmapOverrideSecondary)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->draw.lightmapOverrideSecondary->name);
		}

		// MaterialMemory
		if (data->materialMemory)
		{
			for (int i = 0; i < data->materialMemoryCount; i++)
			{
				if (data->materialMemory[i].material)
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->materialMemory[i].material->name);
				}
			}
		}

		// Sunflare_t
		if (data->sun.spriteMaterial)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->sun.spriteMaterial->name);
		}
		if (data->sun.flareMaterial)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->sun.flareMaterial->name);
		}

		// OutdoorImage
		if (data->outdoorImage)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->outdoorImage->name);
		}

		// Dpvs
		if (data->dpvs.smodelDrawInsts)
		{
			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				if (data->dpvs.smodelDrawInsts[i].model)
				{
					zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->dpvs.smodelDrawInsts[i].model->name);
				}
			}
		}
		if (data->dpvs.surfaces)
		{
			for (unsigned int i = 0; i < data->surfaceCount; i++)
			{
				if (data->dpvs.surfaces[i].material)
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->dpvs.surfaces[i].material->name);
				}
			}
		}

		// Mdao
		if (data->mdaoVolumes)
		{
			for (unsigned int i = 0; i < data->mdaoVolumesCount; i++)
			{
				if (data->mdaoVolumes[i].volumeData)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData->name);
				}
			}
		}

		// fixes
		sort_dpvs_surfaces(this->asset_, zone);
		re_calc_lightmap_params(this->asset_->draw.lightmaps, &this->asset_->draw.lightmapParameters, zone);
		fixup_static_models(this->asset_, zone);
	}

	std::string gfx_world::name()
	{
		return this->name_;
	}

	std::int32_t gfx_world::type()
	{
		return ASSET_TYPE_GFXWORLD;
	}

	void gfx_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name_);
		dest->baseName = buf->write_str(this->base_name_);

		if (data->skies)
		{
			buf->align(3);
			auto* destskies = buf->write(data->skies, data->skyCount);
			for (int i = 0; i < data->skyCount; i++)
			{
				if (data->skies[i].skyStartSurfs)
				{
					buf->align(3);
					buf->write(data->skies[i].skyStartSurfs, data->skies[i].skySurfCount);
					buf->clear_pointer(&destskies[i].skyStartSurfs);
				}

				if (data->skies[i].skyImage)
				{
					destskies[i].skyImage = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						ASSET_TYPE_IMAGE, data->skies[i].skyImage->name));
				}
			}

			buf->clear_pointer(&dest->skies);
		}

		if (data->dpvsPlanes.planes)
		{
			dest->dpvsPlanes.planes = buf->write_s(3, data->dpvsPlanes.planes, data->planeCount);
		}

		if (data->dpvsPlanes.nodes)
		{
			buf->align(1);
			buf->write(data->dpvsPlanes.nodes, data->nodeCount);
			buf->clear_pointer(&dest->dpvsPlanes.nodes);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvsPlanes.sceneEntCellBits)
		{
			buf->align(3);
			buf->write(data->dpvsPlanes.sceneEntCellBits, data->dpvsPlanes.cellCount << 9);
			buf->clear_pointer(&dest->dpvsPlanes.sceneEntCellBits);
		}
		buf->pop_stream();

		if (data->aabbTreeCounts)
		{
			buf->align(3);
			buf->write(data->aabbTreeCounts, data->dpvsPlanes.cellCount);
			buf->clear_pointer(&dest->aabbTreeCounts);
		}

		if (data->aabbTrees)
		{
			buf->align(127);
			auto cell_tree = buf->write(data->aabbTrees, data->dpvsPlanes.cellCount);

			for (int i = 0; i < data->dpvsPlanes.cellCount; i++)
			{
				if (data->aabbTrees[i].aabbTree)
				{
					buf->align(3);
					auto gfx_aabb_tree = buf->write(data->aabbTrees[i].aabbTree,
						data->aabbTreeCounts[i].aabbTreeCount);

					for (int i2 = 0; i2 < data->aabbTreeCounts[i].aabbTreeCount; i2++)
					{
						if (data->aabbTrees[i].aabbTree[i2].smodelIndexes)
						{
							gfx_aabb_tree[i2].smodelIndexes = buf->write_s(
								1, data->aabbTrees[i].aabbTree[i2].smodelIndexes,
								data->aabbTrees[i].aabbTree[i2].smodelIndexCount);
						}
					}

					buf->clear_pointer(&cell_tree[i].aabbTree);
				}
			}

			buf->clear_pointer(&dest->aabbTrees);
		}

		if (data->cells)
		{
			buf->align(3);
			auto gfx_cell = buf->write(data->cells, data->dpvsPlanes.cellCount);

			for (int i = 0; i < data->dpvsPlanes.cellCount; i++)
			{
				if (data->cells[i].portals)
				{
					buf->align(3);
					auto gfx_portal = buf->write(data->cells[i].portals, data->cells[i].portalCount);

					for (int i2 = 0; i2 < data->cells[i].portalCount; i2++)
					{
						if (data->cells[i].portals[i2].vertices)
						{
							buf->align(3);
							buf->write(data->cells[i].portals[i2].vertices, data->cells[i].portals[i2].vertexCount);
							buf->clear_pointer(&gfx_portal[i2].vertices);
						}
					}

					buf->clear_pointer(&gfx_cell[i].portals);
				}

				if (data->cells[i].reflectionProbes)
				{
					buf->align(0);
					buf->write(data->cells[i].reflectionProbes, data->cells[i].reflectionProbeCount);
					buf->clear_pointer(&gfx_cell[i].reflectionProbes);
				}
				if (data->cells[i].reflectionProbeReferences)
				{
					buf->align(0);
					buf->write(data->cells[i].reflectionProbeReferences, data->cells[i].reflectionProbeReferenceCount);
					buf->clear_pointer(&gfx_cell[i].reflectionProbeReferences);
				}

				buf->clear_pointer(&dest->cells);
			}
		}

		if (data->portalGroup)
		{
			buf->align(3);
			auto portalGroups = buf->write(data->portalGroup, data->portalGroupCount);

			for (unsigned int i = 0; i < data->portalGroupCount; i++)
			{
				if (data->portalGroup[i].targetName)
				{
					portalGroups[i].targetName = buf->write_str(data->portalGroup[i].targetName);
				}
				if (data->portalGroup[i].gfxPortalArray)
				{
					buf->align(3);
					buf->write(data->portalGroup[i].gfxPortalArray, data->portalGroup[i].numPortals);
					buf->clear_pointer(&portalGroups[i].gfxPortalArray);
				}
			}

			buf->clear_pointer(&dest->portalGroup);
		}

		if (data->portalDistanceAnchorsAndCloseDistSquared)
		{
			buf->align(3);
			buf->write(data->portalDistanceAnchorsAndCloseDistSquared, data->portalDistanceAnchorCount);
			buf->clear_pointer(&dest->portalDistanceAnchorsAndCloseDistSquared);
		}

		if (data->draw.reflectionProbes)
		{
			buf->align(7);
			auto reflectionProbes = buf->write(data->draw.reflectionProbes,
				data->draw.reflectionProbeCount);

			for (unsigned int i = 0; i < data->draw.reflectionProbeCount; i++)
			{
				if (reflectionProbes[i])
				{
					reflectionProbes[i] = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						ASSET_TYPE_IMAGE, data->draw.reflectionProbes[i]->name));
				}
			}

			buf->clear_pointer(&dest->draw.reflectionProbes);
		}

		if (data->draw.reflectionProbeOrigins)
		{
			buf->align(3);
			auto reflectionProbeOrigins = buf->write(data->draw.reflectionProbeOrigins, data->draw.reflectionProbeCount);

			for (unsigned int i = 0; i < data->draw.reflectionProbeCount; i++)
			{
				if (reflectionProbeOrigins[i].probeVolumes)
				{
					buf->align(3);
					auto probeVolumes = buf->write(data->draw.reflectionProbeOrigins[i].probeVolumes, data->draw.reflectionProbeOrigins[i].probeVolumeCount);

					for (unsigned int j = 0; j < data->draw.reflectionProbeOrigins[i].probeVolumeCount; j++)
					{
						if (probeVolumes[j].probeVolumePlanes)
						{
							buf->align(1);
							buf->write(data->draw.reflectionProbeOrigins[i].probeVolumes[j].probeVolumePlanes, data->draw.reflectionProbeOrigins[i].probeVolumes[j].probeVolumePlaneCount);
							buf->clear_pointer(&probeVolumes[j].probeVolumePlanes);
						}
					}

					buf->clear_pointer(&reflectionProbeOrigins[i].probeVolumes);
				}
			}

			buf->clear_pointer(&dest->draw.reflectionProbeOrigins);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->draw.reflectionProbeTextures)
		{
			buf->align(3);
			buf->write(data->draw.reflectionProbeTextures, data->draw.reflectionProbeCount);
			buf->clear_pointer(&dest->draw.reflectionProbeTextures);
		}
		buf->pop_stream();

		if (data->draw.reflectionProbeReferenceOrigins)
		{
			buf->align(3);
			buf->write(data->draw.reflectionProbeReferenceOrigins, data->draw.reflectionProbeReferenceCount);
			buf->clear_pointer(&dest->draw.reflectionProbeReferenceOrigins);
		}

		if (data->draw.reflectionProbeReferences)
		{
			buf->align(0);
			buf->write(data->draw.reflectionProbeReferences, data->draw.reflectionProbeReferenceCount);
			buf->clear_pointer(&dest->draw.reflectionProbeReferences);
		}

		if (data->draw.lightmaps)
		{
			buf->align(3);
			auto destlightmaps = buf->write(data->draw.lightmaps, data->draw.lightmapCount);

			for (int i = 0; i < data->draw.lightmapCount; i++)
			{
				if (data->draw.lightmaps[i].primary)
				{
					destlightmaps[i].primary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						ASSET_TYPE_IMAGE, data->draw.lightmaps[i].primary->name));
				}

				if (data->draw.lightmaps[i].secondary)
				{
					destlightmaps[i].secondary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
						ASSET_TYPE_IMAGE, data->draw.lightmaps[i].secondary->name));
				}
			}

			buf->clear_pointer(&dest->draw.lightmaps);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->draw.lightmapPrimaryTextures)
		{
			buf->align(3);
			buf->write(data->draw.lightmapPrimaryTextures, data->draw.lightmapCount);
			buf->clear_pointer(&dest->draw.lightmapPrimaryTextures);
		}
		buf->pop_stream();

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->draw.lightmapSecondaryTextures)
		{
			buf->align(3);
			buf->write(data->draw.lightmapSecondaryTextures, data->draw.lightmapCount);
			buf->clear_pointer(&dest->draw.lightmapSecondaryTextures);
		}
		buf->pop_stream();

		if (data->draw.lightmapOverridePrimary)
		{
			dest->draw.lightmapOverridePrimary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
				ASSET_TYPE_IMAGE, data->draw.lightmapOverridePrimary->name));
		}

		if (data->draw.lightmapOverrideSecondary)
		{
			dest->draw.lightmapOverrideSecondary = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
				ASSET_TYPE_IMAGE, data->draw.lightmapOverrideSecondary->name));
		}

		if (data->draw.vd.vertices)
		{
			buf->align(3);
			buf->write(data->draw.vd.vertices, data->draw.vertexCount);
			buf->clear_pointer(&dest->draw.vd.vertices);
		}

		if (data->draw.vld.data)
		{
			buf->align(3);
			buf->write(data->draw.vld.data, data->draw.vertexLayerDataSize);
			buf->clear_pointer(&dest->draw.vld.data);
		}

		if (data->draw.indices)
		{
			buf->align(3);
			buf->write(data->draw.indices, data->draw.indexCount);
			buf->clear_pointer(&dest->draw.indices);
		}

		if (data->lightGrid.rowDataStart)
		{
			buf->align(1);
			buf->write(data->lightGrid.rowDataStart,
				data->lightGrid.maxs[data->lightGrid.rowAxis] - data->lightGrid.mins[data->lightGrid.rowAxis] + 1);
			buf->clear_pointer(&dest->lightGrid.rowDataStart);
		}

		if (data->lightGrid.rawRowData)
		{
			buf->align(0);
			buf->write(data->lightGrid.rawRowData, data->lightGrid.rawRowDataSize);
			buf->clear_pointer(&dest->lightGrid.rawRowData);
		}

		if (data->lightGrid.entries)
		{
			buf->align(3);
			buf->write(data->lightGrid.entries, data->lightGrid.entryCount);
			buf->clear_pointer(&dest->lightGrid.entries);
		}

		if (data->lightGrid.colors)
		{
			buf->align(3);
			buf->write(data->lightGrid.colors, data->lightGrid.colorCount);
			buf->clear_pointer(&dest->lightGrid.colors);
		}

		if (data->lightGrid.stageLightingContrastGain)
		{
			buf->align(3);
			buf->write(data->lightGrid.stageLightingContrastGain, data->lightGrid.stageCount);
			buf->clear_pointer(&dest->lightGrid.stageLightingContrastGain);
		}

		if (data->lightGrid.paletteEntryAddress)
		{
			buf->align(3);
			buf->write(data->lightGrid.paletteEntryAddress, data->lightGrid.paletteEntryCount);
			buf->clear_pointer(&dest->lightGrid.paletteEntryAddress);
		}

		if (data->lightGrid.paletteBitstream)
		{
			buf->align(0);
			buf->write(data->lightGrid.paletteBitstream, data->lightGrid.paletteBitstreamSize);
			buf->clear_pointer(&dest->lightGrid.paletteBitstream);
		}

		for (auto i = 0; i < 3; i++)
		{
			if (data->lightGrid.tree[i].p_nodeTable)
			{
				buf->align(3);
				buf->write(data->lightGrid.tree[i].p_nodeTable, data->lightGrid.tree[i].nodeCount);
				buf->clear_pointer(&dest->lightGrid.tree[i].p_nodeTable);
			}

			if (data->lightGrid.tree[i].p_leafTable)
			{
				buf->align(0);
				buf->write(data->lightGrid.tree[i].p_leafTable, data->lightGrid.tree[i].leafTableSize);
				buf->clear_pointer(&dest->lightGrid.tree[i].p_leafTable);
			}
		}

		if (data->models)
		{
			buf->align(3);
			buf->write(data->models, data->modelCount);
			buf->clear_pointer(&dest->models);
		}

		if (data->materialMemory)
		{
			buf->align(3);
			auto memory = buf->write(data->materialMemory, data->materialMemoryCount);

			for (int i = 0; i < data->materialMemoryCount; i++)
			{
				if (data->materialMemory[i].material)
				{
					memory[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
						ASSET_TYPE_MATERIAL, data->materialMemory[i].material->name));
				}
			}

			buf->clear_pointer(&dest->materialMemory);
		}

		if (data->sun.spriteMaterial)
		{
			dest->sun.spriteMaterial = reinterpret_cast<Material*>(zone->get_asset_pointer(
				ASSET_TYPE_MATERIAL, data->sun.spriteMaterial->name));
		}
		if (data->sun.flareMaterial)
		{
			dest->sun.flareMaterial = reinterpret_cast<Material*>(zone->get_asset_pointer(
				ASSET_TYPE_MATERIAL, data->sun.flareMaterial->name));
		}

		if (data->outdoorImage)
		{
			dest->outdoorImage = reinterpret_cast<GfxImage*>(
				zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->outdoorImage->name));
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->cellCasterBits)
		{
			buf->align(3);
			buf->write(data->cellCasterBits,
				data->dpvsPlanes.cellCount * ((data->dpvsPlanes.cellCount + 31) >> 5));
			buf->clear_pointer(&dest->cellCasterBits);
		}

		if (data->cellHasSunLitSurfsBits)
		{
			buf->align(3);
			buf->write(data->cellHasSunLitSurfsBits, (data->dpvsPlanes.cellCount + 31) >> 5);
			buf->clear_pointer(&dest->cellHasSunLitSurfsBits);
		}

		if (data->sceneDynModel)
		{
			buf->align(3);
			buf->write(data->sceneDynModel, data->dpvsDyn.dynEntClientCount[0]);
			buf->clear_pointer(&dest->sceneDynModel);
		}

		if (data->sceneDynBrush)
		{
			buf->align(3);
			buf->write(data->sceneDynBrush, data->dpvsDyn.dynEntClientCount[1]);
			buf->clear_pointer(&dest->sceneDynBrush);
		}

		if (data->primaryLightEntityShadowVis)
		{
			buf->align(3);
			buf->write(data->primaryLightEntityShadowVis,
				((data->primaryLightCount - data->lastSunPrimaryLightIndex) << 13) - 0x2000);
			buf->clear_pointer(&dest->primaryLightEntityShadowVis);
		}

		if (data->primaryLightDynEntShadowVis[0])
		{
			buf->align(3);
			buf->write(data->primaryLightDynEntShadowVis[0],
				data->dpvsDyn.dynEntClientCount[0] * (data->primaryLightCount - data->lastSunPrimaryLightIndex - 1));
			buf->clear_pointer(&dest->primaryLightDynEntShadowVis[0]);
		}

		if (data->primaryLightDynEntShadowVis[1])
		{
			buf->align(3);
			buf->write(data->primaryLightDynEntShadowVis[1],
				data->dpvsDyn.dynEntClientCount[1] * (data->primaryLightCount - data->lastSunPrimaryLightIndex - 1));
			buf->clear_pointer(&dest->primaryLightDynEntShadowVis[1]);
		}

		if (data->nonSunPrimaryLightForModelDynEnt)
		{
			buf->align(1);
			buf->write(data->nonSunPrimaryLightForModelDynEnt, data->dpvsDyn.dynEntClientCount[0]);
			buf->clear_pointer(&dest->nonSunPrimaryLightForModelDynEnt);
		}
		buf->pop_stream();

		if (data->shadowGeom)
		{
			buf->align(3);
			auto shadow_geometry = buf->write(data->shadowGeom, data->primaryLightCount);

			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->shadowGeom[i].sortedSurfIndex)
				{
					buf->align(3);
					buf->write(data->shadowGeom[i].sortedSurfIndex, data->shadowGeom[i].surfaceCount);
					buf->clear_pointer(&shadow_geometry[i].sortedSurfIndex);
				}
				if (data->shadowGeom[i].smodelIndex)
				{
					buf->align(1);
					buf->write(data->shadowGeom[i].smodelIndex, data->shadowGeom[i].smodelCount);
					buf->clear_pointer(&shadow_geometry[i].smodelIndex);
				}
			}

			buf->clear_pointer(&dest->shadowGeom);
		}
		if (data->shadowGeomOptimized)
		{
			buf->align(3);
			auto shadow_geometry = buf->write(data->shadowGeomOptimized, data->primaryLightCount);

			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->shadowGeomOptimized[i].sortedSurfIndex)
				{
					buf->align(3);
					buf->write(data->shadowGeomOptimized[i].sortedSurfIndex, data->shadowGeomOptimized[i].surfaceCount);
					buf->clear_pointer(&shadow_geometry[i].sortedSurfIndex);
				}
				if (data->shadowGeomOptimized[i].smodelIndex)
				{
					buf->align(1);
					buf->write(data->shadowGeomOptimized[i].smodelIndex, data->shadowGeomOptimized[i].smodelCount);
					buf->clear_pointer(&shadow_geometry[i].smodelIndex);
				}
			}

			buf->clear_pointer(&dest->shadowGeomOptimized);
		}

		if (data->lightRegion)
		{
			buf->align(3);
			auto light_region = buf->write(data->lightRegion, data->primaryLightCount);

			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->lightRegion[i].hulls)
				{
					buf->align(3);
					auto light_region_hull = buf->write(data->lightRegion[i].hulls, data->lightRegion[i].hullCount);

					for (unsigned int i2 = 0; i2 < data->lightRegion[i].hullCount; i2++)
					{
						if (data->lightRegion[i].hulls[i2].axis)
						{
							buf->align(3);
							buf->write(data->lightRegion[i].hulls[i2].axis,
								data->lightRegion[i].hulls[i2].axisCount);
							buf->clear_pointer(&light_region_hull[i2].axis);
						}
					}

					buf->clear_pointer(&light_region[i].hulls);
				}
			}

			buf->clear_pointer(&dest->lightRegion);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		static_assert(offsetof(GfxWorldDpvsStatic, smodelVisData[0]) == 72);
		if (data->dpvs.smodelVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelVisData[0], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelVisData[0]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelVisData[1]) == 80);
		if (data->dpvs.smodelVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelVisData[1], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelVisData[1]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelVisData[2]) == 88);
		if (data->dpvs.smodelVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelVisData[2], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelVisData[2]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceVisData[0]) == 320);
		if (data->dpvs.surfaceVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceVisData[0], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceVisData[0]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceVisData[1]) == 328);
		if (data->dpvs.surfaceVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceVisData[1], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceVisData[1]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceVisData[2]) == 336);
		if (data->dpvs.surfaceVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceVisData[2], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceVisData[2]);
		}
		buf->pop_stream();
		static_assert(offsetof(GfxWorldDpvsStatic, unknownSModelVisData1) == 680);
		if (data->dpvs.unknownSModelVisData1)
		{
			buf->align(127);
			buf->write(data->dpvs.unknownSModelVisData1, data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.unknownSModelVisData1);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, unknownSModelVisData2) == 688);
		if (data->dpvs.unknownSModelVisData2)
		{
			buf->align(127);
			buf->write(data->dpvs.unknownSModelVisData2, 2 * data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.unknownSModelVisData2);
		}
		buf->push_stream(XFILE_BLOCK_RUNTIME);
		static_assert(offsetof(GfxWorldDpvsStatic, smodelVisData[3]) == 96);
		if (data->dpvs.smodelVisData[3])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelVisData[3], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelVisData[3]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceVisData[3]) == 344);
		if (data->dpvs.surfaceVisData[3])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceVisData[3], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceVisData[3]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[0]) == 104);
		if (data->dpvs.smodelUnknownVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[0], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[0]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[1]) == 112);
		if (data->dpvs.smodelUnknownVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[1], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[1]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[2]) == 120);
		if (data->dpvs.smodelUnknownVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[2], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[2]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[3]) == 128);
		if (data->dpvs.smodelUnknownVisData[3])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[3], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[3]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[4]) == 136);
		if (data->dpvs.smodelUnknownVisData[4])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[4], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[4]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[5]) == 144);
		if (data->dpvs.smodelUnknownVisData[5])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[5], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[5]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[6]) == 152);
		if (data->dpvs.smodelUnknownVisData[6])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[6], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[6]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[7]) == 160);
		if (data->dpvs.smodelUnknownVisData[7])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[7], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[7]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[8]) == 168);
		if (data->dpvs.smodelUnknownVisData[8])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[8], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[8]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[0]) == 352);
		if (data->dpvs.surfaceUnknownVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[0], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[0]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[1]) == 360);
		if (data->dpvs.surfaceUnknownVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[1], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[1]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[2]) == 368);
		if (data->dpvs.surfaceUnknownVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[2], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[2]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[3]) == 376);
		if (data->dpvs.surfaceUnknownVisData[3])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[3], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[3]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[4]) == 384);
		if (data->dpvs.surfaceUnknownVisData[4])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[4], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[4]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[5]) == 392);
		if (data->dpvs.surfaceUnknownVisData[5])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[5], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[5]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[6]) == 400);
		if (data->dpvs.surfaceUnknownVisData[6])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[6], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[6]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[7]) == 408);
		if (data->dpvs.surfaceUnknownVisData[7])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[7], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[7]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[8]) == 416);
		if (data->dpvs.surfaceUnknownVisData[8])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[8], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[8]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[9]) == 176);
		if (data->dpvs.smodelUnknownVisData[9])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[9], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[9]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[10]) == 184);
		if (data->dpvs.smodelUnknownVisData[10])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[10], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[10]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[11]) == 192);
		if (data->dpvs.smodelUnknownVisData[11])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[11], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[11]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[12]) == 200);
		if (data->dpvs.smodelUnknownVisData[12])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[12], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[12]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[13]) == 208);
		if (data->dpvs.smodelUnknownVisData[13])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[13], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[13]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[14]) == 216);
		if (data->dpvs.smodelUnknownVisData[14])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[14], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[14]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[15]) == 224);
		if (data->dpvs.smodelUnknownVisData[15])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[15], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[15]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[16]) == 232);
		if (data->dpvs.smodelUnknownVisData[16])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[16], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[16]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[17]) == 240);
		if (data->dpvs.smodelUnknownVisData[17])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[17], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[17]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[9]) == 424);
		if (data->dpvs.surfaceUnknownVisData[9])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[9], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[9]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[10]) == 432);
		if (data->dpvs.surfaceUnknownVisData[10])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[10], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[10]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[11]) == 440);
		if (data->dpvs.surfaceUnknownVisData[11])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[11], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[11]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[12]) == 448);
		if (data->dpvs.surfaceUnknownVisData[12])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[12], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[12]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[13]) == 456);
		if (data->dpvs.surfaceUnknownVisData[13])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[13], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[13]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[14]) == 464);
		if (data->dpvs.surfaceUnknownVisData[14])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[14], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[14]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[15]) == 472);
		if (data->dpvs.surfaceUnknownVisData[15])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[15], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[15]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[16]) == 480);
		if (data->dpvs.surfaceUnknownVisData[16])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[16], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[16]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[17]) == 488);
		if (data->dpvs.surfaceUnknownVisData[17])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[17], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[17]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[18]) == 248);
		if (data->dpvs.smodelUnknownVisData[18])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[18], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[18]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[19]) == 256);
		if (data->dpvs.smodelUnknownVisData[19])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[19], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[19]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[20]) == 264);
		if (data->dpvs.smodelUnknownVisData[20])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[20], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[20]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[21]) == 272);
		if (data->dpvs.smodelUnknownVisData[21])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[21], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[21]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[22]) == 280);
		if (data->dpvs.smodelUnknownVisData[22])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[22], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[22]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[23]) == 288);
		if (data->dpvs.smodelUnknownVisData[23])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[23], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[23]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[24]) == 296);
		if (data->dpvs.smodelUnknownVisData[24])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[24], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[24]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[25]) == 304);
		if (data->dpvs.smodelUnknownVisData[25])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[26], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[25]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData[26]) == 312);
		if (data->dpvs.smodelUnknownVisData[26])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUnknownVisData[26], data->dpvs.smodelVisDataCount);
			buf->clear_pointer(&dest->dpvs.smodelUnknownVisData[26]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[18]) == 496);
		if (data->dpvs.surfaceUnknownVisData[18])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[18], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[18]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[19]) == 504);
		if (data->dpvs.surfaceUnknownVisData[19])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[19], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[19]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[20]) == 512);
		if (data->dpvs.surfaceUnknownVisData[20])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[20], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[20]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[21]) == 520);
		if (data->dpvs.surfaceUnknownVisData[21])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[21], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[21]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[22]) == 528);
		if (data->dpvs.surfaceUnknownVisData[22])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[22], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[22]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[23]) == 536);
		if (data->dpvs.surfaceUnknownVisData[23])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[23], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[23]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[24]) == 544);
		if (data->dpvs.surfaceUnknownVisData[24])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[24], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[24]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[25]) == 552);
		if (data->dpvs.surfaceUnknownVisData[25])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[25], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[25]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData[26]) == 560);
		if (data->dpvs.surfaceUnknownVisData[26])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUnknownVisData[26], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUnknownVisData[26]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUmbraVisData[0]) == 568);
		if (data->dpvs.smodelUmbraVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUmbraVisData[0], data->dpvs.smodelVisDataCount + 1);
			buf->clear_pointer(&dest->dpvs.smodelUmbraVisData[0]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUmbraVisData[1]) == 576);
		if (data->dpvs.smodelUmbraVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUmbraVisData[1], data->dpvs.smodelVisDataCount + 1);
			buf->clear_pointer(&dest->dpvs.smodelUmbraVisData[1]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUmbraVisData[2]) == 584);
		if (data->dpvs.smodelUmbraVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUmbraVisData[2], data->dpvs.smodelVisDataCount + 1);
			buf->clear_pointer(&dest->dpvs.smodelUmbraVisData[2]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUmbraVisData[0]) == 600);
		if (data->dpvs.surfaceUmbraVisData[0])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUmbraVisData[0], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUmbraVisData[0]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUmbraVisData[1]) == 608);
		if (data->dpvs.surfaceUmbraVisData[1])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUmbraVisData[1], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUmbraVisData[1]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUmbraVisData[2]) == 616);
		if (data->dpvs.surfaceUmbraVisData[2])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUmbraVisData[2], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUmbraVisData[2]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, smodelUmbraVisData[3]) == 592);
		if (data->dpvs.smodelUmbraVisData[3])
		{
			buf->align(127);
			buf->write(data->dpvs.smodelUmbraVisData[3], data->dpvs.smodelVisDataCount + 1);
			buf->clear_pointer(&dest->dpvs.smodelUmbraVisData[3]);
		}
		static_assert(offsetof(GfxWorldDpvsStatic, surfaceUmbraVisData[3]) == 624);
		if (data->dpvs.surfaceUmbraVisData[3])
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceUmbraVisData[3], data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceUmbraVisData[3]);
		}
		buf->pop_stream();
		if (data->dpvs.lodData)
		{
			buf->align(3);
			buf->write(data->dpvs.lodData, data->dpvs.smodelCount + 1);
			buf->clear_pointer(&dest->dpvs.lodData);
		}

		if (data->dpvs.sortedSurfIndex)
		{
			buf->align(3);
			buf->write(data->dpvs.sortedSurfIndex, data->dpvs.staticSurfaceCount);
			buf->clear_pointer(&dest->dpvs.sortedSurfIndex);
		}

		if (data->dpvs.smodelInsts)
		{
			buf->align(3);
			buf->write(data->dpvs.smodelInsts, data->dpvs.smodelCount);
			buf->clear_pointer(&dest->dpvs.smodelInsts);
		}

		if (data->dpvs.surfaces)
		{
			buf->align(3);
			auto destgfxsurfs = buf->write(data->dpvs.surfaces, data->surfaceCount);

			for (unsigned int i = 0; i < data->surfaceCount; i++)
			{
				if (data->dpvs.surfaces[i].material)
				{
					destgfxsurfs[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->dpvs.surfaces[i].material->name));
				}
			}

			buf->clear_pointer(&dest->dpvs.surfaces);
		}

		if (data->dpvs.surfacesBounds)
		{
			buf->align(3);
			buf->write(data->dpvs.surfacesBounds, data->surfaceCount);
			buf->clear_pointer(&dest->dpvs.surfacesBounds);
		}

		if (data->dpvs.smodelDrawInsts)
		{
			buf->align(3);
			auto destdrawinst = buf->write(data->dpvs.smodelDrawInsts, data->dpvs.smodelCount);

			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				if (data->dpvs.smodelDrawInsts[i].model)
				{
					destdrawinst[i].model = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->dpvs.smodelDrawInsts[i].model->name));
				}
			}

			buf->clear_pointer(&dest->dpvs.smodelDrawInsts);
		}

		if (data->dpvs.smodelLightingInsts)
		{
			buf->align(3);
			auto destlighting = buf->write(data->dpvs.smodelLightingInsts, data->dpvs.smodelCount);

			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				auto flags = data->dpvs.smodelDrawInsts[i].flags;
				if ((flags & 0x180) != 0 && (flags & 0x80) != 0 && data->dpvs.smodelLightingInsts[i].vertexLightingInfo.lightingValues)
				{
					destlighting[i].vertexLightingInfo.lightingValuesVb = nullptr;
					buf->align(3);
					buf->write(data->dpvs.smodelLightingInsts[i].vertexLightingInfo.lightingValues, data->dpvs.smodelLightingInsts[i].vertexLightingInfo.numLightingValues);
					buf->clear_pointer(&destlighting[i].vertexLightingInfo.lightingValues);
				}
			}

			buf->clear_pointer(&dest->dpvs.smodelLightingInsts);
		}

		if (data->dpvs.subdivVertexLighting)
		{
			buf->align(3);
			auto dest_subdivvertexlighting = buf->write(data->dpvs.subdivVertexLighting, data->dpvs.subdivVertexLightingInfoCount);
			for (unsigned int i = 0; i < data->dpvs.subdivVertexLightingInfoCount; i++)
			{
				dest_subdivvertexlighting[i].vb = nullptr;
				dest_subdivvertexlighting[i].cache.subdivCacheBuffer = nullptr;
				dest_subdivvertexlighting[i].cache.subdivCacheView = nullptr;
			}
			buf->clear_pointer(&dest->dpvs.subdivVertexLighting);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvs.surfaceMaterials)
		{
			buf->align(7);
			buf->write(data->dpvs.surfaceMaterials, data->surfaceCount);
			buf->clear_pointer(&dest->dpvs.surfaceMaterials);
		}
		if (data->dpvs.surfaceCastsSunShadow)
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceCastsSunShadow, data->dpvs.surfaceVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceCastsSunShadow);
		}
		if (data->dpvs.surfaceCastsSunShadowOpt)
		{
			buf->align(127);
			buf->write(data->dpvs.surfaceCastsSunShadowOpt, data->dpvs.sunShadowOptCount * data->dpvs.sunSurfVisDataCount);
			buf->clear_pointer(&dest->dpvs.surfaceCastsSunShadowOpt);
		}
		if (data->dpvs.surfaceDeptAndSurf)
		{
			buf->align(3);
			buf->write(data->dpvs.surfaceDeptAndSurf, data->dpvs.staticSurfaceCount);
			buf->clear_pointer(&dest->dpvs.surfaceDeptAndSurf);
		}
		if (data->dpvs.constantBuffersLit)
		{
			buf->align(7);
			auto destvoid = buf->write(data->dpvs.constantBuffersLit, data->dpvs.smodelCount);

			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				if (data->dpvs.constantBuffersLit[i])
				{
					destvoid[i] = buf->write_s(0, data->dpvs.constantBuffersLit[i], 1);
				}
			}

			buf->clear_pointer(&dest->dpvs.constantBuffersLit);
		}

		if (data->dpvs.constantBuffersAmbient)
		{
			buf->align(7);
			auto destvoid = buf->write(data->dpvs.constantBuffersAmbient, data->dpvs.smodelCount);

			for (unsigned int i = 0; i < data->dpvs.smodelCount; i++)
			{
				if (data->dpvs.constantBuffersAmbient[i])
				{
					destvoid[i] = buf->write_s(0, data->dpvs.constantBuffersAmbient[i], 1);
				}
			}

			buf->clear_pointer(&dest->dpvs.constantBuffersAmbient);
		}
		buf->pop_stream();

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->dpvsDyn.dynEntCellBits[0])
		{
			buf->align(3);
			buf->write(data->dpvsDyn.dynEntCellBits[0],
				data->dpvsDyn.dynEntClientWordCount[0] * data->dpvsPlanes.cellCount);
			buf->clear_pointer(&dest->dpvsDyn.dynEntCellBits[0]);
		}

		if (data->dpvsDyn.dynEntCellBits[1])
		{
			buf->align(3);
			buf->write(data->dpvsDyn.dynEntCellBits[1],
				data->dpvsDyn.dynEntClientWordCount[1] * data->dpvsPlanes.cellCount);
			buf->clear_pointer(&dest->dpvsDyn.dynEntCellBits[1]);
		}

		if (data->dpvsDyn.dynEntVisData[0][0])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[0][0], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[0][0]);
		}

		if (data->dpvsDyn.dynEntVisData[0][1])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[0][1], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[0][1]);
		}

		if (data->dpvsDyn.dynEntVisData[0][2])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[0][2], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[0][2]);
		}

		if (data->dpvsDyn.dynEntVisData[1][0])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[1][0], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[1][0]);
		}

		if (data->dpvsDyn.dynEntVisData[1][1])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[1][2], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[1][1]);
		}

		if (data->dpvsDyn.dynEntVisData[1][2])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[1][2], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[1][2]);
		}

		if (data->dpvsDyn.dynEntVisData[0][3])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[0][3], 32 * data->dpvsDyn.dynEntClientWordCount[0]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[0][3]);
		}

		if (data->dpvsDyn.dynEntVisData[1][3])
		{
			buf->align(15);
			buf->write(data->dpvsDyn.dynEntVisData[1][3], 32 * data->dpvsDyn.dynEntClientWordCount[1]);
			buf->clear_pointer(&dest->dpvsDyn.dynEntVisData[1][3]);
		}
		buf->pop_stream();

		if (data->heroOnlyLights)
		{
			buf->align(3);
			buf->write(data->heroOnlyLights, data->heroOnlyLightCount);
			buf->clear_pointer(&dest->heroOnlyLights);
		}

		if (data->umbraTomeData)
		{
			buf->align(15);
			buf->write(data->umbraTomeData, data->umbraTomeSize);
			buf->clear_pointer(&dest->umbraTomeData);
		}
		dest->umbraTomePtr = nullptr;

		if (data->mdaoVolumes)
		{
			buf->align(3);
			auto dest_mdaovolumes = buf->write(data->mdaoVolumes, data->mdaoVolumesCount);

			for (unsigned int i = 0; i < data->mdaoVolumesCount; i++)
			{
				dest_mdaovolumes[i].volumeData = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData->name));
			}

			buf->clear_pointer(&dest->mdaoVolumes);
		}

		if (data->buildInfo.bspCommandline)
		{
			dest->buildInfo.bspCommandline = buf->write_str(data->buildInfo.bspCommandline);
		}
		if (data->buildInfo.lightCommandline)
		{
			dest->buildInfo.lightCommandline = buf->write_str(data->buildInfo.lightCommandline);
		}
		if (data->buildInfo.bspTimestamp)
		{
			dest->buildInfo.bspTimestamp = buf->write_str(data->buildInfo.bspTimestamp);
		}
		if (data->buildInfo.lightTimestamp)
		{
			dest->buildInfo.lightTimestamp = buf->write_str(data->buildInfo.lightTimestamp);
		}

		buf->pop_stream();
	}

	void gfx_world::dump(GfxWorld* asset)
	{
		const auto path = asset->name + ".gfxmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_string(asset->baseName);

		write.dump_array(asset->skies, asset->skyCount);
		for (int i = 0; i < asset->skyCount; i++)
		{
			write.dump_array(asset->skies[i].skyStartSurfs, asset->skies[i].skySurfCount);
			write.dump_asset(asset->skies[i].skyImage);
		}

		write.dump_array(asset->dpvsPlanes.planes, asset->planeCount);
		write.dump_array(asset->dpvsPlanes.nodes, asset->nodeCount);
		write.dump_array(asset->dpvsPlanes.sceneEntCellBits, asset->dpvsPlanes.cellCount << 9);

		write.dump_array(asset->aabbTreeCounts, asset->dpvsPlanes.cellCount);
		write.dump_array(asset->aabbTrees, asset->dpvsPlanes.cellCount);
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			write.dump_array(asset->aabbTrees[i].aabbTree, asset->aabbTreeCounts[i].aabbTreeCount);
			for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
			{
				write.dump_array(asset->aabbTrees[i].aabbTree[j].smodelIndexes,
					asset->aabbTrees[i].aabbTree[j].smodelIndexCount);
			}
		}

		write.dump_array(asset->cells, asset->dpvsPlanes.cellCount);
		for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
		{
			write.dump_array(asset->cells[i].portals, asset->cells[i].portalCount);
			for (int j = 0; j < asset->cells[i].portalCount; j++)
			{
				write.dump_array(asset->cells[i].portals[j].vertices, asset->cells[i].portals[j].vertexCount);
			}
			write.dump_array(asset->cells[i].reflectionProbes, asset->cells[i].reflectionProbeCount);
			write.dump_array(asset->cells[i].reflectionProbeReferences, asset->cells[i].reflectionProbeReferenceCount);
		}

		write.dump_array(asset->portalGroup, asset->portalGroupCount);
		for (unsigned int i = 0; i < asset->portalGroupCount; i++)
		{
			if (asset->portalGroup[i].targetName)
			{
				write.dump_string(asset->portalGroup[i].targetName);
			}

			write.dump_array(asset->portalGroup[i].gfxPortalArray, asset->portalGroup[i].numPortals);
		}

		write.dump_array(asset->portalDistanceAnchorsAndCloseDistSquared, asset->portalDistanceAnchorCount);

		write.dump_array(asset->draw.reflectionProbes, asset->draw.reflectionProbeCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			write.dump_asset(asset->draw.reflectionProbes[i]);
		}

		write.dump_array(asset->draw.reflectionProbeOrigins, asset->draw.reflectionProbeCount);
		for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
		{
			write.dump_array(asset->draw.reflectionProbeOrigins[i].probeVolumes, asset->draw.reflectionProbeOrigins[i].probeVolumeCount);
			for (unsigned int j = 0; j < asset->draw.reflectionProbeOrigins[i].probeVolumeCount; j++)
			{
				write.dump_array(asset->draw.reflectionProbeOrigins[i].probeVolumes[j].probeVolumePlanes, asset->draw.reflectionProbeOrigins[i].probeVolumes[j].probeVolumePlaneCount);
			}
		}

		write.dump_array(asset->draw.reflectionProbeTextures, asset->draw.reflectionProbeCount);

		write.dump_array(asset->draw.reflectionProbeReferenceOrigins, asset->draw.reflectionProbeReferenceCount);
		write.dump_array(asset->draw.reflectionProbeReferences, asset->draw.reflectionProbeReferenceCount);

		write.dump_array(asset->draw.lightmaps, asset->draw.lightmapCount);
		for (int i = 0; i < asset->draw.lightmapCount; i++)
		{
			write.dump_asset(asset->draw.lightmaps[i].primary);
			write.dump_asset(asset->draw.lightmaps[i].secondary);
		}
		write.dump_array(asset->draw.lightmapPrimaryTextures, asset->draw.lightmapCount);
		write.dump_array(asset->draw.lightmapSecondaryTextures, asset->draw.lightmapCount);
		write.dump_asset(asset->draw.lightmapOverridePrimary);
		write.dump_asset(asset->draw.lightmapOverrideSecondary);

		write.dump_array(asset->draw.vd.vertices, asset->draw.vertexCount);
		write.dump_array(asset->draw.vld.data, asset->draw.vertexLayerDataSize);
		write.dump_array(asset->draw.indices, asset->draw.indexCount);

		write.dump_array(asset->lightGrid.rowDataStart,
			(asset->lightGrid.maxs[asset->lightGrid.rowAxis] - asset->lightGrid.mins[asset->lightGrid.rowAxis] + 1));
		write.dump_array(asset->lightGrid.rawRowData, asset->lightGrid.rawRowDataSize);
		write.dump_array(asset->lightGrid.entries, asset->lightGrid.entryCount);
		write.dump_array(asset->lightGrid.colors, asset->lightGrid.colorCount);

		write.dump_array(asset->lightGrid.stageLightingContrastGain, asset->lightGrid.stageCount);
		write.dump_array(asset->lightGrid.paletteEntryAddress, asset->lightGrid.paletteEntryCount);
		write.dump_array(asset->lightGrid.paletteBitstream, asset->lightGrid.paletteBitstreamSize);

		for (auto i = 0; i < 3; i++)
		{
			write.dump_array(asset->lightGrid.tree[i].p_nodeTable, asset->lightGrid.tree[i].nodeCount);
			write.dump_array(asset->lightGrid.tree[i].p_leafTable, asset->lightGrid.tree[i].leafTableSize);
		}

		write.dump_array(asset->models, asset->modelCount);

		write.dump_array(asset->materialMemory, asset->materialMemoryCount);
		for (int i = 0; i < asset->materialMemoryCount; i++)
		{
			write.dump_asset(asset->materialMemory[i].material);
		}

		write.dump_asset(asset->sun.spriteMaterial);
		write.dump_asset(asset->sun.flareMaterial);

		write.dump_asset(asset->outdoorImage);

		write.dump_array(asset->cellCasterBits,
			asset->dpvsPlanes.cellCount * ((asset->dpvsPlanes.cellCount + 31) >> 5));
		write.dump_array(asset->cellHasSunLitSurfsBits, (asset->dpvsPlanes.cellCount + 31) >> 5);

		write.dump_array(asset->sceneDynModel, asset->dpvsDyn.dynEntClientCount[0]);
		write.dump_array(asset->sceneDynBrush, asset->dpvsDyn.dynEntClientCount[1]);

		write.dump_array(asset->primaryLightEntityShadowVis,
			((asset->primaryLightCount - asset->lastSunPrimaryLightIndex) << 13) - 0x2000);

		write.dump_array(asset->primaryLightDynEntShadowVis[0],
			asset->dpvsDyn.dynEntClientCount[0] * (asset->primaryLightCount - asset->lastSunPrimaryLightIndex - 1));
		write.dump_array(asset->primaryLightDynEntShadowVis[1],
			asset->dpvsDyn.dynEntClientCount[1] * (asset->primaryLightCount - asset->lastSunPrimaryLightIndex - 1));

		write.dump_array(asset->nonSunPrimaryLightForModelDynEnt, asset->dpvsDyn.dynEntClientCount[0]);

		write.dump_array(asset->shadowGeom, asset->primaryLightCount);
		if (asset->shadowGeom)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->shadowGeom[i].sortedSurfIndex, asset->shadowGeom[i].surfaceCount);
				write.dump_array(asset->shadowGeom[i].smodelIndex, asset->shadowGeom[i].smodelCount);
			}
		}
		write.dump_array(asset->shadowGeomOptimized, asset->primaryLightCount);
		if (asset->shadowGeomOptimized)
		{
			for (unsigned int i = 0; i < asset->primaryLightCount; i++)
			{
				write.dump_array(asset->shadowGeomOptimized[i].sortedSurfIndex, asset->shadowGeomOptimized[i].surfaceCount);
				write.dump_array(asset->shadowGeomOptimized[i].smodelIndex, asset->shadowGeomOptimized[i].smodelCount);
			}
		}

		write.dump_array(asset->lightRegion, asset->primaryLightCount);
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			write.dump_array(asset->lightRegion[i].hulls, asset->lightRegion[i].hullCount);
			for (unsigned int j = 0; j < asset->lightRegion[i].hullCount; j++)
			{
				write.dump_array(asset->lightRegion[i].hulls[j].axis, asset->lightRegion[i].hulls[j].axisCount);
			}
		}

		for (auto i = 0; i < 4; i++)
		{
			write.dump_array(asset->dpvs.smodelVisData[i], asset->dpvs.smodelVisDataCount);
		}

		for (auto i = 0; i < 27; i++)
		{
			write.dump_array(asset->dpvs.smodelUnknownVisData[i], asset->dpvs.smodelVisDataCount);
		}

		for (auto i = 0; i < 4; i++)
		{
			write.dump_array(asset->dpvs.surfaceVisData[i], asset->dpvs.surfaceVisDataCount);
		}

		for (auto i = 0; i < 27; i++)
		{
			write.dump_array(asset->dpvs.surfaceUnknownVisData[i], asset->dpvs.surfaceVisDataCount);
		}

		for (auto i = 0; i < 4; i++)
		{
			write.dump_array(asset->dpvs.smodelUmbraVisData[i], asset->dpvs.smodelVisDataCount);
		}

		for (auto i = 0; i < 4; i++)
		{
			write.dump_array(asset->dpvs.surfaceUmbraVisData[i], asset->dpvs.surfaceVisDataCount);
		}

		write.dump_array(asset->dpvs.unknownSModelVisData1, asset->dpvs.smodelVisDataCount);
		write.dump_array(asset->dpvs.unknownSModelVisData2, 2 * asset->dpvs.smodelVisDataCount);

		write.dump_array(asset->dpvs.lodData, asset->dpvs.smodelCount + 1);
		write.dump_array(asset->dpvs.sortedSurfIndex, asset->dpvs.staticSurfaceCount);

		write.dump_array(asset->dpvs.smodelInsts, asset->dpvs.smodelCount);
		write.dump_array(asset->dpvs.surfaces, asset->surfaceCount);
		for (unsigned int i = 0; i < asset->surfaceCount; i++)
		{
			write.dump_asset(asset->dpvs.surfaces[i].material);
		}
		write.dump_array(asset->dpvs.surfacesBounds, asset->surfaceCount);
		write.dump_array(asset->dpvs.smodelDrawInsts, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_asset(asset->dpvs.smodelDrawInsts[i].model);
		}
		write.dump_array(asset->dpvs.smodelLightingInsts, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			auto flags = asset->dpvs.smodelDrawInsts[i].flags;
			if ((flags & 0x180) != 0 && (flags & 0x80) != 0 && asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.lightingValues)
			{
				write.dump_array(asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.lightingValues, asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.numLightingValues);
			}
		}

		write.dump_array(asset->dpvs.subdivVertexLighting, asset->dpvs.subdivVertexLightingInfoCount);

		write.dump_array(asset->dpvs.surfaceMaterials, asset->surfaceCount);
		write.dump_array(asset->dpvs.surfaceCastsSunShadow, asset->dpvs.surfaceVisDataCount);
		write.dump_array(asset->dpvs.surfaceCastsSunShadowOpt, asset->dpvs.sunShadowOptCount * asset->dpvs.sunSurfVisDataCount);

		write.dump_array(asset->dpvs.surfaceDeptAndSurf, asset->dpvs.staticSurfaceCount);

		write.dump_array(asset->dpvs.constantBuffersLit, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_raw(asset->dpvs.constantBuffersLit[i], 1);
		}
		write.dump_array(asset->dpvs.constantBuffersAmbient, asset->dpvs.smodelCount);
		for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
		{
			write.dump_raw(asset->dpvs.constantBuffersAmbient[i], 1);
		}

		write.dump_array(asset->dpvsDyn.dynEntCellBits[0],
			asset->dpvsDyn.dynEntClientWordCount[0] * asset->dpvsPlanes.cellCount);
		write.dump_array(asset->dpvsDyn.dynEntCellBits[1],
			asset->dpvsDyn.dynEntClientWordCount[1] * asset->dpvsPlanes.cellCount);

		write.dump_array(asset->dpvsDyn.dynEntVisData[0][0], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[0][1], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[0][2], asset->dpvsDyn.dynEntClientWordCount[0] * 32);

		write.dump_array(asset->dpvsDyn.dynEntVisData[1][0], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][1], asset->dpvsDyn.dynEntClientWordCount[1] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][2], asset->dpvsDyn.dynEntClientWordCount[1] * 32);

		write.dump_array(asset->dpvsDyn.dynEntVisData[0][3], asset->dpvsDyn.dynEntClientWordCount[0] * 32);
		write.dump_array(asset->dpvsDyn.dynEntVisData[1][3], asset->dpvsDyn.dynEntClientWordCount[1] * 32);

		write.dump_array(asset->heroOnlyLights, asset->heroOnlyLightCount);

		write.dump_array(asset->umbraTomeData, asset->umbraTomeSize);

		write.dump_array(asset->mdaoVolumes, asset->mdaoVolumesCount);
		for (unsigned int i = 0; i < asset->mdaoVolumesCount; i++)
		{
			write.dump_asset(asset->mdaoVolumes[i].volumeData);
		}

		write.dump_string(asset->buildInfo.bspCommandline);
		write.dump_string(asset->buildInfo.lightCommandline);
		write.dump_string(asset->buildInfo.bspTimestamp);
		write.dump_string(asset->buildInfo.lightTimestamp);

		write.close();
	}
}