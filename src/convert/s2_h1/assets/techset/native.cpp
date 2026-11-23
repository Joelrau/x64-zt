#include <std_include.hpp>

#include "../../include.hpp"
#include "args.hpp"
#include "decls.hpp"
#include "native.hpp"
#include "shader.hpp"
#include "slots.hpp"
#include "../techset.hpp"

#include <utils/cryptography.hpp>

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		utils::memory::allocator native_allocator;

		std::map<std::pair<const MaterialTechniqueSet*, std::vector<std::uint32_t>>, zonetool::h1::MaterialTechniqueSet*> converted_techsets;
		std::map<std::tuple<const MaterialTechnique*, std::vector<std::uint32_t>, std::int32_t, light_source_t, bool>, zonetool::h1::MaterialTechnique*> converted_techniques;
		std::unordered_set<const MaterialTechniqueSet*> failed_techsets;
		struct shader_info
		{
			std::set<std::uint32_t> constant_buffers;
			std::optional<resource_usage> usage;
			std::size_t light_loops;
			bool light_array;
			std::optional<light_array_shadow_slots> light_array_shadows;
			std::optional<sun_shadow_slots> sun_shadow;
			std::map<std::uint32_t, std::uint32_t> sampler_moves;
		};

		std::map<std::pair<const void*, std::string>, std::pair<void*, shader_info>> converted_shaders;
		std::unordered_set<const void*> dumped_shaders;
		std::map<std::string, std::vector<std::string>> failure_reasons;
		std::size_t converted_count{};
		std::size_t failed_count{};

		template <typename H1Shader>
		constexpr zonetool::h1::XAssetType shader_type()
		{
			if constexpr (std::is_same_v<H1Shader, zonetool::h1::MaterialVertexShader>)
			{
				return zonetool::h1::ASSET_TYPE_VERTEXSHADER;
			}
			else if constexpr (std::is_same_v<H1Shader, zonetool::h1::MaterialHullShader>)
			{
				return zonetool::h1::ASSET_TYPE_HULLSHADER;
			}
			else if constexpr (std::is_same_v<H1Shader, zonetool::h1::MaterialDomainShader>)
			{
				return zonetool::h1::ASSET_TYPE_DOMAINSHADER;
			}
			else
			{
				return zonetool::h1::ASSET_TYPE_PIXELSHADER;
			}
		}

		template <typename H1Shader>
		struct converted_shader
		{
			H1Shader* shader;
			const shader_info* info;
		};

		template <typename H1Shader, typename S2Shader>
		converted_shader<H1Shader> convert_shader(const S2Shader* source, const std::vector<constant_move>& moves, const std::vector<input_redirect>& redirects = {},
			const std::optional<sun_shadow_request>& sun_shadow = {}, const std::set<std::uint32_t>& srgb_textures = {}, const std::optional<sun_direction>& sun = {},
			const std::optional<light_array_request>& lights = {}, const std::set<std::uint32_t>& moved_samplers = {}, const std::set<std::uint32_t>& reserved_samplers = {},
			const std::optional<atmos_fog_rows>& fog = {})
		{
			if (!source)
			{
				return {};
			}

			std::string suffix;
			for (const auto& redirect : redirects)
			{
				suffix += utils::string::va("_ir%u%s%u%u%s", redirect.texcoord, redirect.target == "TEXCOORD" ? "t" : "p", redirect.target_index, redirect.component, redirect.absolute ? "a" : "");
			}

			for (const auto& move : moves)
			{
				suffix += utils::string::va("_cm%u_%u_%u", move.buffer, move.from, move.to);
			}

			if (sun_shadow)
			{
				suffix += utils::string::va("_sun%u_%u_%u", sun_shadow->mask_texture, sun_shadow->object_row, sun_shadow->stable_row);
				for (const auto slot : sun_shadow->reserved_samplers)
				{
					suffix += utils::string::va("r%u", slot);
				}
			}

			for (const auto slot : moved_samplers)
			{
				suffix += utils::string::va("_ms%u", slot);
			}

			if (sun)
			{
				suffix += utils::string::va("_sd%u_%u_%d", sun->buffer, sun->row, static_cast<int>(sun->w));
			}

			if (lights)
			{
				suffix += utils::string::va("_la%u%s", lights->object_row, lights->sun_first ? "s" : "");
				if (lights->source != LIGHT_SOURCE_ARRAY)
				{
					suffix += utils::string::va("k%d", lights->source);
				}

				for (const auto slot : lights->reserved_samplers)
				{
					suffix += utils::string::va("r%u", slot);
				}

				if (lights->grid_params)
				{
					suffix += utils::string::va("_%u_%u", lights->grid_params->first, lights->grid_params->second);
				}
			}

			if (fog)
			{
				suffix += utils::string::va("_af%u", fog->buffer);
				for (const auto& row : fog->rows)
				{
					suffix += row ? utils::string::va("_%u", *row) : "_n";
				}

				if (fog->lut_texture)
				{
					suffix += utils::string::va("_t%u", *fog->lut_texture);
				}
			}

			if (!srgb_textures.empty())
			{
				suffix += "_srgb";
				for (const auto texture : srgb_textures)
				{
					suffix += utils::string::va("_%u", texture);
				}
			}

			const auto key = std::make_pair(static_cast<const void*>(source), suffix);
			if (const auto entry = converted_shaders.find(key); entry != converted_shaders.end())
			{
				return {static_cast<H1Shader*>(entry->second.first), &entry->second.second};
			}

			if (!source->prog.loadDef.program)
			{
				if (!redirects.empty() || !moves.empty() || sun_shadow || !srgb_textures.empty() || sun || lights || fog || !moved_samplers.empty() || !is_global_shader(shader_type<H1Shader>(), source->name))
				{
					throw std::runtime_error(utils::string::va("shader \"%s\" has no program", source->name));
				}

				auto* global = native_allocator.allocate<H1Shader>();
				global->name = native_allocator.duplicate_string(source->name);
				const auto entry = converted_shaders.emplace(key, std::make_pair(global, shader_info{})).first;
				return {global, &entry->second.second};
			}

			const auto program = convert_program(source->prog.loadDef.program, source->prog.loadDef.programSize, redirects, moves, sun_shadow, srgb_textures, sun, lights, moved_samplers, reserved_samplers, fog);
			auto* result = native_allocator.allocate<H1Shader>();
			result->name = native_allocator.duplicate_string(::game::add_source_postfix(source->name + suffix, ::game::s2));
			result->prog.loadDef.programSize = static_cast<unsigned int>(program.data.size());
			result->prog.loadDef.program = native_allocator.allocate_array<unsigned char>(program.data.size());
			std::memcpy(result->prog.loadDef.program, program.data.data(), program.data.size());

			const auto entry = converted_shaders.emplace(key, std::make_pair(result, shader_info{program.constant_buffers, program.usage, program.light_loops, program.light_array, program.light_array_shadows, program.sun_shadow, program.sampler_moves})).first;
			return {result, &entry->second.second};
		}

		struct stage_usage
		{
			std::uint8_t stage;
			const shader_info* info;
		};

		std::uint32_t arg_buffer(const MaterialShaderArgument& arg, const std::size_t index, const MaterialPass& pass)
		{
			if (arg.type == MTL_ARG_MATERIAL_CONST)
			{
				return 3;
			}

			const auto per_obj = static_cast<std::size_t>(pass.perPrimArgCount);
			return index < per_obj ? 0 : index < per_obj + pass.perObjArgCount ? 1 : 2;
		}

		bool is_read(const MaterialShaderArgument& arg, const std::uint32_t buffer, const resource_usage& usage)
		{
			switch (arg.type)
			{
			case MTL_ARG_CODE_TEXTURE:
			case MTL_ARG_MATERIAL_TEXTURE:
				return usage.textures.contains(arg.dest);
			case MTL_ARG_CODE_SAMPLER:
			case MTL_ARG_MATERIAL_SAMPLER:
				return usage.samplers.contains(arg.dest);
			case MTL_ARG_CODE_CONST:
			case MTL_ARG_LITERAL_CONST:
			case MTL_ARG_MATERIAL_CONST:
			{
				if (usage.whole_buffers.contains(buffer))
				{
					return true;
				}

				const auto rows = usage.rows.find(buffer);
				const auto count = arg.type == MTL_ARG_CODE_CONST ? std::max<std::uint32_t>(arg.u.codeConst.rowCount, 1) : 1;
				return rows != usage.rows.end() && std::any_of(rows->second.begin(), rows->second.end(), [&](const std::uint32_t row)
				{
					return row >= arg.dest && row < arg.dest + count;
				});
			}
			default:
				return true;
			}
		}

		std::vector<MaterialShaderArgument> filter_args(MaterialPass& pass, const std::vector<stage_usage>& stages)
		{
			const auto count = static_cast<std::size_t>(pass.perPrimArgCount) + pass.perObjArgCount + pass.volatileArgCount + pass.stableArgCount;
			std::array<unsigned char*, 4> counts{&pass.perPrimArgCount, &pass.perObjArgCount, &pass.volatileArgCount, &pass.stableArgCount};
			std::array<std::size_t, 4> ends{};
			ends[0] = pass.perPrimArgCount;
			ends[1] = ends[0] + pass.perObjArgCount;
			ends[2] = ends[1] + pass.volatileArgCount;
			ends[3] = ends[2] + pass.stableArgCount;

			std::vector<MaterialShaderArgument> kept;
			std::array<unsigned char, 4> kept_counts{};
			for (auto i = 0u; i < count; i++)
			{
				const auto& arg = pass.args[i];
				const auto buffer = arg_buffer(arg, i, pass);
				const auto used = std::ranges::any_of(stages, [&](const stage_usage& stage)
				{
					return (arg.shader & stage.stage) && stage.info && (!stage.info->usage || is_read(arg, buffer, *stage.info->usage));
				});

				if (used)
				{
					kept.emplace_back(arg);
					kept_counts[std::ranges::upper_bound(ends, i) - ends.begin()]++;
				}
			}

			for (auto i = 0u; i < counts.size(); i++)
			{
				*counts[i] = kept_counts[i];
			}

			return kept;
		}

		std::uint32_t group_rows(const MaterialShaderArgument* args, const std::size_t count, const unsigned short size)
		{
			auto rows = static_cast<std::uint32_t>((size + 15) / 16);
			for (auto i = 0u; i < count; i++)
			{
				const auto& arg = args[i];
				if (arg.type == MTL_ARG_CODE_CONST || arg.type == MTL_ARG_LITERAL_CONST)
				{
					rows = std::max<std::uint32_t>(rows, arg.dest + (arg.type == MTL_ARG_CODE_CONST ? std::max<std::uint32_t>(arg.u.codeConst.rowCount, 1) : 1));
				}
			}

			return rows;
		}

		std::optional<sun_shadow_request> find_sun_shadow_mask(const MaterialPass& pass)
		{
			const auto count = static_cast<std::size_t>(pass.perPrimArgCount) + pass.perObjArgCount + pass.volatileArgCount + pass.stableArgCount;
			for (auto i = 0u; i < count; i++)
			{
				const auto& arg = pass.args[i];
				if (arg.type == MTL_ARG_CODE_TEXTURE && arg.u.codeSampler == TEXTURE_SRC_CODE_DEFERRED_SUN_SHADOW_VISIBILITY && (arg.shader & zonetool::h1::MTL_SHADER_STAGE_PIXEL))
				{
					const auto* per_obj = pass.args + pass.perPrimArgCount;
					const auto* stable = per_obj + pass.perObjArgCount + pass.volatileArgCount;
					return sun_shadow_request{arg.dest, group_rows(per_obj, pass.perObjArgCount, pass.perObjArgSize), group_rows(stable, pass.stableArgCount, pass.stableArgSize)};
				}
			}

			return {};
		}

		std::set<std::uint32_t> custom_sampler_slots(const unsigned char flags)
		{
			std::set<std::uint32_t> result;
			for (const auto [flag, slot] : {std::pair{zonetool::h1::CUSTOM_SAMPLER_FLAG_USE_REFLECTION_PROBE, 1u},
				std::pair{zonetool::h1::CUSTOM_SAMPLER_FLAG_USE_LIGHTMAP_PRIMARY, 2u}, std::pair{zonetool::h1::CUSTOM_SAMPLER_FLAG_USE_LIGHTMAP_SECONDARY, 3u}})
			{
				if (flags & flag)
				{
					result.insert(slot);
				}
			}

			return result;
		}

		bool is_sampler_arg(const MaterialShaderArgument& arg)
		{
			return arg.type == MTL_ARG_CODE_SAMPLER || arg.type == MTL_ARG_MATERIAL_SAMPLER;
		}

		std::set<std::uint32_t> moved_sampler_slots(const MaterialPass& pass, const std::set<std::uint32_t>& reserved)
		{
			std::set<std::uint32_t> result;
			const auto count = static_cast<std::size_t>(pass.perPrimArgCount) + pass.perObjArgCount + pass.volatileArgCount + pass.stableArgCount;
			for (auto i = 0u; i < count; i++)
			{
				const auto& arg = pass.args[i];
				if (is_sampler_arg(arg) && (arg.shader & zonetool::h1::MTL_SHADER_STAGE_PIXEL) && reserved.contains(arg.dest))
				{
					if (arg.shader != zonetool::h1::MTL_SHADER_STAGE_PIXEL)
					{
						throw std::runtime_error(utils::string::va("sampler arg on s%u for stages 0x%X", arg.dest, arg.shader));
					}

					result.insert(arg.dest);
				}
			}

			return result;
		}

		zonetool::h1::MaterialShaderArgument make_code_arg(const std::uint8_t type, const unsigned short dest, const std::uint32_t source)
		{
			zonetool::h1::MaterialShaderArgument arg{};
			arg.type = type;
			arg.shader = zonetool::h1::MTL_SHADER_STAGE_PIXEL;
			arg.dest = dest;
			arg.u.codeSampler = source;
			return arg;
		}

		void add_sun_shadow_args(converted_args& args, const sun_shadow_request& request, const sun_shadow_slots& slots)
		{
			auto lookup = make_code_arg(zonetool::h1::MTL_ARG_CODE_CONST, static_cast<unsigned short>(request.object_row), 0);
			lookup.u.codeConst = {static_cast<unsigned short>(zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX), 0, static_cast<unsigned char>(sun_shadow_object_rows)};
			args.per_obj.emplace_back(lookup);

			for (const auto& [row, source] : sun_shadow_stable_consts())
			{
				auto arg = make_code_arg(zonetool::h1::MTL_ARG_CODE_CONST, static_cast<unsigned short>(request.stable_row + row), 0);
				arg.u.codeConst = {static_cast<unsigned short>(source), 0, 1};
				args.stable.emplace_back(arg);
			}

			args.stable.emplace_back(make_code_arg(zonetool::h1::MTL_ARG_CODE_TEXTURE, static_cast<unsigned short>(slots.shadow_texture), zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SUN));
			args.stable.emplace_back(make_code_arg(zonetool::h1::MTL_ARG_CODE_TEXTURE, static_cast<unsigned short>(slots.translucent_texture), zonetool::h1::TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN));
			args.stable.emplace_back(make_code_arg(zonetool::h1::MTL_ARG_CODE_SAMPLER, static_cast<unsigned short>(slots.compare_sampler), zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SUN));
			args.stable.emplace_back(make_code_arg(zonetool::h1::MTL_ARG_CODE_SAMPLER, static_cast<unsigned short>(slots.linear_sampler), zonetool::h1::TEXTURE_SRC_CODE_LINEAR_CLAMP));
			std::ranges::stable_sort(args.per_obj, {}, &zonetool::h1::MaterialShaderArgument::type);
			std::ranges::stable_sort(args.stable, {}, &zonetool::h1::MaterialShaderArgument::type);
			args.stable_size = std::max<unsigned short>(args.stable_size, static_cast<unsigned short>((request.stable_row + sun_shadow_stable_rows) * 16));
		}

		std::optional<std::pair<std::uint32_t, std::uint32_t>> find_grid_params(const MaterialPass& pass)
		{
			const auto count = static_cast<std::size_t>(pass.perPrimArgCount) + pass.perObjArgCount + pass.volatileArgCount + pass.stableArgCount;
			for (auto i = 0u; i < count; i++)
			{
				const auto& arg = pass.args[i];
				if (arg.type == MTL_ARG_CODE_CONST && arg.u.codeConst.index == CONST_SRC_CODE_FRUSTUM_GRID_LIGHT_PARAMS && (arg.shader & zonetool::h1::MTL_SHADER_STAGE_PIXEL))
				{
					return std::make_pair(arg_buffer(arg, i, pass), static_cast<std::uint32_t>(arg.dest));
				}
			}

			return {};
		}

		void add_light_array_args(converted_args& args, const light_array_request& request, const std::optional<light_array_shadow_slots>& shadows)
		{
			for (const auto& entry : light_array_object_consts(request.source))
			{
				auto arg = make_code_arg(zonetool::h1::MTL_ARG_CODE_CONST, static_cast<unsigned short>(request.object_row + entry.row), 0);
				arg.u.codeConst = {static_cast<unsigned short>(entry.source), 0, entry.rows};
				args.per_obj.emplace_back(arg);
			}

			if (shadows)
			{
				for (auto light = 0u; light < shadows->textures.size(); light++)
				{
					if (const auto texture = shadows->textures[light])
					{
						args.stable.emplace_back(make_code_arg(zonetool::h1::MTL_ARG_CODE_TEXTURE, static_cast<unsigned short>(*texture),
							zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT + light));
					}
				}

				args.stable.emplace_back(make_code_arg(zonetool::h1::MTL_ARG_CODE_SAMPLER, static_cast<unsigned short>(shadows->compare_sampler),
					zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SUN));
				std::ranges::stable_sort(args.stable, {}, &zonetool::h1::MaterialShaderArgument::type);
			}

			std::ranges::stable_sort(args.per_obj, {}, &zonetool::h1::MaterialShaderArgument::type);
		}

		std::set<std::uint32_t> srgb_slots(const MaterialPass& pass, const std::vector<std::uint32_t>& srgb_textures)
		{
			std::set<std::uint32_t> result;
			const auto count = static_cast<std::size_t>(pass.perPrimArgCount) + pass.perObjArgCount + pass.volatileArgCount + pass.stableArgCount;
			for (auto i = 0u; i < count && !srgb_textures.empty(); i++)
			{
				const auto& arg = pass.args[i];
				if (arg.type == MTL_ARG_MATERIAL_TEXTURE && (arg.shader & zonetool::h1::MTL_SHADER_STAGE_PIXEL)
					&& std::ranges::binary_search(srgb_textures, arg.u.nameHash))
				{
					result.insert(arg.dest);
				}
			}

			return result;
		}

		std::optional<sun_direction> find_sun_direction(const MaterialPass& pass, const sun_mode_t mode)
		{
			const auto count = static_cast<std::size_t>(pass.perPrimArgCount) + pass.perObjArgCount + pass.volatileArgCount + pass.stableArgCount;
			for (auto i = 0u; i < count; i++)
			{
				const auto& arg = pass.args[i];
				if (arg.type == MTL_ARG_CODE_CONST && arg.u.codeConst.index == CONST_SRC_CODE_SUN_DIRECTION && (arg.shader & zonetool::h1::MTL_SHADER_STAGE_PIXEL))
				{
					return sun_direction{arg_buffer(arg, i, pass), arg.dest, sun_mode_w(mode)};
				}
			}

			return {};
		}

		std::optional<atmos_fog_rows> find_atmos_fog(const MaterialPass& pass, const std::uint8_t stage)
		{
			atmos_fog_rows result{};
			std::optional<std::uint32_t> buffer;
			const auto count = static_cast<std::size_t>(pass.perPrimArgCount) + pass.perObjArgCount + pass.volatileArgCount + pass.stableArgCount;
			for (auto i = 0u; i < count; i++)
			{
				const auto& arg = pass.args[i];
				if (!(arg.shader & stage))
				{
					continue;
				}

				if ((arg.type == MTL_ARG_CODE_TEXTURE || arg.type == MTL_ARG_CODE_SAMPLER) && arg.u.codeSampler == TEXTURE_SRC_CODE_FOG_SPLINE)
				{
					if (arg.type == MTL_ARG_CODE_TEXTURE)
					{
						result.lut_texture = arg.dest;
					}

					continue;
				}

				if (arg.type != MTL_ARG_CODE_CONST || arg.u.codeConst.index < CONST_SRC_CODE_ATMOS_FOG_PARMS_0 || arg.u.codeConst.index > CONST_SRC_CODE_ATMOS_FOG_PARMS_7)
				{
					continue;
				}

				const auto arg_buffer_index = arg_buffer(arg, i, pass);
				if (buffer && *buffer != arg_buffer_index)
				{
					throw std::runtime_error("atmos fog rows in two constant buffers");
				}

				buffer = arg_buffer_index;
				result.rows[arg.u.codeConst.index - CONST_SRC_CODE_ATMOS_FOG_PARMS_0] = arg.dest;
			}

			if (!buffer && !result.lut_texture)
			{
				return {};
			}

			result.buffer = buffer.value_or(2u);
			return result;
		}

		bool has_sun_direction(const MaterialTechnique* technique)
		{
			for (auto i = 0; i < technique->hdr.passCount; i++)
			{
				if (find_sun_direction(technique->passArray[i], SUN_MODE_NONE))
				{
					return true;
				}
			}

			return false;
		}

		zonetool::h1::MaterialPass convert_pass(const MaterialPass& source, const std::vector<std::uint32_t>& srgb_textures, const sun_mode_t mode, const light_source_t light_source,
			const bool sun_first)
		{
			const auto srgb = srgb_slots(source, srgb_textures);
			const auto sun = find_sun_direction(source, mode);
			if (source.modelVertexType == MODEL_VERTEX_TYPE_LIGHTMAP)
			{
				throw std::runtime_error(utils::string::va("model vertex type %u", source.modelVertexType));
			}

			const auto& decl = convert_decl(source.vertexDecl, native_allocator);
			const auto reserved_samplers = custom_sampler_slots(source.customSamplerFlags);
			const auto moved_samplers = moved_sampler_slots(source, reserved_samplers);
			auto sun_shadow = find_sun_shadow_mask(source);
			if (sun_shadow)
			{
				sun_shadow->reserved_samplers = reserved_samplers;
			}

			std::optional<light_array_request> lights;
			if (light_source != LIGHT_SOURCE_NONE)
			{
				const auto* per_obj = source.args + source.perPrimArgCount;
				const auto object_row = group_rows(per_obj, source.perObjArgCount, source.perObjArgSize) + (sun_shadow ? sun_shadow_object_rows : 0);
				lights = light_array_request{object_row, find_grid_params(source), sun_first, reserved_samplers, light_source};
			}

			const auto vertex_fog = find_atmos_fog(source, zonetool::h1::MTL_SHADER_STAGE_VERTEX);
			const auto pixel_fog = find_atmos_fog(source, zonetool::h1::MTL_SHADER_STAGE_PIXEL);
			const std::vector<stage_usage> stages
			{
				{zonetool::h1::MTL_SHADER_STAGE_VERTEX, convert_shader<zonetool::h1::MaterialVertexShader>(source.vertexShader, {}, decl.redirects, {}, {}, {}, {}, {}, {}, vertex_fog).info},
				{zonetool::h1::MTL_SHADER_STAGE_HULL, convert_shader<zonetool::h1::MaterialHullShader>(source.hullShader, {}).info},
				{zonetool::h1::MTL_SHADER_STAGE_DOMAIN, convert_shader<zonetool::h1::MaterialDomainShader>(source.domainShader, {}).info},
				{zonetool::h1::MTL_SHADER_STAGE_PIXEL, convert_shader<zonetool::h1::MaterialPixelShader>(source.pixelShader, {}, {}, sun_shadow, srgb, sun, lights, moved_samplers, reserved_samplers, pixel_fog).info},
			};

			auto pass = source;
			const auto arg_count = static_cast<std::size_t>(source.perPrimArgCount) + source.perObjArgCount + source.volatileArgCount + source.stableArgCount;
			std::vector<MaterialShaderArgument> source_args(source.args, source.args + arg_count);
			if (const auto* pixel = stages.back().info)
			{
				for (auto& arg : source_args)
				{
					const auto move = pixel->sampler_moves.find(arg.dest);
					if (is_sampler_arg(arg) && (arg.shader & zonetool::h1::MTL_SHADER_STAGE_PIXEL) && move != pixel->sampler_moves.end())
					{
						arg.dest = static_cast<unsigned short>(move->second);
					}
				}
			}

			pass.args = source_args.data();
			const auto filtered = filter_args(pass, stages);
			pass.args = const_cast<MaterialShaderArgument*>(filtered.data());

			auto args = convert_args(pass, sun_shadow ? sun_shadow->stable_row + sun_shadow_stable_rows : 0, sun && mode == SUN_MODE_SHADOW_LIGHT_ARRAY, native_allocator);
			const auto stage_moves = [&](const std::uint8_t stage)
			{
				std::vector<constant_move> moves;
				for (const auto& move : args.moves)
				{
					if (move.stages & stage)
					{
						moves.emplace_back(move.buffer, move.from, move.to);
					}
				}

				return moves;
			};

			std::set<std::uint32_t> constant_buffers;
			auto light_loops = 0u;
			const auto add_info = [&](const auto& converted)
			{
				if (converted.info)
				{
					constant_buffers.insert(converted.info->constant_buffers.begin(), converted.info->constant_buffers.end());
					light_loops += static_cast<unsigned int>(converted.info->light_loops);
				}

				return converted.shader;
			};

			zonetool::h1::MaterialPass result{};
			result.vertexShader = add_info(convert_shader<zonetool::h1::MaterialVertexShader>(pass.vertexShader, stage_moves(zonetool::h1::MTL_SHADER_STAGE_VERTEX), decl.redirects, {}, {}, {}, {}, {}, {}, vertex_fog));
			result.vertexDecl = decl.decl;
			result.hullShader = add_info(convert_shader<zonetool::h1::MaterialHullShader>(pass.hullShader, stage_moves(zonetool::h1::MTL_SHADER_STAGE_HULL)));
			result.domainShader = add_info(convert_shader<zonetool::h1::MaterialDomainShader>(pass.domainShader, stage_moves(zonetool::h1::MTL_SHADER_STAGE_DOMAIN)));
			const auto pixel_shader = convert_shader<zonetool::h1::MaterialPixelShader>(pass.pixelShader, stage_moves(zonetool::h1::MTL_SHADER_STAGE_PIXEL), {}, sun_shadow, srgb, sun, lights, moved_samplers, reserved_samplers, pixel_fog);
			result.pixelShader = add_info(pixel_shader);
			auto per_obj_size = pass.perObjArgSize;
			if (sun_shadow && pixel_shader.info && pixel_shader.info->sun_shadow)
			{
				add_sun_shadow_args(args, *sun_shadow, *pixel_shader.info->sun_shadow);
				per_obj_size = std::max<unsigned short>(per_obj_size, static_cast<unsigned short>((sun_shadow->object_row + sun_shadow_object_rows) * 16));
				constant_buffers.insert(1u);
			}

			const auto fed_lights = lights && pixel_shader.info && pixel_shader.info->light_array;
			if (fed_lights)
			{
				add_light_array_args(args, *lights, pixel_shader.info->light_array_shadows);
				per_obj_size = std::max<unsigned short>(per_obj_size, static_cast<unsigned short>((lights->object_row + light_array_object_rows) * 16));
				constant_buffers.insert(1u);
			}

			result.pixelOutputMask = pass.pixelOutputMask;
			const auto light_buffers = light_loops || fed_lights ? std::uint64_t{CUSTOM_BUFFER_FORWARD_LIGHTING | CUSTOM_BUFFER_LIGHT_CLUSTER_BITS} : 0;
			result.customBufferFlags = convert_custom_buffer_flags(pass.customBufferFlags & ~light_buffers);
			result.customSamplerFlags = pass.customSamplerFlags;
			result.precompiledIndex = 0; // generic per object path (1.4 sub_1405D0950)
			result.stageConfig = pass.hullShader ? 1 : 0;
			result.zone = pass.zone;
			result.perPrimArgSize = pass.perPrimArgSize;
			result.perObjArgSize = per_obj_size;
			result.stableArgSize = args.stable_size;

			for (const auto& [buffer, group] : {std::pair{0u, &args.per_prim}, std::pair{1u, &args.per_obj}, std::pair{2u, &args.stable}})
			{
				if (constant_buffers.contains(buffer) && group->empty())
				{
					throw std::runtime_error(utils::string::va("shaders read cb%u without arguments", buffer));
				}
			}

			result.perPrimArgCount = static_cast<unsigned char>(args.per_prim.size());
			result.perObjArgCount = static_cast<unsigned char>(args.per_obj.size());
			result.stableArgCount = static_cast<unsigned char>(args.stable.size());

			const auto count = args.per_prim.size() + args.per_obj.size() + args.stable.size();
			if (count)
			{
				result.args = native_allocator.allocate_array<zonetool::h1::MaterialShaderArgument>(count);
				auto* dest = std::ranges::copy(args.per_prim, result.args).out;
				dest = std::ranges::copy(args.per_obj, dest).out;
				std::ranges::copy(args.stable, dest);
			}

			return result;
		}

		std::string srgb_suffix(const std::vector<std::uint32_t>& srgb_textures)
		{
			if (srgb_textures.empty())
			{
				return {};
			}

			std::string key;
			for (const auto texture : srgb_textures)
			{
				key += utils::string::va("%08X", texture);
			}

			return utils::string::va("_srgb%08x", utils::cryptography::jenkins_one_at_a_time::compute(key));
		}

		zonetool::h1::MaterialTechnique* convert_technique(const MaterialTechnique* technique, const std::vector<std::uint32_t>& srgb_textures, sun_mode_t mode, const light_source_t light_source)
		{
			const auto sun_first = mode == SUN_MODE_SHADOW_LIGHT_ARRAY;
			mode = has_sun_direction(technique) ? mode : SUN_MODE_NONE;
			const auto key = std::make_tuple(technique, srgb_textures, mode, light_source, sun_first);
			if (const auto entry = converted_techniques.find(key); entry != converted_techniques.end())
			{
				return entry->second;
			}

			const auto pass_count = technique->hdr.passCount;
			const auto size = sizeof(zonetool::h1::MaterialTechniqueHeader) + sizeof(zonetool::h1::MaterialPass) * std::max<std::uint16_t>(pass_count, 1);
			auto* result = reinterpret_cast<zonetool::h1::MaterialTechnique*>(native_allocator.allocate_array<char>(size));
			constexpr const char* sun_suffixes[]{"", "_sun", "_sun_shadow", "_sun_shadow_db"};
			const auto sun_suffix = sun_suffixes[mode];
			constexpr const char* light_suffixes[]{"", "_la", "_lspot", "_lspot_shadow", "_lomni"};
			const auto light_suffix = light_source == LIGHT_SOURCE_ARRAY && sun_first ? "_las" : light_suffixes[light_source];
			result->hdr.name = native_allocator.duplicate_string(::game::add_source_postfix(technique->hdr.name + srgb_suffix(srgb_textures) + sun_suffix + light_suffix, ::game::s2));
			result->hdr.flags = technique->hdr.flags;
			result->hdr.passCount = pass_count;

			for (auto i = 0; i < pass_count; i++)
			{
				try
				{
					result->passArray[i] = convert_pass(technique->passArray[i], srgb_textures, mode, light_source, sun_first);
				}
				catch (const std::exception& e)
				{
					throw std::runtime_error(utils::string::va("technique \"%s\" pass %d: %s", technique->hdr.name, i, e.what()));
				}
			}

			converted_techniques.emplace(key, result);
			return result;
		}

		std::string reason_key(const std::string& error)
		{
			const auto pass = error.find(": ");
			auto key = pass != std::string::npos && error.starts_with("technique") ? error.substr(pass + 2) : error;
			if (const auto quote = key.find(" \""); quote != std::string::npos)
			{
				key = key.substr(0, quote);
			}

			return key;
		}

		zonetool::h1::MaterialTechniqueSet* build_native(const MaterialTechniqueSet* asset, const std::vector<std::uint32_t>& srgb_textures, std::set<std::string>& errors)
		{
			auto* result = native_allocator.allocate<zonetool::h1::MaterialTechniqueSet>();
			result->name = native_allocator.duplicate_string(::game::add_source_postfix(asset->name + srgb_suffix(srgb_textures), ::game::s2));
			result->flags = asset->flags;
			result->worldVertFormat = asset->worldVertFormat;
			result->preDisplacementOnlyCount = asset->preDisplacementOnlyCount;

			auto techniques = 0u;
			for (auto slot = 0; slot < zonetool::h1::TECHNIQUE_COUNT; slot++)
			{
				const auto source = find_source_slot(asset, slot);
				if (source < 0)
				{
					continue;
				}

				try
				{
					result->techniques[slot] = convert_technique(asset->techniques[source], srgb_textures, sun_mode(slot), light_source(slot));
					techniques++;
				}
				catch (const std::exception& e)
				{
					if (errors.insert(utils::string::va("s2 slot %d: %s", source % TECHNIQUE_GROUP_SIZE, reason_key(e.what()).data())).second)
					{
						ZONETOOL_INFO("native techset %s: %s", asset->name, e.what());
					}
				}
			}

			if (!techniques && errors.empty())
			{
				errors.insert("no technique has an h1 slot");
			}

			return errors.empty() ? result : nullptr;
		}

		template <typename AssetClass, typename Shader>
		void dump_shader(Shader* shader)
		{
			if (shader && shader->prog.loadDef.program && dumped_shaders.insert(shader).second)
			{
				AssetClass::dump(shader);
			}
		}
	}

	zonetool::h1::MaterialTechniqueSet* convert_native(const MaterialTechniqueSet* asset, const std::vector<std::uint32_t>& srgb_textures)
	{
		if (!asset || failed_techsets.contains(asset))
		{
			return nullptr;
		}

		const auto key = std::make_pair(asset, srgb_textures);
		if (const auto entry = converted_techsets.find(key); entry != converted_techsets.end())
		{
			return entry->second;
		}

		const auto first = std::ranges::none_of(converted_techsets, [&](const auto& entry)
		{
			return entry.first.first == asset;
		});

		auto& entry = converted_techsets[key];
		std::set<std::string> errors;
		auto* result = build_native(asset, srgb_textures, errors);
		if (!result)
		{
			failed_techsets.insert(asset);
			failed_count++;
			for (const auto& error : errors)
			{
				failure_reasons[error].emplace_back(asset->name);
			}

			ZONETOOL_INFO("native techset %s: not converted, %zu reasons", asset->name, errors.size());
			return entry;
		}

		if (first)
		{
			converted_count++;
			ZONETOOL_INFO("native techset %s: converted", asset->name);
		}

		entry = result;
		return entry;
	}

	void dump_native(zonetool::h1::MaterialTechniqueSet* asset)
	{
		zonetool::h1::techset::dump(asset);
		for (const auto* technique : asset->techniques)
		{
			for (auto i = 0; technique && i < technique->hdr.passCount; i++)
			{
				const auto& pass = technique->passArray[i];
				dump_shader<zonetool::h1::vertex_shader>(pass.vertexShader);
				dump_shader<zonetool::h1::hull_shader>(pass.hullShader);
				dump_shader<zonetool::h1::domain_shader>(pass.domainShader);
				dump_shader<zonetool::h1::pixel_shader>(pass.pixelShader);
			}
		}
	}

	void finish_native()
	{
		if (failed_count)
		{
			ZONETOOL_WARNING("Native techsets: %zu converted, %zu not converted (stock H1 techsets used instead)", converted_count, failed_count);
		}
		else
		{
			ZONETOOL_INFO("Native techsets: %zu converted", converted_count);
		}
		std::vector<std::pair<std::string, std::size_t>> reasons;
		for (const auto& [reason, techsets] : failure_reasons)
		{
			reasons.emplace_back(reason, techsets.size());
		}

		std::ranges::sort(reasons, std::greater{}, &std::pair<std::string, std::size_t>::second);
		for (const auto& [reason, count] : reasons)
		{
			ZONETOOL_INFO("   %zu techsets: %s", count, reason.data());
		}

		converted_techsets.clear();
		converted_techniques.clear();
		failed_techsets.clear();
		converted_shaders.clear();
		dumped_shaders.clear();
		failure_reasons.clear();
		converted_count = 0;
		failed_count = 0;
		clear_decls();
		native_allocator.clear();
	}
}
