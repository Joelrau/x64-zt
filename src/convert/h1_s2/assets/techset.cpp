#include <std_include.hpp>

#include "../include.hpp"
#include "techset.hpp"

#include <shader-tool/shader.hpp>

namespace asm_ = alys::shader::literals;

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		const std::unordered_map<std::string_view, std::string_view> s2_prefixes
		{
			{"m", "mo"},
			{"mc", "mco"},
			{"wc", "wc"},
			{"w", "w"},
			{"me", "me"},
			{"mec", "mco"},
			{"mce", "mco"},
		};

		const std::vector<std::pair<std::string_view, std::string_view>> s2_token_renames
		{
			{"sld0", "sl0"},
			{"sd0", "s0"},
		};

		const std::unordered_map<std::string_view, float> token_weights
		{
			{"l", 20.0f},
			{"sm", 5.0f},
			{"lmpb", 4.0f},
			{"ndw", 8.0f},
			{"add", 10.0f},
			{"blend", 10.0f},
			{"replace", 10.0f},
			{"atest", 10.0f},
			{"sky", 30.0f},
			{"b0", 10.0f},
			{"r0", 10.0f},
			{"t0", 10.0f},
			{"ct0", 6.0f},
			{"cltrans", 6.0f},
		};

		const std::unordered_map<std::string_view, std::string_view> default_techsets
		{
			{"mco", "mco_l_sm_r0c0n0s0_nfwpf"},
			{"mo", "mo_l_sm_lmpb_t0c0n0s0_nfwpf"},
			{"wc", "wc_l_sm_lmpb_r0c0n0s0_nfwpf"},
		};

		struct techset_name
		{
			std::string name;
			std::string prefix;
			std::vector<std::string> tokens;
		};

		std::unordered_set<std::string> dumped_techsets;
		template <typename T>
		std::unordered_set<std::string> dumped_shaders;

		enum class draw_kind
		{
			hidden,
			sky,
			translucent,
			opaque,
		};

		const std::unordered_set<std::string_view> hidden_tokens
		{
			"tools",
			"shadowcaster",
			"distortion",
		};

		// "ndw" is no depth write, "b0" and "ba0" are blended layers
		const std::unordered_set<std::string_view> translucent_tokens
		{
			"ndw",
			"add",
			"blend",
			"multiply",
			"b0",
			"ba0",
		};

		// layer groups mix one and two letter layers, for example "t0c0n0om0" or "b0hb0c0"
		void add_layer_tokens(const std::string& part, std::vector<std::string>& tokens)
		{
			static const std::regex layer_group(R"(^([a-z]+[0-9])+$)");
			static const std::regex layer(R"([a-z]+[0-9])");
			if (!std::regex_match(part, layer_group))
			{
				tokens.emplace_back(part);
				return;
			}

			for (std::sregex_iterator i(part.begin(), part.end(), layer), end; i != end; ++i)
			{
				tokens.emplace_back(i->str());
			}
		}

		techset_name parse_name(std::string name, const bool from_h1)
		{
			if (from_h1)
			{
				for (const auto& [from, to] : s2_token_renames)
				{
					for (auto pos = name.find(from); pos != std::string::npos; pos = name.find(from, pos + to.size()))
					{
						name.replace(pos, from.size(), to);
					}
				}
			}

			techset_name result{name};
			auto parts = utils::string::split(name, '_');
			if (!parts.empty())
			{
				result.prefix = parts.front();
				parts.erase(parts.begin());
			}

			if (from_h1)
			{
				const auto prefix = s2_prefixes.find(result.prefix);
				result.prefix = prefix != s2_prefixes.end() ? std::string(prefix->second) : result.prefix;
			}

			for (const auto& part : parts)
			{
				add_layer_tokens(part, result.tokens);
			}

			return result;
		}

		bool has_any_token(const techset_name& name, const std::unordered_set<std::string_view>& tokens)
		{
			return std::ranges::any_of(name.tokens, [&](const std::string& token)
			{
				return tokens.contains(token);
			});
		}

		draw_kind get_draw_kind(const techset_name& name)
		{
			if (hidden_tokens.contains(name.prefix) || has_any_token(name, hidden_tokens))
			{
				return draw_kind::hidden;
			}

			if (has_any_token(name, {"sky"}))
			{
				return draw_kind::sky;
			}

			return has_any_token(name, translucent_tokens) ? draw_kind::translucent : draw_kind::opaque;
		}

		bool is_model_prefix(const std::string& prefix)
		{
			return prefix.starts_with('m');
		}

		bool lightmap_allowed(const techset_name& source, const techset_name& candidate)
		{
			return !has_any_token(source, {"unlit"}) || !has_any_token(candidate, {"lmpb"});
		}

		float token_weight(const std::string& token)
		{
			const auto weight = token_weights.find(token);
			return weight != token_weights.end() ? weight->second : 2.0f;
		}

		float score(const techset_name& source, const techset_name& candidate)
		{
			auto result = 0.0f;
			for (const auto& token : source.tokens)
			{
				const auto found = std::find(candidate.tokens.begin(), candidate.tokens.end(), token) != candidate.tokens.end();
				result += found ? token_weight(token) * 2.0f : -token_weight(token);
			}

			for (const auto& token : candidate.tokens)
			{
				if (std::find(source.tokens.begin(), source.tokens.end(), token) == source.tokens.end())
				{
					result -= token_weight(token);
				}
			}

			return result;
		}

		std::optional<std::string> find_best(const techset_name& source, const std::function<bool(const techset_name&)>& accept)
		{
			std::optional<std::string> best;
			auto best_score = -std::numeric_limits<float>::infinity();
			for (const auto& name : donor_zone().donor_techsets())
			{
				const auto candidate = parse_name(name, false);
				if (!accept(candidate))
				{
					continue;
				}

				const auto value = score(source, candidate);
				if (value > best_score)
				{
					best_score = value;
					best = name;
				}
			}

			return best;
		}

		template <typename T>
		void dump_shader_file(T* shader)
		{
			if constexpr (std::is_same_v<T, zonetool::s2::MaterialVertexShader>)
			{
				zonetool::s2::vertex_shader::dump(shader);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialPixelShader>)
			{
				zonetool::s2::pixel_shader::dump(shader);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialHullShader>)
			{
				zonetool::s2::hull_shader::dump(shader);
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialDomainShader>)
			{
				zonetool::s2::domain_shader::dump(shader);
			}
			else
			{
				zonetool::s2::vertex_decl::dump(shader);
			}
		}

		template <typename T>
		constexpr std::string_view shader_csv_type()
		{
			if constexpr (std::is_same_v<T, zonetool::s2::MaterialVertexShader>)
			{
				return "vertexshader";
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialPixelShader>)
			{
				return "pixelshader";
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialHullShader>)
			{
				return "hullshader";
			}
			else if constexpr (std::is_same_v<T, zonetool::s2::MaterialDomainShader>)
			{
				return "domainshader";
			}
			else
			{
				return "vertexdecl";
			}
		}

		template <typename T>
		void dump_shader(T* shader)
		{
			if (!shader || !shader->name || !dumped_shaders<T>.insert(shader->name).second)
			{
				return;
			}

			const auto global = donor_zone().is_global_asset(shader); // the donor zone holds only the name, common_mp holds the program
			if (!global)
			{
				dump_shader_file(shader);
			}

			add_csv_row(shader_csv_type<T>(), shader->name, global);
		}

		constexpr auto h1_per_prim_constant_buffer = 0u;
		constexpr auto h1_per_object_constant_buffer = 1u;
		constexpr auto h1_stable_constant_buffer = 2u;
		constexpr auto h1_constant_buffer_count = 5u; // cb4 is the slf buffer
		constexpr auto h1_instance_matrix_buffer = 6u;
		constexpr auto s2_instance_matrix_buffer = 7u;
		constexpr auto h1_instance_row_shift = 2u; // 4 rows for each instance
		constexpr auto s2_instance_row_shift = 3u; // 8 rows for each instance
		constexpr auto h1_first_shifted_stream_source = 6u;
		constexpr auto s2_stream_source_shift = 3u;
		constexpr auto h1_model_lighting_buffer = 7u; // instanced passes read the lighting coords from cb7[instance]
		constexpr auto h1_instance_probe_ambient_flag = 0x400u; // the instanced "amb" passes bind their cb7 probe ambient colors with it
		constexpr auto s2_lit_instance_rows = 9u; // lit instanced passes add the model lighting index row
		constexpr auto s2_lit_instance_lighting_row = 8u;
		// static models drawn through the cached path (sub_1407E8F90) bind a per prim buffer built at load with the stock layout:
		// cb0[0..3] world matrix, cb0[4] const 329, cb0[6..9] previous world matrix, cb0[10] MODEL_LIGHTING_INDEX
		constexpr auto s2_model_lighting_index_dest = 10u;
		// the "ml" passes (vertex type 3) have MODEL_LIGHTMAP_PARAMS in the same row of that buffer
		constexpr auto s2_model_lightmap_params_dest = 10u;
		constexpr auto light_grid_direction_count = 12u;
		constexpr auto light_grid_volume_slot = 7u; // pixel shader t7 of the stock "lmdl" shaders, bound by the custom buffers
		// pixel shader slots of the 2d/3d light grid colors and primary visibility, bound by stable code textures like the stock passes
		constexpr auto light_grid_colors_slot = 19u;
		constexpr auto light_grid_visibility_slot = 20u;
		constexpr auto ambient_light_grid_sampler = 0u; // the stock "plit_lgv" passes sample the 2d/3d light grid with s0 = LINEAR_CLAMP

		utils::memory::allocator native_allocator;

		const std::unordered_map<std::int32_t, std::int32_t> code_consts = []
		{
			std::unordered_map<std::int32_t, std::int32_t> result
			{
				{CONST_SRC_CODE_GAMETIME, zonetool::s2::CONST_SRC_CODE_GAMETIME},
				{CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS, zonetool::s2::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS},
				{CONST_SRC_CODE_ZNEAR, zonetool::s2::CONST_SRC_CODE_ZNEAR}, // the ocean depth fade divides by it
				{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::s2::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
				{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::s2::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
				{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::s2::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
				{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::s2::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
				{CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM, zonetool::s2::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM},
				{CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS, zonetool::s2::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS},
				{CONST_SRC_CODE_GENERIC_MATERIAL_DATA, zonetool::s2::CONST_SRC_CODE_GENERIC_MATERIAL_DATA},
				{CONST_SRC_CODE_EYEOFFSET, zonetool::s2::CONST_SRC_CODE_EYEOFFSET},
				{CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET, zonetool::s2::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET},
				{CONST_SRC_CODE_LIGHT_POSITION, zonetool::s2::CONST_SRC_CODE_SUN_DIRECTION},
				{CONST_SRC_CODE_LIGHT_DIFFUSE, zonetool::s2::CONST_SRC_CODE_SUN_COLOR},
				{CONST_SRC_CODE_LIGHT_SPECULAR, zonetool::s2::CONST_SRC_CODE_SUN_COLOR},
				{CONST_SRC_CODE_EFFECT_MODEL_COLOR, zonetool::s2::CONST_SRC_CODE_EFFECT_MODEL_COLOR},
				{CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE, zonetool::s2::CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE},
				{CONST_SRC_CODE_RENDER_TARGET_SIZE, zonetool::s2::CONST_SRC_CODE_RENDER_TARGET_SIZE},
				{CONST_SRC_CODE_NEARPLANE_ORG, zonetool::s2::CONST_SRC_CODE_NEARPLANE_ORG},
				{CONST_SRC_CODE_NEARPLANE_DX, zonetool::s2::CONST_SRC_CODE_NEARPLANE_DX},
				{CONST_SRC_CODE_NEARPLANE_DY, zonetool::s2::CONST_SRC_CODE_NEARPLANE_DY},
				{CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF, zonetool::s2::CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF},
				{CONST_SRC_CODE_HDR_STAGE_PARMS, zonetool::s2::CONST_SRC_CODE_HDR_STAGE_PARMS},
				{CONST_SRC_CODE_MATERIAL_COLOR, zonetool::s2::CONST_SRC_CODE_MATERIAL_COLOR},
				{CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET, zonetool::s2::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET},
				{CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, zonetool::s2::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE},
				{CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, zonetool::s2::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET},
				{CONST_SRC_CODE_DEPTH_FROM_CLIP, zonetool::s2::CONST_SRC_CODE_DEPTH_FROM_CLIP},
			};

			const auto add_range = [&](const std::int32_t first, const std::int32_t last, const std::int32_t s2_first)
			{
				for (auto index = first; index <= last; index++)
				{
					result.emplace(index, s2_first + index - first);
				}
			};

			add_range(CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0, CONST_SRC_CODE_SHADOWMAP_DISTANCE_BLEND,
				zonetool::s2::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0);
			add_range(CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0, CONST_SRC_CODE_SUN_SHADOWMAP_CASCADE_V_CLAMP,
				zonetool::s2::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0);
			add_range(CONST_SRC_CODE_VIEW_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
				zonetool::s2::CONST_SRC_CODE_VIEW_MATRIX);
			add_range(CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
				zonetool::s2::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX);
			add_range(CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
				zonetool::s2::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX);
			add_range(CONST_SRC_CODE_PARTICLE_CLOUD_TEXTURE_ATLAS_SETTINGS, CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2,
				zonetool::s2::CONST_SRC_CODE_PARTICLE_CLOUD_TEXTURE_ATLAS_SETTINGS);
			add_range(CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX, CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2,
				zonetool::s2::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX);
			return result;
		}();

		const std::unordered_map<std::int32_t, std::int32_t> code_textures
		{
			{TEXTURE_SRC_CODE_BLACK, zonetool::s2::TEXTURE_SRC_CODE_BLACK},
			{TEXTURE_SRC_CODE_WHITE, zonetool::s2::TEXTURE_SRC_CODE_WHITE},
			{TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP, zonetool::s2::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP},
			{TEXTURE_SRC_CODE_SHADOWMAP_SUN, zonetool::s2::TEXTURE_SRC_CODE_SHADOWMAP_SUN},
			{TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN, zonetool::s2::TEXTURE_SRC_CODE_WHITE}, // h1 shadow = saturate(sun + trans - 1), white keeps the sun term
			{TEXTURE_SRC_CODE_SHADOWMAP_SPOT, zonetool::s2::TEXTURE_SRC_CODE_SHADOWMAP_SPOT},
			{TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1, zonetool::s2::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1},
			{TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2, zonetool::s2::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2},
			{TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3, zonetool::s2::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3},
			{TEXTURE_SRC_CODE_FEEDBACK, zonetool::s2::TEXTURE_SRC_CODE_FEEDBACK},
			{TEXTURE_SRC_CODE_RESOLVED_POST_SUN, zonetool::s2::TEXTURE_SRC_CODE_RESOLVED_POST_SUN},
			{TEXTURE_SRC_CODE_SSR_BUFFER, zonetool::s2::TEXTURE_SRC_CODE_SSR_BUFFER},
			{TEXTURE_SRC_CODE_HQ_SSR_DEPTH, zonetool::s2::TEXTURE_SRC_CODE_HQ_SSR_DEPTH},
			{TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER, zonetool::s2::TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER},
			{TEXTURE_SRC_CODE_RESOLVED_SCENE, zonetool::s2::TEXTURE_SRC_CODE_RESOLVED_SCENE},
			{TEXTURE_SRC_CODE_POST_EFFECT_0, zonetool::s2::TEXTURE_SRC_CODE_POST_EFFECT_0},
			{TEXTURE_SRC_CODE_POST_EFFECT_1, zonetool::s2::TEXTURE_SRC_CODE_POST_EFFECT_1},
			{TEXTURE_SRC_CODE_OUTDOOR, zonetool::s2::TEXTURE_SRC_CODE_OUTDOOR},
			{TEXTURE_SRC_CODE_FLOATZ, zonetool::s2::TEXTURE_SRC_CODE_FLOATZ},
			{TEXTURE_SRC_CODE_PROCESSED_FLOATZ, zonetool::s2::TEXTURE_SRC_CODE_PROCESSED_FLOATZ},
			{TEXTURE_SRC_CODE_FLOATZ_STENCIL, zonetool::s2::TEXTURE_SRC_CODE_FLOATZ_STENCIL},
			{TEXTURE_SRC_CODE_CINEMATIC_Y, zonetool::s2::TEXTURE_SRC_CODE_CINEMATIC_Y},
			{TEXTURE_SRC_CODE_CINEMATIC_CR, zonetool::s2::TEXTURE_SRC_CODE_CINEMATIC_CR},
			{TEXTURE_SRC_CODE_CINEMATIC_CB, zonetool::s2::TEXTURE_SRC_CODE_CINEMATIC_CB},
			{TEXTURE_SRC_CODE_CINEMATIC_A, zonetool::s2::TEXTURE_SRC_CODE_CINEMATIC_A},
			{TEXTURE_SRC_CODE_CINEMATIC_CBCR, zonetool::s2::TEXTURE_SRC_CODE_CINEMATIC_CBCR},
			{TEXTURE_SRC_CODE_DALTONIZE_CLUT, zonetool::s2::TEXTURE_SRC_CODE_DALTONIZE_CLUT},
			{TEXTURE_SRC_CODE_CLUT, zonetool::s2::TEXTURE_SRC_CODE_CLUT},
			{TEXTURE_SRC_CODE_LINEAR_CLAMP, zonetool::s2::TEXTURE_SRC_CODE_LINEAR_CLAMP},
			{TEXTURE_SRC_CODE_POINT_CLAMP, zonetool::s2::TEXTURE_SRC_CODE_POINT_CLAMP},
			{TEXTURE_SRC_CODE_RANDOM_ROTATIONS, zonetool::s2::TEXTURE_SRC_CODE_RANDOM_ROTATIONS},
			{TEXTURE_SRC_CODE_SSAO, zonetool::s2::TEXTURE_SRC_CODE_SSAO},
			{TEXTURE_SRC_CODE_MSAA_SCENE, zonetool::s2::TEXTURE_SRC_CODE_MSAA_SCENE},
			{TEXTURE_SRC_CODE_DEPTH_BLUR_0, zonetool::s2::TEXTURE_SRC_CODE_DEPTH_BLUR_0},
			{TEXTURE_SRC_CODE_DEPTH_BLUR_1, zonetool::s2::TEXTURE_SRC_CODE_DEPTH_BLUR_1},
			{TEXTURE_SRC_CODE_DOF_HALF_PREPASS, zonetool::s2::TEXTURE_SRC_CODE_DOF_HALF_PREPASS},
			{TEXTURE_SRC_CODE_DOF_HALF_COLOR, zonetool::s2::TEXTURE_SRC_CODE_DOF_HALF_COLOR},
			{TEXTURE_SRC_CODE_DOF_HALF_ALPHA, zonetool::s2::TEXTURE_SRC_CODE_DOF_HALF_ALPHA},
			{TEXTURE_SRC_CODE_DOF_TILE0, zonetool::s2::TEXTURE_SRC_CODE_DOF_TILE0},
			{TEXTURE_SRC_CODE_DOF_TILE1, zonetool::s2::TEXTURE_SRC_CODE_DOF_TILE1},
			{TEXTURE_SRC_CODE_SCENE_SPECULAR, zonetool::s2::TEXTURE_SRC_CODE_SCENE_SPECULAR},
			{TEXTURE_SRC_CODE_SCENE_ALBEDO, zonetool::s2::TEXTURE_SRC_CODE_SCENE_ALBEDO},
			{TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA, zonetool::s2::TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA},
			{TEXTURE_SRC_CODE_UI3D, zonetool::s2::TEXTURE_SRC_CODE_UI3D},
			{TEXTURE_SRC_CODE_FONT_CACHE, zonetool::s2::TEXTURE_SRC_CODE_FONT_CACHE},
		};

		constexpr auto fog_off_height_falloff = 1.0f / 8192.0f;

		const std::unordered_map<std::int32_t, std::array<float, 4>> code_literals
		{
			{CONST_SRC_CODE_FOG_HEIGHT_FOG, {0.0f, 0.0f, fog_off_height_falloff, 0.0f}},
			{CONST_SRC_CODE_ATMOS_FOG_PARMS_5, {0.0f, 0.0f, fog_off_height_falloff, 0.0f}},
			{CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE, {1.0f, 1.0f, 1.0f, 0.0f}}, // the patched pixel shaders only need the lookup direction
			// h1 script parms, w desaturates the model lighting in the lit pixel shaders, s2 code const 26 holds other data that flickers it
			{CONST_SRC_CODE_SCRIPT_PARMS, {0.0f, 0.0f, 0.0f, 0.0f}},
			// s2 has no code mesh args; the h1 "trailfade" vertex shaders fade the trail ends by saturate((x - u) * z) * saturate((u - y) * w)
			// of CODE_MESH_ARG_0, an open window keeps the whole trail
			{CONST_SRC_CODE_CODE_MESH_ARG_0, {65504.0f, -65504.0f, 1.0f, 1.0f}},
			{CONST_SRC_CODE_CODE_MESH_ARG_1, {0.0f, 0.0f, 0.0f, 0.0f}},
			{CONST_SRC_CODE_CODE_MESH_ARG_3, {0.0f, 0.0f, 0.0f, 0.0f}},
			{CONST_SRC_CODE_CODE_MESH_ARG_4, {0.0f, 0.0f, 0.0f, 0.0f}},
		};

		// s2 model lighting, the icosahedron directions of the 12 probe colors (sub_1407E7170 "plit_lgv" shaders)
		constexpr std::array<std::array<float, 3>, light_grid_direction_count> light_grid_directions
		{{
			{0.850651f, 0.525731f, 0.0f},
			{-0.850651f, 0.525731f, 0.0f},
			{0.850651f, -0.525731f, 0.0f},
			{-0.850651f, -0.525731f, 0.0f},
			{0.0f, 0.850651f, 0.525731f},
			{0.0f, -0.850651f, 0.525731f},
			{0.0f, 0.850651f, -0.525731f},
			{0.0f, -0.850651f, -0.525731f},
			{0.525731f, 0.0f, 0.850651f},
			{-0.525731f, 0.0f, 0.850651f},
			{0.525731f, 0.0f, -0.850651f},
			{-0.525731f, 0.0f, -0.850651f},
		}};

		// the s2 lit slots come in groups of 4 for each light grid volume layout
		enum class light_grid
		{
			probe_buffer, // "lgv0d", probe index from x, 12 colors in buffer t8 and the sun visibility in buffer t9
			array_2d, // "lgv1d2d", Texture2DArray t19 with a slice for each direction, sun visibility in Texture2D t20
			volume_3d, // "lgv3d", Texture3D t19 with the directions stacked along z, sun visibility in Texture3D t20
		};

		light_grid slot_light_grid(const std::int32_t slot)
		{
			const auto type = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE;
			if (type >= zonetool::s2::TECHNIQUE_LIT_BEGIN + 8 && type < zonetool::s2::TECHNIQUE_LIT_END)
			{
				return light_grid::volume_3d;
			}

			if (type >= zonetool::s2::TECHNIQUE_LIT_BEGIN + 4 && type < zonetool::s2::TECHNIQUE_LIT_END)
			{
				return light_grid::array_2d;
			}

			return light_grid::probe_buffer;
		}

		std::uint64_t light_grid_buffer_flags(const light_grid grid)
		{
			constexpr auto common = zonetool::s2::CUSTOM_BUFFER_FORWARD_LIGHTING | zonetool::s2::CUSTOM_BUFFER_LIGHT_GRID_VOLUME;
			return grid == light_grid::probe_buffer ? common | zonetool::s2::CUSTOM_BUFFER_LIGHT_GRID_PROBES : common;
		}

		std::string_view light_grid_suffix(const light_grid grid)
		{
			switch (grid)
			{
			case light_grid::array_2d:
				return "_lgv2d";
			case light_grid::volume_3d:
				return "_lgv3d";
			default:
				return "_lgv0d";
			}
		}

		// h1 models sample their own cube of TEXTURE_SRC_CODE_MODEL_LIGHTING at BASE_LIGHTING_COORDS, s2 has no such atlas
		struct model_lighting
		{
			bool instanced;
			std::uint16_t coords_dest; // per prim row of BASE_LIGHTING_COORDS when not instanced
			std::uint16_t eye_offset_dest; // stable row of the vertex shader WORLD_MATRIX_EYE_OFFSET
			std::uint16_t texture; // pixel shader slot of the model lighting volume
			bool ambient; // the vertex shader passes the per prim LIGHT_PROBE_AMBIENT color, the pixel shader reads it as an input
		};

		constexpr std::array<zonetool::s2::MaterialShaderArgumentType, MTL_ARG_COUNT> argument_types
		{
			zonetool::s2::MTL_ARG_CODE_CONST,
			zonetool::s2::MTL_ARG_CODE_TEXTURE,
			zonetool::s2::MTL_ARG_CODE_SAMPLER,
			zonetool::s2::MTL_ARG_MATERIAL_CONST,
			zonetool::s2::MTL_ARG_LITERAL_CONST,
			zonetool::s2::MTL_ARG_MATERIAL_TEXTURE,
			zonetool::s2::MTL_ARG_MATERIAL_SAMPLER,
		};

		constexpr std::array<std::pair<std::uint32_t, std::uint64_t>, 6> custom_buffers
		{{
			{CUSTOM_BUFFER_PER_PRIM, zonetool::s2::CUSTOM_BUFFER_PER_PRIM},
			{CUSTOM_BUFFER_PER_OBJECT, zonetool::s2::CUSTOM_BUFFER_PER_OBJECT},
			{CUSTOM_BUFFER_PER_STABLE, zonetool::s2::CUSTOM_BUFFER_PER_STABLE},
			{CUSTOM_BUFFER_MATERIAL, zonetool::s2::CUSTOM_BUFFER_MATERIAL},
			{CUSTOM_BUFFER_SMODEL_LIGHTING, zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES},
			{CUSTOM_BUFFER_SLF, zonetool::s2::CUSTOM_BUFFER_SLF},
		}};

		constexpr std::array<std::uint8_t, 18> stream_slot_order{0, 1, 5, 21, 2, 3, 4, 9, 10, 16, 17, 18, 19, 20, 26, 6, 7, 29};

		std::map<std::pair<const void*, std::string>, void*> converted_shaders;
		std::unordered_set<const zonetool::s2::MaterialVertexShader*> lightmap_vertex_shaders;
		std::unordered_map<const zonetool::s2::MaterialVertexShader*, std::uint32_t> lighting_semantics;
		std::unordered_map<const zonetool::s2::MaterialPixelShader*, std::map<std::uint32_t, std::uint32_t>> relocated_textures;
		std::unordered_map<const MaterialVertexDeclaration*, zonetool::s2::MaterialVertexDeclaration*> converted_decls;
		std::map<std::tuple<const MaterialTechnique*, light_grid, zonetool::s2::MaterialPassModelVertexType>, zonetool::s2::MaterialTechnique*> converted_techniques;
		std::unordered_map<std::string, std::optional<native_techset>> native_techsets;

		float* allocate_literal(const float* value)
		{
			auto* result = native_allocator.allocate_array<float>(4);
			std::memcpy(result, value, sizeof(float) * 4);
			return result;
		}

		void add_code_const(const MaterialShaderArgument& arg, const bool literals_allowed, std::vector<zonetool::s2::MaterialShaderArgument>& args)
		{
			const auto& code_const = arg.u.codeConst;
			if (code_const.index < CONST_SRC_CODE_VIEW_MATRIX && code_const.rowCount != 1)
			{
				throw std::runtime_error(utils::string::va("code constant %u has %u rows", code_const.index, code_const.rowCount));
			}

			zonetool::s2::MaterialShaderArgument result{zonetool::s2::MTL_ARG_CODE_CONST, arg.shader, arg.dest};
			if (code_const.index == CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS)
			{
				result.dest = s2_model_lightmap_params_dest; // the patched vertex shader reads it from the stock per prim row
			}

			if (const auto entry = code_consts.find(code_const.index); entry != code_consts.end())
			{
				result.u.codeConst = {static_cast<unsigned short>(entry->second), code_const.firstRow, code_const.rowCount};
				args.emplace_back(result);
				return;
			}

			if (!literals_allowed)
			{
				throw std::runtime_error(utils::string::va("code constant %u has no s2 source", code_const.index));
			}

			constexpr std::array<float, 4> zero{};
			const auto literal = code_literals.find(code_const.index);
			result.type = zonetool::s2::MTL_ARG_LITERAL_CONST;
			result.u.literalConst = allocate_literal(literal != code_literals.end() ? literal->second.data() : zero.data());
			for (auto row = 0u; row < code_const.rowCount; row++)
			{
				result.dest = static_cast<unsigned short>(arg.dest + row);
				args.emplace_back(result);
			}
		}

		std::uint32_t convert_code_texture(const std::uint32_t source, const std::uint32_t fallback)
		{
			const auto entry = code_textures.find(static_cast<std::int32_t>(source));
			return entry != code_textures.end() ? entry->second : fallback;
		}

		void convert_arg(const MaterialShaderArgument& arg, const bool literals_allowed, const bool model_lighting, std::vector<zonetool::s2::MaterialShaderArgument>& args)
		{
			if (arg.type >= MTL_ARG_COUNT)
			{
				throw std::runtime_error(utils::string::va("argument type %u is unknown", arg.type));
			}

			if (model_lighting && arg.type == MTL_ARG_CODE_CONST
				&& (arg.u.codeConst.index == CONST_SRC_CODE_BASE_LIGHTING_COORDS || arg.u.codeConst.index == CONST_SRC_CODE_LIGHT_PROBE_AMBIENT))
			{
				// the patched vertex shader reads the s2 light grid volume index from the stock per prim row
				zonetool::s2::MaterialShaderArgument result{zonetool::s2::MTL_ARG_CODE_CONST, arg.shader, s2_model_lighting_index_dest};
				result.u.codeConst = {zonetool::s2::CONST_SRC_CODE_MODEL_LIGHTING_INDEX, 0, 1};
				args.emplace_back(result);
				return;
			}

			if (model_lighting && arg.type == MTL_ARG_CODE_TEXTURE && arg.u.codeSampler == TEXTURE_SRC_CODE_MODEL_LIGHTING)
			{
				return; // the light grid volumes come from the custom buffers
			}

			if (arg.type == MTL_ARG_CODE_CONST)
			{
				add_code_const(arg, literals_allowed, args);
				return;
			}

			zonetool::s2::MaterialShaderArgument result{argument_types[arg.type], arg.shader, arg.dest};
			switch (arg.type)
			{
			case MTL_ARG_CODE_TEXTURE:
				result.u.codeSampler = convert_code_texture(arg.u.codeSampler, zonetool::s2::TEXTURE_SRC_CODE_BLACK);
				break;
			case MTL_ARG_CODE_SAMPLER:
				result.u.codeSampler = convert_code_texture(arg.u.codeSampler, zonetool::s2::TEXTURE_SRC_CODE_LINEAR_CLAMP);
				break;
			case MTL_ARG_LITERAL_CONST:
				result.u.literalConst = allocate_literal(arg.u.literalConst);
				break;
			default:
				result.u.nameHash = arg.u.nameHash;
				break;
			}

			args.emplace_back(result);
		}

		std::vector<zonetool::s2::MaterialShaderArgument> convert_args(const MaterialShaderArgument* args, const std::size_t count, const bool literals_allowed,
			const bool model_lighting)
		{
			std::vector<zonetool::s2::MaterialShaderArgument> result;
			for (auto i = 0u; i < count; i++)
			{
				convert_arg(args[i], literals_allowed, model_lighting, result);
			}

			std::ranges::stable_sort(result, {}, &zonetool::s2::MaterialShaderArgument::type);
			if (result.size() > std::numeric_limits<unsigned char>::max())
			{
				throw std::runtime_error(utils::string::va("%zu arguments do not fit in one group", result.size()));
			}

			return result;
		}

		std::uint64_t convert_custom_buffer_flags(const std::uint32_t flags, const std::optional<model_lighting>& lighting, const light_grid grid)
		{
			if (lighting)
			{
				// lit instances get the 9 row layout with the model lighting index instead of the h1 lighting coords
				const auto instance = flags & CUSTOM_BUFFER_SMODEL_LIGHTING ? zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES_LIGHTING : 0;
				const auto rest = flags & ~(CUSTOM_BUFFER_SMODEL_LIGHTING | CUSTOM_BUFFER_SMODEL_AMBIENT | h1_instance_probe_ambient_flag);
				return convert_custom_buffer_flags(rest, {}, grid) | instance | light_grid_buffer_flags(grid);
			}

			std::uint64_t result{};
			auto unknown = flags;
			for (const auto& [h1_flag, s2_flag] : custom_buffers)
			{
				if (flags & h1_flag)
				{
					result |= s2_flag;
					unknown &= ~h1_flag;
				}
			}

			if (unknown)
			{
				throw std::runtime_error(utils::string::va("custom buffer flags 0x%X have no s2 mapping", unknown));
			}

			return result;
		}

		// moves the reads of one constant buffer row, the s2 cached per prim buffers have a fixed layout
		void relocate_constant_row(alys::shader::detail::operand_t& operand, const std::uint32_t buffer, const std::uint32_t from, const std::uint32_t to,
			const std::optional<std::uint32_t> to_buffer = {})
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					relocate_constant_row(*index.extra_operand, buffer, from, to, to_buffer);
				}
			}

			if (operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == buffer
				&& operand.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32 && operand.indices[1].value.uint32 == from)
			{
				operand.indices[0].value.uint32 = to_buffer.value_or(buffer);
				operand.indices[1].value.uint32 = to;
			}
		}

		void remap_constant_buffers(alys::shader::detail::operand_t& operand)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					remap_constant_buffers(*index.extra_operand);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER)
			{
				return;
			}

			auto& slot = operand.indices[0].value.uint32;
			if (slot == h1_instance_matrix_buffer)
			{
				slot = s2_instance_matrix_buffer;
				return;
			}

			if (slot >= h1_constant_buffer_count)
			{
				throw std::runtime_error(utils::string::va("constant buffer cb%u has no s2 slot", slot));
			}

			if (slot >= h1_stable_constant_buffer)
			{
				slot++;
			}
		}

		void redirect_input(alys::shader::detail::operand_t& operand, const std::uint32_t input, const std::uint32_t temp)
		{
			for (auto& index : operand.indices)
			{
				if (index.extra_operand)
				{
					redirect_input(*index.extra_operand, input, temp);
				}
			}

			if (operand.type != D3D10_SB_OPERAND_TYPE_INPUT)
			{
				return;
			}

			if (operand.indices[0].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
			{
				throw std::runtime_error("vertex shader indexes its inputs");
			}

			if (operand.indices[0].value.uint32 == input)
			{
				operand.type = D3D10_SB_OPERAND_TYPE_TEMP;
				operand.indices[0].value.uint32 = temp;
			}
		}

		bool is_declaration(const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_CUSTOMDATA
				|| (alys::shader::detail::get_instruction_flags(instruction) & alys::shader::detail::flag_declaration);
		}

		bool is_input_declaration(const alys::shader::detail::instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT
				|| instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SGV
				|| instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SIV;
		}

		struct lightmap_input
		{
			std::uint32_t texcoord;
			std::uint32_t lightmap;
		};

		std::optional<lightmap_input> add_lightmap_input(alys::shader::shader_object::signature& inputs)
		{
			// h1 world vertex shaders read the lightmap uv from texcoord0.zw, s2 streams it as texcoord15
			const auto texcoord = std::ranges::find_if(inputs, [](const alys::shader::shader_object::signature_element& element)
			{
				return element.name == "TEXCOORD" && element.semantic_index == 0 && (element.rw_mask & 0xC);
			});

			if (texcoord == inputs.end())
			{
				return {};
			}

			const auto last = std::ranges::max(inputs, {}, &alys::shader::shader_object::signature_element::register_);
			const lightmap_input result{texcoord->register_, last.register_ + 1};

			// the converted decl only streams uv in texcoord0, declaring zw too is a d3d11 input layout linkage error
			texcoord->mask &= 0x3;
			texcoord->rw_mask &= 0x3;

			alys::shader::shader_object::signature_element lightmap{};
			lightmap.name = "TEXCOORD";
			lightmap.semantic_index = zonetool::s2::STREAM_DST_TEXCOORD_15 - zonetool::s2::STREAM_DST_TEXCOORD_0;
			lightmap.system_value_type = alys::shader::NONE;
			lightmap.component_type = alys::shader::format_float;
			lightmap.register_ = result.lightmap;
			lightmap.mask = 0x3;
			lightmap.rw_mask = 0x3;
			inputs.emplace_back(lightmap);

			return result;
		}

		struct sun_shadow_lookup
		{
			std::optional<std::uint32_t> texture;
			std::unordered_map<std::uint32_t, float> cascade_slices;
		};

		sun_shadow_lookup find_sun_shadow_lookup(const MaterialPass& pass)
		{
			sun_shadow_lookup result;
			const auto first_stable_arg = pass.perPrimArgCount + pass.perObjArgCount;
			for (auto i = 0; i < first_stable_arg + pass.stableArgCount; i++)
			{
				const auto& arg = pass.args[i];
				if (!(arg.shader & MTL_SHADER_STAGE_PIXEL))
				{
					continue;
				}

				if (arg.type == MTL_ARG_CODE_TEXTURE && arg.u.codeSampler == TEXTURE_SRC_CODE_SHADOWMAP_SUN)
				{
					result.texture = arg.dest;
				}

				if (i < first_stable_arg || arg.type != MTL_ARG_CODE_CONST)
				{
					continue;
				}

				const auto index = arg.u.codeConst.index;
				if (index >= CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0 && index <= CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_2)
				{
					result.cascade_slices.emplace(arg.dest, static_cast<float>(index - CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0));
				}
			}

			return result;
		}

		bool is_destination(const alys::shader::detail::operand_t& operand)
		{
			return operand.type == D3D10_SB_OPERAND_TYPE_NULL || (operand.type != D3D10_SB_OPERAND_TYPE_IMMEDIATE32
				&& operand.components.type == D3D10_SB_OPERAND_4_COMPONENT && operand.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE);
		}

		bool is_resource(const alys::shader::detail::operand_t& operand, const std::uint32_t texture)
		{
			return operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE && operand.indices[0].value.uint32 == texture;
		}

		bool is_sample(const alys::shader::detail::instruction_t& instruction, const std::uint32_t texture)
		{
			switch (instruction.opcode.type)
			{
			case D3D10_SB_OPCODE_SAMPLE:
			case D3D10_SB_OPCODE_SAMPLE_C:
			case D3D10_SB_OPCODE_SAMPLE_C_LZ:
			case D3D10_SB_OPCODE_SAMPLE_L:
			case D3D10_SB_OPCODE_SAMPLE_D:
			case D3D10_SB_OPCODE_SAMPLE_B:
			case D3D10_1_SB_OPCODE_GATHER4:
			case D3D11_SB_OPCODE_GATHER4_C:
				return is_resource(instruction.operands[2], texture);
			default:
				return false;
			}
		}

		std::uint8_t read_components(const alys::shader::detail::operand_t& source, const std::uint8_t written, const std::uint32_t opcode)
		{
			if (source.components.type != D3D10_SB_OPERAND_4_COMPONENT)
			{
				return 0;
			}

			if (source.components.selection_mode == D3D10_SB_OPERAND_4_COMPONENT_SELECT_1_MODE)
			{
				return static_cast<std::uint8_t>(1 << source.components.names[0]);
			}

			const auto dot_size = opcode == D3D10_SB_OPCODE_DP2 ? 2 : opcode == D3D10_SB_OPCODE_DP3 ? 3 : opcode == D3D10_SB_OPCODE_DP4 ? 4 : 0;
			std::uint8_t result{};
			for (auto component = 0; component < 4; component++)
			{
				const auto used = dot_size ? component < dot_size : (written >> component) & 1;
				if (used)
				{
					result |= static_cast<std::uint8_t>(1 << source.components.names[component]);
				}
			}

			return result;
		}

		std::optional<float> read_cascade_slice(const alys::shader::detail::operand_t& operand, const sun_shadow_lookup& sun_shadow)
		{
			if (operand.type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER || operand.indices[0].value.uint32 != h1_stable_constant_buffer
				|| operand.indices[1].representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32)
			{
				return {};
			}

			const auto entry = sun_shadow.cascade_slices.find(operand.indices[1].value.uint32);
			return entry != sun_shadow.cascade_slices.end() ? std::optional(entry->second) : std::nullopt;
		}

		std::optional<float> trace_cascade_slice(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::size_t sample,
			const sun_shadow_lookup& sun_shadow)
		{
			std::unordered_map<std::uint32_t, std::uint8_t> wanted;
			const auto& uv = instructions[sample].operands[1];
			if (uv.type == D3D10_SB_OPERAND_TYPE_TEMP)
			{
				wanted[uv.indices[0].value.uint32] = read_components(uv, 0x3, instructions[sample].opcode.type);
			}

			for (auto i = sample; i-- > 0 && !wanted.empty();)
			{
				const auto& operands = instructions[i].operands;
				const auto first_source = static_cast<std::size_t>(std::ranges::find_if_not(operands, is_destination) - operands.begin());
				for (auto dest = 0u; dest < first_source; dest++)
				{
					const auto entry = operands[dest].type == D3D10_SB_OPERAND_TYPE_TEMP ? wanted.find(operands[dest].indices[0].value.uint32) : wanted.end();
					const auto written = static_cast<std::uint8_t>(entry != wanted.end() ? entry->second & operands[dest].components.mask : 0);
					if (!written)
					{
						continue;
					}

					entry->second &= ~written;
					if (!entry->second)
					{
						wanted.erase(entry);
					}

					for (auto source = first_source; source < operands.size(); source++)
					{
						if (const auto slice = read_cascade_slice(operands[source], sun_shadow))
						{
							return *slice;
						}

						if (operands[source].type == D3D10_SB_OPERAND_TYPE_TEMP)
						{
							wanted[operands[source].indices[0].value.uint32] |= read_components(operands[source], written, instructions[i].opcode.type);
						}
					}
				}
			}

			return {};
		}

		float find_cascade_slice(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::size_t sample, const sun_shadow_lookup& sun_shadow)
		{
			if (const auto slice = trace_cascade_slice(instructions, sample, sun_shadow))
			{
				return *slice;
			}

			// the "sst" translucency reads the unfiltered depth of the cascade right after its filtered samples, which have the pixel adjust row
			for (auto i = sample; i-- > 0;)
			{
				if (is_sample(instructions[i], *sun_shadow.texture))
				{
					if (const auto slice = trace_cascade_slice(instructions, i, sun_shadow))
					{
						return *slice;
					}
				}
			}

			throw std::runtime_error("sun shadow sample has no cascade");
		}

		void set_resource_dimension(alys::shader::detail::instruction_t& instruction, const std::uint16_t dimension)
		{
			for (auto& extension : instruction.opcode.extensions)
			{
				if (extension.type == D3D11_SB_EXTENDED_OPCODE_RESOURCE_DIM)
				{
					extension.values[0] = dimension;
				}
			}
		}

		bool declares_instance_matrices(const std::vector<alys::shader::detail::instruction_t>& instructions)
		{
			return std::ranges::any_of(instructions, [](const alys::shader::detail::instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == h1_instance_matrix_buffer;
			});
		}

		std::optional<std::uint32_t> find_instance_input(const std::vector<alys::shader::detail::instruction_t>& instructions)
		{
			const auto input = std::ranges::find_if(instructions, [](const alys::shader::detail::instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_SGV && instruction.operands[1].custom.u.value == D3D10_SB_NAME_INSTANCE_ID;
			});

			return input != instructions.end() ? std::optional(input->operands[0].indices[0].value.uint32) : std::nullopt;
		}

		bool reads_instance_matrices(const alys::shader::detail::operand_t& operand)
		{
			return operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == s2_instance_matrix_buffer;
		}

		std::uint32_t temp_component(const std::uint32_t temp, const std::uint32_t component)
		{
			return temp * 4 + component;
		}

		bool reads_instance_row(const alys::shader::detail::operand_t& operand, const std::unordered_set<std::uint32_t>& instance_rows)
		{
			const auto& row = operand.indices[1];
			const auto is_relative = row.representation == D3D10_SB_OPERAND_INDEX_RELATIVE;
			const auto offset = is_relative ? 0u : row.value.uint32;
			if ((!is_relative && row.representation != D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE) || offset >= (1u << h1_instance_row_shift)
				|| !row.extra_operand || row.extra_operand->type != D3D10_SB_OPERAND_TYPE_TEMP)
			{
				return false;
			}

			const auto& temp = *row.extra_operand;
			return instance_rows.contains(temp_component(temp.indices[0].value.uint32, temp.components.names[0]));
		}

		bool is_instance_row(const alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input)
		{
			const auto& operands = instruction.operands;
			return instruction.opcode.type == D3D10_SB_OPCODE_ISHL && operands[0].type == D3D10_SB_OPERAND_TYPE_TEMP
				&& operands[1].type == D3D10_SB_OPERAND_TYPE_INPUT && operands[1].indices[0].value.uint32 == instance_input
				&& operands[2].type == D3D10_SB_OPERAND_TYPE_IMMEDIATE32 && operands[2].immediate_values[0].uint32 == h1_instance_row_shift;
		}

		void patch_instance_rows(alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input, const bool lit_rows,
			std::unordered_set<std::uint32_t>& instance_rows)
		{
			const auto reads_without_row = std::ranges::any_of(instruction.operands, [&](const alys::shader::detail::operand_t& operand)
			{
				return reads_instance_matrices(operand) && !reads_instance_row(operand, instance_rows);
			});

			if (reads_without_row)
			{
				throw std::runtime_error("shader reads the instance matrices without the instance row");
			}

			const auto& operands = instruction.operands;
			const auto first_source = std::ranges::find_if_not(operands, is_destination) - operands.begin();
			for (auto dest = 0; dest < first_source; dest++)
			{
				for (auto component = 0u; operands[dest].type == D3D10_SB_OPERAND_TYPE_TEMP && component < 4; component++)
				{
					if ((operands[dest].components.mask >> component) & 1)
					{
						instance_rows.erase(temp_component(operands[dest].indices[0].value.uint32, component));
					}
				}
			}

			if (!is_instance_row(instruction, instance_input))
			{
				return;
			}

			if (lit_rows)
			{
				// 9 rows is no shift, the row becomes instance * 9 + 0
				instruction.opcode.type = D3D10_SB_OPCODE_IMAD;
				instruction.operands[2].immediate_values[0].uint32 = s2_lit_instance_rows;
				auto zero = instruction.operands[2];
				zero.immediate_values[0].uint32 = 0;
				instruction.operands.emplace_back(zero);
			}
			else
			{
				instruction.operands[2].immediate_values[0].uint32 = s2_instance_row_shift;
			}

			for (auto component = 0u; component < 4; component++)
			{
				if ((operands[0].components.mask >> component) & 1)
				{
					instance_rows.insert(temp_component(operands[0].indices[0].value.uint32, component));
				}
			}
		}

		// a vertex shader row of the h1 per prim or per object buffer that the converted pass binds as a stable literal
		struct stable_literal_row
		{
			std::uint32_t buffer;
			std::uint32_t from;
			std::uint32_t to;
		};

		struct program_patch
		{
			sun_shadow_lookup sun_shadow;
			std::optional<model_lighting> lighting;
			light_grid grid{};
			std::uint32_t lighting_semantic{}; // pixel shaders only, from the vertex shader of the pass
			std::optional<std::uint32_t> lightmap_params_dest; // vertex shaders only, h1 per prim row of MODEL_LIGHTMAP_PARAMS
			std::vector<stable_literal_row> stable_literal_rows; // vertex shaders only
		};

		struct patched_program
		{
			std::string data;
			bool reads_lightmap;
			std::optional<std::uint32_t> lighting_semantic;
			std::map<std::uint32_t, std::uint32_t> texture_slots; // pixel shader textures moved off the s2 light grid slots
		};

		using signature_element = alys::shader::shader_object::signature_element;

		alys::shader::detail::operand_t uint_literal(const std::uint32_t value)
		{
			auto result = asm_::l(0.0f);
			result.immediate_values[0].uint32 = value;
			return result;
		}

		signature_element& find_signature_element(alys::shader::shader_object& shader, const std::uint32_t chunk,
			const std::function<bool(const signature_element&)>& accept, const char* what)
		{
			auto& signatures = shader.get_signatures();
			if (const auto elements = signatures.find(chunk); elements != signatures.end())
			{
				if (const auto element = std::ranges::find_if(elements->second, accept); element != elements->second.end())
				{
					return *element;
				}
			}

			throw std::runtime_error(utils::string::va("shader has no %s", what));
		}

		bool declares_resource(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot)
		{
			return std::ranges::any_of(instructions, [&](const alys::shader::detail::instruction_t& instruction)
			{
				return is_declaration(instruction) && !instruction.operands.empty() && is_resource(instruction.operands[0], slot);
			});
		}

		bool declares_sampler(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t slot)
		{
			return std::ranges::any_of(instructions, [&](const alys::shader::detail::instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_SAMPLER && instruction.operands[0].indices[0].value.uint32 == slot;
			});
		}

		// the lit custom buffers bind pixel shader t7..t9 and t16..t25, the h1 textures there go to free slots
		bool is_light_grid_slot(const std::uint32_t slot)
		{
			return (slot >= 7 && slot <= 9) || (slot >= 16 && slot <= 25);
		}

		std::map<std::uint32_t, std::uint32_t> relocate_textures(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t lighting_texture)
		{
			constexpr auto texture_slot_count = 16u;

			std::set<std::uint32_t> used;
			for (const auto& instruction : instructions)
			{
				if (is_declaration(instruction) && !instruction.operands.empty() && instruction.operands[0].type == D3D10_SB_OPERAND_TYPE_RESOURCE)
				{
					used.insert(instruction.operands[0].indices[0].value.uint32);
				}
			}

			used.erase(lighting_texture);

			std::vector<std::uint32_t> free;
			for (auto slot = 0u; slot < texture_slot_count; slot++)
			{
				if (!is_light_grid_slot(slot) && !used.contains(slot))
				{
					free.emplace_back(slot);
				}
			}

			std::map<std::uint32_t, std::uint32_t> result;
			auto next = free.begin();
			for (const auto slot : used)
			{
				if (!is_light_grid_slot(slot))
				{
					continue;
				}

				if (next == free.end())
				{
					throw std::runtime_error(utils::string::va("pixel shader has no free slot for t%u", slot));
				}

				result.emplace(slot, *next++);
			}

			return result;
		}

		struct vertex_lighting
		{
			std::size_t coords_write; // mov oN.xyz, cb0[coords] or cb7[instance]
			std::size_t position_write; // last dp4 oPosition, rWorld.xyzw, cb1[n]
			std::uint32_t output;
			std::uint32_t semantic;
		};

		vertex_lighting find_vertex_lighting(alys::shader::shader_object& shader, const model_lighting& lighting)
		{
			const auto& instructions = shader.get_instructions();
			const auto writes_coords = [&](const alys::shader::detail::instruction_t& instruction)
			{
				if (instruction.opcode.type != D3D10_SB_OPCODE_MOV || instruction.operands[0].type != D3D10_SB_OPERAND_TYPE_OUTPUT
					|| instruction.operands[1].type != D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER)
				{
					return false;
				}

				const auto& source = instruction.operands[1];
				if (lighting.instanced)
				{
					return source.indices[0].value.uint32 == h1_model_lighting_buffer;
				}

				return source.indices[0].value.uint32 == 0 && source.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32
					&& source.indices[1].value.uint32 == lighting.coords_dest;
			};

			const auto coords_write = std::ranges::find_if(instructions, writes_coords);
			if (coords_write == instructions.end() || std::find_if(coords_write + 1, instructions.end(), writes_coords) != instructions.end())
			{
				throw std::runtime_error("vertex shader does not write the model lighting coords once");
			}

			vertex_lighting result{static_cast<std::size_t>(coords_write - instructions.begin())};
			result.output = coords_write->operands[0].indices[0].value.uint32;
			result.semantic = find_signature_element(shader, alys::shader::chunk_osgn, [&](const signature_element& element)
			{
				return element.register_ == result.output;
			}, "model lighting output").semantic_index;

			const auto position = find_signature_element(shader, alys::shader::chunk_osgn, [](const signature_element& element)
			{
				return element.system_value_type == alys::shader::POS;
			}, "position output").register_;

			// the clip position is 4 dp4 of the eye relative world position (w = 1) with the view projection rows
			std::optional<std::size_t> position_write;
			for (auto i = 0u; i < result.coords_write; i++)
			{
				const auto& operands = instructions[i].operands;
				if (instructions[i].opcode.type == D3D10_SB_OPCODE_DP4 && operands[0].type == D3D10_SB_OPERAND_TYPE_OUTPUT
					&& operands[0].indices[0].value.uint32 == position && operands[1].type == D3D10_SB_OPERAND_TYPE_TEMP)
				{
					position_write = i;
				}
			}

			if (!position_write)
			{
				throw std::runtime_error("vertex shader writes no clip position before the model lighting coords");
			}

			result.position_write = *position_write;
			return result;
		}

		// computes the s2 light grid volume coords like the stock "lmdl" vertex shaders, xyz = t6[volume] * world position, w = volume
		void emit_vertex_lighting(alys::shader::assembler& a, const alys::shader::detail::operand_t& position, const model_lighting& lighting,
			const std::optional<std::uint32_t> instance_input, const std::uint32_t temp)
		{
			const auto world = temp;
			const auto volume = temp + 1;
			const auto coords = temp + 2;

			a.add(asm_::r(world).xyz(), position, asm_::cb(h1_stable_constant_buffer + 1, lighting.eye_offset_dest).select("xyzx"));
			a.mov(asm_::r(world).w(), asm_::l(1.0f));

			if (lighting.instanced)
			{
				a.imad(asm_::r(volume).x(), asm_::v(*instance_input).x(), uint_literal(s2_lit_instance_rows), uint_literal(s2_lit_instance_lighting_row));
				alys::shader::detail::operand_t index = asm_::cb(s2_instance_matrix_buffer, 0).select("xxxx").swz();
				index.indices[1].representation = D3D10_SB_OPERAND_INDEX_RELATIVE;
				index.indices[1].value.uint32 = 0;
				index.indices[1].extra_operand = std::make_shared<alys::shader::detail::operand_t>(asm_::r(volume).x().scalar());
				a.ftou(asm_::r(volume).x(), index);
			}
			else
			{
				a.ftou(asm_::r(volume).x(), asm_::cb(0, s2_model_lighting_index_dest).select("xxxx").swz());
			}

			a.ld_structured(asm_::r(volume).x(), asm_::r(volume).x(), uint_literal(0), asm_::t(5).select("xxxx"));
			for (auto row = 0u; row < 3; row++)
			{
				a.ld_structured(asm_::r(coords).select("xyzw"), asm_::r(volume).x(), uint_literal(row * 16), asm_::t(6).select("xyzw"));
				a.dp4(asm_::r(volume).select(alys::shader::component_y + row), asm_::r(coords).select("xyzw"), asm_::r(world).select("xyzw"));
			}

			a.mov(asm_::r(coords).xyz(), asm_::r(volume).select("yzwy"));
			a.utof(asm_::r(coords).w(), asm_::r(volume).x());
		}

		// sums the 12 s2 probe colors of the light grid voxel at the input coords into r(temp + 5), w is the sun visibility; directional
		// lookups weight them by the unit direction in r(temp), ambient ones give each the mean weight of a directional lookup over all normals
		void emit_light_grid_lookup(alys::shader::assembler& a, const program_patch& patch, const alys::shader::detail::operand_t& sampler,
			const std::uint32_t input, const std::uint32_t temp, const bool directional)
		{
			// mean of 0.7 * saturate(cos)^2 + 0.3 * saturate(cos)^4 over the sphere
			constexpr auto ambient_weight = 0.7f / 6.0f + 0.3f / 10.0f;

			const auto dir = temp;
			const auto volume = temp + 1;
			const auto coords = temp + 2;
			const auto probe = temp + 3;
			const auto weight = temp + 4;
			const auto color = temp + 5;
			const auto texture = light_grid_volume_slot;

			a.add(asm_::r(volume).x(), asm_::v(input).w(), asm_::l(0.5f));
			a.ftou(asm_::r(volume).x(), asm_::r(volume).x());
			a.ld_structured(asm_::r(coords).xyz(), asm_::r(volume).x(), uint_literal(0), asm_::t(texture).select("xyzx"));
			a.ld_structured(asm_::r(probe).xyz(), asm_::r(volume).x(), uint_literal(16), asm_::t(texture).select("xyzx"));
			a.mov_sat(asm_::r(weight).xyz(), asm_::v(input).select("xyzx"));
			a.mad(asm_::r(coords).xyz(), asm_::r(weight).select("xyzx"), asm_::r(coords).select("xyzx"), asm_::r(probe).select("xyzx"));
			if (patch.grid == light_grid::probe_buffer)
			{
				a.add(asm_::r(coords).x(), asm_::r(coords).x(), asm_::l(0.5f));
				a.ftoi(asm_::r(coords).x(), asm_::r(coords).x());
			}

			a.mov(asm_::r(color).select("xyzw"), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));
			for (auto i = 0u; i < light_grid_direction_count; i++)
			{
				const auto& direction = light_grid_directions[i];
				if (directional)
				{
					a.dp3_sat(asm_::r(weight).x(), asm_::r(dir).select("xyzx"), asm_::l(direction[0], direction[1], direction[2], 0.0f));
					a.mul(asm_::r(weight).x(), asm_::r(weight).x(), asm_::r(weight).x());
					a.mul(asm_::r(weight).y(), asm_::r(weight).x(), asm_::r(weight).x());
					a.mul(asm_::r(weight).y(), asm_::r(weight).y(), asm_::l(0.3f));
					a.mad(asm_::r(weight).x(), asm_::r(weight).x(), asm_::l(0.7f), asm_::r(weight).y());
				}
				else
				{
					a.mov(asm_::r(weight).x(), asm_::l(ambient_weight));
				}

				switch (patch.grid)
				{
				case light_grid::probe_buffer:
					a.imad(asm_::r(probe).x(), asm_::r(coords).x(), uint_literal(light_grid_direction_count), uint_literal(i));
					a.ld_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xxxx"), asm_::t(8).select("xyzw"));
					break;
				case light_grid::array_2d:
					a.mov(asm_::r(probe).xy(), asm_::r(coords).select("xyxx"));
					a.mov(asm_::r(probe).z(), asm_::l(static_cast<float>(i)));
					a.sample_l_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xyzx"), asm_::t(light_grid_colors_slot).select("xyzw"), sampler, asm_::l(0.0f));
					break;
				case light_grid::volume_3d:
					a.mov(asm_::r(probe).xy(), asm_::r(coords).select("xyxx"));
					a.mad(asm_::r(probe).z(), asm_::r(coords).z(), asm_::l(1.0f / light_grid_direction_count),
						asm_::l(static_cast<float>(i) / light_grid_direction_count));
					a.sample_l_indexable(asm_::r(probe).xyz(), asm_::r(probe).select("xyzx"), asm_::t(light_grid_colors_slot).select("xyzw"), sampler, asm_::l(0.0f));
					break;
				}

				a.mad(asm_::r(color).xyz(), asm_::r(probe).select("xyzx"), asm_::r(weight).select("xxxx"), asm_::r(color).select("xyzx"));
			}

			switch (patch.grid)
			{
			case light_grid::probe_buffer:
				a.ld_indexable(asm_::r(probe).x(), asm_::r(coords).select("xxxx"), asm_::t(9).select("xyzw"));
				break;
			case light_grid::array_2d:
				a.sample_l_indexable(asm_::r(probe).x(), asm_::r(coords).select("xyxx"), asm_::t(light_grid_visibility_slot).select("xyzw"), sampler, asm_::l(0.0f));
				break;
			case light_grid::volume_3d:
				a.sample_l_indexable(asm_::r(probe).x(), asm_::r(coords).select("xyzx"), asm_::t(light_grid_visibility_slot).select("xyzw"), sampler, asm_::l(0.0f));
				break;
			}

			a.mov(asm_::r(color).w(), asm_::r(probe).x());
		}

		// replaces a sample of the h1 model lighting cube with the 12 s2 probe colors of the same direction, w is the sun visibility
		void emit_probe_lookup(alys::shader::assembler& a, const alys::shader::detail::instruction_t& sample, const program_patch& patch,
			const std::uint32_t input, const std::uint32_t temp)
		{
			const auto dir = temp;
			const auto color = temp + 5;

			// the h1 lookup is coords + dir * scale / max(|dir|), scale is 1 now
			a.mad(asm_::r(dir).xyz(), asm_::v(input).select("xyzx"), asm_::l(-1.0f), sample.operands[1]);
			a.dp3(asm_::r(dir).w(), asm_::r(dir).select("xyzx"), asm_::r(dir).select("xyzx"));
			a.rsq(asm_::r(dir).w(), asm_::r(dir).w());
			a.mul(asm_::r(dir).xyz(), asm_::r(dir).select("xyzx"), asm_::r(dir).select("wwww"));

			emit_light_grid_lookup(a, patch, sample.operands[3], input, temp, true);

			alys::shader::detail::operand_t result = asm_::r(color).select("xyzw").swz();
			std::ranges::copy(sample.operands[2].components.names, result.components.names);
			a.mov(sample.operands[0], result);
		}

		void declare_light_grid(alys::shader::assembler& a, const program_patch& patch)
		{
			constexpr std::array<std::uint8_t, 4> float4{asm_::t_float, asm_::t_float, asm_::t_float, asm_::t_float};

			a.dcl_resource_structured(asm_::t(light_grid_volume_slot), 32u);
			switch (patch.grid)
			{
			case light_grid::probe_buffer:
				a.dcl_resource(asm_::t(8), asm_::buffer, float4);
				a.dcl_resource(asm_::t(9), asm_::buffer, float4);
				break;
			case light_grid::array_2d:
				a.dcl_resource(asm_::t(light_grid_colors_slot), asm_::texture2darray, float4);
				a.dcl_resource(asm_::t(light_grid_visibility_slot), asm_::texture2d, float4);
				break;
			case light_grid::volume_3d:
				a.dcl_resource(asm_::t(light_grid_colors_slot), asm_::texture3d, float4);
				a.dcl_resource(asm_::t(light_grid_visibility_slot), asm_::texture3d, float4);
				break;
			}
		}

		patched_program patch_program(const unsigned char* program, const unsigned int size, const bool is_vertex_shader, const program_patch& patch)
		{
			auto shader = alys::shader::shader_object::parse({reinterpret_cast<const char*>(program), size});
			const auto& sun_shadow = patch.sun_shadow;

			alys::shader::shader_object result;
			result.get_info() = shader.get_info();
			result.get_signatures() = shader.get_signatures();
			result.get_unknown_chunks() = shader.get_unknown_chunks();

			auto& signatures = result.get_signatures();
			const auto inputs = signatures.find(alys::shader::chunk_isgn);
			const auto lightmap = is_vertex_shader && inputs != signatures.end() ? add_lightmap_input(inputs->second) : std::nullopt;

			const auto& instructions = shader.get_instructions();
			const auto first_code = std::ranges::find_if_not(instructions, is_declaration) - instructions.begin();
			const auto last_input = std::find_if(instructions.rbegin(), instructions.rend(), is_input_declaration);
			if (lightmap && (last_input == instructions.rend() || first_code == static_cast<std::ptrdiff_t>(instructions.size())))
			{
				throw std::runtime_error("vertex shader has no input declarations or code");
			}

			const auto instance_matrices = declares_instance_matrices(instructions);
			const auto instance_input = instance_matrices ? find_instance_input(instructions) : std::nullopt;
			if (instance_matrices && !instance_input)
			{
				throw std::runtime_error("shader reads the instance matrices without the instance id");
			}

			const auto vs_lighting = is_vertex_shader && patch.lighting ? std::optional(find_vertex_lighting(shader, *patch.lighting)) : std::nullopt;
			const auto pixel_lighting = !is_vertex_shader && patch.lighting;
			const auto pixel_ambient = pixel_lighting && patch.lighting->ambient;
			if (pixel_ambient && patch.grid != light_grid::probe_buffer && declares_sampler(instructions, ambient_light_grid_sampler))
			{
				throw std::runtime_error(utils::string::va("pixel shader already uses the light grid sampler s%u", ambient_light_grid_sampler));
			}
			if (vs_lighting && (declares_resource(instructions, 5) || declares_resource(instructions, 6)))
			{
				throw std::runtime_error("vertex shader already uses the light grid volume slots");
			}

			if (vs_lighting && patch.lighting->instanced && !instance_input)
			{
				throw std::runtime_error("instanced model lighting without the instance id");
			}

			std::optional<std::uint32_t> lighting_input;
			if (vs_lighting)
			{
				auto& output = find_signature_element(result, alys::shader::chunk_osgn, [&](const signature_element& element)
				{
					return element.register_ == vs_lighting->output;
				}, "model lighting output");
				output.mask = 0xF;
				output.rw_mask = 0;
			}
			else if (pixel_lighting)
			{
				auto& input = find_signature_element(result, alys::shader::chunk_isgn, [&](const signature_element& element)
				{
					return element.name == "TEXCOORD" && element.semantic_index == patch.lighting_semantic;
				}, "model lighting input");
				input.mask = 0xF;
				input.rw_mask = 0xF;
				lighting_input = input.register_;
			}

			const auto texture_slots = pixel_lighting ? relocate_textures(instructions, patch.lighting->texture) : std::map<std::uint32_t, std::uint32_t>{};

			std::unordered_set<std::uint32_t> instance_rows;
			const auto lit_instance_rows = vs_lighting && patch.lighting->instanced;

			const auto last_input_index = instructions.rend() - last_input - 1;
			const auto base_temps = lightmap.has_value() || sun_shadow.texture.has_value() ? 1u : 0u;
			const auto lighting_temps = vs_lighting ? 3u : pixel_lighting ? 6u : 0u;
			const auto temp_count = base_temps + lighting_temps;
			std::optional<std::uint32_t> first_temp;
			std::optional<std::uint32_t> temp;

			const auto reads_sun_shadow = [&](const alys::shader::detail::instruction_t& instruction)
			{
				return std::ranges::any_of(instruction.operands, [&](const alys::shader::detail::operand_t& operand)
				{
					return is_resource(operand, *sun_shadow.texture);
				});
			};

			const auto reads_model_lighting = [&](const alys::shader::detail::instruction_t& instruction)
			{
				return pixel_lighting && std::ranges::any_of(instruction.operands, [&](const alys::shader::detail::operand_t& operand)
				{
					return is_resource(operand, patch.lighting->texture);
				});
			};

			const auto writes_operand = [](const alys::shader::detail::instruction_t& instruction, const std::uint32_t type, const std::uint32_t index)
			{
				return !instruction.operands.empty() && instruction.operands[0].type == type && instruction.operands[0].indices[0].value.uint32 == index;
			};

			auto a = result.get_assembler();
			for (auto i = 0; i < static_cast<std::ptrdiff_t>(instructions.size()); i++)
			{
				auto instruction = instructions[i];

				if (lit_instance_rows && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER
					&& instruction.operands[0].indices[0].value.uint32 == h1_model_lighting_buffer)
				{
					continue; // the lighting coords buffer, the patched shader reads the index from the instance rows
				}

				if (vs_lighting && i == static_cast<std::ptrdiff_t>(vs_lighting->coords_write))
				{
					a.mov(asm_::o(vs_lighting->output).select("xyzw"), asm_::r(*first_temp + base_temps + 2).select("xyzw"));
					continue;
				}

				if (i >= first_code)
				{
					for (const auto& [buffer, from, to] : patch.stable_literal_rows)
					{
						for (auto& operand : instruction.operands)
						{
							relocate_constant_row(operand, buffer, from, to, h1_stable_constant_buffer);
						}
					}
				}

				if (!patch.stable_literal_rows.empty() && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER
					&& instruction.operands[0].indices[0].value.uint32 == h1_stable_constant_buffer)
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					for (const auto& row : patch.stable_literal_rows)
					{
						rows = std::max(rows, row.to + 1);
					}
				}

				for (auto& operand : instruction.operands)
				{
					remap_constant_buffers(operand);
				}

				if (patch.lightmap_params_dest && i >= first_code)
				{
					for (auto& operand : instruction.operands)
					{
						relocate_constant_row(operand, 0, *patch.lightmap_params_dest, s2_model_lightmap_params_dest);
					}
				}

				const auto per_prim_rows = vs_lighting && !patch.lighting->instanced ? s2_model_lighting_index_dest + 1
					: patch.lightmap_params_dest ? s2_model_lightmap_params_dest + 1 : 0u;
				if (per_prim_rows && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER
					&& instruction.operands[0].indices[0].value.uint32 == 0)
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					rows = std::max(rows, per_prim_rows);
				}

				if (instance_input && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && reads_instance_matrices(instruction.operands[0]))
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					rows = lit_instance_rows ? (rows >> h1_instance_row_shift) * s2_lit_instance_rows : rows >> h1_instance_row_shift << s2_instance_row_shift;
				}
				else if (instance_input && i >= first_code)
				{
					patch_instance_rows(instruction, *instance_input, lit_instance_rows, instance_rows);
				}

				if (temp_count && instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS)
				{
					first_temp = instruction.operands[0].custom.u.value;
					instruction.operands[0].custom.u.value += temp_count;
				}

				if (temp_count && i == first_code && !first_temp)
				{
					first_temp = 0;
					a.dcl_temps(temp_count);
				}

				if (i == first_code && first_temp && base_temps)
				{
					temp = *first_temp;
				}

				if (vs_lighting && i == first_code)
				{
					a.dcl_resource_structured(asm_::t(5), 4u);
					a.dcl_resource_structured(asm_::t(6), 64u);
				}

				if (pixel_ambient && i == first_code)
				{
					declare_light_grid(a, patch);
					if (patch.grid != light_grid::probe_buffer)
					{
						a.dcl_sampler(asm_::s(ambient_light_grid_sampler));
					}

					emit_light_grid_lookup(a, patch, asm_::s(ambient_light_grid_sampler), *lighting_input, *first_temp + base_temps, false);
				}

				if (pixel_ambient && i >= first_code)
				{
					for (auto& operand : instruction.operands)
					{
						redirect_input(operand, *lighting_input, *first_temp + base_temps + 5);
					}
				}

				if (vs_lighting && instruction.opcode.type == D3D10_SB_OPCODE_DCL_OUTPUT
					&& writes_operand(instruction, D3D10_SB_OPERAND_TYPE_OUTPUT, vs_lighting->output))
				{
					instruction.operands[0].components.mask = 0xF;
				}

				if (pixel_lighting && instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT_PS
					&& writes_operand(instruction, D3D10_SB_OPERAND_TYPE_INPUT, *lighting_input))
				{
					instruction.operands[0].components.mask = 0xF;
				}

				if (lightmap && instruction.opcode.type == D3D10_SB_OPCODE_DCL_INPUT
					&& writes_operand(instruction, D3D10_SB_OPERAND_TYPE_INPUT, lightmap->texcoord))
				{
					instruction.operands[0].components.mask &= 0x3;
				}

				if (lightmap && i == first_code)
				{
					a.mov(asm_::r(*temp).xy(), asm_::v(lightmap->texcoord).xyxx());
					a.mov(asm_::r(*temp).zw(), asm_::v(lightmap->lightmap).xxxy());
				}

				if (lightmap && i >= first_code)
				{
					for (auto& operand : instruction.operands)
					{
						redirect_input(operand, lightmap->texcoord, *temp);
					}
				}

				if (pixel_lighting && reads_model_lighting(instruction))
				{
					if (instruction.opcode.type == D3D10_SB_OPCODE_DCL_RESOURCE)
					{
						if (instruction.opcode.controls != D3D10_SB_RESOURCE_DIMENSION_TEXTURE3D)
						{
							throw std::runtime_error("model lighting is not a texture3d");
						}

						declare_light_grid(a, patch);
						continue;
					}

					if (instruction.opcode.type != D3D10_SB_OPCODE_SAMPLE || !is_sample(instruction, patch.lighting->texture))
					{
						throw std::runtime_error("pixel shader reads the model lighting without a sample");
					}

					emit_probe_lookup(a, instruction, patch, *lighting_input, *first_temp + base_temps);
					continue;
				}

				if (sun_shadow.texture && instruction.opcode.type == D3D10_SB_OPCODE_DCL_RESOURCE && reads_sun_shadow(instruction))
				{
					if (instruction.opcode.controls != D3D10_SB_RESOURCE_DIMENSION_TEXTURE2D)
					{
						throw std::runtime_error("sun shadowmap is not a texture2d");
					}

					instruction.opcode.controls = D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DARRAY; // s2 keeps each sun cascade in its own slice
				}
				else if (sun_shadow.texture && is_sample(instruction, *sun_shadow.texture))
				{
					a.mov(asm_::r(*temp).xy(), instruction.operands[1]);
					a.mov(asm_::r(*temp).z(), asm_::l(find_cascade_slice(instructions, i, sun_shadow)));
					instruction.operands[1] = asm_::r(*temp).xyzx().swz();
					set_resource_dimension(instruction, D3D10_SB_RESOURCE_DIMENSION_TEXTURE2DARRAY);
				}
				else if (sun_shadow.texture && reads_sun_shadow(instruction))
				{
					throw std::runtime_error("pixel shader reads the sun shadowmap without a sample");
				}

				for (auto& operand : instruction.operands)
				{
					const auto slot = operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE ? texture_slots.find(operand.indices[0].value.uint32) : texture_slots.end();
					if (slot != texture_slots.end())
					{
						operand.indices[0].value.uint32 = slot->second;
					}
				}

				a(instruction);

				if (lightmap && i == last_input_index)
				{
					a.dcl_input(asm_::v(lightmap->lightmap).xy());
				}

				if (vs_lighting && i == static_cast<std::ptrdiff_t>(vs_lighting->position_write))
				{
					emit_vertex_lighting(a, instruction.operands[1], *patch.lighting, instance_input, *first_temp + base_temps);
				}
			}

			return {result.serialize(), lightmap.has_value(), vs_lighting ? std::optional(vs_lighting->semantic) : std::nullopt, texture_slots};
		}

		template <typename S2Shader, typename H1Shader>
		S2Shader* convert_shader(const H1Shader* shader, const program_patch& patch = {})
		{
			if (!shader)
			{
				return nullptr;
			}

			constexpr auto is_vertex_shader = std::is_same_v<S2Shader, zonetool::s2::MaterialVertexShader>;
			auto suffix = std::string(!patch.lighting ? std::string_view{} : is_vertex_shader ? std::string_view{"_lgv"} : light_grid_suffix(patch.grid));
			if (patch.lightmap_params_dest)
			{
				suffix += "_mlm";
			}
			for (const auto& [buffer, from, to] : patch.stable_literal_rows)
			{
				suffix += utils::string::va("_sl%u_%u_%u", buffer, from, to);
			}
			const auto key = std::make_pair(static_cast<const void*>(shader), suffix);
			if (const auto entry = converted_shaders.find(key); entry != converted_shaders.end())
			{
				return static_cast<S2Shader*>(entry->second);
			}

			auto* result = native_allocator.allocate<S2Shader>();
			result->name = native_allocator.duplicate_string(::game::add_source_postfix(shader->name + key.second, ::game::h1));
			if (shader->prog.loadDef.program) // 0x14080B9F0
			{
				const auto patched = patch_program(shader->prog.loadDef.program, shader->prog.loadDef.programSize, is_vertex_shader, patch);
				result->prog.loadDef.programSize = static_cast<unsigned int>(patched.data.size());
				result->prog.loadDef.program = native_allocator.allocate_array<unsigned char>(patched.data.size());
				std::memcpy(result->prog.loadDef.program, patched.data.data(), patched.data.size());

				if constexpr (is_vertex_shader)
				{
					if (patched.reads_lightmap)
					{
						lightmap_vertex_shaders.insert(result);
					}

					if (patched.lighting_semantic)
					{
						lighting_semantics.emplace(result, *patched.lighting_semantic);
					}
				}
				else if constexpr (std::is_same_v<S2Shader, zonetool::s2::MaterialPixelShader>)
				{
					if (!patched.texture_slots.empty())
					{
						relocated_textures.emplace(result, patched.texture_slots);
					}
				}
			}

			converted_shaders.emplace(key, result);
			return result;
		}

		struct decl_stream
		{
			zonetool::s2::MaterialStreamRouting routing;
			std::string token;
		};

		std::vector<std::string> parse_decl_tokens(const MaterialVertexDeclaration* decl)
		{
			// "pp.ft0t0.3nn.ftt2.f"
			const auto pieces = utils::string::split(decl->name, '.');
			std::vector<std::string> tokens;
			for (auto i = 0u; i + 1 < pieces.size(); i++)
			{
				tokens.emplace_back(i ? pieces[i].substr(1) : pieces[i]);
			}

			if (tokens.size() != decl->streamCount)
			{
				throw std::runtime_error(utils::string::va("vertexdecl \"%s\" does not name its %u streams", decl->name, decl->streamCount));
			}

			return tokens;
		}

		std::uint8_t convert_stream_source(const std::uint8_t source)
		{
			if (source <= STREAM_SRC_TANGENT)
			{
				return source;
			}

			if (source < h1_first_shifted_stream_source)
			{
				throw std::runtime_error(utils::string::va("stream source %u has no s2 mapping", source));
			}

			return static_cast<std::uint8_t>(source + s2_stream_source_shift);
		}

		std::size_t stream_slot_priority(const std::uint8_t source)
		{
			const auto entry = std::ranges::find(stream_slot_order, source);
			return entry != stream_slot_order.end() ? entry - stream_slot_order.begin() : stream_slot_order.size() + source;
		}

		zonetool::s2::MaterialVertexDeclaration* convert_decl(const MaterialVertexDeclaration* decl)
		{
			if (!decl)
			{
				return nullptr;
			}

			if (const auto entry = converted_decls.find(decl); entry != converted_decls.end())
			{
				return entry->second;
			}

			const auto tokens = parse_decl_tokens(decl);
			std::vector<decl_stream> streams;
			for (auto i = 0u; i < decl->streamCount; i++)
			{
				const auto& routing = decl->routing.data[i];
				decl_stream stream{{convert_stream_source(routing.source), routing.dest, routing.mask}, tokens[i]};
				if (routing.source == STREAM_SRC_TEXCOORD_0 && routing.dest == zonetool::s2::STREAM_DST_TEXCOORD_0 && (routing.mask & 0xC))
				{
					stream.routing.mask &= 0x3;
					streams.emplace_back(decl_stream{{zonetool::s2::STREAM_SRC_LIGHTMAP, zonetool::s2::STREAM_DST_TEXCOORD_15, 0x3}, "lmtf"});
				}

				streams.emplace_back(stream);
			}

			auto* result = native_allocator.allocate<zonetool::s2::MaterialVertexDeclaration>();
			if (streams.size() > std::size(result->routing.data))
			{
				throw std::runtime_error(utils::string::va("vertexdecl \"%s\" has too many streams", decl->name));
			}

			std::ranges::stable_sort(streams, {}, [](const decl_stream& stream)
			{
				return stream.routing.source;
			});

			std::vector<std::uint8_t> sources;
			for (const auto& stream : streams)
			{
				if (std::ranges::find(sources, stream.routing.source) == sources.end())
				{
					sources.emplace_back(stream.routing.source);
				}
			}

			std::ranges::stable_sort(sources, {}, stream_slot_priority);

			std::string name;
			for (auto i = 0u; i < streams.size(); i++)
			{
				auto& routing = streams[i].routing;
				routing.slot = static_cast<unsigned char>(std::ranges::find(sources, routing.source) - sources.begin());
				result->routing.data[i] = routing;
				name += utils::string::va("%s.%x.%u", streams[i].token.data(), routing.mask, routing.slot);
			}

			result->name = native_allocator.duplicate_string(name);
			result->streamCount = static_cast<unsigned char>(streams.size());
			result->hasOptionalSource = decl->hasOptionalSource;

			converted_decls.emplace(decl, result);
			return result;
		}

		bool has_lightmap_stream(const zonetool::s2::MaterialVertexDeclaration* decl)
		{
			return decl && std::any_of(decl->routing.data, decl->routing.data + decl->streamCount, [](const zonetool::s2::MaterialStreamRouting& routing)
			{
				return routing.source == zonetool::s2::STREAM_SRC_LIGHTMAP && routing.dest == zonetool::s2::STREAM_DST_TEXCOORD_15;
			});
		}

		bool has_vertex_lighting_stream(const zonetool::s2::MaterialVertexDeclaration* decl)
		{
			return decl && std::any_of(decl->routing.data, decl->routing.data + decl->streamCount, [](const zonetool::s2::MaterialStreamRouting& routing)
			{
				return routing.source >= zonetool::s2::STREAM_SRC_VERTEX_LIGHTING_VISIBILITY && routing.source <= zonetool::s2::STREAM_SRC_VERTEX_LIGHTING_HIGHLIGHT;
			});
		}

		void relocate_texture_args(std::vector<zonetool::s2::MaterialShaderArgument>& args, const std::map<std::uint32_t, std::uint32_t>& slots)
		{
			for (auto& arg : args)
			{
				const auto is_texture = arg.type == zonetool::s2::MTL_ARG_MATERIAL_TEXTURE || arg.type == zonetool::s2::MTL_ARG_CODE_TEXTURE;
				const auto slot = is_texture && (arg.shader & MTL_SHADER_STAGE_PIXEL) ? slots.find(arg.dest) : slots.end();
				if (slot == slots.end())
				{
					continue;
				}

				if (arg.shader != MTL_SHADER_STAGE_PIXEL)
				{
					throw std::runtime_error(utils::string::va("texture t%u of the pixel shader is shared with another stage", arg.dest));
				}

				arg.dest = static_cast<unsigned short>(slot->second);
			}
		}

		// the h1 per prim row of the model lightmap uv scale and offset ("ml" techsets)
		std::optional<std::uint32_t> find_model_lightmap_params(const MaterialPass& pass)
		{
			for (auto i = 0; pass.args && i < pass.perPrimArgCount; i++)
			{
				const auto& arg = pass.args[i];
				if (arg.type == MTL_ARG_CODE_CONST && arg.u.codeConst.index == CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS)
				{
					if (!(arg.shader & MTL_SHADER_STAGE_VERTEX) || arg.u.codeConst.rowCount != 1)
					{
						throw std::runtime_error("model lightmap params are not one vertex shader row");
					}

					return arg.dest;
				}
			}

			return {};
		}

		std::optional<model_lighting> find_model_lighting(const MaterialPass& pass)
		{
			const auto arg_count = pass.args ? static_cast<std::size_t>(pass.perPrimArgCount + pass.perObjArgCount + pass.stableArgCount) : 0u;
			const auto args = std::span{pass.args, arg_count};
			const auto texture = std::ranges::find_if(args, [](const MaterialShaderArgument& arg)
			{
				return arg.type == MTL_ARG_CODE_TEXTURE && arg.u.codeSampler == TEXTURE_SRC_CODE_MODEL_LIGHTING && (arg.shader & MTL_SHADER_STAGE_PIXEL);
			});

			const auto is_code_const = [](const std::int32_t index, const std::uint8_t stage)
			{
				return [=](const MaterialShaderArgument& arg)
				{
					return arg.type == MTL_ARG_CODE_CONST && arg.u.codeConst.index == index && (arg.shader & stage);
				};
			};

			const auto per_prim = args.first(pass.perPrimArgCount);
			const auto stable = args.subspan(pass.perPrimArgCount + pass.perObjArgCount);
			// the h1 "amb" passes give the pixel shader the probe ambient color, per prim or from the instance buffer cb7 like the lighting coords
			const auto ambient = std::ranges::find_if(per_prim, is_code_const(CONST_SRC_CODE_LIGHT_PROBE_AMBIENT, MTL_SHADER_STAGE_VERTEX));
			const auto instance_flag = texture != args.end() ? CUSTOM_BUFFER_SMODEL_AMBIENT : h1_instance_probe_ambient_flag;
			const auto instanced = (pass.customBufferFlags & instance_flag) != 0;
			if (texture == args.end() && ambient == per_prim.end() && !instanced)
			{
				return {};
			}

			if (texture != args.end() && ambient != per_prim.end())
			{
				throw std::runtime_error("model lighting has both the lighting volume and the probe ambient");
			}

			const auto coords = ambient != per_prim.end() ? ambient
				: std::ranges::find_if(per_prim, is_code_const(CONST_SRC_CODE_BASE_LIGHTING_COORDS, MTL_SHADER_STAGE_VERTEX));
			const auto eye_offset = std::ranges::find_if(stable, is_code_const(CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET, MTL_SHADER_STAGE_VERTEX));

			model_lighting result{};
			result.ambient = texture == args.end();
			result.instanced = instanced;
			result.texture = result.ambient ? std::numeric_limits<std::uint16_t>::max() : texture->dest;
			if (!result.instanced && coords == per_prim.end())
			{
				throw std::runtime_error("model lighting has no lighting coords");
			}

			if (eye_offset == stable.end())
			{
				throw std::runtime_error("model lighting vertex shader has no eye offset");
			}

			result.coords_dest = result.instanced ? 0 : coords->dest;
			result.eye_offset_dest = eye_offset->dest;
			return result;
		}

		// every stock 2d/3d "plit_lgv" pass binds the light grid textures as stable code textures (the 0d probe buffers come
		// from the custom buffers), without them the slots keep whatever the previous draw bound and the lighting flickers
		void add_light_grid_textures(std::vector<zonetool::s2::MaterialShaderArgument>& stable, const light_grid grid, const bool sampler)
		{
			if (grid == light_grid::probe_buffer)
			{
				return;
			}

			if (sampler)
			{
				// the h1 ambient passes sample nothing in the light grid, the patched pixel shader samples it with the stock sampler
				zonetool::s2::MaterialShaderArgument arg{};
				arg.type = zonetool::s2::MTL_ARG_CODE_SAMPLER;
				arg.shader = MTL_SHADER_STAGE_PIXEL;
				arg.dest = ambient_light_grid_sampler;
				arg.u.codeSampler = zonetool::s2::TEXTURE_SRC_CODE_LINEAR_CLAMP;
				stable.emplace_back(arg);
			}

			const auto volume_3d = grid == light_grid::volume_3d;
			const std::array<std::pair<std::uint16_t, std::uint32_t>, 2> textures
			{{
				{light_grid_colors_slot, volume_3d ? zonetool::s2::TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_DATA_3D
					: zonetool::s2::TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_DATA_2D},
				{light_grid_visibility_slot, volume_3d ? zonetool::s2::TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_PRIMARY_VIS_DATA_3D
					: zonetool::s2::TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_PRIMARY_VIS_DATA_2D},
			}};

			for (const auto& [slot, texture] : textures)
			{
				zonetool::s2::MaterialShaderArgument arg{};
				arg.type = zonetool::s2::MTL_ARG_CODE_TEXTURE;
				arg.shader = MTL_SHADER_STAGE_PIXEL;
				arg.dest = slot;
				arg.u.codeSampler = texture;
				stable.emplace_back(arg);
			}

			// stock orders the code textures by stage and slot
			std::ranges::stable_sort(stable, {}, [](const zonetool::s2::MaterialShaderArgument& arg)
			{
				const auto is_code_texture = arg.type == zonetool::s2::MTL_ARG_CODE_TEXTURE;
				return std::make_tuple(arg.type, is_code_texture ? arg.shader : 0, is_code_texture ? arg.dest : 0);
			});

			if (stable.size() > std::numeric_limits<unsigned char>::max())
			{
				throw std::runtime_error(utils::string::va("%zu arguments do not fit in one group", stable.size()));
			}
		}

		// the model vertex type of the stock passes in each slot that are neither lit nor vertex lit, it selects the model draw path
		zonetool::s2::MaterialPassModelVertexType stock_model_vertex_type(const std::int32_t slot)
		{
			if (slot >= zonetool::s2::TECHNIQUE_INSTANCED_BEGIN && slot < zonetool::s2::TECHNIQUE_NO_TESS_BEGIN)
			{
				return zonetool::s2::MODEL_VERTEX_TYPE_INSTANCE_DATA;
			}

			switch (slot % zonetool::s2::TECHNIQUE_GROUP_SIZE)
			{
			case zonetool::s2::TECHNIQUE_ZPREPASS:
			case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_RIGID:
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR:
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID:
			case zonetool::s2::TECHNIQUE_VELOCITY_RIGID:
				return zonetool::s2::MODEL_VERTEX_TYPE_PREV_FRAME;
			case zonetool::s2::TECHNIQUE_BUILD_SHADOWMAP_DEPTH:
			case zonetool::s2::TECHNIQUE_UNLIT:
				return zonetool::s2::MODEL_VERTEX_TYPE_INSTANCE_DATA;
			default:
				return zonetool::s2::MODEL_VERTEX_TYPE_SURFACE;
			}
		}

		void convert_pass(const MaterialPass& pass, const light_grid grid, const std::int32_t slot, zonetool::s2::MaterialPass& result)
		{
			const auto lighting = find_model_lighting(pass);
			const auto lightmap_params = find_model_lightmap_params(pass);

			// per prim and per object code consts that s2 has no source for become stable literals, in rows after the h1 stable ones
			const MaterialShaderArgument* per_prim_args = pass.args;
			const auto* per_obj_args = per_prim_args ? per_prim_args + pass.perPrimArgCount : nullptr;
			const auto* stable_args = per_prim_args ? per_obj_args + pass.perObjArgCount : nullptr;
			std::vector<MaterialShaderArgument> per_prim_list(per_prim_args, per_obj_args);
			std::vector<MaterialShaderArgument> per_obj_list(per_obj_args, stable_args);
			std::vector<MaterialShaderArgument> stable_list(stable_args, per_prim_args ? stable_args + pass.stableArgCount : nullptr);
			std::vector<stable_literal_row> stable_literal_rows;
			constexpr auto row_size = 16u;
			auto next_stable_row = (pass.stableArgSize + row_size - 1) / row_size;
			for (const auto& [list, buffer] : {std::pair{&per_prim_list, h1_per_prim_constant_buffer}, std::pair{&per_obj_list, h1_per_object_constant_buffer}})
			{
				std::erase_if(*list, [&](const MaterialShaderArgument& arg)
				{
					const auto index = static_cast<std::int32_t>(arg.u.codeConst.index);
					if (arg.type != MTL_ARG_CODE_CONST || arg.shader != MTL_SHADER_STAGE_VERTEX || arg.u.codeConst.rowCount != 1
						|| code_consts.contains(index) || !code_literals.contains(index))
					{
						return false;
					}

					stable_literal_rows.emplace_back(buffer, arg.dest, next_stable_row);
					auto& moved = stable_list.emplace_back(arg);
					moved.dest = static_cast<unsigned short>(next_stable_row++);
					return true;
				});
			}

			program_patch vertex_patch{{}, lighting};
			vertex_patch.lightmap_params_dest = lightmap_params;
			vertex_patch.stable_literal_rows = stable_literal_rows;
			result.vertexShader = convert_shader<zonetool::s2::MaterialVertexShader>(pass.vertexShader, vertex_patch);
			result.vertexDecl = convert_decl(pass.vertexDecl);
			result.hullShader = convert_shader<zonetool::s2::MaterialHullShader>(pass.hullShader);
			result.domainShader = convert_shader<zonetool::s2::MaterialDomainShader>(pass.domainShader);

			program_patch pixel_patch{find_sun_shadow_lookup(pass), lighting, grid};
			if (lighting)
			{
				const auto semantic = lighting_semantics.find(result.vertexShader);
				if (semantic == lighting_semantics.end())
				{
					throw std::runtime_error("model lighting pass has no vertex shader");
				}

				pixel_patch.lighting_semantic = semantic->second;
			}

			result.pixelShader = convert_shader<zonetool::s2::MaterialPixelShader>(pass.pixelShader, pixel_patch);

			if (lightmap_vertex_shaders.contains(result.vertexShader) && !has_lightmap_stream(result.vertexDecl))
			{
				throw std::runtime_error(utils::string::va("vertex shader \"%s\" reads a lightmap uv that its vertexdecl lacks", pass.vertexShader->name));
			}

			auto per_prim = convert_args(per_prim_list.data(), per_prim_list.size(), false, lighting.has_value());
			auto per_obj = convert_args(per_obj_list.data(), per_obj_list.size(), false, lighting.has_value());
			auto stable = convert_args(stable_list.data(), stable_list.size(), true, lighting.has_value());
			if (const auto slots = relocated_textures.find(result.pixelShader); slots != relocated_textures.end())
			{
				for (auto* group : {&per_prim, &per_obj, &stable})
				{
					relocate_texture_args(*group, slots->second);
				}
			}

			if (lighting)
			{
				add_light_grid_textures(stable, grid, lighting->ambient);
			}

			result.pixelOutputMask = pass.pixelOutputMask;
			result.perPrimArgCount = static_cast<unsigned char>(per_prim.size());
			result.perObjArgCount = static_cast<unsigned char>(per_obj.size());
			result.stableArgCount = static_cast<unsigned char>(stable.size());
			result.perPrimArgSize = pass.perPrimArgSize;
			if (lighting && !lighting->instanced)
			{
				result.perPrimArgSize = static_cast<unsigned short>(std::max(static_cast<std::uint32_t>(result.perPrimArgSize), (s2_model_lighting_index_dest + 1) * row_size));
			}
			if (lightmap_params)
			{
				result.perPrimArgSize = static_cast<unsigned short>(std::max(static_cast<std::uint32_t>(result.perPrimArgSize), (s2_model_lightmap_params_dest + 1) * row_size));
			}
			result.perObjArgSize = pass.perObjArgSize;
			result.stableArgSize = static_cast<unsigned short>(std::max(static_cast<std::uint32_t>(pass.stableArgSize), next_stable_row * row_size));
			result.customBufferFlags = convert_custom_buffer_flags(pass.customBufferFlags, lighting, grid);
			result.customSamplerFlags = pass.customSamplerFlags;
			result.modelVertexType = stock_model_vertex_type(slot);
			if (lightmap_params)
			{
				// model lightmap draws bind the lightmap uv stream, the lightmaps and the per prim lightmap params (sub_1407E95E0)
				result.modelVertexType = zonetool::s2::MODEL_VERTEX_TYPE_LIGHTMAP;
			}
			else if (lighting)
			{
				// the model draw only uploads the light grid volume constants for this type (0x1407E7F40), the stock "plit_lgv" passes use it
				result.modelVertexType = zonetool::s2::MODEL_VERTEX_TYPE_SELF_VISIBILITY;
			}
			else if (has_vertex_lighting_stream(result.vertexDecl))
			{
				result.modelVertexType = zonetool::s2::MODEL_VERTEX_TYPE_VERTEX_LIGHTING;
			}

			const auto arg_count = per_prim.size() + per_obj.size() + stable.size();
			result.args = arg_count ? native_allocator.allocate_array<zonetool::s2::MaterialShaderArgument>(arg_count) : nullptr;
			auto* dest = result.args;
			for (const auto* group : {&per_prim, &per_obj, &stable})
			{
				dest = std::ranges::copy(*group, dest).out;
			}
		}

		zonetool::s2::MaterialTechnique* convert_technique(const MaterialTechnique* technique, const light_grid grid, const std::int32_t slot)
		{
			const auto key = std::make_tuple(technique, grid, stock_model_vertex_type(slot));
			if (const auto entry = converted_techniques.find(key); entry != converted_techniques.end())
			{
				return entry->second;
			}

			const auto size = offsetof(zonetool::s2::MaterialTechnique, passArray) + technique->hdr.passCount * sizeof(zonetool::s2::MaterialPass);
			auto* result = static_cast<zonetool::s2::MaterialTechnique*>(native_allocator.allocate(size));
			result->hdr.name = native_allocator.duplicate_string(technique->hdr.name);
			result->hdr.flags = technique->hdr.flags;
			result->hdr.passCount = technique->hdr.passCount;
			for (auto i = 0u; i < technique->hdr.passCount; i++)
			{
				convert_pass(technique->passArray[i], grid, slot, result->passArray[i]);
			}

			converted_techniques.emplace(key, result);
			return result;
		}

		std::vector<std::int32_t> slot_candidates(const std::int32_t slot)
		{
			if (slot >= zonetool::s2::TECHNIQUE_LIT_BEGIN && slot < zonetool::s2::TECHNIQUE_LIT_END)
			{
				if ((slot - zonetool::s2::TECHNIQUE_LIT_BEGIN) % 2)
				{
					return {TECHNIQUE_LIT_DIR_SHADOW_DFOG, TECHNIQUE_LIT_DIR_DFOG, TECHNIQUE_LIT_DFOG};
				}

				return {TECHNIQUE_LIT_DIR_SHADOW, TECHNIQUE_LIT_DIR, TECHNIQUE_LIT};
			}

			if (slot >= zonetool::s2::TECHNIQUE_EMISSIVE && slot <= zonetool::s2::TECHNIQUE_EMISSIVE_SHADOW_DFOG)
			{
				return {TECHNIQUE_EMISSIVE + slot - zonetool::s2::TECHNIQUE_EMISSIVE};
			}

			switch (slot)
			{
			case zonetool::s2::TECHNIQUE_ZPREPASS:
				return {TECHNIQUE_ZPREPASS};
			case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_RIGID:
				return {TECHNIQUE_ZPREPASS_VELOCITY_RIGID, TECHNIQUE_ZPREPASS};
			case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_SKINNED:
				return {TECHNIQUE_ZPREPASS_VELOCITY_SKINNED, TECHNIQUE_ZPREPASS};
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR:
				return {TECHNIQUE_ZPREPASS_HIDIR};
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID:
				return {TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID, TECHNIQUE_ZPREPASS_HIDIR};
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED:
				return {TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED, TECHNIQUE_ZPREPASS_HIDIR};
			case zonetool::s2::TECHNIQUE_BUILD_SHADOWMAP_DEPTH:
				return {TECHNIQUE_BUILD_SHADOWMAP_DEPTH};
			case zonetool::s2::TECHNIQUE_UNLIT:
			case zonetool::s2::TECHNIQUE_UNLIT_SRGB_GAMMA:
			case zonetool::s2::TECHNIQUE_UNLIT_BT2020_PQ:
				return {TECHNIQUE_UNLIT};
			case zonetool::s2::TECHNIQUE_WIREFRAME_SOLID:
				return {TECHNIQUE_WIREFRAME_SOLID};
			default:
				return {};
			}
		}

		bool has_hull_shader(const MaterialTechnique* technique)
		{
			return std::any_of(technique->passArray, technique->passArray + technique->hdr.passCount, [](const MaterialPass& pass)
			{
				return pass.hullShader != nullptr;
			});
		}

		std::int32_t find_group_technique(const MaterialTechniqueSet* techset, const std::int32_t group, const std::int32_t type)
		{
			const auto exists = [&](const std::int32_t index)
			{
				return techset->techniques[index] != nullptr;
			};

			const auto standard = TECHNIQUE_STANDARD_BEGIN + type;
			const auto instanced = TECHNIQUE_INSTANCED_BEGIN + type;
			const auto no_displacement = TECHNIQUE_NO_DISPLACEMENT_BEGIN + type;
			const auto tessellated = exists(standard) && has_hull_shader(techset->techniques[standard]);

			if (group == zonetool::s2::TECHNIQUE_INSTANCED_BEGIN)
			{
				return exists(instanced) ? instanced : -1;
			}

			if (group == zonetool::s2::TECHNIQUE_NO_TESS_BEGIN)
			{
				if (exists(no_displacement))
				{
					return no_displacement;
				}

				return exists(standard) && !tessellated ? standard : -1;
			}

			if (tessellated && exists(no_displacement))
			{
				return no_displacement;
			}

			return exists(standard) ? standard : -1;
		}

		void convert_slot(const MaterialTechniqueSet* techset, const std::int32_t slot, native_techset& result)
		{
			const auto group = slot - slot % zonetool::s2::TECHNIQUE_GROUP_SIZE;
			std::string error;
			for (const auto type : slot_candidates(slot % zonetool::s2::TECHNIQUE_GROUP_SIZE))
			{
				const auto source = find_group_technique(techset, group, type);
				if (source < 0)
				{
					continue;
				}

				try
				{
					result.techset->techniques[slot] = convert_technique(techset->techniques[source], slot_light_grid(slot), slot);
					result.sources[slot] = source;
					return;
				}
				catch (const std::exception& e)
				{
					error = utils::string::va("technique \"%s\": %s", techset->techniques[source]->hdr.name, e.what());
				}
			}

			// the wireframe debug view is not worth a donor, the displacement ones use custom buffers s2 lacks
			const auto optional = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE == zonetool::s2::TECHNIQUE_WIREFRAME_SOLID;
			if (!error.empty() && !optional)
			{
				throw std::runtime_error(error);
			}
		}

		native_techset build_native(const std::string& name)
		{
			// h1 never instances the reactive motion models, their techsets lack the instanced techniques s2 draws repeated
			// static models with, and s2 has no reactive motion (cb9) either; the stock donor techsets cover them
			if (name.find("_reactivemotion") != std::string::npos)
			{
				throw std::runtime_error("reactive motion uses a donor");
			}

			const auto* techset = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_TECHNIQUE_SET, name).techniqueSet;
			if (!techset)
			{
				throw std::runtime_error("the h1 techset is not loaded");
			}

			if (techset->worldVertFormat >= zonetool::s2::world_vert_format_count)
			{
				throw std::runtime_error(utils::string::va("world vertex format %u has no s2 mapping", techset->worldVertFormat));
			}

			native_techset result{native_allocator.allocate<zonetool::s2::MaterialTechniqueSet>()};
			result.sources.fill(-1);
			result.techset->name = native_allocator.duplicate_string(::game::add_source_postfix(name, ::game::h1));
			result.techset->flags = techset->flags;
			result.techset->worldVertFormat = techset->worldVertFormat;
			result.techset->preDisplacementOnlyCount = techset->preDisplacementOnlyCount;
			for (auto slot = 0; slot < zonetool::s2::technique_count; slot++)
			{
				convert_slot(techset, slot, result);
			}

			const auto empty = std::ranges::all_of(result.sources, [](const std::int32_t source)
			{
				return source < 0;
			});

			if (empty)
			{
				throw std::runtime_error("no technique has an s2 slot");
			}

			return result;
		}
	}

	s2_zone::s2_zone(const std::filesystem::path& path)
		: database_(db_create())
	{
		db_scope _(*this->database_);
		this->zone_ = zonetool::s2::load_zone(path);

		for (const auto& asset : this->zone_->assets)
		{
			if (asset.referenced)
			{
				this->global_assets_.insert(asset.header);
			}
			else
			{
				this->assets_[asset.type].emplace_back(asset.header);
			}

			if (asset.type == zonetool::s2::ASSET_TYPE_TECHNIQUE_SET)
			{
				const auto techset = static_cast<zonetool::s2::MaterialTechniqueSet*>(asset.header);
				this->techsets_.try_emplace(techset->name, techset);
				if (asset.referenced)
				{
					this->global_techsets_.insert(techset->name);
				}
			}
			else if (asset.type == zonetool::s2::ASSET_TYPE_IMAGE && asset.referenced)
			{
				this->global_images_.insert(static_cast<zonetool::s2::GfxImage*>(asset.header)->name);
			}
			else if (asset.type == zonetool::s2::ASSET_TYPE_MATERIAL && !asset.referenced)
			{
				const auto material = static_cast<zonetool::s2::Material*>(asset.header);
				if (!material->techniqueSet || !material->techniqueSet->name || !material->textureTable)
				{
					continue;
				}

				auto& donor = this->donors_[material->techniqueSet->name];
				if (!donor || (donor->parentMaterial && !material->parentMaterial))
				{
					donor = material;
				}
			}
		}
	}

	void* s2_zone::find_asset(const zonetool::s2::XAssetType type) const
	{
		const auto& assets = this->find_assets(type);
		return assets.empty() ? nullptr : assets.front();
	}

	const std::vector<void*>& s2_zone::find_assets(const zonetool::s2::XAssetType type) const
	{
		static const std::vector<void*> none;
		const auto entry = this->assets_.find(type);
		return entry != this->assets_.end() ? entry->second : none;
	}

	zonetool::s2::Material* s2_zone::find_donor(const std::string& techset) const
	{
		const auto donor = this->donors_.find(techset);
		return donor != this->donors_.end() ? donor->second : nullptr;
	}

	zonetool::s2::MaterialTechniqueSet* s2_zone::find_techset(const std::string& techset) const
	{
		const auto entry = this->techsets_.find(techset);
		return entry != this->techsets_.end() ? entry->second : nullptr;
	}

	bool s2_zone::is_global_techset(const std::string& techset) const
	{
		return this->global_techsets_.contains(techset);
	}

	bool s2_zone::is_global_image(const std::string& image) const
	{
		return this->global_images_.contains(image);
	}

	bool s2_zone::is_global_asset(const void* header) const
	{
		return this->global_assets_.contains(header);
	}

	std::vector<std::string> s2_zone::donor_techsets() const
	{
		std::vector<std::string> result;
		for (const auto& [name, _] : this->donors_)
		{
			result.emplace_back(name);
		}

		std::sort(result.begin(), result.end());
		return result;
	}

	database& s2_zone::db() const
	{
		return *this->database_;
	}

	const s2_zone& donor_zone()
	{
		static const s2_zone zone(s2_folder() / "zone" / (std::string(donor_zone_name) + ".ff"));
		return zone;
	}

	std::string select(const std::string& h1_techset)
	{
		const auto source = parse_name(h1_techset, true);
		const auto source_kind = get_draw_kind(source);
		const auto same_kind_and_prefix = [&](const techset_name& candidate)
		{
			return get_draw_kind(candidate) == source_kind && candidate.prefix == source.prefix && lightmap_allowed(source, candidate);
		};

		const auto hidden_same_geometry = [&](const techset_name& candidate)
		{
			return get_draw_kind(candidate) == draw_kind::hidden && is_model_prefix(candidate.prefix) == is_model_prefix(source.prefix);
		};
		const auto hidden = [&](const techset_name& candidate)
		{
			return get_draw_kind(candidate) == draw_kind::hidden;
		};

		const auto drawn_same_prefix = [&](const techset_name& candidate)
		{
			const auto kind = get_draw_kind(candidate);
			return kind != draw_kind::hidden && kind != draw_kind::sky && candidate.prefix == source.prefix && lightmap_allowed(source, candidate);
		};

		// world and model techsets have different vertex types, s2 has no input layout for the other one ("816" fatal error)
		const auto drawn_same_geometry = [&](const techset_name& candidate)
		{
			const auto kind = get_draw_kind(candidate);
			return kind == source_kind && is_model_prefix(candidate.prefix) == is_model_prefix(source.prefix) && lightmap_allowed(source, candidate);
		};

		// h1 grass bends with reactive motion, s2 foliage sways with the "fwa" wind techsets that read the wind stream these surfaces get
		if (h1_techset.find("_reactivemotion") != std::string::npos)
		{
			const auto wind = find_best(source, [&](const techset_name& candidate)
			{
				return std::ranges::find(candidate.tokens, "fwa") != candidate.tokens.end() && drawn_same_geometry(candidate);
			});

			if (wind)
			{
				return *wind;
			}
		}

		const auto filters = source_kind == draw_kind::hidden
			? std::vector<std::function<bool(const techset_name&)>>{same_kind_and_prefix, hidden_same_geometry, hidden}
			: std::vector<std::function<bool(const techset_name&)>>{same_kind_and_prefix, drawn_same_prefix, drawn_same_geometry};

		for (const auto& accept : filters)
		{
			if (const auto best = find_best(source, accept))
			{
				return *best;
			}
		}

		const auto fallback = default_techsets.find(source.prefix);
		if (fallback != default_techsets.end())
		{
			return std::string(fallback->second);
		}

		return std::string(default_techsets.at(is_model_prefix(source.prefix) ? "mco" : "wc"));
	}

	const native_techset* convert_native(const std::string& h1_techset)
	{
		if (const auto entry = native_techsets.find(h1_techset); entry != native_techsets.end())
		{
			return entry->second ? &*entry->second : nullptr;
		}

		auto& entry = native_techsets[h1_techset];
		try
		{
			entry = build_native(h1_techset);
			ZONETOOL_INFO("techset %s -> %s (native)", h1_techset.data(), entry->techset->name);
		}
		catch (const std::exception& e)
		{
			ZONETOOL_WARNING("techset %s needs a donor: %s", h1_techset.data(), e.what());
		}

		return entry ? &*entry : nullptr;
	}

	void dump(zonetool::s2::MaterialTechniqueSet* asset)
	{
		if (!dumped_techsets.insert(asset->name).second)
		{
			return;
		}

		zonetool::s2::techset::dump(asset);
		for (const auto* technique : asset->techniques)
		{
			for (auto i = 0u; technique && i < technique->hdr.passCount; i++)
			{
				const auto& pass = technique->passArray[i];
				dump_shader(pass.vertexShader);
				dump_shader(pass.vertexDecl);
				dump_shader(pass.hullShader);
				dump_shader(pass.domainShader);
				dump_shader(pass.pixelShader);
			}
		}
	}
}
