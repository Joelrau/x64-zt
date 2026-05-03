#include <std_include.hpp>

#include "zonetool/h1/converter/iw7/include.hpp"
#include "material.hpp"
#include "zonetool/iw7/assets/material.hpp"

#include "zonetool/h1/converter/iw7/assets/techset.hpp"

namespace zonetool::h1
{
	namespace converter::iw7
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

				enum MaterialType_ : std::uint8_t
				{
					MTL_TYPE_DEFAULT = 0x0, // ""
					MTL_TYPE_MODEL = 1, // "m"
					MLT_TYPE_MODEL_UNK2 = 2,
					MTL_TYPE_MODEL_UNK3 = 3,
					MTL_TYPE_MODEL_UNK4 = 4,
					MTL_TYPE_MODEL_UNK5 = 5,
					MTL_TYPE_MODEL_UNK6 = 6,
					MTL_TYPE_MODEL_UNK7 = 7,
					MTL_TYPE_MODEL_UNK8 = 8,
					MTL_TYPE_MODEL_UNK9 = 9,
					MTL_TYPE_MODEL_UNK10 = 10,
					MTL_TYPE_MODEL_UNK11 = 11,
					MTL_TYPE_MODEL_UNK12 = 12,
					MTL_TYPE_MODEL_UNK13 = 13,
					MTL_TYPE_MODEL_UNK14 = 14,
					MTL_TYPE_MODEL_UNK15 = 15,
					MTL_TYPE_MODEL_LMAP = 16, // "ml"
					MTL_TYPE_MODEL_LMAP_VERTCOL = 17, // "mlc"
					MTL_TYPE_MODEL_UNK18 = 18,
					MTL_TYPE_MODEL_UNK19 = 19,
					MTL_TYPE_MODEL_UNK20 = 20,
					MTL_TYPE_MODEL_UNK21 = 21,
					MTL_TYPE_MODEL_IMPACT = 22, // "mim"
					MTL_TYPE_MODEL_SELFVIS = 23, // "mo"
					MTL_TYPE_MODEL_VERTCOL_SELFVIS = 24, // "mco"
					MTL_TYPE_MODEL_VERTLIT_SELFVIS = 25, // "mvo"
					MTL_TYPE_MODEL_VERTLIT_VERTCOL_SELFVIS = 26, // "mvco"
					MTL_TYPE_MODEL_UNK27 = 27,
					MTL_TYPE_MODEL_SECONDUV_SELFVIS = 28, // "m2o"
					MTL_TYPE_MODEL_SECONDUV_VERTCOL_SELFVIS = 29, // "m2co"
					MTL_TYPE_MODEL_UNK30 = 30, // "mop"
					MTL_TYPE_MODEL_UNK31 = 31, // "m2op"
					MTL_TYPE_MODEL_UNK32 = 32,
					MTL_TYPE_MODEL_UNK33 = 33,
					MTL_TYPE_MODEL_UNK34 = 34,
					MTL_TYPE_MODEL_UNK35 = 35,
					MTL_TYPE_MODEL_UNK36 = 36,
					MTL_TYPE_MODEL_UNK37 = 37, // "mopw"
					MTL_TYPE_MODEL_UNK38 = 38, // "m2opw"
					MTL_TYPE_MODEL_UNK39 = 39, // "m2copw"
					MTL_TYPE_MODEL_UNK40 = 40, // "mcopw"
					MTL_TYPE_MODEL_UNK41 = 41, // "mvopw"
					MTL_TYPE_MODEL_UNK42 = 42,
					MTL_TYPE_MODEL_UNK43 = 43,
					MTL_TYPE_MODEL_SUBDIV = 44, // "ms"
					MTL_TYPE_MODEL_UNK45 = 45,
					MTL_TYPE_MODEL_UNK46 = 46,
					MTL_TYPE_MODEL_UNK47 = 47, // "msa"
					MTL_TYPE_MODEL_UNK48 = 48,
					MTL_TYPE_MODEL_UNK49 = 49,
					MTL_TYPE_MODEL_UNK50 = 50,
					MTL_TYPE_MODEL_UNK51 = 51,
					MTL_TYPE_MODEL_UNK52 = 52,
					MTL_TYPE_MODEL_SUBDIV_VERTCOL_SELFVIS = 53, // "msco"
					MTL_TYPE_MODEL_UNK54 = 54, // "mszo"
					MTL_TYPE_MODEL_UNK55 = 55, // "msvo"
					MTL_TYPE_MODEL_UNK56 = 56, // "msvco"
					MTL_TYPE_MODEL_SUBDIV_SELFVIS = 57, // "mso"
					MTL_TYPE_MODEL_UNK58 = 58, // "msop"
					MTL_TYPE_MODEL_UNK59 = 59,
					MTL_TYPE_MODEL_UNK60 = 60,
					MTL_TYPE_MODEL_UNK61 = 61,
					MTL_TYPE_MODEL_UNK62 = 62,
					MTL_TYPE_MODEL_UNK63 = 63, // "msopw"
					MTL_TYPE_MODEL_UNK64 = 64, // "mscopw"
					MTL_TYPE_MODEL_UNK65 = 65,
					MTL_TYPE_MODEL_UNK66 = 66,
					MTL_TYPE_MODEL_UNK67 = 67,
					MTL_TYPE_WORLD = 68, // "w"
					MTL_TYPE_WORLD_VERTCOL = 69, // "wc"
					MTL_TYPE_WORLD_IMPACT = 70, // "wim"
					MTL_TYPE_EFFECT_LMAP = 71, // "el"
					MTL_TYPE_EFFECT_VERTLIT = 72, // "ev"
					MTL_TYPE_EFFECT_QUAD = 73, // "eq"
				};

				enum techset_map_type_e
				{
					t_regular,
					t_color_tint,
					t_count,
				};

				enum techset_map_type_effect_e
				{
					te_regular,
					te_vertlit,
					te_count,
				};

				struct techset_map_s
				{
					std::string techset[techset_map_type_e::t_count];
				};

				struct techset_map_effect_s
				{
					std::string techset[techset_map_type_effect_e::te_count];
				};

				// pa0 = packed

				std::unordered_map<std::string, techset_map_s> mapped_techsets =
				{
					{"m_l_sm_lmpb_reactivemotion_t0c0n0_nfwpf_sst_smt",  {"mopw_l_sm_reactivemotion_atest_i0c0_nocast"}},
					{"m_l_sm_lmpb_reactivemotion_t0c0sd0_nfwpf_dsl",  {"mopw_l_sm_reactivemotion_atest_i0c0_nocast"}},
					{"m_l_sm_lmpb_reactivemotion_t0c0n0sd0om0_nfwpf_sst_smt_dsl",  {"mopw_l_sm_reactivemotion_atest_i0c0s0n0pa0_nocast"}},

					{"m_l_sm_r0c0_nfwpf",                      {"mo_l_sm_replace_i0c0"}},
					{"m_l_sm_r0c0_nfwpf_nocast",               {"mo_l_sm_replace_i0c0"}},
					{"m_l_sm_r0c0n0_nfwpf",                    {"mo_l_sm_replace_i0c0n0"}},
					{"m_l_sm_r0c0n0sd0_nfwpf",                 {"mo_l_sm_replace_i0c0s0n0"}},
					{"m_l_sm_r0c0sd0_nfwpf",                   {"mo_l_sm_replace_i0c0s0"}},

					{"mc_l_sm_r0c0_nfwpf",                     {"mo_l_sm_replace_i0c0"}},
					{"mc_l_sm_r0c0sd0_nfwpf",                  {"mo_l_sm_replace_i0c0n0"}},
					{"mc_l_sm_r0c0n0sd0_nfwpf",                {"mo_l_sm_replace_i0c0s0n0"}},

					{"mc_unlit",                               {"mo_unlit_replace_lin", "mo_unlit_replace_lin_ct"}},

					{"wc_tools_r0c0ct0",                       {"w_tools_r0c0"}}, // wc_tools_r0c0

					{"wc_shadowcaster",                        {"w_shadowcaster"}},
					
					{"wc_sky_cso_nfwpf",                       {"w_sky"}},

					{"wc_unlit_multiply_lin_ndw_nfwpf",        {"wc_unlit_multiply_lin_ndw"}},
					{"wc_unlit_multiply_lin_ndw_cltrans",      {"wc_unlit_multiply_lin_ndw"}},
					{"wc_unlit_add_lin_ndw_nfwpf",             {"wc_unlit_add_lin_ndw"}},

					{"wc_unlit_blend_lin_ndw_ua_cltrans",      {"wc_unlit_blend_lin_ndw_ua"}},

					{"wc_l_sm_r0c0_nfwpf",                     {"w_l_sm_replace_i0c0"}},
					{"wc_l_sm_r0c0sd0_nfwpf",                  {"w_l_sm_replace_i0c0s0"}},
					{"wc_l_sm_r0c0n0sd0_nfwpf",                {"w_l_sm_replace_i0c0s0n0"}},
					{"wc_l_sm_r0c0n0sd0p0_nfwpf",              {"w_l_sm_replace_i0c0s0n0p0"}}, // wc_l_sm_replace_i0c0s0n0p0

					{"wc_l_sm_lmpb_r0c0_nfwpf",                {"w_l_sm_replace_i0c0"}},
					{"wc_l_sm_lmpb_r0c0n0_nfwpf",              {"w_l_sm_replace_i0c0n0"}},
					{"wc_l_sm_lmpb_r0c0sd0_nfwpf",             {"w_l_sm_replace_i0c0s0"}},
					{"wc_l_sm_lmpb_r0c0n0sd0_nfwpf",           {"w_l_sm_replace_i0c0s0n0"}}, // wc_l_sm_replace_i0c0s0n0
					{"wc_l_sm_lmpb_r0c0n0sd0ct0_nfwpf",        {"w_l_sm_replace_i0c0s0n0"}}, // ct?
					{"wc_l_sm_lmpb_r0c0n0sd0om0_nfwpf",        {"w_l_sm_replace_i0c0s0n0"}}, // om0? w_l_sm_replace_i0c0s0o0n0
					{"wc_l_sm_lmpb_r0c0d0n0sd0_nfwpf",         {"w_l_sm_replace_i0c0s0n0d0p0"}},
					{"wc_l_sm_lmpb_r0c0q0n0sd0_nfwpf",         {"w_l_sm_replace_i0c0s0n0q0"}},
					{"wc_l_sm_lmpb_r0c0n0sd0sr0_nfwpf",        {"w_l_sm_replace_i0c0s0n0"}}, // sr0? w_l_sm_replace_i0c0s0n0_sref0?
					{"wc_l_sm_lmpb_r0c0q0n0sd0om0_nfwpf",      {"w_l_sm_replace_i0c0s0n0q0"}}, // om0? w_l_sm_replace_i0c0s0o0n0q0

					{"wc_l_sm_ndw_b0c0_nfwpf_frt_aat",         {"wc_l_sm_ndw_blend_i0c0"}},
					{"wc_l_sm_ndw_b0c0ct0_nfwpf_frt_aat",      {"wc_l_sm_ndw_blend_i0c0"}},
					{"wc_l_sm_ndw_b0c0sd0_cltrans",            {"wc_l_sm_ndw_blend_i0c0s0"}},
					{"wc_l_sm_ndw_b0c0n0_nfwpf_frt_aat",       {"wc_l_sm_ndw_blend_i0c0n0"}},
					{"wc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_aat",    {"wc_l_sm_ndw_blend_i0c0s0n0"}},

					{"wc_l_sm_lmpb_ndw_b0c0_nfwpf_frt_aat",               {"wc_l_sm_ndw_blend_i0c0"}},
					{"wc_l_sm_lmpb_ndw_b0c0sd0_nfwpf_frt_aat",            {"wc_l_sm_ndw_blend_i0c0s0"}},
					{"wc_l_sm_lmpb_ndw_b0c0n0_nfwpf_frt_aat",             {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0c0n0sd0_nfwpf_frt_aat",          {"wc_l_sm_ndw_blend_i0c0s0n0"}},
					{"wc_l_sm_lmpb_ndw_b0c0n0sd0om0_nfwpf_frt_aat",       {"wc_l_sm_ndw_blend_i0c0s0n0"}}, // om0 not found
					{"wc_l_sm_lmpb_ndw_b0c0n0sd0om0_cltrans",             {"wc_l_sm_ndw_blend_i0c0s0n0"}}, // om0 not found
					{"wc_l_sm_lmpb_ndw_b0c0v0z0n0sd0_nfwpf",              {"wc_l_sm_ndw_blend_i0c0s0n0v0z0p0"}}, // idk wc_l_sm_ndw_blend_i0c0s0n0v0z0p0
					{"wc_l_sm_lmpb_ndw_b0c0v0z0n0sd0om0_nfwpf",           {"wc_l_sm_ndw_blend_i0c0s0n0v0z0p0"}}, // idk wc_l_sm_ndw_blend_i0c0s0n0v0z0p0
					{"wc_l_sm_lmpb_ndw_b0c0v0z0n0sd0_nfwpf_frt_aat",      {"wc_l_sm_ndw_blend_i0c0s0n0v0z0p0"}}, // idk wc_l_sm_ndw_blend_i0c0s0n0v0z0p0
					{"wc_l_sm_lmpb_ndw_b0c0v0z0n0sd0om0_nfwpf_frt_aat",   {"wc_l_sm_ndw_blend_i0c0s0n0v0z0p0"}}, // idk wc_l_sm_ndw_blend_i0c0s0n0v0z0p0
					{"wc_l_sm_lmpb_ndw_b0c0v0n0sd0_nfwpf_frt_aat",        {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0c0v0n0sd0om0_nfwpf_frt_aat",     {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0ha0c0v0n0sd0ct0_nfwpf_frt_aat",  {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					
					{"wc_l_sm_lmpb_ndw_b0c0v0d0n0sd0om0_nfwpf_frt_aat",   {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0c0n0sd0sr0ct0om0_nfwpf_frt_aat", {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0c0v0n0sd0sr0om0_nfwpf_frt_aat",  {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0ha0c0v0n0sd0_nfwpf_frt_aat",     {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0ha0c0n0sd0_nfwpf_frt_aat",       {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0ha0c0_nfwpf_frt_aat",            {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0c0v0sd0_nfwpf_frt_aat",          {"wc_l_sm_ndw_blend_i0c0"}}, // idk
					{"wc_l_sm_lmpb_ndw_b0c0v0n0sd0sr0_nfwpf_frt_aat",     {"wc_l_sm_ndw_blend_i0c0s0n0v0p0_sref0"}}, // idk
					

					{"wc_l_sm_ndw_ua_b0c0_cltrans_nocast",     {"wc_l_ndw_ua_blend_i0c0_nocast"}},

					{"wc_l_sm_lmpb_t0c0n0sd0_nfwpf",           {"wc_l_sm_ndw_blend_i0c0s0n0"}}, // ->blend
					{"wc_l_sm_lmpb_t0c0n0sd0_nfwpf_dsl",       {"wc_l_sm_ndw_blend_i0c0s0n0"}}, // ->blend, dsl?
				};

				std::unordered_map<std::string, techset_map_effect_s> mapped_techsets_effect =
				{
					{"effect",                                 {"eq_effect_blend_lin_ndw_nocast", "ev_effect_blend_lin_ct_ndw_nocast"}},
				};
					
				std::unordered_map<std::uint8_t, std::uint8_t> mapped_camera_regions =
				{
					{zonetool::h1::CAMERA_REGION_LIT_OPAQUE, 0},
					{zonetool::h1::CAMERA_REGION_LIT_OPAQUE_SSS, 1},
					{zonetool::h1::CAMERA_REGION_LIT_DECAL, 2},
					{zonetool::h1::CAMERA_REGION_LIT_TRANS, 3},
					{zonetool::h1::CAMERA_REGION_EMISSIVE, 4},
					{zonetool::h1::CAMERA_REGION_DEPTH_HACK, 4},
					{zonetool::h1::CAMERA_REGION_NONE, 4},
				};

				std::unordered_map<std::uint8_t, std::uint8_t> mapped_sort_keys =
				{
					{0, 0},     // opaque sss
					{1, 1},     // opaque ambient
					{2, 2},     // opaque
					{3, 3},     // sky
					{4, 4},     // ?
					{7, 7},		// decal bottom 1
					{8, 8},		// decal bottom 2
					{9, 9},		// decal bottom 3
					{10, 10},	// decal static
					{11, 11},	// decal mid 1
					{12, 12},	// decal mid 2
					{13, 13},	// decal mid 3
					{14, 14},	// decal effect
					{15, 15},	// decal top 1
					{16, 16},	// decal top 2
					{17, 17},	// decal top 3
					{18, 18},   // ? (trans begin)
					{19, 19},   // ?
					{26, 25},   // window inside
					{27, 26},   // window outside
					{28, 27},   // cloud 1
					{29, 28},   // cloud 2
					{30, 29},   // cloud 3
					{31, 30},   // blend 1
					{32, 31},   // blend 2
					{33, 32},   // blend 3
					{35, 2},    // gun emissive (2 for now)
					{36, 2},    // gun blend (2 for now)
					{37, 2},    // gun depth hack (2 for now)
					{38, 2},    // shadowcaster
					{46, 2},    // mdao?
					{48, 24},   // distort
					{49, 35},   // effect post light sprite?
					{51, 33},   // effect ?
					{52, 34},   // effect 1
					{53, 35},   // effect 2/autosort
					{54, 36},   // effect 3
					{55, 38},   // ?
					{56, 38},   // ?
					{57, 35},   // effect ?
					{59, 35},   // effect ?
					{60, 41},   // 2d
				};

				std::unordered_map<std::uint8_t, std::uint8_t> mapped_material_types =
				{
					{zonetool::h1::MaterialType::MTL_TYPE_DEFAULT, MaterialType_::MTL_TYPE_DEFAULT},
					{zonetool::h1::MaterialType::MTL_TYPE_MODEL, MaterialType_::MTL_TYPE_MODEL_SELFVIS},
					{zonetool::h1::MaterialType::MTL_TYPE_MODEL_EFFECT, MaterialType_::MLT_TYPE_MODEL_UNK2}, // ??
					{zonetool::h1::MaterialType::MTL_TYPE_MODEL_VERTCOL, MaterialType_::MTL_TYPE_MODEL_VERTCOL_SELFVIS},
					{zonetool::h1::MaterialType::MTL_TYPE_MODEL_VERTCOL_EFFECT, MaterialType_::MTL_TYPE_MODEL_UNK4}, // ??
					{zonetool::h1::MaterialType::MTL_TYPE_MODEL_VERTLIT, MaterialType_::MTL_TYPE_MODEL_VERTLIT_SELFVIS},
					{zonetool::h1::MaterialType::MTL_TYPE_MODEL_VERTLIT_VERTCOL, MaterialType_::MTL_TYPE_MODEL_VERTLIT_VERTCOL_SELFVIS},
					{zonetool::h1::MaterialType::MTL_TYPE_MODEL_LMAP, MaterialType_::MTL_TYPE_MODEL_LMAP},
					{zonetool::h1::MaterialType::MTL_TYPE_MODEL_LMAP_VERTCOL, MaterialType_::MTL_TYPE_MODEL_LMAP_VERTCOL},

					{zonetool::h1::MaterialType::MTL_TYPE_WORLD, MaterialType_::MTL_TYPE_WORLD},
					{zonetool::h1::MaterialType::MTL_TYPE_WORLD_VERTCOL, MaterialType_::MTL_TYPE_WORLD_VERTCOL},
				};

				std::string get_mapped_techset(const std::string& techset, const bool color_tint, bool* result)
				{
					const auto it = mapped_techsets.find(techset);
					if (it != mapped_techsets.end())
					{
						auto tech = it->second.techset[color_tint ? techset_map_type_e::t_color_tint : techset_map_type_e::t_regular];
						return tech.empty() ? it->second.techset[techset_map_type_e::t_regular] : tech;
					}

					*result = false;
					return "2d";
				}

				std::string get_mapped_techset_effect(const std::string& techset, const bool vertlit, bool* result)
				{
					const auto it = mapped_techsets_effect.find(techset);
					if (it != mapped_techsets_effect.end())
					{
						auto tech = it->second.techset[vertlit ? techset_map_type_effect_e::te_vertlit : techset_map_type_effect_e::te_regular];
						return tech.empty() ? it->second.techset[techset_map_type_effect_e::te_regular] : tech;
					}

					*result = false;
					return "2d";
				}

				std::string get_iw7_techset(const std::string& techset, const bool is_effect, const bool color_tint, const bool vertlit, bool* result)
				{
					if (is_effect)
					{
						return get_mapped_techset_effect(techset, vertlit, result);
					}
					else
					{
						return get_mapped_techset(techset, color_tint, result);
					}
				}

				std::uint8_t convert_sortkey(const std::uint8_t sort_key)
				{
					if (mapped_sort_keys.find(sort_key) != mapped_sort_keys.end())
					{
						return mapped_sort_keys[sort_key];
					}

					ZONETOOL_FATAL("Unknown sort key: %d", sort_key);
					return sort_key;
				}

				std::uint8_t convert_camera_region(const std::uint8_t camera_region)
				{
					if (mapped_camera_regions.find(camera_region) != mapped_camera_regions.end())
					{
						return mapped_camera_regions[camera_region];
					}

					ZONETOOL_FATAL("Unknown camera region: %d", camera_region);
					return camera_region;
				}

				std::uint8_t convert_material_type(const std::uint8_t material_type)
				{
					if (mapped_material_types.find(material_type) != mapped_material_types.end())
					{
						return mapped_material_types[material_type];
					}

					ZONETOOL_FATAL("Unknown material type: %d", material_type);
					return material_type;
				}
			}

#define MATERIAL_DUMP_STRING(entry) \
	matdata[#entry] = std::string(asset->entry);

#define MATERIAL_DUMP(entry) \
	matdata[#entry] = asset->entry;

#define MATERIAL_DUMP_INFO(entry) \
	matdata[#entry] = asset->info.entry;

			void dump_json(zonetool::h1::Material* asset)
			{
				auto c_name = clean_name(asset->name);

				const auto path = "materials\\"s + c_name + ".json"s;
				auto file = zonetool::filesystem::file(path);

				ordered_json matdata;

				MATERIAL_DUMP_STRING(name);

				std::string techset;
				std::string iw7_techset;
				if (asset->techniqueSet)
				{
					techset = asset->techniqueSet->name;
					bool result = true;
					iw7_techset = get_iw7_techset(techset, false, true, false, &result);
					if (!result)
					{
						ZONETOOL_ERROR("Not dumping material \"%s\" : Unknown techset \"%s\"", asset->name, techset.c_str());
						return;
					}

					matdata["techniqueSet->name"] = iw7_techset;
					matdata["techniqueSet->og_name"] = techset;
				}

				matdata["gameFlags"] = asset->info.gameFlags; // convert
				matdata["unkFlags"] = 0; // h1 has no unkFlags
				matdata["sortKey"] = convert_sortkey(asset->info.sortKey);
				matdata["renderFlags"] = asset->info.renderFlags;

				matdata["textureAtlasRowCount"] = asset->info.textureAtlasRowCount;
				matdata["textureAtlasColumnCount"] = asset->info.textureAtlasColumnCount;
				matdata["textureAtlasFrameBlend"] = asset->info.textureAtlasFrameBlend;
				matdata["textureAtlasAsArray"] = asset->info.textureAtlasAsArray;

				matdata["surfaceTypeBits"] = asset->info.surfaceTypeBits; // convert
				// hashIndex;

				matdata["stateFlags"] = asset->stateFlags; // same

				matdata["cameraRegion"] = convert_camera_region(asset->cameraRegion);
				matdata["materialType"] = convert_material_type(asset->materialType);
				matdata["assetFlags"] = asset->assetFlags;

				ordered_json constant_table = json::array();
				for (int i = 0; i < asset->constantCount && techset != "2d"; i++)
				{
					ordered_json table;
					std::string constant_name = asset->constantTable[i].name;
					const auto constant_hash = asset->constantTable[i].nameHash;

					if (constant_name.size() > 12)
					{
						constant_name.resize(12);
					}

					if (constant_hash == 1033475292) // envMapParms
					{
						continue;
					}

					table["name"] = constant_name.data();
					table["nameHash"] = constant_hash;

					nlohmann::json literal_entry;

					literal_entry[0] = asset->constantTable[i].literal[0];
					literal_entry[1] = asset->constantTable[i].literal[1];
					literal_entry[2] = asset->constantTable[i].literal[2];
					literal_entry[3] = asset->constantTable[i].literal[3];

					table["literal"] = literal_entry;

					constant_table[constant_table.size()] = table;
				}

#define CONSTANT_TABLE_ADD_IF_NOT_FOUND(CONST_NAME, CONST_HASH, LITERAL_1, LITERAL_2, LITERAL_3, LITERAL_4) \
				{ \
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
				} \
				}

				CONSTANT_TABLE_ADD_IF_NOT_FOUND("colorTint", 3054254906, 1.0f, 1.0f, 1.0f, 1.0f);

				if (iw7_techset.find("s0") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("reflectionRa", 3344177073u, 8096.0f, 0.0f, 0.0f, 0.0f);
				}
				if (iw7_techset.find("_lin") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("textureAtlas", 1128936273u,
						static_cast<float>(asset->info.textureAtlasColumnCount), static_cast<float>(asset->info.textureAtlasRowCount), 1.0f, 1.0f);
				}

				matdata["constantTable"] = constant_table;

				if (iw7_techset == "w_sky")
					matdata["constantTable"] = nullptr;

				ordered_json material_images = json::array();
				for (auto i = 0; i < asset->textureCount; i++)
				{
					if (asset->textureTable[i].nameHash == 3447584578) // ~envbrdflut_ggx_16-rg
					{
						continue;
					}

					ordered_json image;
					if (asset->textureTable[i].u.image && asset->textureTable[i].u.image->name)
					{
						image["image"] = asset->textureTable[i].u.image->name;
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

				if (iw7_techset.find("v0") != std::string::npos)
				{
					IMAGE_ADD_IF_NOT_FOUND("$identitynormalmap", 16, 19, 112, 115, 2771134132);
				}
				if (iw7_techset.find("pa0") != std::string::npos)
				{
					IMAGE_ADD_IF_NOT_FOUND("$identitynormalmap", 16, 20, 112, 115, 2771134132);
				}

				matdata["textureTable"] = material_images;

				ordered_json sub_materials = json::array();
				for (auto i = 0; i < asset->layerCount; i++)
				{
					if (asset->subMaterials && asset->subMaterials[i])
					{
						sub_materials.push_back(asset->subMaterials[i]);
					}
				}
				matdata["subMaterials"] = sub_materials;

				auto str = matdata.dump(4, ' ', true, nlohmann::detail::error_handler_t::replace);

				matdata.clear();

				file.open("wb");
				file.write(str);
				file.close();
			}

			zonetool::iw7::Material* convert(zonetool::h1::Material* asset, utils::memory::allocator& allocator)
			{
				auto techset_name = std::string(asset->name);

				const auto new_asset = allocator.allocate<zonetool::iw7::Material>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->info.gameFlags = asset->info.gameFlags;
				new_asset->info.unkFlags = 0;
				new_asset->info.sortKey = convert_sortkey(asset->info.sortKey);
				new_asset->info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
				new_asset->info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
				new_asset->info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
				new_asset->info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
				new_asset->info.renderFlags = asset->info.renderFlags;
				COPY_VALUE_CAST(info.drawSurf);
				COPY_VALUE_CAST(info.surfaceTypeBits);
				new_asset->info.hashIndex = asset->info.hashIndex;

				if (techset_name == "2d")
				{
					new_asset->info.renderFlags |= 0x1;
				}

				std::memset(new_asset->stateBitsEntry, 0xFF, zonetool::iw7::MaterialTechniqueType::TECHNIQUE_COUNT);
				for (auto i = 0; i < zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT; i++)
				{
					if (converter::iw7::techset::technique_index_map.contains(i))
					{
						new_asset->stateBitsEntry[converter::iw7::techset::technique_index_map[i]] = asset->stateBitsEntry[i];
					}
				}

				new_asset->textureCount = asset->textureCount;
				new_asset->constantCount = asset->constantCount;
				new_asset->stateBitsCount = asset->stateBitsCount;
				new_asset->stateFlags = asset->stateFlags;
				new_asset->cameraRegion = convert_camera_region(asset->cameraRegion);
				new_asset->materialType = convert_material_type(asset->materialType);
				new_asset->layerCount = asset->layerCount;
				new_asset->assetFlags = asset->assetFlags;

				new_asset->techniqueSet = allocator.manual_allocate<zonetool::iw7::MaterialTechniqueSet>(sizeof(char*));
				new_asset->techniqueSet->name = allocator.duplicate_string(game::add_source_postfix(asset->techniqueSet->name, game::h1));
				
				new_asset->textureTable = allocator.allocate_array<zonetool::iw7::MaterialTextureDef>(new_asset->textureCount);
				for (auto i = 0; i < new_asset->textureCount; i++)
				{
					new_asset->textureTable[i].nameHash = asset->textureTable[i].nameHash;
					new_asset->textureTable[i].nameStart = asset->textureTable[i].nameStart;
					new_asset->textureTable[i].nameEnd = asset->textureTable[i].nameEnd;
					new_asset->textureTable[i].samplerState = asset->textureTable[i].samplerState;
					new_asset->textureTable[i].semantic = asset->textureTable[i].semantic;
					new_asset->textureTable[i].image = allocator.allocate<zonetool::iw7::GfxImage>();
					new_asset->textureTable[i].image->name = asset->textureTable[i].u.image->name;
				}

				REINTERPRET_CAST_SAFE(constantTable);
				
				new_asset->stateBitsTable = allocator.allocate_array<zonetool::iw7::GfxStateBits>(new_asset->stateBitsCount);
				for (auto i = 0; i < new_asset->stateBitsCount; i++)
				{
					new_asset->stateBitsTable[i].loadBits[0] = asset->stateBitsTable[i].loadBits[0];
					new_asset->stateBitsTable[i].loadBits[1] = asset->stateBitsTable[i].loadBits[1];
					new_asset->stateBitsTable[i].loadBits[2] = asset->stateBitsTable[i].loadBits[3];

					new_asset->stateBitsTable[i].loadBits[3] = asset->stateBitsTable[i].loadBits[4];
					new_asset->stateBitsTable[i].loadBits[4] = asset->stateBitsTable[i].loadBits[5];

					new_asset->stateBitsTable[i].depthStencilState[0] = asset->stateBitsTable[i].depthStencilState[0]; // ?
					new_asset->stateBitsTable[i].depthStencilState[1] = asset->stateBitsTable[i].depthStencilState[0]; // ?
					new_asset->stateBitsTable[i].depthStencilState[2] = asset->stateBitsTable[i].depthStencilState[0];
					new_asset->stateBitsTable[i].depthStencilState[3] = asset->stateBitsTable[i].depthStencilState[1];
					new_asset->stateBitsTable[i].depthStencilState[4] = asset->stateBitsTable[i].depthStencilState[2];
					new_asset->stateBitsTable[i].depthStencilState[5] = asset->stateBitsTable[i].depthStencilState[3]; // ?
					new_asset->stateBitsTable[i].depthStencilState[6] = asset->stateBitsTable[i].depthStencilState[7]; // ?
					new_asset->stateBitsTable[i].depthStencilState[7] = asset->stateBitsTable[i].depthStencilState[8]; // ?
					new_asset->stateBitsTable[i].depthStencilState[8] = asset->stateBitsTable[i].depthStencilState[9]; // ?
					new_asset->stateBitsTable[i].depthStencilState[9] = asset->stateBitsTable[i].depthStencilState[0]; // ??
					new_asset->stateBitsTable[i].depthStencilState[10] = new_asset->stateBitsTable[i].depthStencilState[0];
					new_asset->stateBitsTable[i].depthStencilState[11] = new_asset->stateBitsTable[i].depthStencilState[1];
					new_asset->stateBitsTable[i].depthStencilState[12] = new_asset->stateBitsTable[i].depthStencilState[2];
					new_asset->stateBitsTable[i].depthStencilState[13] = new_asset->stateBitsTable[i].depthStencilState[3];//

					//std::memcpy(new_asset->stateBitsTable[i].loadBits, asset->stateBitsTable[i].loadBits, sizeof(int[6])); // convert needed?
					//std::memcpy(new_asset->stateBitsTable[i].depthStencilState, asset->stateBitsTable[i].depthStencilState, sizeof(char[10])); // maybe wrong
					new_asset->stateBitsTable[i].zone = asset->stateBitsTable[i].zone;
					new_asset->stateBitsTable[i].blendState = asset->stateBitsTable[i].blendState;
					new_asset->stateBitsTable[i].rasterizerState = asset->stateBitsTable[i].rasterizerState;
				}

				new_asset->constantBufferIndex = allocator.allocate_array<unsigned char>(zonetool::iw7::MaterialTechniqueType::TECHNIQUE_COUNT);
				std::memset(new_asset->constantBufferIndex, 0xFF, zonetool::iw7::MaterialTechniqueType::TECHNIQUE_COUNT);
				for (auto i = 0; i < zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT; i++)
				{
					if (converter::iw7::techset::technique_index_map.contains(i))
					{
						new_asset->constantBufferIndex[converter::iw7::techset::technique_index_map[i]] = asset->constantBufferIndex[i];
					}
				}

				REINTERPRET_CAST_SAFE(constantBufferTable);
				new_asset->constantBufferCount = asset->constantBufferCount;

				REINTERPRET_CAST_SAFE(subMaterials);

				return new_asset;
			}

			void dump(zonetool::h1::Material* asset)
			{
				dump_json(asset);

				//utils::memory::allocator allocator;
				//const auto converted_asset = convert(asset, allocator);
				//zonetool::iw7::material::dump(converted_asset);
			}
		}
	}
}
