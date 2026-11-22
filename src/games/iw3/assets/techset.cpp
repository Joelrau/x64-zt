#include <std_include.hpp>
#include "techset.hpp"

namespace zonetool::iw3
{
	namespace
	{
		void read_vertex_shader(zone_reader32& reader, x86::pointer<MaterialVertexShader>& field)
		{
			if (const auto shader = reader.read_single(field, 3))
			{
				reader.read_string(shader->name);
				reader.read_array(shader->prog.loadDef.program, 3, shader->prog.loadDef.programSize);
			}
		}

		void read_pixel_shader(zone_reader32& reader, x86::pointer<MaterialPixelShader>& field)
		{
			if (const auto shader = reader.read_single(field, 3))
			{
				reader.read_string(shader->name);
				reader.read_array(shader->prog.loadDef.program, 3, shader->prog.loadDef.programSize);
			}
		}

		void read_pass(zone_reader32& reader, MaterialPass& pass)
		{
			reader.read_single(pass.vertexDecl, 3);
			read_vertex_shader(reader, pass.vertexShader);
			read_pixel_shader(reader, pass.pixelShader);

			const auto count = pass.perPrimArgCount + pass.perObjArgCount + pass.stableArgCount;
			if (const auto args = reader.read_array(pass.args, 3, count))
			{
				for (auto i = 0; i < count; i++)
				{
					if (args[i].type == MTL_ARG_LITERAL_VERTEX_CONST || args[i].type == MTL_ARG_LITERAL_PIXEL_CONST)
					{
						reader.read_array(args[i].u.literalConst, 3);
					}
				}
			}
		}

		void read_technique(zone_reader32& reader, MaterialTechnique* technique)
		{
			for (auto i = 0; i < technique->passCount; i++)
			{
				read_pass(reader, technique->passArray[i]);
			}

			reader.read_string(technique->name);
		}
	}

	void tech_set::read(zone_reader32& reader, MaterialTechniqueSet* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		for (auto& field : asset->techniques)
		{
			const auto technique = reader.read_dynamic(field, 3, offsetof(MaterialTechnique, passArray), [](const MaterialTechnique* data)
			{
				return offsetof(MaterialTechnique, passArray) + sizeof(MaterialPass) * data->passCount;
			});

			if (technique)
			{
				read_technique(reader, technique);
			}
		}

		reader.pop_stream();
	}
}
