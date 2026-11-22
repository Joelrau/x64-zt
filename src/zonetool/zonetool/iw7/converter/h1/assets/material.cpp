#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "material.hpp"
#include <utils/string.hpp>

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
				const std::string v72_name = asset->name ? asset->name : "";
				// v113: with reflection probes in (v101) draw facade glass as opaque reflective dark glass (IW7's mirror facades) instead
				//       of the v72 see-through pane, which showed the unlit interiors as a black void (building right of spawn)
				constexpr bool v113_mirror_glass = true;
				const bool facade_glass_raw = asset->info.sortKey < 7 && v72_name.find("glass") != std::string::npos && (v72_name.find("building") != std::string::npos || v72_name.find("tinted") != std::string::npos || v72_name.find("opaque_02") != std::string::npos); // v72: dark facade glass rendered pitch black; draw it see-through
				const bool facade_glass = facade_glass_raw && !v113_mirror_glass;
				// v113: IW7 world glass on the opaque "replace" techsets (glass_building_*, glass_opaque_*, solar panels: sortKey 7, colour "blank")
				//       is a mirror facade lit only by reflections; it went to the translucent techset and showed the void behind it
				const bool mirror_glass = v113_mirror_glass && v72_name.rfind("w/", 0) == 0 && v72_name.find("glass") != std::string::npos
					&& og_techset_name.find("replace") != std::string::npos;
				const bool blended = (asset->info.sortKey >= 7 || alpha_tested || facade_glass) && !mirror_glass;
				std::string h1_techset = blended ? "wc_l_sm_ndw_b0c0l0_cltrans_iw6" : "wc_l_sm_ndw_b0c0l0_nfwpf_iw6";
				if (asset->techniqueSet)
				{
					techset = asset->techniqueSet->name;

					matdata["techniqueSet->name"] = h1_techset;
					matdata["techniqueSet->og_name"] = techset;
				}

				// v102 shadows: IW7 gameFlags bits differ from H1/IW6 (opaque IW7 world = 0x92, Ghosts mp_dig opaque world = 0x52).
				//      H1 0x40 = MTL_GAMEFLAG_CASTS_SHADOW, 0x80 = CASTS_SHADOW_EFFECT. Keep the low bits, drop IW7's 0x40/0x80; 0x40 is
				//      set again below only where the H1 techset really has a build_shadowmap technique.
				matdata["gameFlags"] = asset->info.gameFlags; // v104: masking to 0x3F (v102) made the building facade glass vanish; only shadow-caster models are remapped below
				matdata["sortKey"] = ((alpha_tested || facade_glass) && asset->info.sortKey < 7) ? 7 : convert_sortkey(asset->info.sortKey);
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

				// v80: log IW7 material parameters (constant table + pixel-shader constant buffers) to recover tints/emissive colours
				{
					std::string line = ::utils::string::va("v80 mat %s tech=%s cc=%u cbc=%u", asset->name ? asset->name : "?", techset.data(), (unsigned)asset->constantCount, (unsigned)asset->constantBufferCount);
					for (int i = 0; i < asset->constantCount && asset->constantTable; i++)
						line += ::utils::string::va(" c[%.12s]=%g,%g,%g,%g", asset->constantTable[i].name, asset->constantTable[i].literal[0], asset->constantTable[i].literal[1], asset->constantTable[i].literal[2], asset->constantTable[i].literal[3]);
					for (int b = 0; b < asset->constantBufferCount && asset->constantBufferTable && b < 3; b++)
					{
						const auto& cb = asset->constantBufferTable[b];
						if (!cb.psData || cb.psDataSize < 4) continue;
						const auto* f = reinterpret_cast<const float*>(cb.psData);
						const unsigned n = std::min<unsigned>(cb.psDataSize / 4, 48);
						line += ::utils::string::va(" ps%d(%u):", b, cb.psDataSize);
						for (unsigned k = 0; k < n; k++) line += ::utils::string::va("%s%g", k ? "," : "", f[k]);
					}
					{ static FILE* vf = fopen("v80_materials.txt", "w"); if (vf) { fprintf(vf, "%s\n", line.data()); fflush(vf); } }
				}

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
					const std::string lname = asset->name ? asset->name : "";
					const bool is_glass = lname.find("glass") != std::string::npos || og_techset_name.find("blendadd") != std::string::npos || og_techset_name.find("glass") != std::string::npos;
					ordered_json c1; c1["name"] = "colorTint"; c1["nameHash"] = 3054254906u; // v67: glass was opaque dark; make it see-through
					bool emissive_img = false; // v70: lit-blend emissive/vista images were far too dark
					for (auto i = 0; i < asset->textureCount; i++) if (asset->textureTable[i].image && asset->textureTable[i].image->name) { const std::string in = asset->textureTable[i].image->name; if (in.find("skyline") != std::string::npos || in.find("light_line") != std::string::npos || in.find("_e_warm") != std::string::npos) emissive_img = true; }
					if (facade_glass) c1["literal"] = { 0.08f, 0.2f, 0.26f, 0.55f }; else if (is_glass) c1["literal"] = { 0.55f, 0.7f, 0.75f, 0.12f }; else if (emissive_img) c1["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f }; // v83: was 3 -> every ledge glowed neon yellow; IW7 emissivePara is 1 else c1["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
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
						// v76: anisotropy (_a), occlusion (_o), height (_h), gloss (_sg) and detail maps showed as solid blue/red/green
						{
							auto ends = [&](const char* suf) { const size_t l = strlen(suf); return n.size() > l && n.compare(n.size() - l, l, suf) == 0; };
							if (ends("_a") || ends("_a&") || ends("_o") || ends("_h") || ends("_sg") || ends("_mask") || n.find("detail_") != std::string::npos || n.find("_detail") != std::string::npos) score = -50;
						}
						if (asset->textureTable[i].nameHash == 2695565377u && score > -50) score += 10; // v76: the sampler literally named colorMap
						if (score > best) { best = score; col_name = n; }
					}
					if (best <= -50) // v76: no colour image at all -> neutral grey, or the bronze swatch for brass/gold metals
					{
						const std::string mn = asset->name ? asset->name : "";
						col_name = (mn.find("brass") != std::string::npos || mn.find("bronze") != std::string::npos || mn.find("gold") != std::string::npos) ? "metal_bare_bronze_01_c" : "blank";
					}
					// v94: IW7 metal colour maps (packed colour+spec) hold the metal's specular F0 colour; drawn as Ghosts diffuse
					//      under the sun, gold/brass trims glowed neon yellow. Use the map's own bronze swatch (warm, darker gold).
					{
						const std::string mn = asset->name ? asset->name : "";
						if ((mn.find("gold") != std::string::npos || mn.find("brass") != std::string::npos) && col_name.find("_packed_cs") != std::string::npos)
							col_name = (mn.size() > 1 && mn[0] == 'w' && (mn[1] == '/' || mn[1] == 'c')) ? "$black" : "metal_bare_bronze_01_c";
						// v95: world trims stayed bright orange even as bronze (in-game test with $black matches the IW7 reference:
						//      thin dark metal edges, no glowing lines along planters and the tower)
						// v102 aqua: same for copper. w/metal_bare_copper_01's packed_cs map averages a pale pink-cream (223,160,135),
						//      so the big curved pipe over the tower (surfaces 3888-3902 around (480,100,150-656)) drew white/cream
						//      instead of the dark reddish copper in IW7. Use the darker, warmer bronze swatch (160,110,60), which is
						//      loaded by w/metal_bare_bronze_01.
						if (mn.find("copper") != std::string::npos && col_name.find("_packed_cs") != std::string::npos)
							col_name = "iw7_copper_dark"; // v104: bronze still read as a glowing yellow tube under the sun; flat dark copper (iw7_port2.ps1)
					}
					// v40: images that exist in no loaded zone render solid red
					static const char* const missing_imgs[] = { "default", "$white", "black", "$gray", "transparent_invisible", "glass_clear_01_c", "koth_grid_03_c", "koth_grid_01_c", "falloff_linear", "vfx_energy_dome_shield_ping", "vfx_energy_beam_nrm" };
					for (const auto* mi : missing_imgs) if (col_name == mi) col_name = "blank";
					// v70: ambient-occlusion-only images ("ng_ambocc_black") are not colour maps; they made whole wall panels render black
					if (col_name.find("ambocc") != std::string::npos || facade_glass) col_name = "blank";
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
				// v76: unlit blended light cards/emblems (hdr_light, emissive decals) were lit by the model light grid -> drawn black; make them additive glows
				const bool unlit_glow = blended && techset.find("unlit") != std::string::npos && techset.find("_add_") == std::string::npos && (v72_name.find("light") != std::string::npos || v72_name.find("emissive") != std::string::npos || v72_name.find("glow") != std::string::npos);
				const bool distortion = techset.find("distortion") != std::string::npos; // v76: heat/water distortion draws its normal map as colour (purple ripples) -> invisible additive
				// v76: IW7 blendadd glass came out as milky opaque panes (colour alpha = 1); a faint additive sheen reads as clear glass
				const bool add_glass = techset.find("blendadd") != std::string::npos || facade_glass; // facade glass ignored colorTint and drew as cream panes
				// v88: world unlit blend overlays (screen_crt_dots_01 over the HAILE billboards) went to the lit blend techset; with no
				//      lightmap (index 31) they drew an opaque black layer over the posters. Additive with the global exposure = faint dots.
				const bool world_unlit_blend = asset->name && asset->name[0] == 'w' && asset->name[1] == '/' && techset.find("unlit_blend") != std::string::npos
					&& std::string(asset->name).find("crt_dots") != std::string::npos; // v92: only the CRT overlay; other unlit blend panels went black-void
				// v92: tool surfaces (caulk_shadow shadow casters, lightmap index 31) drew as solid black boxes next to the spawn.
				//      Additive with zero tint = invisible.
				const bool tool_surf = techset.find("_tools") != std::string::npos || (asset->name && std::string(asset->name).find("caulk") != std::string::npos);
				// v102 aqua: the tower aquarium (box at (160,39,309-559): marble walls, pebble floor, wc/atmos_caustics_03 over the walls)
				//      is fronted by wc/water_mp_riot_fountain_creek_b (IW7 colorTint 0,1,1 = cyan). As a lit cltrans blend with
				//      its murky grey packed map it drew a murky grey pane; IW7 shows a glowing teal case. Draw it additive teal.
				const bool aqua_water = v72_name.find("water_mp_riot_fountain_creek") != std::string::npos;
				const bool aqua_caustics = v72_name.find("atmos_caustics") != std::string::npos;
				// v102 fx: IW7 particle materials (eq_/ev_effect_* techsets, no w/ or m/ prefix) fell to the world lit techset (no
				//      particle vertex path -> invisible/garbage sprites). Send them to the Ghosts additive effect techset too; blended
				//      smoke then reads as a soft glow, which is acceptable for a first pass (needs a Ghosts effect blend techset).
				const auto v102_slash = c_name.find('/');
				const std::string v102_prefix = v102_slash == std::string::npos ? "" : c_name.substr(0, v102_slash);
				constexpr bool v109_fx_mat_on = true; // v112: back on (the v109 void test ran a stale binary)
				const bool v102_fx_mat = v109_fx_mat_on && techset.find("effect") != std::string::npos && techset.find("sky") == std::string::npos
					&& (v102_prefix.empty() || (v102_prefix[0] != 'w' && v102_prefix[0] != 'm'));
				const bool v102_fx_add = v102_fx_mat && (techset.find("_add") != std::string::npos || techset.find("add_") != std::string::npos);
				if (v102_fx_mat)
				{
					ZONETOOL_INFO("v102 fx: effect material %s (%s) -> effect_add_lin_ct_nofog_ndw_bl_blmod_iw6%s", c_name.data(), techset.data(), v102_fx_add ? "" : " (blend drawn additive)");
				}
				if ((v102_fx_mat || distortion || tool_surf || unlit_glow || add_glass || world_unlit_blend || aqua_water || techset.find("_add_") != std::string::npos) && techset.find("sky") == std::string::npos && asset->name && std::string(asset->name).find("sky") == std::string::npos && !material_images.empty())
				{
					const auto glow_img = material_images.back()["image"].get<std::string>();
					matdata["techniqueSet->name"] = "effect_add_lin_ct_nofog_ndw_bl_blmod_iw6";
					matdata["gameFlags"] = 8;
					matdata["sortKey"] = 53;
					matdata["cameraRegion"] = 4;
					matdata["materialType"] = 0;
					ordered_json a1; a1["name"] = "textureAtlas"; a1["nameHash"] = 1128936273u; a1["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
					ordered_json a2; a2["name"] = "colorTint"; a2["nameHash"] = 3054254906u; if (distortion || tool_surf) a2["literal"] = { 0.0f, 0.0f, 0.0f, 0.0f }; else if (unlit_glow) a2["literal"] = { 1.5f, 1.5f, 1.5f, 1.0f }; else if (add_glass) a2["literal"] = { 0.12f, 0.14f, 0.16f, 1.0f }; else if (v102_fx_mat) a2["literal"] = v102_fx_add ? ordered_json{ 1.0f, 1.0f, 1.0f, 1.0f } : ordered_json{ 0.6f, 0.6f, 0.6f, 1.0f }; else a2["literal"] = { 2.0f, 2.0f, 2.0f, 1.0f };
					// v80: keep the real IW7 colour of glows (e.g. light_line_lightblue_02_emissive colorTint 5,17,17 = teal LED strips; we drew them white/yellow)
					if (!distortion && !add_glass && !tool_surf && !v102_fx_mat) // v102 fx: particle colour comes from the effect, keep tint 1
					{
						for (int ci = 0; ci < asset->constantCount && asset->constantTable; ci++)
						{
							if (std::strncmp(asset->constantTable[ci].name, "colorTint", 9) != 0 || asset->constantTable[ci].name[9] != 0) continue;
							const float* l = asset->constantTable[ci].literal;
							const float mx = std::max({ l[0], l[1], l[2] });
							if (mx <= 0.0f) break;
							// v85: one global IW7->H1 emissive exposure instead of per-material normalising (light bands 0.75 stayed neon-bright,
							//      LED strips 17 got the same brightness as everything else). E=0.15, peak clamped to 3.
							constexpr float E = 0.15f;
							const float k = std::min(mx * E, 3.0f) / mx;
							a2["literal"] = { l[0] * k, l[1] * k, l[2] * k, 1.0f };
							// v102 aqua: the water's tint (0,1,1) is an albedo tint (max <= 1), not HDR emissive; E=0.15 would leave
							//      0.15 teal = invisible over the murky map. Use the real tint at 0.6 so the case reads teal and glowing.
							if (aqua_water && mx <= 1.0f)
								a2["literal"] = { l[0] * 0.6f, l[1] * 0.6f, l[2] * 0.6f, 1.0f };
							// v102 aqua: caustics (IW7 tint 50 -> clamped 3, white) always sit under the cyan fountain/aquarium water;
							//      in IW7 they read teal through it. Filter by the water tint (0,1,1), keeping some red for the highlights.
							if (aqua_caustics)
								a2["literal"] = { l[0] * k * 0.35f, l[1] * k, l[2] * k, 1.0f };
							break;
						}
					}
					ordered_json a3; a3["name"] = "bloomParams"; a3["nameHash"] = 3733414975u; a3["literal"] = { 1.0f, 0.0f, 0.0f, 0.0f };
					matdata["constantTable"] = ordered_json::array({ a1, a2, a3 });
					ordered_json gi; gi["image"] = glow_img; gi["semantic"] = 2; gi["samplerState"] = 243;
					gi["lastCharacter"] = 112; gi["firstCharacter"] = 99; gi["typeHash"] = 2695565377u;
					material_images = ordered_json::array({ gi });
				}

				// v77: xmodel materials (mo/, mopw/, mco/, mso/, m/ ...) must use Ghosts *model* techsets: world wc_* techsets have no
				//      static-model technique, so all 2332 static models (plants, vines, rails, statues, lights) were skipped by the renderer
				{
					const auto slash = c_name.find('/');
					const std::string prefix = slash == std::string::npos ? "" : c_name.substr(0, slash);
					const auto cur_ts = matdata["techniqueSet->name"].is_string() ? matdata["techniqueSet->name"].get<std::string>() : "";
					// v89: Ghosts' model unlit blend techset draws opaque in H1, so overlay layers (CRT scanlines, glass over screens and
					//      windows) covered what is behind them. Hide them: alpha-tested model techset + fully transparent image (iw7_clear,
					//      written by iw7_port2.ps1).
					const auto make_hidden = [&]()
					{
						matdata["techniqueSet->name"] = "mc_l_t0c0n0s0l0_nfwpf_iw6";
						matdata["sortKey"] = 2; matdata["cameraRegion"] = 0; matdata["materialType"] = 3; matdata["gameFlags"] = 0;
						ordered_json h1; h1["name"] = "envMapParms"; h1["nameHash"] = 1033475292u; h1["literal"] = { 0.0f, 1.0f, 2.0f, 2.0f };
						ordered_json h2; h2["name"] = "colorTint"; h2["nameHash"] = 3054254906u; h2["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
						ordered_json h3; h3["name"] = "reflectionRa"; h3["nameHash"] = 3344177073u; h3["literal"] = { 8096.0f, 0.0f, 0.0f, 0.0f };
						ordered_json h4; h4["name"] = "bloomParams"; h4["nameHash"] = 3733414975u; h4["literal"] = { 1.0f, 0.0f, 0.0f, 0.0f };
						matdata["constantTable"] = ordered_json::array({ h1, h2, h3, h4 });
						ordered_json t1; t1["image"] = "$black"; t1["semantic"] = 8; t1["samplerState"] = 19; t1["lastCharacter"] = 112; t1["firstCharacter"] = 115; t1["typeHash"] = 887934131u;
						ordered_json t2; t2["image"] = "$identitynormalmap"; t2["semantic"] = 5; t2["samplerState"] = 19; t2["lastCharacter"] = 112; t2["firstCharacter"] = 110; t2["typeHash"] = 1507003663u;
						ordered_json t3; t3["image"] = "iw7_clear"; t3["semantic"] = 2; t3["samplerState"] = 19; t3["lastCharacter"] = 112; t3["firstCharacter"] = 99; t3["typeHash"] = 2695565377u;
						material_images = ordered_json::array({ t1, t2, t3 });
					};
					// v84: world unlit_replace (signs, posters, emissive panels) went to the LIT opaque techset; with no lightmap (index 31) they drew black.
					//      Use Ghosts' world unlit replace techset and keep the IW7 tint (HDR values like 10 clamped to a displayable 1.4).
					if (!prefix.empty() && prefix[0] == 'w' && techset.find("unlit_replace") != std::string::npos)
					{
						std::string col = "blank";
						for (const auto& im : material_images) if (im["typeHash"].get<std::size_t>() == 2695565377u) col = im["image"].get<std::string>();
						float t[3] = { 1.0f, 1.0f, 1.0f };
						for (int ci = 0; ci < asset->constantCount && asset->constantTable; ci++)
							if (std::strncmp(asset->constantTable[ci].name, "colorTint", 9) == 0 && asset->constantTable[ci].name[9] == 0) { for (int c = 0; c < 3; c++) t[c] = asset->constantTable[ci].literal[c]; }
						const float mx = std::max({ t[0], t[1], t[2], 0.0001f });
						const float k = std::clamp(mx * 0.15f, 0.5f, 1.4f) / mx; // v85: same global emissive exposure (floor 0.5 for untinted signs)
						matdata["techniqueSet->name"] = "wc_unlit_replace_lin_ct_bl_nfwpf_iw6";
						matdata["sortKey"] = 2;
						matdata["cameraRegion"] = 0;
						matdata["materialType"] = 35;
						ordered_json c1; c1["name"] = "colorTint"; c1["nameHash"] = 3054254906u; c1["literal"] = { t[0] * k, t[1] * k, t[2] * k, 1.0f };
						matdata["constantTable"] = ordered_json::array({ c1 });
						ordered_json t1; t1["image"] = col; t1["semantic"] = 2; t1["samplerState"] = 19; t1["lastCharacter"] = 112; t1["firstCharacter"] = 99; t1["typeHash"] = 2695565377u;
						material_images = ordered_json::array({ t1 });
					}
					else
					// v78: glow cards on static models (lion emblems, light strips) -> Ghosts model unlit blend (mp_dig's lit candle)
					if (!prefix.empty() && prefix[0] == 'm' && cur_ts == "effect_add_lin_ct_nofog_ndw_bl_blmod_iw6" && (distortion || (add_glass && !facade_glass)))
					{
						make_hidden();
					}
					else if (!prefix.empty() && prefix[0] == 'm' && cur_ts == "effect_add_lin_ct_nofog_ndw_bl_blmod_iw6")
					{
						ordered_json tint = { 1.0f, 1.0f, 1.0f, 1.0f };
						for (const auto& c : matdata["constantTable"]) if (c["name"] == "colorTint") tint = c["literal"];
						matdata["techniqueSet->name"] = "mc_unlit_blend_lin_ct_ndw_bl_abl_cltrans_iw6";
						matdata["sortKey"] = 31;
						matdata["cameraRegion"] = 3;
						matdata["materialType"] = 3;
						matdata["gameFlags"] = 0;
						ordered_json c1; c1["name"] = "textureAtlas"; c1["nameHash"] = 1128936273u; c1["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
						ordered_json c2; c2["name"] = "colorTint"; c2["nameHash"] = 3054254906u; c2["literal"] = tint;
						ordered_json c3; c3["name"] = "bloomParams"; c3["nameHash"] = 3733414975u; c3["literal"] = { 1.4f, 0.0f, 0.0f, 0.0f };
						matdata["constantTable"] = ordered_json::array({ c1, c2, c3 });
						for (auto& im : material_images) im["samplerState"] = 19;
					}
					// v86: model screens (cinematic) and unlit blend overlays (CRT dots, LED cards) are unlit in IW7 -> Ghosts model unlit blend.
					//      As lit alpha-test they covered the HAILE billboard with an opaque dark layer.
					else if (!prefix.empty() && prefix[0] == 'm' && cur_ts.rfind("wc_l_sm_ndw_b0c0l0_", 0) == 0
						&& (techset.find("cinematic") != std::string::npos || techset.find("unlit_blend") != std::string::npos || techset.find("unlit_replace") != std::string::npos))
					{
						// v89: model unlit_replace signs (advertisement_screen_01's HAILE sign) were lit by the light grid -> dark panel
						const bool cine = techset.find("cinematic") != std::string::npos;
						const bool repl = techset.find("unlit_replace") != std::string::npos;
						const bool overlay = c_name.find("crt_dots") != std::string::npos; // CRT scanline overlay: alpha layer in Ghosts darkens the sign below
						float t[3] = { 1.0f, 1.0f, 1.0f };
						for (int ci = 0; ci < asset->constantCount && asset->constantTable; ci++)
							if (std::strncmp(asset->constantTable[ci].name, "colorTint", 9) == 0 && asset->constantTable[ci].name[9] == 0) { for (int c = 0; c < 3; c++) t[c] = asset->constantTable[ci].literal[c]; }
						const float mx = std::max({ t[0], t[1], t[2], 0.0001f });
						const float k = cine ? 1.2f / mx : repl ? std::clamp(mx * 0.15f, 0.5f, 1.4f) / mx : std::min(mx * 0.15f, 3.0f) / mx;
						std::string col = "blank";
						for (const auto& im : material_images) if (im["typeHash"].get<std::size_t>() == 2695565377u) col = im["image"].get<std::string>();
						matdata["techniqueSet->name"] = "mc_unlit_blend_lin_ct_ndw_bl_abl_cltrans_iw6";
						matdata["sortKey"] = (cine || repl) ? 2 : 31;
						matdata["cameraRegion"] = 3;
						matdata["materialType"] = 3;
						matdata["gameFlags"] = 0;
						ordered_json c1; c1["name"] = "textureAtlas"; c1["nameHash"] = 1128936273u; c1["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
						ordered_json c2; c2["name"] = "colorTint"; c2["nameHash"] = 3054254906u; c2["literal"] = { t[0] * k, t[1] * k, t[2] * k, 1.0f };
						ordered_json c3; c3["name"] = "bloomParams"; c3["nameHash"] = 3733414975u; c3["literal"] = { 1.0f, 0.0f, 0.0f, 0.0f };
						matdata["constantTable"] = ordered_json::array({ c1, c2, c3 });
						ordered_json t1; t1["image"] = col; t1["semantic"] = 2; t1["samplerState"] = 19; t1["lastCharacter"] = 112; t1["firstCharacter"] = 99; t1["typeHash"] = 2695565377u;
						material_images = ordered_json::array({ t1 });
						if (overlay) make_hidden();
					}
					else if (!prefix.empty() && prefix[0] == 'm' && cur_ts.rfind("wc_l_sm_ndw_b0c0l0_", 0) == 0)
					{
						std::string col = "blank", nml = "$identitynormalmap";
						for (const auto& im : material_images)
						{
							if (im["typeHash"].get<std::size_t>() == 2695565377u) col = im["image"].get<std::string>();
						}
						matdata["techniqueSet->name"] = blended ? "mc_l_t0c0n0s0l0_nfwpf_iw6" : "mc_l_sm_r0c0n0s0l0_nfwpf_iw6";
						// v102 shadows: mc_l_sm_r0c0n0s0l0_nfwpf_iw6 has zprepass_iw6 + build_shadowmap_model_iw6, so these static-model
						//      materials can cast sun shadows; H1 only puts them in the shadow map with MTL_GAMEFLAG_CASTS_SHADOW (0x40).
						matdata["sortKey"] = 2; // v104: restored (v102 dropped it by accident)
						constexpr bool v108_model_casts_shadow = false; // v108: suspected of hiding the building facade (black void right of spawn)
						if (v108_model_casts_shadow && !blended)
						{
							const unsigned int gf = (matdata["gameFlags"].get<unsigned int>() & 0x3F) | zonetool::h1::MTL_GAMEFLAG_CASTS_SHADOW;
							matdata["gameFlags"] = gf;
							static unsigned int casters = 0;
							if (++casters <= 32 || casters % 100 == 0)
								ZONETOOL_INFO("v102 shadows: shadow caster model material #%u %s gameFlags 0x%X -> 0x%X", casters, asset->name ? asset->name : "?", (unsigned)asset->info.gameFlags, gf);
						}
						matdata["cameraRegion"] = 0;
						matdata["materialType"] = 3;
						ordered_json c1; c1["name"] = "envMapParms"; c1["nameHash"] = 1033475292u; c1["literal"] = { 0.0f, 1.0f, 2.0f, 2.0f };
						ordered_json c2; c2["name"] = "colorTint"; c2["nameHash"] = 3054254906u; c2["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
						ordered_json c3; c3["name"] = "reflectionRa"; c3["nameHash"] = 3344177073u; c3["literal"] = { 8096.0f, 0.0f, 0.0f, 0.0f };
						ordered_json c4; c4["name"] = "bloomParams"; c4["nameHash"] = 3733414975u; c4["literal"] = { 1.0f, 0.0f, 0.0f, 0.0f };
						matdata["constantTable"] = ordered_json::array({ c1, c2, c3, c4 });
						ordered_json t1; t1["image"] = "$black"; t1["semantic"] = 8; t1["samplerState"] = 19; t1["lastCharacter"] = 112; t1["firstCharacter"] = 115; t1["typeHash"] = 887934131u;
						ordered_json t2; t2["image"] = nml; t2["semantic"] = 5; t2["samplerState"] = 19; t2["lastCharacter"] = 112; t2["firstCharacter"] = 110; t2["typeHash"] = 1507003663u;
						ordered_json t3; t3["image"] = col; t3["semantic"] = 2; t3["samplerState"] = 19; t3["lastCharacter"] = 112; t3["firstCharacter"] = 99; t3["typeHash"] = 2695565377u;
						material_images = ordered_json::array({ t1, t2, t3 });
					}
				}

				// v102 gloss: IW7 glossy world floors (sref0 / _ssr / glossy / marble) looked matte with the colour+lightmap-only techset.
				//      Use Ghosts' reflective opaque world techset exactly like mp_dig's wc/dig_floor_tile_01 (spec + normal + colour,
				//      reflection from the surface's reflection probe). Spec image iw7_spec_gloss (RGB = reflectance, A = gloss) is
				//      written by iw7_port2.ps1 as images\iw7_spec_gloss.dds. Set to false to go back to the matte v101 look.
				constexpr bool v102_gloss = true; // v103: r0c0n0s0l0 blew marble to white (no colour-alpha factor); v107: on again with techset 1 (b0c0n0s0l0, PS checked)
				constexpr bool v104_ghosts_flags = true; // v104: materialType 36 + gameFlags 82 exactly as mp_dig's wc/dig_floor_tile_01
				constexpr float v104_bloom = 0.0f;       // v104: bloom mask of the gloss materials (mp_dig uses 1.0; our world has 0)
				// v105: which reflective Ghosts world techset the gloss materials use.
				//   0 = wc_l_sm_r0c0n0s0l0_nfwpf_iw6 (mp_dig's floor; v102/v104). Disassembled (PS lmo_sm_l0d_l0sd_r0c0n0s0l0_nofog_vcw_blm_sm5):
				//       out = (col.rgb * vcol.rgb)^2 * light + spec^2 * fresnel * (probe + sun highlight). It does NOT multiply by alpha.
				//   1 = wc_l_sm_ndw_b0c0n0s0l0_nfwpf_iw6 (mp_dig's dig_signs). Same "b0" (blend) family as our working world techset
				//       wc_l_sm_ndw_b0c0l0_nfwpf_iw6, whose PS (lmo_sm_l0d_l0sd_b0c0l0_ndw_nofog_blm_sm5) outputs
				//       (col.rgb * vcol.rgb)^2 * (col.a * vcol.a) * light and discards col.a * vcol.a <= 1/255.
				//       IW7 colour maps are _packed_cs (alpha = smoothness), and our IW7 lightmap scale (S=26) was tuned with that
				//       alpha factor in place, so under techset 0 the same surfaces come out 1/col.a times brighter (white).
				//       Techset 1 keeps that factor and adds normal + spec + probe reflection. Its state files must be the same
				//       opaque / depth-write / cull-none patched pair iw7_port2.ps1 makes for wc_l_sm_ndw_b0c0l0_nfwpf_iw6 (they come
				//       from this very techset's dig_signs), copied to the top of techsets\state\wc_l_sm_ndw_b0c0n0s0l0_nfwpf_iw6\.
				constexpr int v105_gloss_techset = 1;
				if (v102_gloss && asset->name && asset->name[0] == 'w' && asset->name[1] == '/' && !blended
					&& matdata["techniqueSet->name"].is_string() && matdata["techniqueSet->name"].get<std::string>() == "wc_l_sm_ndw_b0c0l0_nfwpf_iw6")
				{
					const std::string gn = asset->name;
					const auto has = [&](const char* s) { return gn.find(s) != std::string::npos; };
					const bool reflective = techset.find("sref") != std::string::npos || has("_ssr") || has("glossy") || has("marble") || has("polished");
					const bool mirror = mirror_glass;
					const bool excluded = !mirror && (has("glass") || has("gold") || has("brass") || has("_no_ssr") || has("decal")); // v95 trims, glass handled elsewhere
					if ((reflective || mirror) && !excluded)
					{
						std::string col = "blank";
						for (const auto& im : material_images) if (im["typeHash"].get<std::size_t>() == 2695565377u) col = im["image"].get<std::string>();
						if (mirror && (col == "blank" || col.empty())) col = "iw7_glass_dark"; // v113: alpha 1 (the b0 PS discards col.a <= 1/255), dark so the probe reflection dominates
						const bool dull = has("dull");
						matdata["techniqueSet->name"] = v105_gloss_techset == 1 ? "wc_l_sm_ndw_b0c0n0s0l0_nfwpf_iw6" : "wc_l_sm_r0c0n0s0l0_nfwpf_iw6";
						matdata["sortKey"] = 2;
						matdata["cameraRegion"] = 0;
						// v104 gloss: v102 kept IW7's materialType (68) and gameFlags. H1's MaterialType enum ends at 36 (MTL_TYPE_COUNT = 37),
						//      so 68 is out of range; the wc_ techsets belong to MTL_TYPE_WORLD_VERTCOL (36), which mp_dig's
						//      wc/dig_floor_tile_01 uses with gameFlags 82 (0x52 = CASTS_SHADOW | MARK_HAS_REFLECTION | MARK_HAS_LIGHTMAP).
						//      This techset has build_shadowmap_depth, so 0x40 is right here.
						if (v104_ghosts_flags && v105_gloss_techset == 0) // v105: techset 1 keeps the world flags our b0c0l0 materials use
						{
							matdata["materialType"] = static_cast<int>(zonetool::h1::MTL_TYPE_WORLD_VERTCOL);
							matdata["gameFlags"] = 82;
						}
						// envMapParms = min / max / fresnel exponent (mp_dig dig_floor_tile_01: 0.2, 1.0, 2.5, 2.0); "dull" variants reflect less
						ordered_json c1; c1["name"] = "envMapParms"; c1["nameHash"] = 1033475292u; c1["literal"] = mirror ? ordered_json{ 0.6f, 1.0f, 2.5f, 2.0f } : dull ? ordered_json{ 0.1f, 0.6f, 2.5f, 2.0f } : ordered_json{ 0.25f, 1.0f, 2.5f, 2.0f };
						ordered_json c2; c2["name"] = "colorTint"; c2["nameHash"] = 3054254906u; c2["literal"] = { 1.0f, 1.0f, 1.0f, 1.0f };
						ordered_json c3; c3["name"] = "reflectionRa"; c3["nameHash"] = 3344177073u; c3["literal"] = { 8096.0f, 0.0f, 0.0f, 0.0f };
						// v104 gloss: bloomParams.x is written to the pixel alpha (= bloom mask: out.a = vertexColour.a * bloomParams.x, IW6_SHADERS.md 4/8).
						//      Our other opaque world materials have NO constants, so no constant buffer and a bloom mask of 0; at 1 these
						//      HDR-lit surfaces were the only bloom sources in the map. Match the rest of the world (0).
						ordered_json c4; c4["name"] = "bloomParams"; c4["nameHash"] = 3733414975u; c4["literal"] = { v104_bloom, 0.0f, 0.0f, 0.0f };
						matdata["constantTable"] = ordered_json::array({ c1, c2, c3, c4 });
						// sorted by typeHash like the Ghosts material: spec (887934131), normal (1507003663), colour (2695565377)
						// v106: checked against mp_dig's wc/dig_signs.json (techset 1) and wc/dig_floor_tile_01.json (techset 0): both use
						//      exactly these 4 constants in this order (the constant buffer offsets are per index) and these 3 textures.
						ordered_json t1; t1["image"] = "iw7_spec_gloss"; t1["semantic"] = 8; t1["samplerState"] = 19; t1["lastCharacter"] = 112; t1["firstCharacter"] = 115; t1["typeHash"] = 887934131u;
						ordered_json t2; t2["image"] = "$identitynormalmap"; t2["semantic"] = 5; t2["samplerState"] = 19; t2["lastCharacter"] = 112; t2["firstCharacter"] = 110; t2["typeHash"] = 1507003663u;
						ordered_json t3; t3["image"] = col; t3["semantic"] = 2; t3["samplerState"] = 19; t3["lastCharacter"] = 112; t3["firstCharacter"] = 99; t3["typeHash"] = 2695565377u;
						material_images = ordered_json::array({ t1, t2, t3 });
						ZONETOOL_INFO("v102 gloss: %s (%s)%s", asset->name, techset.data(), mirror ? " mirror glass" : dull ? " dull" : "");
					}
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
