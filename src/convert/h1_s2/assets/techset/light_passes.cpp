#include <std_include.hpp>

#include "../../include.hpp"
#include "../techset.hpp"
#include "args.hpp"
#include "common.hpp"
#include "light_passes.hpp"
#include "passes.hpp"
#include "slots.hpp"

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		std::map<std::pair<const zonetool::s2::MaterialTechnique*, const MaterialPass*>, zonetool::s2::MaterialTechnique*> light_pass_techniques;
		std::map<std::tuple<const zonetool::s2::MaterialTechnique*, const MaterialPass*, bool>, zonetool::s2::MaterialTechnique*> merged_light_techniques;

		constexpr std::array<std::uint16_t, 3> light_cluster_consts{zonetool::s2::CONST_SRC_CODE_LIGHT_CLUSTER_CELLS,
			zonetool::s2::CONST_SRC_CODE_LIGHT_CLUSTER_SLICE_STARTS, zonetool::s2::CONST_SRC_CODE_LIGHT_CLUSTER_SLICE_SCALES};

		std::unordered_map<std::string, zonetool::s2::MaterialPixelShader*> merged_pixel_shaders;
		std::unordered_map<std::string, unsigned int> merged_pixel_shader_names;

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

				const auto dest = is_constant ? next_row : is_sampler ? free_slot(samplers, 0, 16u) : free_slot(textures, 0, 26u);
				next_row += is_constant ? rows_count : 0;
				for (auto row = 0u; row < rows_count; row++)
				{
					slots.emplace(arg.dest + row, dest + row);
				}

				arg.dest = static_cast<unsigned short>(dest);
				stable.emplace_back(arg);
			}

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
				for (const auto& [type, dest, source] : {std::tuple{zonetool::s2::MTL_ARG_CODE_TEXTURE, shader::spot_shadow_slot, 8u},
					std::tuple{zonetool::s2::MTL_ARG_CODE_TEXTURE, shader::spot_shadow_dynamic_slot, 13u},
					std::tuple{zonetool::s2::MTL_ARG_CODE_SAMPLER, shader::spot_shadow_sampler, 6u}})
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
			pass.customBufferFlags |= clustered ? (zonetool::s2::CUSTOM_BUFFER_FORWARD_LIGHTS | zonetool::s2::CUSTOM_BUFFER_LIGHT_CLUSTER_BITS) : zonetool::s2::CUSTOM_BUFFER_FORWARD_LIGHTING;
			if (clustered)
			{
				pass.customBufferFlags &= ~(zonetool::s2::CUSTOM_BUFFER_LIGHT_TILE_BITS | zonetool::s2::CUSTOM_BUFFER_LIGHT_TILE_ZBINS);
			}

			merged_light_techniques.emplace(key, result);
			return result;
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
}
