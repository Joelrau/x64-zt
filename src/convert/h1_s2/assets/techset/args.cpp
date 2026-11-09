#include <std_include.hpp>

#include "../../include.hpp"
#include "args.hpp"
#include "common.hpp"

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		const std::unordered_map<std::int32_t, std::int32_t> code_textures
		{
			{TEXTURE_SRC_CODE_BLACK, zonetool::s2::TEXTURE_SRC_CODE_BLACK},
			{TEXTURE_SRC_CODE_WHITE, zonetool::s2::TEXTURE_SRC_CODE_WHITE},
			{TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP, zonetool::s2::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP},
			{TEXTURE_SRC_CODE_SHADOWMAP_SUN, zonetool::s2::TEXTURE_SRC_CODE_SHADOWMAP_SUN},
			{TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN, zonetool::s2::TEXTURE_SRC_CODE_WHITE},
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

		std::uint64_t light_grid_buffer_flags(const light_grid grid)
		{
			constexpr auto common = zonetool::s2::CUSTOM_BUFFER_FORWARD_LIGHTING | zonetool::s2::CUSTOM_BUFFER_LIGHT_GRID_VOLUME;
			return grid == light_grid::probe_buffer ? common | zonetool::s2::CUSTOM_BUFFER_LIGHT_GRID_PROBES : common;
		}

		constexpr std::array<std::pair<std::uint32_t, std::uint64_t>, 7> custom_buffers
		{{
			{CUSTOM_BUFFER_PER_PRIM, zonetool::s2::CUSTOM_BUFFER_PER_PRIM},
			{CUSTOM_BUFFER_PER_OBJECT, zonetool::s2::CUSTOM_BUFFER_PER_OBJECT},
			{CUSTOM_BUFFER_PER_STABLE, zonetool::s2::CUSTOM_BUFFER_PER_STABLE},
			{CUSTOM_BUFFER_MATERIAL, zonetool::s2::CUSTOM_BUFFER_MATERIAL},
			{CUSTOM_BUFFER_SMODEL_LIGHTING, zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES},
			{CUSTOM_BUFFER_SMODEL_AMBIENT, zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES},
			{CUSTOM_BUFFER_SLF, zonetool::s2::CUSTOM_BUFFER_SLF},
		}};

		void add_code_const(const MaterialShaderArgument& arg, const bool literals_allowed, std::vector<zonetool::s2::MaterialShaderArgument>& args)
		{
			const auto& code_const = arg.u.codeConst;
			if (code_const.index < CONST_SRC_CODE_VIEW_MATRIX && code_const.rowCount != 1)
			{
				throw std::runtime_error(utils::string::va("code constant %u has %u rows", code_const.index, code_const.rowCount));
			}

			auto result = make_arg(zonetool::s2::MTL_ARG_CODE_CONST, arg.shader, arg.dest);
			if (code_const.index == CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS)
			{
				result.dest = shader::s2_model_lightmap_params_dest;
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
			static std::unordered_set<std::uint16_t> warned;
			if (literal == code_literals.end() && warned.insert(code_const.index).second)
			{
				ZONETOOL_WARNING("code constant %u has no s2 source, the shaders read zero", code_const.index);
			}

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
				auto result = make_arg(zonetool::s2::MTL_ARG_CODE_CONST, arg.shader, shader::s2_model_lighting_index_dest);
				result.u.codeConst = {zonetool::s2::CONST_SRC_CODE_MODEL_LIGHTING_INDEX, 0, 1};
				args.emplace_back(result);
				return;
			}

			if (model_lighting && arg.type == MTL_ARG_CODE_TEXTURE && arg.u.codeSampler == TEXTURE_SRC_CODE_MODEL_LIGHTING)
			{
				return;
			}

			if (arg.type == MTL_ARG_CODE_CONST)
			{
				add_code_const(arg, literals_allowed, args);
				return;
			}

			auto result = make_arg(argument_types[arg.type], arg.shader, arg.dest);
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
	}

	const std::unordered_map<std::int32_t, std::int32_t> code_consts = []
	{
		std::unordered_map<std::int32_t, std::int32_t> result
		{
			{CONST_SRC_CODE_GAMETIME, zonetool::s2::CONST_SRC_CODE_GAMETIME},
			{CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS, zonetool::s2::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS},
			{CONST_SRC_CODE_ZNEAR, zonetool::s2::CONST_SRC_CODE_ZNEAR},
			{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::s2::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
			{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::s2::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
			{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::s2::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
			{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::s2::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
			{CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM, zonetool::s2::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM},
			{CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS, zonetool::s2::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS},
			{CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET, zonetool::s2::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET},
			{CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS, zonetool::s2::CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS},
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
			{CONST_SRC_CODE_MODEL_VELOCITY_SKINNED_PARMS, zonetool::s2::CONST_SRC_CODE_MODEL_VELOCITY_SKINNED_PARMS},
			{CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS, zonetool::s2::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS},
			{CONST_SRC_CODE_GUN_RETICLE_COLOR, zonetool::s2::CONST_SRC_CODE_GUN_RETICLE_COLOR},
			{CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, zonetool::s2::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE},
			{CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, zonetool::s2::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET},
			{CONST_SRC_CODE_DEPTH_FROM_CLIP, zonetool::s2::CONST_SRC_CODE_DEPTH_FROM_CLIP},
			{CONST_SRC_CODE_INV_SCENE_PROJECTION, zonetool::s2::CONST_SRC_CODE_INV_SCENE_PROJECTION},
			{CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS, zonetool::s2::CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS},
			{CONST_SRC_CODE_ATMOS_FOG_PARMS_0, zonetool::s2::CONST_SRC_CODE_ATMOS_FOG_PARMS_0},
			{CONST_SRC_CODE_ATMOS_FOG_PARMS_1, zonetool::s2::CONST_SRC_CODE_ATMOS_FOG_PARMS_1},
			{CONST_SRC_CODE_ATMOS_FOG_PARMS_2, zonetool::s2::CONST_SRC_CODE_ATMOS_FOG_PARMS_2},
			{CONST_SRC_CODE_ATMOS_FOG_PARMS_3, zonetool::s2::CONST_SRC_CODE_ATMOS_FOG_PARMS_3},
			{CONST_SRC_CODE_ATMOS_FOG_PARMS_4, zonetool::s2::CONST_SRC_CODE_ATMOS_FOG_PARMS_4},
			{CONST_SRC_CODE_ATMOS_FOG_PARMS_5, zonetool::s2::CONST_SRC_CODE_ATMOS_FOG_PARMS_5},
			{CONST_SRC_CODE_CODE_MESH_ARG_0, zonetool::s2::CONST_SRC_CODE_CODE_MESH_ARG_0},
			{CONST_SRC_CODE_CODE_MESH_ARG_1, zonetool::s2::CONST_SRC_CODE_CODE_MESH_ARG_1},
			{CONST_SRC_CODE_CODE_MESH_ARG_3, zonetool::s2::CONST_SRC_CODE_CODE_MESH_ARG_3},
			{CONST_SRC_CODE_CODE_MESH_ARG_4, zonetool::s2::CONST_SRC_CODE_CODE_MESH_ARG_4},
		};

		const auto add_range = [&](const std::int32_t first, const std::int32_t last, const std::int32_t s2_first)
		{
			for (auto index = first; index <= last; index++)
			{
				result.emplace(index, s2_first + index - first);
			}
		};

		add_range(CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R3,
			zonetool::s2::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R0);
		add_range(CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0, CONST_SRC_CODE_SHADOWMAP_DISTANCE_BLEND,
			zonetool::s2::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0);
		add_range(CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0, CONST_SRC_CODE_SUN_SHADOWMAP_NEAR_FAR_PLANE,
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

	const std::unordered_map<std::int32_t, std::array<float, 4>> code_literals
	{
		{CONST_SRC_CODE_FOG_HEIGHT_FOG, {0.0f, 0.0f, 1.0f / 8192.0f, 0.0f}},
		{CONST_SRC_CODE_FOG, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_FOG_COLOR_LINEAR, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_FOG_COLOR_GAMMA, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_FOG_SUN_CONSTS, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_FOG_SUN_DIR, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_TESSELLATION_PARMS, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_TESSELLATION_PARMS2, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_SCENE_UV_MULT, {1.0f, 1.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_PARTICLE_CLOUD_COLOR, {1.0f, 1.0f, 1.0f, 1.0f}},
		{CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE, {1.0f, 1.0f, 1.0f, 0.0f}},
		{CONST_SRC_CODE_SCRIPT_PARMS, {0.0f, 0.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_RIM_LIGHT_0_DIR, {0.0f, 0.0f, 1.0f, 0.0f}}, // keeps _mpr shaders from dividing by zero
		{CONST_SRC_CODE_RIM_LIGHT_0_COLOR, {0.0f, 0.0f, 0.0f, 1.0f}},
		{CONST_SRC_CODE_RIM_LIGHT_1_DIR, {0.0f, 1.0f, 0.0f, 0.0f}},
		{CONST_SRC_CODE_RIM_LIGHT_1_COLOR, {0.0f, 0.0f, 0.0f, 1.0f}},
	};

	float* allocate_literal(const float* value)
	{
		auto* result = native_allocator.allocate_array<float>(4);
		std::memcpy(result, value, sizeof(float) * 4);
		return result;
	}

	zonetool::s2::MaterialShaderArgument make_arg(const zonetool::s2::MaterialShaderArgumentType type, const unsigned char stage, const unsigned short dest)
	{
		zonetool::s2::MaterialShaderArgument arg;
		std::memset(&arg, 0, sizeof(arg));
		arg.type = type;
		arg.shader = stage;
		arg.dest = dest;
		return arg;
	}

	void check_group_size(const std::vector<zonetool::s2::MaterialShaderArgument>& args)
	{
		if (args.size() > std::numeric_limits<unsigned char>::max())
		{
			throw std::runtime_error(utils::string::va("%zu arguments do not fit in one group", args.size()));
		}
	}

	unsigned short arg_size(const std::uint32_t size, const std::uint32_t rows)
	{
		return static_cast<unsigned short>(std::max(size, rows * constant_row_size));
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
		check_group_size(result);
		return result;
	}

	std::uint64_t convert_custom_buffer_flags(const std::uint32_t flags, const std::optional<model_lighting>& lighting, const light_grid grid)
	{
		if (lighting)
		{
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
}
