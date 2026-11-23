#include <std_include.hpp>

#include "../../include.hpp"
#include "args.hpp"

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		const std::unordered_map<std::int32_t, std::int32_t> code_textures
		{
			{TEXTURE_SRC_CODE_BLACK, zonetool::h1::TEXTURE_SRC_CODE_BLACK},
			{TEXTURE_SRC_CODE_WHITE, zonetool::h1::TEXTURE_SRC_CODE_WHITE},
			{TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP, zonetool::h1::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP},
			{TEXTURE_SRC_CODE_SHADOWMAP_SUN, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SUN},
			{TEXTURE_SRC_CODE_SHADOWMAP_SPOT, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT},
			{TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1},
			{TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2},
			{TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3},
			{TEXTURE_SRC_CODE_FEEDBACK, zonetool::h1::TEXTURE_SRC_CODE_FEEDBACK},
			{TEXTURE_SRC_CODE_RESOLVED_POST_SUN, zonetool::h1::TEXTURE_SRC_CODE_RESOLVED_POST_SUN},
			{TEXTURE_SRC_CODE_SSR_BUFFER, zonetool::h1::TEXTURE_SRC_CODE_SSR_BUFFER},
			{TEXTURE_SRC_CODE_HQ_SSR_DEPTH, zonetool::h1::TEXTURE_SRC_CODE_HQ_SSR_DEPTH},
			{TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER, zonetool::h1::TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER},
			{TEXTURE_SRC_CODE_RESOLVED_SCENE, zonetool::h1::TEXTURE_SRC_CODE_RESOLVED_SCENE},
			{TEXTURE_SRC_CODE_POST_EFFECT_0, zonetool::h1::TEXTURE_SRC_CODE_POST_EFFECT_0},
			{TEXTURE_SRC_CODE_POST_EFFECT_1, zonetool::h1::TEXTURE_SRC_CODE_POST_EFFECT_1},
			{TEXTURE_SRC_CODE_OUTDOOR, zonetool::h1::TEXTURE_SRC_CODE_OUTDOOR},
			{TEXTURE_SRC_CODE_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_FLOATZ},
			{TEXTURE_SRC_CODE_PROCESSED_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_BLACK},
			{TEXTURE_SRC_CODE_FLOATZ_STENCIL, zonetool::h1::TEXTURE_SRC_CODE_FLOATZ_STENCIL},
			{TEXTURE_SRC_CODE_CINEMATIC_Y, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_Y},
			{TEXTURE_SRC_CODE_CINEMATIC_CR, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CR},
			{TEXTURE_SRC_CODE_CINEMATIC_CB, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CB},
			{TEXTURE_SRC_CODE_CINEMATIC_A, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_A},
			{TEXTURE_SRC_CODE_CINEMATIC_CBCR, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CBCR},
			{TEXTURE_SRC_CODE_DALTONIZE_CLUT, zonetool::h1::TEXTURE_SRC_CODE_DALTONIZE_CLUT},
			{TEXTURE_SRC_CODE_CLUT, zonetool::h1::TEXTURE_SRC_CODE_CLUT},
			{TEXTURE_SRC_CODE_LINEAR_CLAMP, zonetool::h1::TEXTURE_SRC_CODE_LINEAR_CLAMP},
			{TEXTURE_SRC_CODE_POINT_CLAMP, zonetool::h1::TEXTURE_SRC_CODE_POINT_CLAMP},
			{TEXTURE_SRC_CODE_RANDOM_ROTATIONS, zonetool::h1::TEXTURE_SRC_CODE_RANDOM_ROTATIONS},
			{TEXTURE_SRC_CODE_SSAO, zonetool::h1::TEXTURE_SRC_CODE_WHITE}, // h1 aborts on ssao in material passes
			{TEXTURE_SRC_CODE_MSAA_SCENE, zonetool::h1::TEXTURE_SRC_CODE_MSAA_SCENE},
			{TEXTURE_SRC_CODE_DEPTH_BLUR_0, zonetool::h1::TEXTURE_SRC_CODE_DEPTH_BLUR_0},
			{TEXTURE_SRC_CODE_DEPTH_BLUR_1, zonetool::h1::TEXTURE_SRC_CODE_DEPTH_BLUR_1},
			{TEXTURE_SRC_CODE_DOF_HALF_PREPASS, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_PREPASS},
			{TEXTURE_SRC_CODE_DOF_HALF_COLOR, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_COLOR},
			{TEXTURE_SRC_CODE_DOF_HALF_ALPHA, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_ALPHA},
			{TEXTURE_SRC_CODE_DOF_TILE0, zonetool::h1::TEXTURE_SRC_CODE_DOF_TILE0},
			{TEXTURE_SRC_CODE_DOF_TILE1, zonetool::h1::TEXTURE_SRC_CODE_DOF_TILE1},
			{TEXTURE_SRC_CODE_SCENE_SPECULAR, zonetool::h1::TEXTURE_SRC_CODE_SCENE_SPECULAR},
			{TEXTURE_SRC_CODE_SCENE_ALBEDO, zonetool::h1::TEXTURE_SRC_CODE_SCENE_ALBEDO},
			{TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA, zonetool::h1::TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA},
			{TEXTURE_SRC_CODE_UI3D, zonetool::h1::TEXTURE_SRC_CODE_UI3D},
			{TEXTURE_SRC_CODE_FONT_CACHE, zonetool::h1::TEXTURE_SRC_CODE_FONT_CACHE},
		};

		const std::unordered_map<std::int32_t, std::int32_t> code_consts = []
		{
			std::unordered_map<std::int32_t, std::int32_t> result
			{
				{CONST_SRC_CODE_GAMETIME, zonetool::h1::CONST_SRC_CODE_GAMETIME},
				{CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS, zonetool::h1::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS},
				{CONST_SRC_CODE_ZNEAR, zonetool::h1::CONST_SRC_CODE_ZNEAR},
				{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
				{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
				{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
				{CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
				{CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM, zonetool::h1::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM},
				{CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS, zonetool::h1::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS},
				{CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET},
				{CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS, zonetool::h1::CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS},
				{CONST_SRC_CODE_GENERIC_MATERIAL_DATA, zonetool::h1::CONST_SRC_CODE_GENERIC_MATERIAL_DATA},
				{CONST_SRC_CODE_EYEOFFSET, zonetool::h1::CONST_SRC_CODE_EYEOFFSET},
				{CONST_SRC_CODE_VIEW_WORLD_ORIGIN, zonetool::h1::CONST_SRC_CODE_EYEOFFSET},
				{CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET},
				{CONST_SRC_CODE_SUN_DIRECTION, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION},
				{CONST_SRC_CODE_SUN_COLOR, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE},
				{CONST_SRC_CODE_EFFECT_MODEL_COLOR, zonetool::h1::CONST_SRC_CODE_EFFECT_MODEL_COLOR},
				{CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE, zonetool::h1::CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE},
				{CONST_SRC_CODE_RENDER_TARGET_SIZE, zonetool::h1::CONST_SRC_CODE_RENDER_TARGET_SIZE},
				{CONST_SRC_CODE_NEARPLANE_ORG, zonetool::h1::CONST_SRC_CODE_NEARPLANE_ORG},
				{CONST_SRC_CODE_NEARPLANE_DX, zonetool::h1::CONST_SRC_CODE_NEARPLANE_DX},
				{CONST_SRC_CODE_NEARPLANE_DY, zonetool::h1::CONST_SRC_CODE_NEARPLANE_DY},
				{CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF, zonetool::h1::CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF},
				{CONST_SRC_CODE_HDR_STAGE_PARMS, zonetool::h1::CONST_SRC_CODE_HDR_STAGE_PARMS},
				{CONST_SRC_CODE_MATERIAL_COLOR, zonetool::h1::CONST_SRC_CODE_MATERIAL_COLOR},
				{CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET},
				{CONST_SRC_CODE_MODEL_VELOCITY_SKINNED_PARMS, zonetool::h1::CONST_SRC_CODE_MODEL_VELOCITY_SKINNED_PARMS},
				{CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS, zonetool::h1::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS},
				{CONST_SRC_CODE_GUN_RETICLE_COLOR, zonetool::h1::CONST_SRC_CODE_GUN_RETICLE_COLOR},
				{CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE},
				{CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET},
				{CONST_SRC_CODE_DEPTH_FROM_CLIP, zonetool::h1::CONST_SRC_CODE_DEPTH_FROM_CLIP},
				{CONST_SRC_CODE_INV_SCENE_PROJECTION, zonetool::h1::CONST_SRC_CODE_INV_SCENE_PROJECTION},
				{CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS, zonetool::h1::CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS},
				{CONST_SRC_CODE_ATMOS_FOG_PARMS_0, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_0},
				{CONST_SRC_CODE_ATMOS_FOG_PARMS_1, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_1},
				{CONST_SRC_CODE_ATMOS_FOG_PARMS_2, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_2},
				{CONST_SRC_CODE_ATMOS_FOG_PARMS_3, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_3},
				{CONST_SRC_CODE_ATMOS_FOG_PARMS_4, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_4},
				{CONST_SRC_CODE_ATMOS_FOG_PARMS_5, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_5},
				{CONST_SRC_CODE_CODE_MESH_ARG_0, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_0},
				{CONST_SRC_CODE_CODE_MESH_ARG_1, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_1},
				{CONST_SRC_CODE_CODE_MESH_ARG_3, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_3},
				{CONST_SRC_CODE_CODE_MESH_ARG_4, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_4},
			};

			const auto add_range = [&](const std::int32_t first, const std::int32_t last, const std::int32_t h1_first)
			{
				for (auto index = first; index <= last; index++)
				{
					result.emplace(index, h1_first + index - first);
				}
			};

#define ADD_RANGE(first, last) \
			static_assert(CONST_SRC_CODE_##last - CONST_SRC_CODE_##first == zonetool::h1::CONST_SRC_CODE_##last - zonetool::h1::CONST_SRC_CODE_##first); \
			add_range(CONST_SRC_CODE_##first, CONST_SRC_CODE_##last, zonetool::h1::CONST_SRC_CODE_##first)

			ADD_RANGE(PREV_FRAME_VIEWPROJECTION_MATRIX_R0, PREV_FRAME_VIEWPROJECTION_MATRIX_R3);
			ADD_RANGE(SHADOWMAP_SWITCH_PARTITION_ARRAY_0, SHADOWMAP_DISTANCE_BLEND);
			ADD_RANGE(SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0, SUN_SHADOWMAP_NEAR_FAR_PLANE);
			ADD_RANGE(VIEW_MATRIX, INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX);
			ADD_RANGE(SHADOW_LOOKUP_MATRIX, INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX);
			ADD_RANGE(WORLD_OUTDOOR_LOOKUP_MATRIX, INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX);
			ADD_RANGE(PARTICLE_CLOUD_TEXTURE_ATLAS_SETTINGS, PARTICLE_CLOUD_SPARK_COLOR2);
			ADD_RANGE(PREV_FRAME_WORLD_MATRIX, INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2);
#undef ADD_RANGE

			return result;
		}();

		const auto code_literals = []
		{
			std::unordered_map<std::int32_t, std::array<float, 4>> result
			{
				{CONST_SRC_CODE_TEMPORAL_JITTER_PARAMS, {0.0f, 0.0f, 0.0f, 0.0f}},
				{CONST_SRC_CODE_WORLD_MATRIX_INV_SCALE_SQ, {1.0f, 1.0f, 1.0f, 1.0f}}, // TODO: exact for world surfaces only, scaled or mirrored models need the model scale
				{CONST_SRC_CODE_FRAME_COUNT_PARAMS, {1.0f, 0.0f, 0.0f, 0.0f}},
				{CONST_SRC_CODE_SSR_QUALITY_PARAMS, {0.0f, 0.0f, 0.0f, 0.0f}},
				{CONST_SRC_CODE_VOLUMETRIC_DEPTH, {0.0f, 0.0f, 0.0f, 0.0f}},
				{CONST_SRC_CODE_VOLUMETRIC_DEPTH_RCP, {0.0f, 0.0f, 0.0f, 0.0f}},
				{CONST_SRC_CODE_FRUSTUM_GRID_CLUSTER_SIZE, {0.0f, 0.0f, 0.0f, 0.0f}},
				{CONST_SRC_CODE_FRUSTUM_GRID_LIGHT_PARAMS, {0.0f, 0.0f, 0.0f, 0.0f}},
				{CONST_SRC_CODE_ATMOS_FOG_PARMS_7, {0.0f, 1.0f, std::numeric_limits<float>::max(), std::numeric_limits<float>::max()}},
				{CONST_SRC_CODE_VDH_HEIGHT_RANGE, {300.0f, 600.0f, 0.0f, 0.0f}}, // TODO: map value (2.5 0x1407F3586)
			};

			for (std::int32_t index = CONST_SRC_CODE_AIR_LIGHT_PARAMS; index <= CONST_SRC_CODE_AIR_LIGHT_ATTENUATION_7; index++)
			{
				result.emplace(index, std::array{0.0f, 0.0f, 0.0f, 0.0f});
			}

			return result;
		}();

		constexpr std::array<std::pair<std::uint64_t, std::uint32_t>, 6> custom_buffers
		{{
			{CUSTOM_BUFFER_PER_PRIM, zonetool::h1::CUSTOM_BUFFER_PER_PRIM},
			{CUSTOM_BUFFER_PER_OBJECT, zonetool::h1::CUSTOM_BUFFER_PER_OBJECT},
			{CUSTOM_BUFFER_PER_STABLE, zonetool::h1::CUSTOM_BUFFER_PER_STABLE},
			{CUSTOM_BUFFER_MATERIAL, zonetool::h1::CUSTOM_BUFFER_MATERIAL},
			{CUSTOM_BUFFER_INSTANCE_MATRICES, zonetool::h1::CUSTOM_BUFFER_SMODEL_LIGHTING},
			{CUSTOM_BUFFER_SLF, zonetool::h1::CUSTOM_BUFFER_SLF},
		}};

		std::uint8_t convert_arg_type(const std::uint8_t type)
		{
			switch (type)
			{
			case MTL_ARG_CODE_CONST:
				return zonetool::h1::MTL_ARG_CODE_CONST;
			case MTL_ARG_CODE_TEXTURE:
				return zonetool::h1::MTL_ARG_CODE_TEXTURE;
			case MTL_ARG_CODE_SAMPLER:
				return zonetool::h1::MTL_ARG_CODE_SAMPLER;
			case MTL_ARG_MATERIAL_CONST:
				return zonetool::h1::MTL_ARG_MATERIAL_CONST;
			case MTL_ARG_LITERAL_CONST:
				return zonetool::h1::MTL_ARG_LITERAL_CONST;
			case MTL_ARG_MATERIAL_TEXTURE:
				return zonetool::h1::MTL_ARG_MATERIAL_TEXTURE;
			case MTL_ARG_MATERIAL_SAMPLER:
				return zonetool::h1::MTL_ARG_MATERIAL_SAMPLER;
			default:
				throw std::runtime_error(utils::string::va("argument type %u has no h1 type", type));
			}
		}

		zonetool::h1::MaterialShaderArgument convert_arg(const MaterialShaderArgument& arg, utils::memory::allocator& allocator)
		{
			zonetool::h1::MaterialShaderArgument result{};
			result.type = convert_arg_type(arg.type);
			result.shader = arg.shader;
			result.dest = arg.dest;

			switch (arg.type)
			{
			case MTL_ARG_CODE_CONST:
			{
				const auto entry = code_consts.find(arg.u.codeConst.index);
				if (entry == code_consts.end())
				{
					const auto literal = code_literals.find(arg.u.codeConst.index);
					if (literal == code_literals.end() || arg.u.codeConst.rowCount != 1)
					{
						throw std::runtime_error(utils::string::va("code constant %u has no h1 source", arg.u.codeConst.index));
					}

					static std::unordered_set<std::uint16_t> logged;
					if (logged.insert(arg.u.codeConst.index).second)
					{
						ZONETOOL_INFO("native techsets: code constant %u is a literal in h1", arg.u.codeConst.index);
					}

					result.type = zonetool::h1::MTL_ARG_LITERAL_CONST;
					result.u.literalConst = allocator.allocate_array<float>(4);
					std::memcpy(result.u.literalConst, literal->second.data(), sizeof(float) * 4);
					break;
				}

				result.u.codeConst = {static_cast<unsigned short>(entry->second), arg.u.codeConst.firstRow, arg.u.codeConst.rowCount};
				break;
			}
			case MTL_ARG_CODE_TEXTURE:
			case MTL_ARG_CODE_SAMPLER:
			{
				const auto entry = code_textures.find(static_cast<std::int32_t>(arg.u.codeSampler));
				if (entry == code_textures.end())
				{
					throw std::runtime_error(utils::string::va("code texture %u has no h1 source", arg.u.codeSampler));
				}

				result.u.codeSampler = static_cast<unsigned int>(entry->second);
				break;
			}
			case MTL_ARG_LITERAL_CONST:
				result.u.literalConst = allocator.allocate_array<float>(4);
				std::memcpy(result.u.literalConst, arg.u.literalConst, sizeof(float) * 4);
				break;
			default:
				result.u.nameHash = arg.u.nameHash;
				break;
			}

			return result;
		}

		std::vector<zonetool::h1::MaterialShaderArgument> convert_group(const MaterialShaderArgument* args, const std::size_t count, utils::memory::allocator& allocator)
		{
			std::vector<zonetool::h1::MaterialShaderArgument> result;
			for (auto i = 0u; i < count; i++)
			{
				result.emplace_back(convert_arg(args[i], allocator));
			}

			std::ranges::stable_sort(result, {}, &zonetool::h1::MaterialShaderArgument::type);
			return result;
		}

		bool is_code_arg(const zonetool::h1::MaterialShaderArgument& arg)
		{
			return arg.type <= zonetool::h1::MTL_ARG_CODE_SAMPLER;
		}

		std::uint32_t arg_rows(const zonetool::h1::MaterialShaderArgument& arg)
		{
			return arg.type == zonetool::h1::MTL_ARG_CODE_CONST ? std::max<std::uint32_t>(arg.u.codeConst.rowCount, 1) : 1;
		}

		bool uses_constant_rows(const zonetool::h1::MaterialShaderArgument& arg)
		{
			return arg.type == zonetool::h1::MTL_ARG_CODE_CONST || arg.type == zonetool::h1::MTL_ARG_LITERAL_CONST;
		}
	}

	converted_args convert_args(const MaterialPass& pass, const std::uint32_t reserved_stable_rows, const bool sun_light_array, utils::memory::allocator& allocator)
	{
		if (pass.volatileArgCount)
		{
			throw std::runtime_error(utils::string::va("%u volatile arguments", pass.volatileArgCount));
		}

		converted_args result;
		const auto* args = pass.args;
		result.per_prim = convert_group(args, pass.perPrimArgCount, allocator);
		result.per_obj = convert_group(args + pass.perPrimArgCount, pass.perObjArgCount, allocator);
		result.stable = convert_group(args + pass.perPrimArgCount + pass.perObjArgCount, pass.stableArgCount, allocator);

		if (sun_light_array)
		{
			for (auto* group : {&result.per_prim, &result.per_obj, &result.stable})
			{
				for (auto& arg : *group)
				{
					if (arg.type != zonetool::h1::MTL_ARG_CODE_CONST)
					{
						continue;
					}

					if (arg.u.codeConst.index == zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION)
					{
						arg.u.codeConst.index = zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_0;
					}
					else if (arg.u.codeConst.index == zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE)
					{
						arg.u.codeConst.index = zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_0;
					}
				}
			}
		}

		auto stable_rows = std::max(static_cast<std::uint32_t>((pass.stableArgSize + 15) / 16), reserved_stable_rows);
		for (const auto& arg : result.stable)
		{
			if (uses_constant_rows(arg))
			{
				stable_rows = std::max(stable_rows, arg.dest + arg_rows(arg));
			}
		}

		for (const auto& [buffer, group] : {std::pair{0u, &result.per_prim}, std::pair{1u, &result.per_obj}})
		{
			for (auto arg = group->begin(); arg != group->end();)
			{
				if (arg->type != zonetool::h1::MTL_ARG_LITERAL_CONST)
				{
					++arg;
					continue;
				}

				result.moves.emplace_back(arg->shader, buffer, arg->dest, stable_rows);
				arg->dest = static_cast<unsigned short>(stable_rows++);
				result.stable.emplace_back(*arg);
				arg = group->erase(arg);
			}

			if (!std::ranges::all_of(*group, is_code_arg))
			{
				throw std::runtime_error("a per prim or per object argument is not a code argument");
			}
		}

		std::ranges::stable_sort(result.stable, {}, &zonetool::h1::MaterialShaderArgument::type);
		result.stable_size = static_cast<unsigned short>(result.moves.empty() ? pass.stableArgSize : stable_rows * 16);
		return result;
	}

	std::uint32_t convert_custom_buffer_flags(const std::uint64_t flags)
	{
		std::uint32_t result{};
		auto unknown = flags;
		for (const auto& [s2_flag, h1_flag] : custom_buffers)
		{
			if (flags & s2_flag)
			{
				result |= h1_flag;
				unknown &= ~s2_flag;
			}
		}

		if (unknown)
		{
			throw std::runtime_error(utils::string::va("custom buffer flags 0x%llX have no h1 mapping", unknown));
		}

		return result;
	}
}
