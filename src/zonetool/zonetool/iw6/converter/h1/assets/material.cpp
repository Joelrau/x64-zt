#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "material.hpp"

#include "zonetool/h1/assets/material.hpp"

#include "techset.hpp"

#include <utils/string.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace material
		{
			std::unordered_map<std::uint8_t, std::uint8_t> mapped_sortkeys =
			{
				{0, 1},		// Opaque ambient
				{1, 2},		// Opaque
				{2, 3},		// Sky
				{3, 4},		// Skybox sun/moon
				{4, 5},		// Clouds  NOT SURE
				{5, 6},		// Horizon NOT SURE
				{6, 7},		// Decal bottom 1
				{7, 8},		// Decal bottom 2
				{8, 9},		// Decal bottom 3
				{9, 10},	// Decal static
				{10, 11},	// Decal mid 1
				{11, 12},	// Decal mid 2
				{12, 13},	// Decal mid 3
				{13, 14},	// Weapon Impact
				{14, 15},	// Decal top 1
				{15, 16},	// Decal top 2
				{16, 17},	// Decal top 3
				{17, 19},	// Shadow
				//{18, },	// ?
				//{19, },	// ?
				{20, 22},	// ?
				//{21, },	// ?
				//{22, },	// ?
				{23, 25},	// ?
				{24, 26},	// Window inside
				{25, 27},	// Window outside
				{26, 28},	// Cloud 1
				{27, 29},	// Cloud 2
				{28, 30},	// Cloud 3
				{29, 31},	// Blend 1
				{30, 32},	// Blend 2
				{31, 33},	// Blend 3
				//{32, },	// ?
				{33, 35},	// ?
				{34, 38},	// Cloud dust?
				//{35, 35},	// Opaque
				//{36, },	// ?
				//{37, },	// ?
				//{38, },	// ?
				//{39, },	// ?
				//{40, },	// ?
				//{41, },	// ?
				//{42, },	// ?
				{43, 48},	// Distortion
				{44, 49},	// Distortion heavy
				{45, 50},	// ?
				{46, 51},	// ?
				{47, 52},	// Effect 1
				{48, 53},	// Effect 2
				{49, 54},	// Effect 3
				{50, 55},	// ?
				{51, 56},	// ?
				//{52, 57},	// ?
				{53, 36},	// Viewmodel Effect
				{54, 60},	// 2D
			};

			std::uint8_t convert_sortkey(std::uint8_t sortkey, std::string matname)
			{
				if (mapped_sortkeys.contains(sortkey))
				{
					return mapped_sortkeys[sortkey];
				}

				ZONETOOL_FATAL("Could not find mapped sortkey: %d (material: %s)", sortkey, matname.data());

				//return sortkey;
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_camera_regions =
			{
				{zonetool::iw6::CAMERA_REGION_LIT_OPAQUE, zonetool::h1::CAMERA_REGION_LIT_OPAQUE},
				{zonetool::iw6::CAMERA_REGION_LIT_DECAL, zonetool::h1::CAMERA_REGION_LIT_DECAL},
				{zonetool::iw6::CAMERA_REGION_LIT_TRANS, zonetool::h1::CAMERA_REGION_LIT_TRANS},
				{zonetool::iw6::CAMERA_REGION_EMISSIVE, zonetool::h1::CAMERA_REGION_EMISSIVE},
				{zonetool::iw6::CAMERA_REGION_DEPTH_HACK, zonetool::h1::CAMERA_REGION_DEPTH_HACK},
				{zonetool::iw6::CAMERA_REGION_LIGHT_MAP_OPAQUE, zonetool::h1::CAMERA_REGION_LIT_OPAQUE},
				{zonetool::iw6::CAMERA_REGION_HUD_OUTLINE, 0},
				{zonetool::iw6::CAMERA_REGION_MOTIONBLUR, 0},
				{zonetool::iw6::CAMERA_REGION_NONE, zonetool::h1::CAMERA_REGION_NONE},
			};

			std::uint8_t convert_camera_region(std::uint8_t camera_region, std::string matname)
			{
				if (mapped_camera_regions.contains(camera_region))
				{
					return mapped_camera_regions[camera_region];
				}

				ZONETOOL_FATAL("Could not find mapped camera region: %d (material: %s)", camera_region, matname.data());

				//return camera_region;
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_material_type =
			{
				{zonetool::iw6::MTL_TYPE_DEFAULT, zonetool::h1::MTL_TYPE_DEFAULT},
				{zonetool::iw6::MTL_TYPE_MODEL, zonetool::h1::MTL_TYPE_MODEL},
				{zonetool::iw6::MTL_TYPE_MODEL_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_VERTCOL},
				{zonetool::iw6::MTL_TYPE_MODEL_VERTCOL_GREY, zonetool::h1::MTL_TYPE_MODEL_VERTCOL_GREY},
				//{zonetool::iw6::MTL_TYPE_MODEL_QUANTIZED, zonetool::h1::MTL_TYPE_MODEL_QUANTIZED},
				//{zonetool::iw6::MTL_TYPE_MODEL_QUANTIZED_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_QUANTIZED_VERTCOL},
				//{zonetool::iw6::MTL_TYPE_MODEL_QUANTIZED_VERTCOL_GREY, zonetool::h1::MTL_TYPE_MODEL_QUANTIZED_VERTCOL_GREY},
				{zonetool::iw6::MTL_TYPE_MODEL_VERTLIT, zonetool::h1::MTL_TYPE_MODEL_VERTLIT},
				{zonetool::iw6::MTL_TYPE_MODEL_VERTLIT_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_VERTLIT_VERTCOL},
				//{zonetool::iw6::MTL_TYPE_MODEL_VERTLIT_VERTCOL_GREY, zonetool::h1::MTL_TYPE_MODEL_VERTLIT_VERTCOL_GREY},
				//{zonetool::iw6::MTL_TYPE_MODEL_VERTLIT_QUANTIZED, zonetool::h1::MTL_TYPE_MODEL_VERTLIT_QUANTIZED},
				//{zonetool::iw6::MTL_TYPE_MODEL_VERTLIT_QUANTIZED_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_VERTLIT_QUANTIZED_VERTCOL},
				//{zonetool::iw6::MTL_TYPE_MODEL_VERTLIT_QUANTIZED_VERTCOL_GREY, zonetool::h1::MTL_TYPE_MODEL_VERTLIT_QUANTIZED_VERTCOL_GREY},
				{zonetool::iw6::MTL_TYPE_MODEL_LMAP, zonetool::h1::MTL_TYPE_MODEL_LMAP},
				{zonetool::iw6::MTL_TYPE_MODEL_LMAP_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_LMAP_VERTCOL},
				//{zonetool::iw6::MTL_TYPE_MODEL_LMAP_VERTCOL_GREY, zonetool::h1::MTL_TYPE_MODEL_LMAP_VERTCOL_GREY},
				//{zonetool::iw6::MTL_TYPE_MODEL_LMAP_QUANTIZED, zonetool::h1::MTL_TYPE_MODEL_LMAP_QUANTIZED},
				//{zonetool::iw6::MTL_TYPE_MODEL_LMAP_QUANTIZED_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_LMAP_QUANTIZED_VERTCOL},
				//{zonetool::iw6::MTL_TYPE_MODEL_LMAP_QUANTIZED_VERTCOL_GREY, zonetool::h1::MTL_TYPE_MODEL_LMAP_QUANTIZED_VERTCOL_GREY},
				{zonetool::iw6::MTL_TYPE_MODEL_SUBDIV, zonetool::h1::MTL_TYPE_MODEL_SUBDIV},
				{zonetool::iw6::MTL_TYPE_MODEL_SUBDIV_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_SUBDIV_VERTCOL},
				//{zonetool::iw6::MTL_TYPE_MODEL_SUBDIV_TENSION, zonetool::h1::MTL_TYPE_MODEL_SUBDIV_TENSION},
				{zonetool::iw6::MTL_TYPE_MODEL_SUBDIV_VERTLIT, zonetool::h1::MTL_TYPE_MODEL_SUBDIV_VERTLIT},
				//{zonetool::iw6::MTL_TYPE_MODEL_SUBDIV_VERTLIT_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_SUBDIV_VERTLIT_VERTCOL},
				//{zonetool::iw6::MTL_TYPE_MODEL_SUBDIV_LMAP, zonetool::h1::MTL_TYPE_MODEL_SUBDIV_LMAP},
				//{zonetool::iw6::MTL_TYPE_MODEL_SUBDIV_LMAP_VERTCOL, zonetool::h1::MTL_TYPE_MODEL_SUBDIV_LMAP_VERTCOL},
				{zonetool::iw6::MTL_TYPE_WORLD, zonetool::h1::MTL_TYPE_WORLD},
				{zonetool::iw6::MTL_TYPE_WORLD_VERTCOL, zonetool::h1::MTL_TYPE_WORLD_VERTCOL},
			};

			std::uint8_t convert_material_type(std::uint8_t material_type, std::string matname)
			{
				if (mapped_material_type.contains(material_type))
				{
					return mapped_material_type[material_type];
				}

				ZONETOOL_FATAL("Could not find mapped material type: %d (material: %s)", material_type, matname.data());

				//return material_type;
			}

			zonetool::h1::Material* convert(Material* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::Material>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->info.gameFlags = asset->info.gameFlags; // convert?
				new_asset->info.sortKey = convert_sortkey(asset->info.sortKey, asset->name);
				new_asset->info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
				new_asset->info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
				new_asset->info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
				new_asset->info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
				new_asset->info.renderFlags = asset->info.renderFlags; // convert?
				COPY_VALUE_CAST(info.drawSurf); // todo?
				new_asset->info.surfaceTypeBits = zonetool::h1::SURFTYPE_BITS_DEFAULT; // todo
				new_asset->info.hashIndex = asset->info.hashIndex;

				std::memset(new_asset->stateBitsEntry, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
				for (auto i = 0; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
				{
					if (asset->stateBitsEntry[i] != 0xFF)
					{
						if (converter::h1::techset::technique_index_map.contains(i))
						{
							const auto new_index = converter::h1::techset::technique_index_map.at(i);
							new_asset->stateBitsEntry[new_index] = asset->stateBitsEntry[i];
						}
					}
				}

				new_asset->textureCount = asset->textureCount;
				new_asset->constantCount = asset->constantCount;
				new_asset->stateBitsCount = asset->stateBitsCount;
				new_asset->stateFlags = asset->stateFlags;
				if (game::get_mode() == game::game_mode::iw6)
				{
					if ((asset->stateFlags & 0x2) != 0) // stateflag 0x2 is different for iw4, iw5 and iw6. it's not cull front
					{
						// fix stateflags, we could check if the material has front cull from loadbits but thats too much work, lets just use "none" if front is used...
						new_asset->stateFlags = asset->stateFlags & ~0x2;

					}
				}
				new_asset->cameraRegion = convert_camera_region(asset->cameraRegion, asset->name);
				new_asset->materialType = convert_material_type(asset->materialType, asset->name);
				new_asset->layerCount = asset->layerCount;
				new_asset->assetFlags = asset->assetFlags; // convert?

				new_asset->techniqueSet = allocator.allocate<zonetool::h1::MaterialTechniqueSet>();
				new_asset->techniqueSet->name = allocator.duplicate_string(game::add_source_postfix(asset->techniqueSet->name, game::iw6));
				REINTERPRET_CAST_SAFE(textureTable);
				//REINTERPRET_CAST_SAFE(constantTable);
				new_asset->constantTable = allocator.allocate_array<zonetool::h1::MaterialConstantDef>(new_asset->constantCount);
				for (auto i = 0; i < new_asset->constantCount; i++)
				{
					memcpy(&new_asset->constantTable[i], &asset->constantTable[i], sizeof(zonetool::h1::MaterialConstantDef));
					//auto* constant = &new_asset->constantTable[i];
					//if (constant->nameHash == 3733414975) // bloomParams
					//{
					//	constant->literal[0] = 0.0f;
					//}
				}
				
				// statemap
				//new_asset->stateBitsTable = allocator.allocate_array<zonetool::h1::GfxStateBits>(asset->stateBitsCount); // don't convert, dump from old
				new_asset->stateBitsTable = (zonetool::h1::GfxStateBits*)asset->stateMap;
				new_asset->stateBitsCount = asset->stateBitsCount;

				std::memset(new_asset->constantBufferIndex, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
				for (auto i = 0; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
				{
					if (asset->constantBufferIndex[i] != 0xFF)
					{
						if (converter::h1::techset::technique_index_map.contains(i))
						{
							const auto new_index = converter::h1::techset::technique_index_map.at(i);
							new_asset->constantBufferIndex[new_index] = asset->constantBufferIndex[i];
						}
					}
				}

				const auto merge_techs = [&](std::vector<MaterialTechniqueType> techs, zonetool::h1::MaterialTechniqueType dest_tech)
				{
					for (auto idx = 0; idx < 4; idx++)
					{
						const auto increase_type = [&]()
						{
							for (auto& s : techs)
							{
								s = zonetool::iw6::MaterialTechniqueType(s + 103);
							}
							dest_tech = zonetool::h1::MaterialTechniqueType(dest_tech + 60);
						};

						if (new_asset->stateBitsEntry[dest_tech] == 0xFF)
						{
							for (const auto& tech : techs)
							{
								if (asset->stateBitsEntry[tech] != 0xFF)
								{
									new_asset->stateBitsEntry[dest_tech] = asset->stateBitsEntry[tech];
									break;
								}
							}
						}

						if (new_asset->constantBufferIndex[dest_tech] == 0xFF)
						{
							for (const auto& tech : techs)
							{
								if (asset->constantBufferIndex[tech] != 0xFF)
								{
									new_asset->constantBufferIndex[dest_tech] = asset->constantBufferIndex[tech];
									break;
								}
							}
						}

						increase_type();
					}
				};

				// nofog
				{
					merge_techs(
						{
							TECHNIQUE_LIT_DIR_SPOT,
							TECHNIQUE_LIT_DIR_SPOT_SHADOW,
							TECHNIQUE_LIT_DIR_OMNI,
							TECHNIQUE_LIT_DIR_OMNI_SHADOW,
							TECHNIQUE_LIT_DIR_SHADOW_SPOT,
							TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW,
							TECHNIQUE_LIT_DIR_SHADOW_OMNI,
							TECHNIQUE_LIT_DIR_SHADOW_OMNI_SHADOW,
						},
						zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING);

					merge_techs(
						{
							TECHNIQUE_LIT_DIR_SPOT_SHADOW_CUCOLORIS,
							TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS,
						},
						zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS);

					merge_techs(
						{
							TECHNIQUE_LIT_SPOT_SPOT,
							TECHNIQUE_LIT_SPOT_SPOT_SHADOW,
							TECHNIQUE_LIT_SPOT_OMNI,
							TECHNIQUE_LIT_SPOT_OMNI_SHADOW,
							TECHNIQUE_LIT_SPOT_SHADOW_SPOT,
							TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW,
							TECHNIQUE_LIT_SPOT_SHADOW_OMNI,
							TECHNIQUE_LIT_SPOT_SHADOW_OMNI_SHADOW,
							TECHNIQUE_LIT_OMNI_OMNI,
							TECHNIQUE_LIT_OMNI_OMNI_SHADOW,
							TECHNIQUE_LIT_OMNI_SHADOW_OMNI,
							TECHNIQUE_LIT_OMNI_SHADOW_OMNI_SHADOW,
						},
						zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING);

					merge_techs(
						{
							TECHNIQUE_LIT_SPOT_SPOT_SHADOW_CUCOLORIS,
							TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW,
						},
						zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS);
				}

				// fog
				{
					merge_techs(
						{
							TECHNIQUE_LIT_DIR_SPOT_DFOG,
							TECHNIQUE_LIT_DIR_SPOT_SHADOW_DFOG,
							TECHNIQUE_LIT_DIR_OMNI_DFOG,
							TECHNIQUE_LIT_DIR_OMNI_SHADOW_DFOG,
							TECHNIQUE_LIT_DIR_SHADOW_SPOT_DFOG,
							TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG,
							TECHNIQUE_LIT_DIR_SHADOW_OMNI_DFOG,
							TECHNIQUE_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG,
						},
						zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_DFOG);

					merge_techs(
						{
							TECHNIQUE_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG,
							TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG,
						},
						zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG);

					merge_techs(
						{
							TECHNIQUE_LIT_SPOT_SPOT_DFOG,
							TECHNIQUE_LIT_SPOT_SPOT_SHADOW_DFOG,
							TECHNIQUE_LIT_SPOT_OMNI_DFOG,
							TECHNIQUE_LIT_SPOT_OMNI_SHADOW_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_SPOT_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_OMNI_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG,
							TECHNIQUE_LIT_OMNI_OMNI_DFOG,
							TECHNIQUE_LIT_OMNI_OMNI_SHADOW_DFOG,
							TECHNIQUE_LIT_OMNI_SHADOW_OMNI_DFOG,
							TECHNIQUE_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG,
						},
						zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_DFOG);

					merge_techs(
						{
							TECHNIQUE_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG,
							TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG,
						},
						zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG);
				}

				REINTERPRET_CAST_SAFE(constantBufferTable);
				new_asset->constantBufferCount = asset->constantBufferCount;

				REINTERPRET_CAST_SAFE(subMaterials);

				return new_asset;
			}

			void dump(Material* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::material::dump(converted_asset);
			}
		}
	}
}