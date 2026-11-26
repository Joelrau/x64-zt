#include <std_include.hpp>
#include "shaderset.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_program(auto& reader, ShaderProgram& program)
		{
			reader.read_array(program.bytecode, 0xFF, program.bytecodeSize);
			reader.read_array(program.args, 3, program.argCount);

			if (const auto images = reader.read_array(program.images, 7, program.imageCount))
			{
				for (auto i = 0u; i < program.imageCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, images[i].image);
				}
			}

			reader.read_array(program.values, 0xF, program.valueCount);
		}

		void read_stage(auto& reader, RaytraceShaderStage& stage)
		{
			if (const auto library = reader.read_single(stage.library, 7))
			{
				reader.read_array(library->bytecode, 0xFF, library->bytecodeSize);
			}

			if (const auto hit_groups = reader.read_array(stage.hitGroups, 7, stage.hitGroupCount))
			{
				for (auto i = 0ull; i < stage.hitGroupCount; i++)
				{
					reader.read_string(hit_groups[i].name);
					reader.read_string(hit_groups[i].entryName);
					if (const auto program = reader.read_single(hit_groups[i].program, 7))
					{
						read_program(reader, *program);
					}
				}
			}

			for (auto& list : stage.exportLists)
			{
				if (const auto exports = reader.read_array(list.exports, 7, list.count))
				{
					for (auto i = 0ull; i < list.count; i++)
					{
						reader.read_string(exports[i].name);
						reader.read_string(exports[i].entryName);
					}
				}
			}

			if (const auto symbols = reader.read_array(stage.symbols, 7, stage.symbolCount))
			{
				for (auto i = 0ull; i < stage.symbolCount; i++)
				{
					reader.read_string(symbols[i].name);
				}
			}
		}
	}

	void compute_shader_set::read(auto& reader, ComputeShaderSet* asset)
	{
		if (const auto shaders = reader.read_array(asset->shaders, 7, asset->shaderCount))
		{
			for (auto i = 0u; i < asset->shaderCount; i++)
			{
				if (const auto shader = reader.read_single(shaders[i], 7))
				{
					for (auto& program : shader->programs)
					{
						read_program(reader, program);
					}
				}
			}
		}
	}

	void raytrace_shader_set::read(auto& reader, RaytraceShaderSet* asset)
	{
		if (const auto shaders = reader.read_array(asset->shaders, 7, asset->shaderCount))
		{
			for (auto i = 0u; i < asset->shaderCount; i++)
			{
				if (const auto shader = reader.read_single(shaders[i], 7))
				{
					for (auto& stage : shader->stages)
					{
						read_stage(reader, stage);
					}
				}
			}
		}
	}

	namespace
	{
		void read_render_target_nodes(auto& reader, RenderTargetNode*& field)
		{
			for (auto* node = &field; *node;)
			{
				const auto loaded = reader.read_array(*node, 7);
				if (!loaded)
				{
					break;
				}

				node = &loaded->next;
			}
		}

		void read_render_target_def(auto& reader, RenderTargetDef*& field)
		{
			if (const auto def = reader.read_array(field, 7))
			{
				read_render_target_def(reader, def->child);
				read_render_target_nodes(reader, def->nodes);
			}
		}
	}

	void render_targets::read(auto& reader, RenderTargets* asset)
	{
		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0ull; i < asset->entryCount; i++)
			{
				read_render_target_def(reader, entries[i].def);
				if (const auto defs = reader.read_array(entries[i].defs, 7, entries[i].defCount))
				{
					for (auto j = 0ull; j < entries[i].defCount; j++)
					{
						read_render_target_def(reader, defs[j]);
					}
				}
			}
		}
	}

	void hw_platform::read(auto& reader, HwPlatform* asset)
	{
		for (auto& list : asset->lists)
		{
			reader.read_array(list.values, 7, list.count);
		}
	}
}

namespace zonetool::t9
{
	template void compute_shader_set::read(zone_reader&, ComputeShaderSet*);
	template void render_targets::read(zone_reader&, RenderTargets*);
	template void hw_platform::read(zone_reader&, HwPlatform*);
	template void raytrace_shader_set::read(zone_reader&, RaytraceShaderSet*);
	template void compute_shader_set::read(zone_writer&, ComputeShaderSet*);
	template void render_targets::read(zone_writer&, RenderTargets*);
	template void hw_platform::read(zone_writer&, HwPlatform*);
	template void raytrace_shader_set::read(zone_writer&, RaytraceShaderSet*);
}
