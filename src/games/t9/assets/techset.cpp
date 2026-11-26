#include <std_include.hpp>
#include "techset.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_program(auto& reader, MaterialShaderProgram*& field)
		{
			if (const auto program = reader.read_array(field, 7))
			{
				reader.read_array(program->bytecode, 0xFF, program->bytecodeSize);
			}
		}

		void read_shader_set(auto& reader, MaterialShaderSet*& field)
		{
			const auto shader_set = reader.read_array(field, 7);
			if (!shader_set)
			{
				return;
			}

			reader.read_array(shader_set->layout, 7);
			for (const auto index : {0, 1, 3, 2, 4, 5})
			{
				read_program(reader, shader_set->programs[index]);
			}
			reader.read_array(shader_set->info, 7);
		}

		void read_pass(auto& reader, MaterialPass& pass)
		{
			reader.read_array(pass.args, 3, pass.argCounts[0] + pass.argCounts[1]);
			reader.read_array(pass.values, 3, pass.valueCount);
			read_shader_set(reader, pass.shaderSet);

			if (const auto table = reader.read_array(pass.table, 7))
			{
				reader.read_array(table->table, 0, 0x8000);
				reader.read_array(table->entries, 7, table->entryCount);
			}
		}

		void read_info(auto& reader, MaterialTechniqueSetInfo*& field)
		{
			const auto info = reader.read_array(field, 7);
			if (!info)
			{
				return;
			}

			for (auto& string : info->strings)
			{
				reader.read_string(string);
			}

			if (const auto names = reader.read_array(info->names, 7, info->nameCount))
			{
				for (auto i = 0ull; i < info->nameCount; i++)
				{
					reader.read_string(names[i]);
				}
			}
		}
	}

	void techset::read(auto& reader, MaterialTechniqueSet* asset)
	{
		for (auto& field : asset->techniques)
		{
			const auto technique = reader.read_array(field, 7);
			if (!technique)
			{
				continue;
			}

			for (auto& pass : technique->passes)
			{
				read_pass(reader, pass);
			}
			reader.read_array(technique->hashes, 7, technique->hashCount);
		}

		read_info(reader, asset->info);
	}

	void techset::dump(MaterialTechniqueSet* asset)
	{
		filesystem::file file(std::format("techsets/{}.techset.txt", xhash_name(asset->name)));
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		std::string text;
		for (auto i = 0u; i < std::size(asset->techniques); i++)
		{
			const auto technique = asset->techniques[i];
			if (!technique)
			{
				continue;
			}

			text += std::format("{}: {}\n", i, xhash_name(technique->name));
			for (auto pass_index = 0u; pass_index < std::size(technique->passes); pass_index++)
			{
				const auto& pass = technique->passes[pass_index];
				if (!pass.shaderSet)
				{
					continue;
				}

				for (auto program_index = 0u; program_index < std::size(pass.shaderSet->programs); program_index++)
				{
					const auto program = pass.shaderSet->programs[program_index];
					if (program)
					{
						text += std::format("\tpass {} program {}: {} bytes\n", pass_index, program_index, program->bytecodeSize);
					}
				}
			}
		}

		if (asset->info)
		{
			for (const auto string : asset->info->strings)
			{
				text += std::format("info: {}\n", string ? string : "");
			}

			for (auto i = 0ull; i < asset->info->nameCount; i++)
			{
				text += std::format("name: {}\n", asset->info->names[i] ? asset->info->names[i] : "");
			}
		}

		file.write(text);
		file.close();
	}
}

namespace zonetool::t9
{
	template void techset::read(zone_reader&, MaterialTechniqueSet*);
	template void techset::read(zone_writer&, MaterialTechniqueSet*);
}
