#include <std_include.hpp>

#include "../../include.hpp"
#include "args.hpp"
#include "common.hpp"
#include "decls.hpp"
#include "passes.hpp"

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		constexpr auto h1_per_prim_constant_buffer = 0u;
		constexpr auto h1_per_object_constant_buffer = 1u;

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

		std::map<std::pair<const void*, std::string>, void*> converted_shaders;
		std::unordered_set<const zonetool::s2::MaterialVertexShader*> lightmap_vertex_shaders;
		std::unordered_map<const zonetool::s2::MaterialVertexShader*, std::uint32_t> lighting_semantics;
		std::unordered_map<const zonetool::s2::MaterialPixelShader*, std::map<std::uint32_t, std::uint32_t>> relocated_textures;

		std::map<std::tuple<const MaterialTechnique*, light_grid, zonetool::s2::MaterialPassModelVertexType, bool>, zonetool::s2::MaterialTechnique*> converted_techniques;

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

		void add_foliage_wind_args(const shader::foliage_wind& wind, std::vector<zonetool::s2::MaterialShaderArgument>& stable)
		{
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
				return zonetool::s2::MODEL_VERTEX_TYPE_SELF_VISIBILITY;
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

	void convert_pass(const MaterialPass& pass, const light_grid grid, const std::int32_t slot, const bool sprite, zonetool::s2::MaterialPass& result,
		const material_rows* light_pass_rows, const bool lit_instances)
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
		result.vertexDecl = convert_decl(pass.vertexDecl, wind.has_value(), lightmap_vertex_shaders.contains(result.vertexShader));
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
}
