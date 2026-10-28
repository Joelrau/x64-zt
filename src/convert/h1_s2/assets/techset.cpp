#include <std_include.hpp>

#include "../include.hpp"
#include "techset.hpp"
#include "shader.hpp"

#include <utils/compression.hpp>

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		using shader::light_grid;
		using shader::model_lighting;
		using shader::program_patch;
		using shader::stable_literal_row;
		using shader::sun_shadow_lookup;

		std::unordered_set<std::string> dumped_techsets;
		template <typename T>
		std::unordered_set<std::string> dumped_shaders;

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

			const auto global = stock_zone().is_global_asset(shader);
			if (!global)
			{
				dump_shader_file(shader);
			}

			add_csv_row(shader_csv_type<T>(), shader->name, global);
		}

		constexpr auto h1_per_prim_constant_buffer = 0u;
		constexpr auto h1_per_object_constant_buffer = 1u;
		constexpr auto constant_row_size = 16u;
		constexpr auto h1_first_shifted_stream_source = 6u;
		constexpr auto s2_stream_source_shift = 3u;
		constexpr auto h1_instance_probe_ambient_flag = 0x400u;

		utils::memory::allocator native_allocator;

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

		const std::unordered_map<std::int32_t, std::int32_t> code_textures
		{
			{TEXTURE_SRC_CODE_BLACK, zonetool::s2::TEXTURE_SRC_CODE_BLACK},
			{TEXTURE_SRC_CODE_WHITE, zonetool::s2::TEXTURE_SRC_CODE_WHITE},
			{TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP, zonetool::s2::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP},
			{TEXTURE_SRC_CODE_SHADOWMAP_SUN, zonetool::s2::TEXTURE_SRC_CODE_SHADOWMAP_SUN},
			{TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN, zonetool::s2::TEXTURE_SRC_CODE_WHITE}, // h1 shadow is saturate(sun + trans - 1), white keeps the sun term
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
			{CONST_SRC_CODE_SCRIPT_PARMS, {0.0f, 0.0f, 0.0f, 0.0f}}, // the s2 script parms hold other data
			// s2 has no rim lights, these values keep the _mpr shaders from dividing by zero
			{CONST_SRC_CODE_RIM_LIGHT_0_DIR, {0.0f, 0.0f, 1.0f, 0.0f}},
			{CONST_SRC_CODE_RIM_LIGHT_0_COLOR, {0.0f, 0.0f, 0.0f, 1.0f}},
			{CONST_SRC_CODE_RIM_LIGHT_1_DIR, {0.0f, 1.0f, 0.0f, 0.0f}},
			{CONST_SRC_CODE_RIM_LIGHT_1_COLOR, {0.0f, 0.0f, 0.0f, 1.0f}},
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
		std::map<std::pair<const MaterialVertexDeclaration*, bool>, zonetool::s2::MaterialVertexDeclaration*> converted_decls;
		std::map<std::pair<const MaterialVertexShader*, std::optional<std::uint32_t>>, MaterialVertexShader*> instanced_vertex_shaders;
		std::map<std::tuple<const MaterialTechnique*, light_grid, zonetool::s2::MaterialPassModelVertexType, bool>, zonetool::s2::MaterialTechnique*> converted_techniques;
		std::map<std::pair<const zonetool::s2::MaterialTechnique*, const MaterialPass*>, zonetool::s2::MaterialTechnique*> light_pass_techniques;
		std::map<std::tuple<const zonetool::s2::MaterialTechnique*, const MaterialPass*, bool>, zonetool::s2::MaterialTechnique*> merged_light_techniques;

		constexpr auto custom_buffer_forward_cluster_lighting = zonetool::s2::CUSTOM_BUFFER_FORWARD_LIGHTS | zonetool::s2::CUSTOM_BUFFER_LIGHT_CLUSTER_BITS;
		constexpr auto custom_buffer_light_tiles = zonetool::s2::CUSTOM_BUFFER_LIGHT_TILE_BITS | zonetool::s2::CUSTOM_BUFFER_LIGHT_TILE_ZBINS;
		constexpr std::array<std::uint16_t, 3> light_cluster_consts{zonetool::s2::CONST_SRC_CODE_LIGHT_CLUSTER_CELLS,
			zonetool::s2::CONST_SRC_CODE_LIGHT_CLUSTER_SLICE_STARTS, zonetool::s2::CONST_SRC_CODE_LIGHT_CLUSTER_SLICE_SCALES};

		std::unordered_map<std::string, zonetool::s2::MaterialPixelShader*> merged_pixel_shaders;
		std::unordered_map<std::string, unsigned int> merged_pixel_shader_names;
		std::map<std::pair<std::string, bool>, std::optional<native_techset>> native_techsets;

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

		std::optional<std::uint32_t> find_pixel_code_texture(const MaterialPass& pass, const std::uint32_t code_texture)
		{
			const auto count = pass.perPrimArgCount + pass.perObjArgCount + pass.stableArgCount;
			for (auto i = 0; i < count; i++)
			{
				const auto& arg = pass.args[i];
				if ((arg.shader & MTL_SHADER_STAGE_PIXEL) && arg.type == MTL_ARG_CODE_TEXTURE && arg.u.codeSampler == code_texture)
				{
					return arg.dest;
				}
			}

			return {};
		}

		shader::atmos_fog_rows find_atmos_fog_rows(const MaterialPass& pass, const unsigned int stage)
		{
			shader::atmos_fog_rows result;
			const auto first_stable_arg = pass.perPrimArgCount + pass.perObjArgCount;
			for (auto i = first_stable_arg; i < first_stable_arg + pass.stableArgCount; i++)
			{
				const auto& arg = pass.args[i];
				if (!(arg.shader & stage) || arg.type != MTL_ARG_CODE_CONST)
				{
					continue;
				}

				switch (arg.u.codeConst.index)
				{
				case CONST_SRC_CODE_ATMOS_FOG_PARMS_2:
					result.extinction = arg.dest;
					break;
				case CONST_SRC_CODE_ATMOS_FOG_PARMS_3:
					result.in_scatter = arg.dest;
					break;
				case CONST_SRC_CODE_ATMOS_FOG_PARMS_4:
					result.sun_dir = arg.dest;
					break;
				}
			}

			return result;
		}

		template <typename S2Shader, typename H1Shader>
		S2Shader* convert_shader(const H1Shader* source, const program_patch& patch = {})
		{
			if (!source)
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
			if (patch.lights)
			{
				suffix += utils::string::va("_fpl%u_%u", patch.lights->render_target_size_row, patch.lights->tile_parms_row);
			}
			for (const auto& [from, to] : patch.material_rows)
			{
				suffix += utils::string::va("_mr%u_%u", from, to);
			}
			if (patch.sprite)
			{
				suffix += "_sprite";
			}
			if (patch.lit_instance_rows)
			{
				suffix += "_li9";
			}
			if (patch.wind)
			{
				suffix += utils::string::va("_fw%u_%u_%u", patch.wind->time_row, patch.wind->wind_row, patch.wind->params_row);
			}
			const auto key = std::make_pair(static_cast<const void*>(source), suffix);
			if (const auto entry = converted_shaders.find(key); entry != converted_shaders.end())
			{
				return static_cast<S2Shader*>(entry->second);
			}

			auto* result = native_allocator.allocate<S2Shader>();
			result->name = native_allocator.duplicate_string(::game::add_source_postfix(source->name + key.second, ::game::h1));
			if (source->prog.loadDef.program)
			{
				const auto patched = shader::patch_program(source->prog.loadDef.program, source->prog.loadDef.programSize, is_vertex_shader, patch);
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

		zonetool::s2::MaterialVertexDeclaration* convert_decl(const MaterialVertexDeclaration* decl, const bool wind)
		{
			if (!decl)
			{
				return nullptr;
			}

			if (const auto entry = converted_decls.find({decl, wind}); entry != converted_decls.end())
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

			if (wind)
			{
				const auto has_normal = std::ranges::any_of(streams, [](const decl_stream& stream)
				{
					return stream.routing.source == zonetool::s2::STREAM_SRC_NORMAL;
				});

				if (!has_normal)
				{
					streams.emplace_back(decl_stream{{zonetool::s2::STREAM_SRC_NORMAL, zonetool::s2::STREAM_DST_NORMAL, 0xF}, "nn"});
				}

				streams.emplace_back(decl_stream{{zonetool::s2::STREAM_SRC_WIND, zonetool::s2::STREAM_DST_COLOR_1, 0xF}, "ec1"});
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

			converted_decls.emplace(std::make_pair(decl, wind), result);
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

		void add_light_grid_textures(std::vector<zonetool::s2::MaterialShaderArgument>& stable, const light_grid grid, const bool sampler)
		{
			if (grid == light_grid::probe_buffer)
			{
				return;
			}

			if (sampler)
			{
				auto arg = make_arg(zonetool::s2::MTL_ARG_CODE_SAMPLER, MTL_SHADER_STAGE_PIXEL, shader::ambient_light_grid_sampler);
				arg.u.codeSampler = zonetool::s2::TEXTURE_SRC_CODE_LINEAR_CLAMP;
				stable.emplace_back(arg);
			}

			const auto volume_3d = grid == light_grid::volume_3d;
			const std::array<std::pair<std::uint16_t, std::uint32_t>, 2> textures
			{{
				{shader::light_grid_colors_slot, volume_3d ? zonetool::s2::TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_DATA_3D
					: zonetool::s2::TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_DATA_2D},
				{shader::light_grid_visibility_slot, volume_3d ? zonetool::s2::TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_PRIMARY_VIS_DATA_3D
					: zonetool::s2::TEXTURE_SRC_CODE_LIGHT_GRID_VOLUME_PRIMARY_VIS_DATA_2D},
			}};

			for (const auto& [slot, texture] : textures)
			{
				auto arg = make_arg(zonetool::s2::MTL_ARG_CODE_TEXTURE, MTL_SHADER_STAGE_PIXEL, slot);
				arg.u.codeSampler = texture;
				stable.emplace_back(arg);
			}

			std::ranges::stable_sort(stable, {}, [](const zonetool::s2::MaterialShaderArgument& arg)
			{
				const auto is_code_texture = arg.type == zonetool::s2::MTL_ARG_CODE_TEXTURE;
				return std::make_tuple(arg.type, is_code_texture ? arg.shader : 0, is_code_texture ? arg.dest : 0);
			});

			check_group_size(stable);
		}

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

		void add_sprite_pass(zonetool::s2::MaterialPass& pass, std::vector<zonetool::s2::MaterialShaderArgument>& per_prim, const std::int32_t slot)
		{
			constexpr auto prev_world_dest = 6u;

			auto prev_world = make_arg(zonetool::s2::MTL_ARG_CODE_CONST, MTL_SHADER_STAGE_VERTEX, prev_world_dest);
			prev_world.u.codeConst = {zonetool::s2::CONST_SRC_CODE_PREV_FRAME_TRANSPOSE_WORLD_MATRIX, 0, 4};
			per_prim.emplace_back(prev_world);

			pass.vertexDecl = nullptr;
			pass.perPrimArgSize = arg_size(pass.perPrimArgSize, prev_world_dest + 4);
			pass.customBufferFlags |= zonetool::s2::CUSTOM_BUFFER_SPRITES;
			pass.modelVertexType = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE == zonetool::s2::TECHNIQUE_BUILD_SHADOWMAP_DEPTH
				? zonetool::s2::MODEL_VERTEX_TYPE_INSTANCE_DATA : zonetool::s2::MODEL_VERTEX_TYPE_VERTEX_LIGHTING;
		}

		shader::primary_lights take_light_consts(std::vector<MaterialShaderArgument>& per_prim, std::vector<MaterialShaderArgument>& per_obj,
			std::vector<MaterialShaderArgument>& stable)
		{
			constexpr std::array<std::uint32_t, shader::light_const_count> light_consts
			{
				CONST_SRC_CODE_LIGHT_POSITION,
				CONST_SRC_CODE_LIGHT_DIFFUSE,
				CONST_SRC_CODE_LIGHT_PHYSICALSIZE,
				CONST_SRC_CODE_LIGHT_FADEOFFSET,
				CONST_SRC_CODE_LIGHT_SPECULAR,
			};

			shader::primary_lights result{};
			std::optional<std::uint32_t> buffer;
			for (const auto& [list, index] : {std::pair{&per_prim, h1_per_prim_constant_buffer}, std::pair{&per_obj, h1_per_object_constant_buffer},
				std::pair{&stable, shader::h1_stable_constant_buffer}})
			{
				std::erase_if(*list, [&](const MaterialShaderArgument& arg)
				{
					if (arg.type != MTL_ARG_CODE_CONST || arg.u.codeConst.index > CONST_SRC_CODE_LIGHT_DYN_SHADOW_TYPES)
					{
						return false;
					}

					const auto light = std::ranges::find(light_consts, arg.u.codeConst.index);
					if (light == light_consts.end() || arg.shader != MTL_SHADER_STAGE_PIXEL || (buffer && *buffer != index))
					{
						throw std::runtime_error(utils::string::va("light pass reads light constant %u", arg.u.codeConst.index));
					}

					buffer = index;
					result.rows[light - light_consts.begin()] = arg.dest;
					return true;
				});
			}

			if (!buffer)
			{
				throw std::runtime_error("light pass reads no light constant");
			}

			result.buffer = *buffer;
			return result;
		}

		struct material_rows
		{
			shader::constant_rows pixel;
			shader::constant_rows vertex;
		};

		void remap_material_constants(std::vector<MaterialShaderArgument>& args, const material_rows& rows)
		{
			for (auto& arg : args)
			{
				const auto& stage = arg.shader == MTL_SHADER_STAGE_VERTEX ? rows.vertex : rows.pixel;
				if (const auto row = stage.find(arg.dest); arg.type == MTL_ARG_MATERIAL_CONST && row != stage.end())
				{
					arg.dest = static_cast<unsigned short>(row->second);
				}
			}
		}

		struct pass_args
		{
			std::vector<MaterialShaderArgument> per_prim;
			std::vector<MaterialShaderArgument> per_obj;
			std::vector<MaterialShaderArgument> stable;
		};

		pass_args split_args(const MaterialPass& pass)
		{
			const MaterialShaderArgument* per_prim_args = pass.args;
			const auto* per_obj_args = per_prim_args ? per_prim_args + pass.perPrimArgCount : nullptr;
			const auto* stable_args = per_prim_args ? per_obj_args + pass.perObjArgCount : nullptr;
			pass_args result;
			result.per_prim.assign(per_prim_args, per_obj_args);
			result.per_obj.assign(per_obj_args, stable_args);
			result.stable.assign(stable_args, per_prim_args ? stable_args + pass.stableArgCount : nullptr);
			return result;
		}

		void add_light_tile_args(const shader::primary_lights& lights, std::vector<zonetool::s2::MaterialShaderArgument>& stable)
		{
			for (const auto& [row, index] : {std::pair{lights.render_target_size_row, zonetool::s2::CONST_SRC_CODE_RENDER_TARGET_SIZE},
				std::pair{lights.tile_parms_row, zonetool::s2::CONST_SRC_CODE_LIGHT_TILE_PARMS}})
			{
				auto arg = make_arg(zonetool::s2::MTL_ARG_CODE_CONST, MTL_SHADER_STAGE_PIXEL, static_cast<unsigned short>(row));
				arg.u.codeConst = {static_cast<unsigned short>(index), 0, 1};
				stable.emplace_back(arg);
			}
		}

		void add_foliage_wind_args(const shader::foliage_wind& wind, std::vector<zonetool::s2::MaterialShaderArgument>& stable)
		{
			// a fixed wind, the s2 wind const comes from a level wind fx that converted maps lack
			constexpr std::array<float, 4> foliage_wind{1.0f, 1.0f, -0.7071f, 0.7071f};
			constexpr std::array<float, 4> foliage_wind_a{0.3f, 3.0f, 4.0f, 10.0f};

			auto time = make_arg(zonetool::s2::MTL_ARG_CODE_CONST, MTL_SHADER_STAGE_VERTEX, static_cast<unsigned short>(wind.time_row));
			time.u.codeConst = {static_cast<unsigned short>(zonetool::s2::CONST_SRC_CODE_GAMETIME), 0, 1};
			stable.emplace_back(time);

			for (const auto& [row, value] : {std::pair{wind.wind_row, &foliage_wind}, std::pair{wind.params_row, &foliage_wind_a}})
			{
				auto arg = make_arg(zonetool::s2::MTL_ARG_LITERAL_CONST, MTL_SHADER_STAGE_VERTEX, static_cast<unsigned short>(row));
				arg.u.literalConst = allocate_literal(value->data());
				stable.emplace_back(arg);
			}
		}

		std::vector<stable_literal_row> move_literal_consts_to_stable(pass_args& args, std::uint32_t& next_stable_row)
		{
			std::vector<stable_literal_row> result;
			for (const auto& [list, buffer] : {std::pair{&args.per_prim, h1_per_prim_constant_buffer}, std::pair{&args.per_obj, h1_per_object_constant_buffer}})
			{
				std::erase_if(*list, [&](const MaterialShaderArgument& arg)
				{
					const auto index = static_cast<std::int32_t>(arg.u.codeConst.index);
					if (arg.type != MTL_ARG_CODE_CONST || arg.shader != MTL_SHADER_STAGE_VERTEX || arg.u.codeConst.rowCount != 1
						|| code_consts.contains(index) || !code_literals.contains(index))
					{
						return false;
					}

					result.emplace_back(buffer, arg.dest, next_stable_row);
					auto& moved = args.stable.emplace_back(arg);
					moved.dest = static_cast<unsigned short>(next_stable_row++);
					return true;
				});
			}

			return result;
		}

		zonetool::s2::MaterialPassModelVertexType pass_model_vertex_type(const std::int32_t slot, const bool lightmap, const bool lighting,
			const zonetool::s2::MaterialVertexDeclaration* decl)
		{
			if (lightmap)
			{
				return zonetool::s2::MODEL_VERTEX_TYPE_LIGHTMAP;
			}

			if (lighting)
			{
				return zonetool::s2::MODEL_VERTEX_TYPE_SELF_VISIBILITY; // s2 uploads the light grid constants only for this vertex type
			}

			if (has_vertex_lighting_stream(decl))
			{
				return zonetool::s2::MODEL_VERTEX_TYPE_VERTEX_LIGHTING;
			}

			return stock_model_vertex_type(slot);
		}

		std::uint64_t pass_custom_buffer_flags(const MaterialPass& pass, const std::optional<model_lighting>& lighting, const light_grid grid,
			const bool lit_instances, const bool primary_lights)
		{
			auto flags = convert_custom_buffer_flags(pass.customBufferFlags & ~static_cast<std::uint32_t>(CUSTOM_BUFFER_REACTIVE_MOTION), lighting, grid);
			if (lit_instances && (flags & zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES))
			{
				flags = (flags & ~zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES) | zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES_LIGHTING;
			}

			if (primary_lights)
			{
				flags |= zonetool::s2::CUSTOM_BUFFER_FORWARD_LIGHTING;
			}

			return flags;
		}

		void convert_pass(const MaterialPass& pass, const light_grid grid, const std::int32_t slot, const bool sprite, zonetool::s2::MaterialPass& result,
			const material_rows* light_pass_rows = nullptr, const bool lit_instances = false)
		{
			const auto lighting = find_model_lighting(pass);
			const auto lightmap_params = find_model_lightmap_params(pass);

			auto args = split_args(pass);
			auto& per_prim_list = args.per_prim;
			auto& per_obj_list = args.per_obj;
			auto& stable_list = args.stable;
			auto next_stable_row = (pass.stableArgSize + constant_row_size - 1) / constant_row_size;
			const auto stable_literal_rows = move_literal_consts_to_stable(args, next_stable_row);

			std::optional<shader::primary_lights> primary_lights;
			if (light_pass_rows)
			{
				primary_lights = take_light_consts(per_prim_list, per_obj_list, stable_list);
				primary_lights->render_target_size_row = next_stable_row++;
				primary_lights->tile_parms_row = next_stable_row++;
				for (auto* list : {&per_prim_list, &per_obj_list, &stable_list})
				{
					remap_material_constants(*list, *light_pass_rows);
				}
			}

			std::optional<shader::foliage_wind> wind;
			if (pass.customBufferFlags & CUSTOM_BUFFER_REACTIVE_MOTION)
			{
				const auto row = static_cast<std::uint32_t>(next_stable_row);
				wind = shader::foliage_wind{row, row + 1, row + 2};
				next_stable_row += 3;
			}

			program_patch vertex_patch{{}, lighting};
			vertex_patch.lightmap_params_dest = lightmap_params;
			vertex_patch.wind = wind;
			vertex_patch.stable_literal_rows = stable_literal_rows;
			vertex_patch.sprite = sprite;
			vertex_patch.lit_instance_rows = lit_instances;
			vertex_patch.fog = find_atmos_fog_rows(pass, MTL_SHADER_STAGE_VERTEX);
			if (light_pass_rows)
			{
				vertex_patch.material_rows = light_pass_rows->vertex;
			}
			result.vertexShader = convert_shader<zonetool::s2::MaterialVertexShader>(pass.vertexShader, vertex_patch);
			result.vertexDecl = convert_decl(pass.vertexDecl, wind.has_value());
			result.hullShader = convert_shader<zonetool::s2::MaterialHullShader>(pass.hullShader);
			result.domainShader = convert_shader<zonetool::s2::MaterialDomainShader>(pass.domainShader);

			program_patch pixel_patch{find_sun_shadow_lookup(pass), lighting, grid};
			pixel_patch.lights = primary_lights;
			pixel_patch.ssr_texture = find_pixel_code_texture(pass, TEXTURE_SRC_CODE_SSR_BUFFER);
			pixel_patch.fog = find_atmos_fog_rows(pass, MTL_SHADER_STAGE_PIXEL);
			if (light_pass_rows)
			{
				pixel_patch.material_rows = light_pass_rows->pixel;
			}
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

			if (primary_lights)
			{
				add_light_tile_args(*primary_lights, stable);
			}

			if (wind)
			{
				add_foliage_wind_args(*wind, stable);
			}

			if (primary_lights || wind)
			{
				std::ranges::stable_sort(stable, {}, &zonetool::s2::MaterialShaderArgument::type);
				check_group_size(stable);
			}

			result.pixelOutputMask = pass.pixelOutputMask;
			result.perPrimArgCount = static_cast<unsigned char>(per_prim.size());
			result.perObjArgCount = static_cast<unsigned char>(per_obj.size());
			result.stableArgCount = static_cast<unsigned char>(stable.size());
			result.perPrimArgSize = pass.perPrimArgSize;
			if (lighting && !lighting->instanced)
			{
				result.perPrimArgSize = arg_size(result.perPrimArgSize, shader::s2_model_lighting_index_dest + 1);
			}
			if (lightmap_params)
			{
				result.perPrimArgSize = arg_size(result.perPrimArgSize, shader::s2_model_lightmap_params_dest + 1);
			}
			result.perObjArgSize = pass.perObjArgSize;
			result.stableArgSize = arg_size(pass.stableArgSize, next_stable_row);
			result.customBufferFlags = pass_custom_buffer_flags(pass, lighting, grid, lit_instances, primary_lights.has_value());
			result.customSamplerFlags = pass.customSamplerFlags;
			result.modelVertexType = pass_model_vertex_type(slot, lightmap_params.has_value(), lighting.has_value(), result.vertexDecl);
			if (sprite)
			{
				add_sprite_pass(result, per_prim, slot);
				result.perPrimArgCount = static_cast<unsigned char>(per_prim.size());
			}

			const auto arg_count = per_prim.size() + per_obj.size() + stable.size();
			result.args = arg_count ? native_allocator.allocate_array<zonetool::s2::MaterialShaderArgument>(arg_count) : nullptr;
			auto* dest = result.args;
			for (const auto* group : {&per_prim, &per_obj, &stable})
			{
				dest = std::ranges::copy(*group, dest).out;
			}
		}

		zonetool::s2::MaterialTechnique* convert_technique(const MaterialTechnique* technique, const light_grid grid, const std::int32_t slot, const bool sprite)
		{
			const auto key = std::make_tuple(technique, grid, stock_model_vertex_type(slot), sprite);
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
				convert_pass(technique->passArray[i], grid, slot, sprite, result->passArray[i]);
			}

			converted_techniques.emplace(key, result);
			return result;
		}

		zonetool::s2::MaterialTechnique* append_light_pass(const zonetool::s2::MaterialTechnique* base, const MaterialPass& light, const material_rows& rows,
			const std::int32_t slot)
		{
			const auto key = std::make_pair(base, &light);
			if (const auto entry = light_pass_techniques.find(key); entry != light_pass_techniques.end())
			{
				return entry->second;
			}

			zonetool::s2::MaterialPass light_pass;
			std::memset(&light_pass, 0, sizeof(light_pass));
			const auto lit_instances = (base->passArray[0].customBufferFlags & zonetool::s2::CUSTOM_BUFFER_INSTANCE_MATRICES_LIGHTING) != 0;
			convert_pass(light, light_grid::probe_buffer, slot, false, light_pass, &rows, lit_instances);

			const auto base_type = base->passArray[0].modelVertexType;
			if (base_type == zonetool::s2::MODEL_VERTEX_TYPE_SELF_VISIBILITY || base_type == zonetool::s2::MODEL_VERTEX_TYPE_LIGHTMAP)
			{
				light_pass.modelVertexType = base_type;
			}

			const auto pass_count = base->hdr.passCount + 1u;
			auto* result = static_cast<zonetool::s2::MaterialTechnique*>(native_allocator.allocate(offsetof(zonetool::s2::MaterialTechnique, passArray)
				+ pass_count * sizeof(zonetool::s2::MaterialPass)));
			result->hdr = base->hdr;
			result->hdr.name = native_allocator.duplicate_string(std::string(base->hdr.name) + "_plight");
			result->hdr.passCount = static_cast<unsigned short>(pass_count);
			std::copy_n(base->passArray, base->hdr.passCount, result->passArray);
			result->passArray[base->hdr.passCount] = light_pass;

			light_pass_techniques.emplace(key, result);
			return result;
		}

		bool same_argument(const zonetool::s2::MaterialShaderArgument& a, const zonetool::s2::MaterialShaderArgument& b)
		{
			if (a.type != b.type || a.shader != b.shader)
			{
				return false;
			}

			switch (a.type)
			{
			case zonetool::s2::MTL_ARG_CODE_CONST:
				return a.u.codeConst.index == b.u.codeConst.index && a.u.codeConst.firstRow == b.u.codeConst.firstRow && a.u.codeConst.rowCount == b.u.codeConst.rowCount;
			case zonetool::s2::MTL_ARG_LITERAL_CONST:
				return std::memcmp(a.u.literalConst, b.u.literalConst, sizeof(float) * 4) == 0;
			default:
				return a.u.nameHash == b.u.nameHash;
			}
		}

		std::uint32_t free_slot(std::set<std::uint32_t>& used, std::uint32_t first, const std::uint32_t end)
		{
			while (first < end && (used.contains(first) || first == shader::spot_shadow_sampler
				|| std::ranges::find(shader::forward_light_slots, first) != shader::forward_light_slots.end()))
			{
				first++;
			}

			if (first >= end)
			{
				throw std::runtime_error("no free slot for a light pass texture or sampler");
			}

			used.insert(first);
			return first;
		}

		zonetool::s2::MaterialTechnique* merge_light_pass(const zonetool::s2::MaterialTechnique* base, const MaterialPass& light, const material_rows& rows,
			const bool clustered)
		{
			const auto key = std::make_tuple(base, &light, clustered);
			if (const auto entry = merged_light_techniques.find(key); entry != merged_light_techniques.end())
			{
				return entry->second;
			}

			constexpr auto texture_slot_count = 26u;
			constexpr auto sampler_slot_count = 16u;

			if (base->hdr.passCount != 1 || !base->passArray[0].pixelShader || !base->passArray[0].pixelShader->prog.loadDef.program || !light.pixelShader
				|| !light.pixelShader->prog.loadDef.program)
			{
				throw std::runtime_error("the lit technique is not one pass with a pixel shader");
			}

			const auto& lit = base->passArray[0];
			const auto lit_count = lit.perPrimArgCount + lit.perObjArgCount + lit.stableArgCount;
			const std::span lit_args(lit.args, lit.args ? lit_count : 0);

			auto light_args = split_args(light);
			auto& per_prim_list = light_args.per_prim;
			auto& per_obj_list = light_args.per_obj;
			auto& stable_list = light_args.stable;

			shader::light_merge merge{take_light_consts(per_prim_list, per_obj_list, stable_list)};
			merge.material_rows = rows.pixel;

			const auto is_pixel = [](const MaterialShaderArgument& arg) { return (arg.shader & MTL_SHADER_STAGE_PIXEL) != 0; };
			const auto pixel_args = [&](std::vector<MaterialShaderArgument>& list)
			{
				std::erase_if(list, [&](const MaterialShaderArgument& arg) { return !is_pixel(arg) || arg.type == MTL_ARG_MATERIAL_CONST; });
				for (auto& arg : list)
				{
					arg.shader = MTL_SHADER_STAGE_PIXEL;
				}
			};
			pixel_args(per_prim_list);
			pixel_args(per_obj_list);
			pixel_args(stable_list);
			if (!per_prim_list.empty() || !per_obj_list.empty())
			{
				throw std::runtime_error("the light pixel shader reads per prim or per object constants");
			}

			auto [textures, samplers] = shader::declared_slots(lit.pixelShader->prog.loadDef.program, lit.pixelShader->prog.loadDef.programSize);

			constexpr std::uint32_t spot_shadow_code_texture = 8;
			constexpr std::uint32_t spot_shadow_dynamic_code_texture = 13;
			constexpr std::uint32_t spot_shadow_code_sampler = 6;
			merge.lights.spot_shadows = !samplers.contains(shader::spot_shadow_sampler);
			std::vector<zonetool::s2::MaterialShaderArgument> stable(lit_args.begin() + lit.perPrimArgCount + lit.perObjArgCount, lit_args.end());
			auto next_row = (lit.stableArgSize + constant_row_size - 1) / constant_row_size;
			merge.lights.render_target_size_row = next_row++;
			merge.lights.tile_parms_row = next_row++;
			if (clustered)
			{
				merge.lights.cluster_row = next_row;
				next_row += static_cast<std::uint32_t>(light_cluster_consts.size());
			}

			add_light_tile_args(merge.lights, stable);

			for (auto i = 0u; clustered && i < light_cluster_consts.size(); i++)
			{
				auto arg = make_arg(zonetool::s2::MTL_ARG_CODE_CONST, MTL_SHADER_STAGE_PIXEL, static_cast<unsigned short>(*merge.lights.cluster_row + i));
				arg.u.codeConst = {light_cluster_consts[i], 0, 1};
				stable.emplace_back(arg);
			}

			for (auto arg : convert_args(stable_list.data(), stable_list.size(), true, false))
			{
				const auto lit_arg = std::ranges::find_if(lit_args, [&](const zonetool::s2::MaterialShaderArgument& other) { return same_argument(arg, other); });
				const auto is_constant = arg.type == zonetool::s2::MTL_ARG_CODE_CONST || arg.type == zonetool::s2::MTL_ARG_LITERAL_CONST;
				const auto rows_count = arg.type == zonetool::s2::MTL_ARG_CODE_CONST ? std::max<std::uint32_t>(arg.u.codeConst.rowCount, 1u) : 1u;
				const auto is_sampler = arg.type == zonetool::s2::MTL_ARG_CODE_SAMPLER || arg.type == zonetool::s2::MTL_ARG_MATERIAL_SAMPLER;
				auto& slots = is_constant ? merge.stable_rows : is_sampler ? merge.sampler_slots : merge.texture_slots;
				if (lit_arg != lit_args.end() && (!is_constant || lit_arg - lit_args.begin() >= lit.perPrimArgCount + lit.perObjArgCount))
				{
					for (auto row = 0u; row < rows_count; row++)
					{
						slots.emplace(arg.dest + row, lit_arg->dest + row);
					}
					continue;
				}

				const auto dest = is_constant ? next_row : is_sampler ? free_slot(samplers, 0, sampler_slot_count) : free_slot(textures, 0, texture_slot_count);
				next_row += is_constant ? rows_count : 0;
				for (auto row = 0u; row < rows_count; row++)
				{
					slots.emplace(arg.dest + row, dest + row);
				}

				arg.dest = static_cast<unsigned short>(dest);
				stable.emplace_back(arg);
			}

			// s2 caps the loaded pixel shaders, so equal merged programs share one shader
			const auto merged = shader::merge_light_program(lit.pixelShader->prog.loadDef.program, lit.pixelShader->prog.loadDef.programSize,
				light.pixelShader->prog.loadDef.program, light.pixelShader->prog.loadDef.programSize, merge);
			auto& pixel_shader = merged_pixel_shaders[merged.data];
			if (!pixel_shader)
			{
				const auto variant = merged_pixel_shader_names[lit.pixelShader->name]++;
				pixel_shader = native_allocator.allocate<zonetool::s2::MaterialPixelShader>();
				pixel_shader->name = native_allocator.duplicate_string(variant ? utils::string::va("%s_fpl%u", lit.pixelShader->name, variant)
					: utils::string::va("%s_fpl", lit.pixelShader->name));
				pixel_shader->prog.loadDef.programSize = static_cast<unsigned int>(merged.data.size());
				pixel_shader->prog.loadDef.program = native_allocator.allocate_array<unsigned char>(merged.data.size());
				std::memcpy(pixel_shader->prog.loadDef.program, merged.data.data(), merged.data.size());
			}

			std::vector<zonetool::s2::MaterialShaderArgument> per_prim_obj(lit_args.begin(), lit_args.begin() + lit.perPrimArgCount + lit.perObjArgCount);
			relocate_texture_args(per_prim_obj, merged.texture_slots);
			relocate_texture_args(stable, merged.texture_slots);

			if (merge.lights.spot_shadows)
			{
				for (const auto& [type, dest, source] : {std::tuple{zonetool::s2::MTL_ARG_CODE_TEXTURE, shader::spot_shadow_slot, spot_shadow_code_texture},
					std::tuple{zonetool::s2::MTL_ARG_CODE_TEXTURE, shader::spot_shadow_dynamic_slot, spot_shadow_dynamic_code_texture},
					std::tuple{zonetool::s2::MTL_ARG_CODE_SAMPLER, shader::spot_shadow_sampler, spot_shadow_code_sampler}})
				{
					auto arg = make_arg(type, MTL_SHADER_STAGE_PIXEL, static_cast<unsigned short>(dest));
					arg.u.codeSampler = source;
					stable.emplace_back(arg);
				}
			}
			std::ranges::stable_sort(stable, {}, &zonetool::s2::MaterialShaderArgument::type);
			check_group_size(stable);

			const auto arg_count = lit.perPrimArgCount + lit.perObjArgCount + stable.size();
			auto* args = native_allocator.allocate_array<zonetool::s2::MaterialShaderArgument>(arg_count);
			std::ranges::copy(per_prim_obj, args);
			std::ranges::copy(stable, args + lit.perPrimArgCount + lit.perObjArgCount);

			auto* result = static_cast<zonetool::s2::MaterialTechnique*>(native_allocator.allocate(sizeof(zonetool::s2::MaterialTechnique)));
			result->hdr = base->hdr;
			result->hdr.name = native_allocator.duplicate_string(std::string(base->hdr.name) + (clustered ? "_plightc" : "_plight"));
			result->passArray[0] = lit;
			auto& pass = result->passArray[0];
			pass.pixelShader = pixel_shader;
			pass.args = args;
			pass.stableArgCount = static_cast<unsigned char>(stable.size());
			pass.stableArgSize = arg_size(lit.stableArgSize, next_row);
			pass.customBufferFlags |= clustered ? custom_buffer_forward_cluster_lighting : zonetool::s2::CUSTOM_BUFFER_FORWARD_LIGHTING;
			if (clustered)
			{
				pass.customBufferFlags &= ~custom_buffer_light_tiles;
			}

			merged_light_techniques.emplace(key, result);
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

		std::optional<std::int32_t> velocity_source_type(const std::int32_t type)
		{
			switch (type)
			{
			case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_RIGID:
				return TECHNIQUE_ZPREPASS_VELOCITY_RIGID;
			case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_SKINNED:
				return TECHNIQUE_ZPREPASS_VELOCITY_SKINNED;
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID:
				return TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID;
			case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED:
				return TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED;
			case zonetool::s2::TECHNIQUE_VELOCITY_RIGID:
				return TECHNIQUE_VELOCITY_RIGID;
			case zonetool::s2::TECHNIQUE_VELOCITY_SKINNED:
				return TECHNIQUE_VELOCITY_SKINNED;
			default:
				return {};
			}
		}

		template <typename Technique>
		bool uses_material_arguments(const Technique* technique)
		{
			for (auto i = 0; i < technique->hdr.passCount; i++)
			{
				const auto& pass = technique->passArray[i];
				if (pass.hullShader)
				{
					return true;
				}

				auto arg_count = pass.perPrimArgCount + pass.perObjArgCount + pass.stableArgCount;
				auto material_types = std::array<std::uint8_t, 3>{MTL_ARG_MATERIAL_CONST, MTL_ARG_MATERIAL_TEXTURE, MTL_ARG_MATERIAL_SAMPLER};
				if constexpr (std::is_same_v<Technique, zonetool::s2::MaterialTechnique>)
				{
					arg_count += pass.volatileArgCount;
					material_types = {argument_types[MTL_ARG_MATERIAL_CONST], argument_types[MTL_ARG_MATERIAL_TEXTURE], argument_types[MTL_ARG_MATERIAL_SAMPLER]};
				}

				const auto args = std::span{pass.args, pass.args ? static_cast<std::size_t>(arg_count) : 0u};
				const auto is_material = std::ranges::any_of(args, [&](const auto& arg)
				{
					return std::ranges::find(material_types, arg.type) != material_types.end();
				});

				if (is_material)
				{
					return true;
				}
			}

			return false;
		}

		zonetool::s2::MaterialTechnique* find_stock_velocity_technique(const std::int32_t slot)
		{
			static std::unordered_map<std::int32_t, zonetool::s2::MaterialTechnique*> techniques;
			if (const auto entry = techniques.find(slot); entry != techniques.end())
			{
				return entry->second;
			}

			auto& result = techniques[slot];
			for (auto* header : stock_zone().find_assets(zonetool::s2::ASSET_TYPE_TECHNIQUE_SET))
			{
				const auto* techset = static_cast<zonetool::s2::MaterialTechniqueSet*>(header);
				auto* technique = techset->techniques[slot];
				if (techset->worldVertFormat == 0 && technique && !uses_material_arguments(technique))
				{
					ZONETOOL_INFO("velocity slot %d uses stock technique \"%s\" of \"%s\"", slot, technique->hdr.name, techset->name);
					result = technique;
					break;
				}
			}

			return result;
		}

		bool use_stock_velocity_technique(const MaterialTechniqueSet* techset, const std::int32_t slot, native_techset& result)
		{
			const auto type = velocity_source_type(slot % zonetool::s2::TECHNIQUE_GROUP_SIZE);
			if (!type)
			{
				return false;
			}

			const auto source = find_group_technique(techset, slot - slot % zonetool::s2::TECHNIQUE_GROUP_SIZE, *type);
			if (source < 0 || uses_material_arguments(techset->techniques[source]))
			{
				return false;
			}

			auto* technique = find_stock_velocity_technique(slot);
			if (!technique)
			{
				return false;
			}

			result.techset->techniques[slot] = technique;
			result.sources[slot] = source;
			return true;
		}

		std::optional<material_rows> map_material_constants(const MaterialPass& base, const MaterialPass& light)
		{
			const auto args = [](const MaterialPass& pass)
			{
				return std::span(pass.args, pass.args ? pass.perPrimArgCount + pass.perObjArgCount + pass.stableArgCount : 0);
			};

			material_rows result;
			for (const auto& arg : args(light))
			{
				if (arg.type != MTL_ARG_MATERIAL_CONST)
				{
					continue;
				}

				const auto lit = std::ranges::find_if(args(base), [&](const MaterialShaderArgument& other)
				{
					return other.type == MTL_ARG_MATERIAL_CONST && other.shader == arg.shader && other.u.nameHash == arg.u.nameHash;
				});

				if (lit == args(base).end())
				{
					return {};
				}

				if (arg.shader != MTL_SHADER_STAGE_VERTEX && arg.shader != MTL_SHADER_STAGE_PIXEL)
				{
					return {};
				}

				if (arg.dest != lit->dest)
				{
					(arg.shader == MTL_SHADER_STAGE_VERTEX ? result.vertex : result.pixel).emplace(arg.dest, lit->dest);
				}
			}

			return result;
		}

		void add_light_pass(const MaterialTechniqueSet* techset, const std::int32_t slot, native_techset& result)
		{
			const auto type = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE;
			if (type <zonetool::s2::TECHNIQUE_LIT_BEGIN || type >= zonetool::s2::TECHNIQUE_LIT_END || !result.techset->techniques[slot])
			{
				return;
			}

			const auto group = slot - type;
			auto source = find_group_technique(techset, group, TECHNIQUE_LIGHT_OMNI);
			if (source < 0)
			{
				source = find_group_technique(techset, group, TECHNIQUE_LIGHT_OMNI_DFOG);
			}

			if (source < 0 || techset->techniques[source]->hdr.passCount != 1)
			{
				return;
			}

			const auto* light = techset->techniques[source];
			static std::unordered_set<std::string> warned;
			const auto rows = map_material_constants(techset->techniques[result.sources[slot]]->passArray[0], light->passArray[0]);
			if (!rows)
			{
				if (warned.insert(light->hdr.name).second)
				{
					ZONETOOL_WARNING("technique \"%s\" draws no primary lights: its material constants differ from the lit pass", light->hdr.name);
				}

				return;
			}

			try
			{
				const auto clustered = find_group_technique(techset, group, TECHNIQUE_ZPREPASS) < 0;
				result.techset->techniques[slot] = merge_light_pass(result.techset->techniques[slot], light->passArray[0], *rows, clustered);
				return;
			}
			catch (const std::exception& e)
			{
				static std::unordered_set<std::string> warned_merge;
				if (warned_merge.insert(light->hdr.name).second)
				{
					ZONETOOL_WARNING("technique \"%s\" gets a separate light pass: %s", light->hdr.name, e.what());
				}
			}

			try
			{
				result.techset->techniques[slot] = append_light_pass(result.techset->techniques[slot], light->passArray[0], *rows, slot);
				result.light_sources[slot] = source;
			}
			catch (const std::exception& e)
			{
				if (warned.insert(light->hdr.name).second)
				{
					ZONETOOL_WARNING("technique \"%s\" draws no primary lights: %s", light->hdr.name, e.what());
				}
			}
		}

		void convert_slot(const MaterialTechniqueSet* techset, const std::int32_t slot, const bool sprite, native_techset& result)
		{
			if (!sprite && use_stock_velocity_technique(techset, slot, result))
			{
				return;
			}

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
					result.techset->techniques[slot] = convert_technique(techset->techniques[source], slot_light_grid(slot), slot, sprite);
					result.sources[slot] = source;
					if (!sprite)
					{
						add_light_pass(techset, slot, result);
					}

					return;
				}
				catch (const std::exception& e)
				{
					error = utils::string::va("technique \"%s\": %s", techset->techniques[source]->hdr.name, e.what());
				}
			}

			const auto is_wireframe = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE == zonetool::s2::TECHNIQUE_WIREFRAME_SOLID;
			if (!error.empty() && !is_wireframe)
			{
				throw std::runtime_error(error);
			}
		}

		MaterialPass instance_pass(const MaterialPass& pass)
		{
			if (!pass.vertexShader || !pass.vertexShader->prog.loadDef.program || pass.hullShader || pass.domainShader)
			{
				throw std::runtime_error("pass has no plain vertex shader to instance");
			}

			const auto per_prim = std::span{pass.args, pass.args ? pass.perPrimArgCount : 0u};
			std::optional<std::uint32_t> coords_row;
			auto instance_flags = static_cast<std::uint32_t>(CUSTOM_BUFFER_SMODEL_LIGHTING);
			for (const auto& arg : per_prim)
			{
				const auto& code_const = arg.u.codeConst;
				if (arg.type != MTL_ARG_CODE_CONST || arg.shader != MTL_SHADER_STAGE_VERTEX)
				{
					throw std::runtime_error("per prim argument has no instanced source");
				}

				if (arg.dest == 0 && code_const.rowCount == 4)
				{
					continue;
				}

				if (code_const.rowCount != 1 || (code_const.index != CONST_SRC_CODE_BASE_LIGHTING_COORDS && code_const.index != CONST_SRC_CODE_LIGHT_PROBE_AMBIENT))
				{
					throw std::runtime_error(utils::string::va("per prim code constant %u has no instanced source", code_const.index));
				}

				coords_row = arg.dest;
				instance_flags |= code_const.index == CONST_SRC_CODE_BASE_LIGHTING_COORDS ? CUSTOM_BUFFER_SMODEL_AMBIENT : h1_instance_probe_ambient_flag;
			}

			auto& shader = instanced_vertex_shaders[{pass.vertexShader, coords_row}];
			if (!shader)
			{
				const auto& load_def = pass.vertexShader->prog.loadDef;
				const auto program = shader::instance_program(load_def.program, load_def.programSize, coords_row);
				shader = native_allocator.allocate<MaterialVertexShader>();
				shader->name = native_allocator.duplicate_string(pass.vertexShader->name + std::string("_inst"));
				shader->prog.loadDef.programSize = static_cast<unsigned int>(program.size());
				shader->prog.loadDef.program = native_allocator.allocate_array<unsigned char>(program.size());
				std::memcpy(shader->prog.loadDef.program, program.data(), program.size());
			}

			auto result = pass;
			result.vertexShader = shader;
			result.args = pass.args ? pass.args + pass.perPrimArgCount : nullptr;
			result.perPrimArgCount = 0;
			result.perPrimArgSize = 0;
			result.customBufferFlags = (pass.customBufferFlags & ~static_cast<std::uint32_t>(CUSTOM_BUFFER_PER_PRIM)) | instance_flags;
			return result;
		}

		const MaterialTechniqueSet* add_instanced_techniques(const MaterialTechniqueSet* techset)
		{
			auto* result = native_allocator.allocate<MaterialTechniqueSet>();
			*result = *techset;
			for (auto type = 0; type < TECHNIQUE_INSTANCED_BEGIN - TECHNIQUE_STANDARD_BEGIN; type++)
			{
				const auto* standard = techset->techniques[TECHNIQUE_STANDARD_BEGIN + type];
				if (!standard || techset->techniques[TECHNIQUE_INSTANCED_BEGIN + type])
				{
					continue;
				}

				try
				{
					const auto passes = standard->hdr.passCount;
					const auto size = offsetof(MaterialTechnique, passArray) + sizeof(MaterialPass) * std::max<std::size_t>(passes, 1);
					auto* technique = native_allocator.manual_allocate<MaterialTechnique>(size);
					technique->hdr = standard->hdr;
					technique->hdr.name = native_allocator.duplicate_string(std::string("instanced_") + standard->hdr.name);
					for (auto i = 0; i < passes; i++)
					{
						technique->passArray[i] = instance_pass(standard->passArray[i]);
					}

					result->techniques[TECHNIQUE_INSTANCED_BEGIN + type] = technique;
				}
				catch (const std::exception& e)
				{
					static std::unordered_set<std::string> warned;
					if (warned.insert(standard->hdr.name).second)
					{
						ZONETOOL_WARNING("technique \"%s\" has no instanced version: %s", standard->hdr.name, e.what());
					}
				}
			}

			return result;
		}

		native_techset build_native(const std::string& name, const bool sprite)
		{
			const auto* techset = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_TECHNIQUE_SET, name).techniqueSet;
			if (!techset)
			{
				throw std::runtime_error("the h1 techset is not loaded");
			}

			if (name.find("_reactivemotion") != std::string::npos)
			{
				techset = add_instanced_techniques(techset);
			}

			if (techset->worldVertFormat >= zonetool::s2::MTL_WORLDVERT_COUNT)
			{
				throw std::runtime_error(utils::string::va("world vertex format %u has no s2 mapping", techset->worldVertFormat));
			}

			native_techset result{native_allocator.allocate<zonetool::s2::MaterialTechniqueSet>()};
			result.sources.fill(-1);
			result.light_sources.fill(-1);
			result.techset->name = native_allocator.duplicate_string(::game::add_source_postfix(sprite ? name + "_sprite" : name, ::game::h1));
			result.techset->flags = techset->flags;
			result.techset->worldVertFormat = techset->worldVertFormat;
			result.techset->preDisplacementOnlyCount = techset->preDisplacementOnlyCount;

			const auto slot_count = sprite ? zonetool::s2::TECHNIQUE_GROUP_SIZE : zonetool::s2::technique_count;
			for (auto slot = 0; slot < slot_count; slot++)
			{
				convert_slot(techset, slot, sprite, result);
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

			if (asset.type == zonetool::s2::ASSET_TYPE_TECHNIQUE_SET && asset.referenced)
			{
				this->global_techsets_.insert(static_cast<zonetool::s2::MaterialTechniqueSet*>(asset.header)->name);
			}
			else if (asset.type == zonetool::s2::ASSET_TYPE_IMAGE && asset.referenced)
			{
				this->global_images_.insert(static_cast<zonetool::s2::GfxImage*>(asset.header)->name);
			}
		}
	}

	const std::vector<void*>& s2_zone::find_assets(const zonetool::s2::XAssetType type) const
	{
		static const std::vector<void*> none;
		const auto entry = this->assets_.find(type);
		return entry != this->assets_.end() ? entry->second : none;
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

	database& s2_zone::db() const
	{
		return *this->database_;
	}

	const std::unordered_map<std::string, std::string>& common_rawfiles()
	{
		static const auto rawfiles = []
		{
			std::unordered_map<std::string, std::string> result;
			for (const auto* asset : common_zone().find_assets(zonetool::s2::ASSET_TYPE_RAWFILE))
			{
				const auto* rawfile = static_cast<const zonetool::s2::RawFile*>(asset);
				if (!rawfile->name || !rawfile->buffer)
				{
					continue;
				}

				result.try_emplace(rawfile->name, rawfile->compressedLen > 0
					? utils::compression::zlib::decompress({rawfile->buffer, static_cast<std::size_t>(rawfile->compressedLen)})
					: std::string(rawfile->buffer, rawfile->len));
			}

			return result;
		}();

		return rawfiles;
	}

	const s2_zone& common_zone()
	{
		static const s2_zone zone(s2_folder() / "zone" / "common_mp.ff");
		return zone;
	}

	const s2_zone& stock_zone()
	{
		static const s2_zone zone(s2_folder() / "zone" / (std::string(stock_zone_name) + ".ff"));
		return zone;
	}

	const native_techset* convert_native(const std::string& h1_techset, const bool sprite)
	{
		const auto key = std::make_pair(h1_techset, sprite);
		if (const auto entry = native_techsets.find(key); entry != native_techsets.end())
		{
			return entry->second ? &*entry->second : nullptr;
		}

		auto& entry = native_techsets[key];
		try
		{
			entry = build_native(h1_techset, sprite);
			ZONETOOL_INFO("techset %s -> %s (native)", h1_techset.data(), entry->techset->name);
		}
		catch (const std::exception& e)
		{
			ZONETOOL_WARNING("techset %s is not converted: %s", h1_techset.data(), e.what());
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

	void clear()
	{
		dumped_techsets.clear();
		dumped_shaders<zonetool::s2::MaterialVertexShader>.clear();
		dumped_shaders<zonetool::s2::MaterialVertexDeclaration>.clear();
		dumped_shaders<zonetool::s2::MaterialHullShader>.clear();
		dumped_shaders<zonetool::s2::MaterialDomainShader>.clear();
		dumped_shaders<zonetool::s2::MaterialPixelShader>.clear();
	}
}
