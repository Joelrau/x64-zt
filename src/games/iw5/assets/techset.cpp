#include <std_include.hpp>
#include "techset.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_pass(zone_reader32& reader, MaterialPass& pass)
		{
			reader.read_asset(ASSET_TYPE_VERTEXDECL, pass.vertexDecl);
			reader.read_asset(ASSET_TYPE_VERTEXSHADER, pass.vertexShader);
			reader.read_asset(ASSET_TYPE_PIXELSHADER, pass.pixelShader);

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
	}

	void vertex_decl::read(zone_reader32& reader, MaterialVertexDeclaration* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void vertex_shader::read(zone_reader32& reader, MaterialVertexShader* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->prog.loadDef.program, 3, asset->prog.loadDef.programSize);
		reader.pop_stream();
	}

	void pixel_shader::read(zone_reader32& reader, MaterialPixelShader* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->prog.loadDef.program, 3, asset->prog.loadDef.programSize);
		reader.pop_stream();
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
				for (auto i = 0; i < technique->passCount; i++)
				{
					read_pass(reader, technique->passArray[i]);
				}

				reader.read_string(technique->name);
			}
		}

		reader.pop_stream();
	}
}
