#include <std_include.hpp>

#include "../../include.hpp"
#include "common.hpp"
#include "instancing.hpp"

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		std::map<std::pair<const MaterialVertexShader*, std::optional<std::uint32_t>>, MaterialVertexShader*> instanced_vertex_shaders;

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
	}

	const MaterialTechniqueSet* add_instanced_techniques(const MaterialTechniqueSet* techset)
	{
		auto* result = native_allocator.allocate<MaterialTechniqueSet>();
		*result = *techset;
		for (auto type = 0; type < TECHNIQUE_INSTANCED_BEGIN - TECHNIQUE_STANDARD_BEGIN; type++)
		{
			const auto* standard = techset->techniques[TECHNIQUE_STANDARD_BEGIN + type];
			const auto* instanced = techset->techniques[TECHNIQUE_INSTANCED_BEGIN + type];

			const auto rebuild = instanced && std::string_view(instanced->hdr.name).contains("_amb_") && instanced->hdr.passCount && (instanced->passArray[0].customBufferFlags & CUSTOM_BUFFER_SMODEL_AMBIENT);
			if (!standard || (instanced && !rebuild))
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
}
