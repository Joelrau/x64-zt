#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "material.hpp"

//#include "zonetool/h1/assets/material.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace material
		{
			namespace
			{
				std::string clean_name(const std::string& name)
				{
					auto new_name = name;

					for (auto i = 0u; i < name.size(); i++)
					{
						switch (new_name[i])
						{
						case '*':
							new_name[i] = '_';
							break;
						}
					}

					return new_name;
				}

				std::unordered_map<std::uint8_t, std::uint8_t> mapped_camera_regions =
				{
					/*
					{zonetool::iw7::CAMERA_REGION_LIT_OPAQUE, zonetool::h1::CAMERA_REGION_LIT_OPAQUE},
					{zonetool::iw7::CAMERA_REGION_LIT_DECAL, zonetool::h1::CAMERA_REGION_LIT_DECAL},
					{zonetool::iw7::CAMERA_REGION_LIT_TRANS, zonetool::h1::CAMERA_REGION_LIT_TRANS},
					{zonetool::iw7::CAMERA_REGION_EMISSIVE, zonetool::h1::CAMERA_REGION_EMISSIVE},
					{zonetool::iw7::CAMERA_REGION_DEPTH_HACK, zonetool::h1::CAMERA_REGION_DEPTH_HACK},
					{zonetool::iw7::CAMERA_REGION_LIGHT_MAP_OPAQUE, zonetool::h1::CAMERA_REGION_LIT_OPAQUE},
					{zonetool::iw7::CAMERA_REGION_HUD_OUTLINE, zonetool::h1::CAMERA_REGION_NONE},
					{zonetool::iw7::CAMERA_REGION_MOTIONBLUR, zonetool::h1::CAMERA_REGION_NONE},
					{zonetool::iw7::CAMERA_REGION_NONE, zonetool::h1::CAMERA_REGION_NONE},
					*/
				};

				std::unordered_map<std::uint8_t, std::uint8_t> mapped_sort_keys =
				{
				};

				std::unordered_map<std::uint8_t, std::uint8_t> mapped_material_types =
				{
				};

				std::uint8_t convert_sortkey(const std::uint8_t sort_key)
				{
					if (mapped_sort_keys.find(sort_key) != mapped_sort_keys.end())
					{
						return mapped_sort_keys[sort_key];
					}

					return sort_key;
				}

				std::uint8_t convert_camera_region(const std::uint8_t camera_region)
				{
					if (mapped_camera_regions.find(camera_region) != mapped_camera_regions.end())
					{
						return mapped_camera_regions[camera_region];
					}

					return camera_region;
				}

				std::uint8_t convert_material_type(const std::uint8_t material_type)
				{
					if (mapped_material_types.find(material_type) != mapped_material_types.end())
					{
						return mapped_material_types[material_type];
					}

					return material_type;
				}
			}

#define MATERIAL_DUMP_STRING(entry) \
	matdata[#entry] = std::string(asset->entry);

#define MATERIAL_DUMP(entry) \
	matdata[#entry] = asset->entry;

#define MATERIAL_DUMP_INFO(entry) \
	matdata[#entry] = asset->info.entry;

			void dump_json(zonetool::iw7::Material* asset)
			{
				auto c_name = clean_name(asset->name);

				const auto path = "materials\\"s + c_name + ".json"s;
				auto file = zonetool::filesystem::file(path);

				ordered_json matdata;

				MATERIAL_DUMP_STRING(name);

				std::string techset;
				// v51: IW7 sortKey 2 = opaque world; >= 7 = decal (7/10/15), translucent (24-30), emissive (36+).
				//      Opaque keeps the depth-writing remap; the rest go to Ghosts' alpha-blended, no-depth-write lit techset.
				// v60: alpha-tested lit materials (palm leaves, plants, fences) have no matching Ghosts atest techset, so draw them alpha-blended (colour map alpha)
				const std::string og_techset_name = (asset->techniqueSet && asset->techniqueSet->name) ? asset->techniqueSet->name : "";
				const bool alpha_tested = og_techset_name.find("_atest") != std::string::npos && og_techset_name.find("unlit") == std::string::npos;
				const bool blended = asset->info.sortKey >= 7 || alpha_tested;
				std::string h1_techset = blended ? "wc_l_sm_ndw_b0c0l0_cltrans_iw6" : "wc_l_sm_ndw_b0c0l0_nfwpf_iw6";
				if (asset->techniqueSet)
				{
					techset = asset->techniqueSet->name;

					matdata["techniqueSet->name"] = h1_techset;
					matdata["techniqueSet->og_name"] = techset;
				}

				matdata["gameFlags"] = asset->info.gameFlags; // convert
				matdata["sortKey"] = (alpha_tested && asset->info.sortKey < 7) ? 7 : convert_sortkey(asset->info.sortKey);
				matdata["renderFlags"] = 0; // idk

				matdata["textureAtlasRowCount"] = asset->info.textureAtlasRowCount;
				matdata["textureAtlasColumnCount"] = asset->info.textureAtlasColumnCount;
				matdata["textureAtlasFrameBlend"] = 0;
				matdata["textureAtlasAsArray"] = 0;

				matdata["surfaceTypeBits"] = asset->info.surfaceTypeBits; // convert
				// hashIndex;

				matdata["stateFlags"] = asset->stateFlags; // same

				matdata["cameraRegion"] = blended ? 3 : convert_camera_region(asset->cameraRegion); // v51: 3 = lit trans (as Ghosts cltrans materials)
				matdata["materialType"] = convert_material_type(asset->materialType);
				matdata["assetFlags"] = zonetool::h1::MTL_ASSETFLAG_NONE;

				ordered_json constant_table = json::array();
				for (int i = 0; i < 0 && techset != "2d"; i++)
				{
					ordered_json table;
					std::string constant_name = asset->constantTable[i].name;
					//const auto constant_hash = asset->constantTable[i].nameHash;

					if (constant_name.size() > 12)
					{
						constant_name.resize(12);
					}

					table["name"] = constant_name.data();
					table["nameHash"] = asset->constantTable[i].nameHash;

					nlohmann::json literal_entry;

					literal_entry[0] = asset->constantTable[i].literal[0];
					literal_entry[1] = asset->constantTable[i].literal[1];
					literal_entry[2] = asset->constantTable[i].literal[2];
					literal_entry[3] = asset->constantTable[i].literal[3];

					table["literal"] = literal_entry;

					constant_table[constant_table.size()] = table;
				}

#define CONSTANT_TABLE_ADD_IF_NOT_FOUND(CONST_NAME, CONST_HASH, LITERAL_1, LITERAL_2, LITERAL_3, LITERAL_4) \
				bool has_const = false; \
				std::size_t insert_position = constant_table.size(); \
				for (std::size_t i = 0; i < constant_table.size(); i++) \
				{ \
					if (constant_table[i]["nameHash"].get<std::size_t>() == CONST_HASH) \
					{ \
						has_const = true; \
						break; \
					} \
					if (constant_table[i]["nameHash"].get<std::size_t>() > CONST_HASH) \
					{ \
						insert_position = i; \
						break; \
					} \
				} \
				if (!has_const) \
				{ \
					ordered_json table; \
					table["name"] = CONST_NAME; \
					table["nameHash"] = CONST_HASH; \
					nlohmann::json literal_entry; \
					literal_entry[0] = LITERAL_1; \
					literal_entry[1] = LITERAL_2; \
					literal_entry[2] = LITERAL_3; \
					literal_entry[3] = LITERAL_4; \
					table["literal"] = literal_entry; \
					constant_table.insert(constant_table.begin() + insert_position, table); \
				}

				if (blended)
				{
					// v51: values from Ghosts' wc/dig_web_01 (cltrans); without colorTint the blend would come out black/invisible
					ordered_json c1; c1["name"] = "colorTint"; c1["nameHash"] = 3054254906u; c1["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
					ordered_json c2; c2["name"] = "bloomParams"; c2["nameHash"] = 3733414975u; c2["literal"] = { 1.0f, 0.0f, 0.0f, 0.0f };
					constant_table.push_back(c1); constant_table.push_back(c2);
				}
				matdata["constantTable"] = constant_table;

				ordered_json material_images = json::array();
				{
					// IW7 -> IW6 techset remap: keep a single color map
					std::string col_name = "blank"; // v35: "default" does not exist as an image (rendered solid red)
					int best = -100; // v36: materials that only have packed/spec images must still pick something (was -1 -> "default")
					for (auto i = 0; i < asset->textureCount; i++)
					{
						if (!asset->textureTable[i].image || !asset->textureTable[i].image->name) continue;
						const std::string n = asset->textureTable[i].image->name;
						int score = 0;
						if (n.size() > 2 && n.compare(n.size() - 2, 2, "_c") == 0) score = 4;
						if (n.size() > 4 && n.compare(n.size() - 4, 4, "_col") == 0) score = 4;
						if (n.size() > 3 && n.compare(n.size() - 3, 3, "_bc") == 0) score = 4;
						if (n.find("_nml") != std::string::npos || n.find("_n") == n.size() - 2) score = -6; // v38: normal maps are never colour
						if (n.find("_spc") != std::string::npos || n.find("_cos") != std::string::npos || n.find("_ao") != std::string::npos) score = -1;
						if (n.find("_packed_cs") != std::string::npos) score = -2; // packed colour+spec: best fallback
						else if (n.find("_packed") != std::string::npos || n.find("_ng") != std::string::npos) score = -5;
						if (score == 0 && asset->textureTable[i].semantic == 2) score = 2;
						if (score == 0) score = 1;
						if (score > best) { best = score; col_name = n; }
					}
					// v40: images that exist in no loaded zone render solid red
					static const char* const missing_imgs[] = { "default", "$white", "black", "$gray", "transparent_invisible", "glass_clear_01_c", "koth_grid_03_c", "koth_grid_01_c", "falloff_linear", "vfx_energy_dome_shield_ping", "vfx_energy_beam_nrm" };
					for (const auto* mi : missing_imgs) if (col_name == mi) col_name = "blank";
					// v45: near-black floor/ground materials render as pitch black with placeholder lighting; use the grey blank so floors are visible
					// v51: v45 forced-blank for floor/black materials removed (floors were a depth-write issue, not textures)
					if (blended)
					{
						ordered_json nimg; // v51: cltrans materials in Ghosts carry the identity normal map first (sorted by typeHash)
						nimg["image"] = "$identitynormalmap"; nimg["semantic"] = 5; nimg["samplerState"] = 1;
						nimg["lastCharacter"] = 112; nimg["firstCharacter"] = 110; nimg["typeHash"] = 1507003663u;
						material_images.push_back(nimg);
					}
					ordered_json image;
					image["image"] = col_name;
					image["semantic"] = 2;
					image["samplerState"] = 19;
					image["lastCharacter"] = 112;
					image["firstCharacter"] = 99;
					image["typeHash"] = 2695565377u;
					material_images.push_back(image);
				}
				for (auto i = 0; i < 0; i++)
				{
					ordered_json image;
					if (asset->textureTable[i].image && asset->textureTable[i].image->name)
					{
						image["image"] = asset->textureTable[i].image->name;
					}
					else
					{
						ZONETOOL_WARNING("material %s has an empty image, assigning default...", asset->name);
						image["image"] = "default";
					}

					image["semantic"] = asset->textureTable[i].semantic;
					image["samplerState"] = asset->textureTable[i].samplerState;
					image["lastCharacter"] = asset->textureTable[i].nameEnd;
					image["firstCharacter"] = asset->textureTable[i].nameStart;
					image["typeHash"] = asset->textureTable[i].nameHash;

					// add image data to material
					material_images.push_back(image);
				}

#define IMAGE_ADD_IF_NOT_FOUND(IMAGE, SEMANTIC, SAMPLER_STATE, LAST_CHARACTER, FIRST_CHARACTER, HASH) \
				bool has_image = false; \
				std::size_t insert_position = material_images.size(); \
				for (std::size_t i = 0; i < material_images.size(); i++) \
				{ \
					if (material_images[i]["typeHash"].get<std::size_t>() == HASH) \
					{ \
						has_image = true; \
						break; \
					} \
					if (material_images[i]["typeHash"].get<std::size_t>() > HASH) \
					{ \
						insert_position = i; \
						break; \
					} \
				} \
				if (!has_image) \
				{ \
					ordered_json image; \
					image["image"] = IMAGE; \
					image["semantic"] = SEMANTIC; \
					image["samplerState"] = SAMPLER_STATE; \
					image["lastCharacter"] = LAST_CHARACTER; \
					image["firstCharacter"] = FIRST_CHARACTER; \
					image["typeHash"] = HASH; \
					material_images.insert(material_images.begin() + insert_position, image); \
				}

				// v54: IW7 sky dome = cubemap on a 2D world techset (black). Use the Ghosts sky techset exactly like wc/sky_mp_dig
				{
					const std::string mn = asset->name ? asset->name : "";
					if (mn.find("/sky_") != std::string::npos || mn.rfind("sky_", 0) == 0)
					{
						std::string sky_img = "hdr_sunset13";
						for (auto i = 0; i < asset->textureCount; i++)
						{
							if (asset->textureTable[i].image && asset->textureTable[i].image->name) { sky_img = asset->textureTable[i].image->name; break; }
						}
						matdata["techniqueSet->name"] = "wc_sky_bl_iw6";
						matdata["gameFlags"] = 12;
						matdata["sortKey"] = 3;
						matdata["cameraRegion"] = 0;
						matdata["materialType"] = 36;
						ordered_json sc; sc["name"] = "bloomParams"; sc["nameHash"] = 3733414975u; sc["literal"] = { 1.02f, 0.0f, 0.0f, 0.0f };
						matdata["constantTable"] = ordered_json::array({ sc });
						ordered_json si; si["image"] = sky_img; si["semantic"] = 2; si["samplerState"] = 243;
						si["lastCharacter"] = 112; si["firstCharacter"] = 99; si["typeHash"] = 2695565377u;
						material_images = ordered_json::array({ si });
					}
				}

				// v56: additive glow world materials (IW7 *_add_* techsets) -> Ghosts additive effect techset (additive blend: dark texels add nothing)
				if (techset.find("_add_") != std::string::npos && techset.find("sky") == std::string::npos && asset->name && std::string(asset->name).find("sky") == std::string::npos && !material_images.empty())
				{
					const auto glow_img = material_images.back()["image"].get<std::string>();
					matdata["techniqueSet->name"] = "effect_add_lin_ct_nofog_ndw_bl_blmod_iw6";
					matdata["gameFlags"] = 8;
					matdata["sortKey"] = 53;
					matdata["cameraRegion"] = 4;
					matdata["materialType"] = 0;
					ordered_json a1; a1["name"] = "textureAtlas"; a1["nameHash"] = 1128936273u; a1["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
					ordered_json a2; a2["name"] = "colorTint"; a2["nameHash"] = 3054254906u; a2["literal"] = { 2.0f, 2.0f, 2.0f, 1.0f };
					ordered_json a3; a3["name"] = "bloomParams"; a3["nameHash"] = 3733414975u; a3["literal"] = { 1.0f, 0.0f, 0.0f, 0.0f };
					matdata["constantTable"] = ordered_json::array({ a1, a2, a3 });
					ordered_json gi; gi["image"] = glow_img; gi["semantic"] = 2; gi["samplerState"] = 243;
					gi["lastCharacter"] = 112; gi["firstCharacter"] = 99; gi["typeHash"] = 2695565377u;
					material_images = ordered_json::array({ gi });
				}

				matdata["textureTable"] = material_images;

				auto str = matdata.dump(4, ' ', true, nlohmann::detail::error_handler_t::replace);

				matdata.clear();

				file.open("wb");
				file.write(str);
				file.close();
			}

			void dump(zonetool::iw7::Material* asset)
			{
				dump_json(asset);
			}
		}
	}
}
