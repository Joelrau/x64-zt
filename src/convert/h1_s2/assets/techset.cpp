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

		constexpr auto h1_stable_constant_buffer = 2u;
		constexpr auto h1_constant_buffer_count = 5u; // cb4 is the slf buffer
		constexpr auto h1_instance_matrix_buffer = 6u;
		constexpr auto s2_instance_matrix_buffer = 7u;
		constexpr auto h1_instance_row_shift = 2u; // 4 rows for each instance
		constexpr auto s2_instance_row_shift = 3u; // 8 rows for each instance
		constexpr auto h1_first_shifted_stream_source = 6u;
		constexpr auto s2_stream_source_shift = 3u;

		utils::memory::allocator native_allocator;

		const std::unordered_map<std::int32_t, std::int32_t> code_consts = []
		{
			std::unordered_map<std::int32_t, std::int32_t> result
			{
				{CONST_SRC_CODE_GAMETIME, zonetool::s2::CONST_SRC_CODE_GAMETIME},
				{CONST_SRC_CODE_GENERIC_MATERIAL_DATA, zonetool::s2::CONST_SRC_CODE_GENERIC_MATERIAL_DATA},
				{CONST_SRC_CODE_EYEOFFSET, zonetool::s2::CONST_SRC_CODE_EYEOFFSET},
				{CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET, zonetool::s2::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET},
				{CONST_SRC_CODE_LIGHT_POSITION, zonetool::s2::CONST_SRC_CODE_SUN_DIRECTION},
				{CONST_SRC_CODE_LIGHT_DIFFUSE, zonetool::s2::CONST_SRC_CODE_SUN_COLOR},
				{CONST_SRC_CODE_LIGHT_SPECULAR, zonetool::s2::CONST_SRC_CODE_SUN_COLOR},
				{CONST_SRC_CODE_SCRIPT_PARMS, zonetool::s2::CONST_SRC_CODE_SCRIPT_PARMS},
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

		const std::unordered_map<std::int32_t, std::array<float, 4>> fog_off_literals
		{
			{CONST_SRC_CODE_FOG_HEIGHT_FOG, {0.0f, 0.0f, fog_off_height_falloff, 0.0f}},
			{CONST_SRC_CODE_ATMOS_FOG_PARMS_5, {0.0f, 0.0f, fog_off_height_falloff, 0.0f}},
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

		std::unordered_map<const void*, void*> converted_shaders;
		std::unordered_set<const zonetool::s2::MaterialVertexShader*> lightmap_vertex_shaders;
		std::unordered_map<const MaterialVertexDeclaration*, zonetool::s2::MaterialVertexDeclaration*> converted_decls;
		std::unordered_map<const MaterialTechnique*, zonetool::s2::MaterialTechnique*> converted_techniques;
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
			const auto fog_off = fog_off_literals.find(code_const.index);
			result.type = zonetool::s2::MTL_ARG_LITERAL_CONST;
			result.u.literalConst = allocate_literal(fog_off != fog_off_literals.end() ? fog_off->second.data() : zero.data());
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

		void convert_arg(const MaterialShaderArgument& arg, const bool literals_allowed, std::vector<zonetool::s2::MaterialShaderArgument>& args)
		{
			if (arg.type >= MTL_ARG_COUNT)
			{
				throw std::runtime_error(utils::string::va("argument type %u is unknown", arg.type));
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

		std::vector<zonetool::s2::MaterialShaderArgument> convert_args(const MaterialShaderArgument* args, const std::size_t count, const bool literals_allowed)
		{
			std::vector<zonetool::s2::MaterialShaderArgument> result;
			for (auto i = 0u; i < count; i++)
			{
				convert_arg(args[i], literals_allowed, result);
			}

			std::ranges::stable_sort(result, {}, &zonetool::s2::MaterialShaderArgument::type);
			if (result.size() > std::numeric_limits<unsigned char>::max())
			{
				throw std::runtime_error(utils::string::va("%zu arguments do not fit in one group", result.size()));
			}

			return result;
		}

		std::uint64_t convert_custom_buffer_flags(const std::uint32_t flags)
		{
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

		float find_cascade_slice(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::size_t sample, const sun_shadow_lookup& sun_shadow)
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

		void patch_instance_rows(alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input, std::unordered_set<std::uint32_t>& instance_rows)
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

			instruction.operands[2].immediate_values[0].uint32 = s2_instance_row_shift;
			for (auto component = 0u; component < 4; component++)
			{
				if ((operands[0].components.mask >> component) & 1)
				{
					instance_rows.insert(temp_component(operands[0].indices[0].value.uint32, component));
				}
			}
		}

		struct patched_program
		{
			std::string data;
			bool reads_lightmap;
		};

		patched_program patch_program(const unsigned char* program, const unsigned int size, const bool is_vertex_shader, const sun_shadow_lookup& sun_shadow)
		{
			auto shader = alys::shader::shader_object::parse({reinterpret_cast<const char*>(program), size});

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

			std::unordered_set<std::uint32_t> instance_rows;

			const auto last_input_index = instructions.rend() - last_input - 1;
			const auto needs_temp = lightmap.has_value() || sun_shadow.texture.has_value();
			std::optional<std::uint32_t> temp;

			const auto reads_sun_shadow = [&](const alys::shader::detail::instruction_t& instruction)
			{
				return std::ranges::any_of(instruction.operands, [&](const alys::shader::detail::operand_t& operand)
				{
					return is_resource(operand, *sun_shadow.texture);
				});
			};

			auto a = result.get_assembler();
			for (auto i = 0; i < static_cast<std::ptrdiff_t>(instructions.size()); i++)
			{
				auto instruction = instructions[i];
				for (auto& operand : instruction.operands)
				{
					remap_constant_buffers(operand);
				}

				if (instance_input && instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && reads_instance_matrices(instruction.operands[0]))
				{
					auto& rows = instruction.operands[0].indices[1].value.uint32;
					rows = rows >> h1_instance_row_shift << s2_instance_row_shift;
				}
				else if (instance_input && i >= first_code)
				{
					patch_instance_rows(instruction, *instance_input, instance_rows);
				}

				if (needs_temp && instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS)
				{
					temp = instruction.operands[0].custom.u.value++;
				}

				if (needs_temp && i == first_code && !temp)
				{
					temp = 0;
					a.dcl_temps(1u);
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

				a(instruction);

				if (lightmap && i == last_input_index)
				{
					a.dcl_input(asm_::v(lightmap->lightmap).xy());
				}
			}

			return {result.serialize(), lightmap.has_value()};
		}

		template <typename S2Shader, typename H1Shader>
		S2Shader* convert_shader(const H1Shader* shader, const sun_shadow_lookup& sun_shadow = {})
		{
			if (!shader)
			{
				return nullptr;
			}

			if (const auto entry = converted_shaders.find(shader); entry != converted_shaders.end())
			{
				return static_cast<S2Shader*>(entry->second);
			}

			auto* result = native_allocator.allocate<S2Shader>();
			result->name = native_allocator.duplicate_string(::game::add_source_postfix(shader->name, ::game::h1));
			if (shader->prog.loadDef.program) // 0x14080B9F0
			{
				constexpr auto is_vertex_shader = std::is_same_v<S2Shader, zonetool::s2::MaterialVertexShader>;
				const auto patched = patch_program(shader->prog.loadDef.program, shader->prog.loadDef.programSize, is_vertex_shader, sun_shadow);
				result->prog.loadDef.programSize = static_cast<unsigned int>(patched.data.size());
				result->prog.loadDef.program = native_allocator.allocate_array<unsigned char>(patched.data.size());
				std::memcpy(result->prog.loadDef.program, patched.data.data(), patched.data.size());

				if constexpr (is_vertex_shader)
				{
					if (patched.reads_lightmap)
					{
						lightmap_vertex_shaders.insert(result);
					}
				}
			}

			converted_shaders.emplace(shader, result);
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

		void convert_pass(const MaterialPass& pass, zonetool::s2::MaterialPass& result)
		{
			result.vertexShader = convert_shader<zonetool::s2::MaterialVertexShader>(pass.vertexShader);
			result.vertexDecl = convert_decl(pass.vertexDecl);
			result.hullShader = convert_shader<zonetool::s2::MaterialHullShader>(pass.hullShader);
			result.domainShader = convert_shader<zonetool::s2::MaterialDomainShader>(pass.domainShader);
			result.pixelShader = convert_shader<zonetool::s2::MaterialPixelShader>(pass.pixelShader, find_sun_shadow_lookup(pass));

			if (lightmap_vertex_shaders.contains(result.vertexShader) && !has_lightmap_stream(result.vertexDecl))
			{
				throw std::runtime_error(utils::string::va("vertex shader \"%s\" reads a lightmap uv that its vertexdecl lacks", pass.vertexShader->name));
			}

			const auto* per_obj_args = pass.args ? pass.args + pass.perPrimArgCount : nullptr;
			const auto* stable_args = pass.args ? per_obj_args + pass.perObjArgCount : nullptr;
			const auto per_prim = convert_args(pass.args, pass.perPrimArgCount, false);
			const auto per_obj = convert_args(per_obj_args, pass.perObjArgCount, false);
			const auto stable = convert_args(stable_args, pass.stableArgCount, true);

			result.pixelOutputMask = pass.pixelOutputMask;
			result.perPrimArgCount = static_cast<unsigned char>(per_prim.size());
			result.perObjArgCount = static_cast<unsigned char>(per_obj.size());
			result.stableArgCount = static_cast<unsigned char>(stable.size());
			result.perPrimArgSize = pass.perPrimArgSize;
			result.perObjArgSize = pass.perObjArgSize;
			result.stableArgSize = pass.stableArgSize;
			result.customBufferFlags = convert_custom_buffer_flags(pass.customBufferFlags);
			result.customSamplerFlags = pass.customSamplerFlags;
			if (has_vertex_lighting_stream(result.vertexDecl))
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

		zonetool::s2::MaterialTechnique* convert_technique(const MaterialTechnique* technique)
		{
			if (const auto entry = converted_techniques.find(technique); entry != converted_techniques.end())
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
				convert_pass(technique->passArray[i], result->passArray[i]);
			}

			converted_techniques.emplace(technique, result);
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
					result.techset->techniques[slot] = convert_technique(techset->techniques[source]);
					result.sources[slot] = source;
					return;
				}
				catch (const std::exception& e)
				{
					error = utils::string::va("technique \"%s\": %s", techset->techniques[source]->hdr.name, e.what());
				}
			}

			if (!error.empty())
			{
				throw std::runtime_error(error);
			}
		}

		native_techset build_native(const std::string& name)
		{
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
			else if (asset.type == zonetool::s2::ASSET_TYPE_XMODEL_BASE && !asset.referenced && !this->model_base_donor_)
			{
				const auto base = static_cast<zonetool::s2::XModelBase*>(asset.header);
				if (base->numBones == 1 && !base->physRigCount && !base->cloth && !base->rig && !base->imageRefCount)
				{
					this->model_base_donor_ = base;
				}
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

	zonetool::s2::XModelBase* s2_zone::model_base_donor() const
	{
		return this->model_base_donor_;
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

		const auto filters = source_kind == draw_kind::hidden
			? std::vector<std::function<bool(const techset_name&)>>{same_kind_and_prefix, hidden_same_geometry, hidden}
			: std::vector<std::function<bool(const techset_name&)>>{same_kind_and_prefix, drawn_same_prefix};

		for (const auto& accept : filters)
		{
			if (const auto best = find_best(source, accept))
			{
				return *best;
			}
		}

		const auto fallback = default_techsets.find(source.prefix);
		return fallback != default_techsets.end() ? std::string(fallback->second) : std::string(default_techsets.at("mco"));
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
