#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_technique_pass(zone_reader& reader, MaterialPass* pass)
		{
			reader.read_asset(ASSET_TYPE_VERTEXSHADER, pass->vertexShader);
			reader.read_asset(ASSET_TYPE_VERTEXDECL, pass->vertexDecl);
			reader.read_asset(ASSET_TYPE_HULLSHADER, pass->hullShader);
			reader.read_asset(ASSET_TYPE_DOMAINSHADER, pass->domainShader);
			reader.read_asset(ASSET_TYPE_PIXELSHADER, pass->pixelShader);

			const auto arg_count = pass->perPrimArgCount + pass->perObjArgCount + pass->stableArgCount + pass->perViewArgCount;
			if (const auto args = reader.read_array(pass->args, 7, arg_count))
			{
				for (auto i = 0; i < arg_count; i++)
				{
					if (args[i].type == MTL_ARG_LITERAL_CONST)
					{
						reader.read_array(args[i].u.literalConst, 3, 4);
					}
				}
			}
		}

		void read_technique(zone_reader& reader, MaterialTechnique*& field)
		{
			if (!field)
			{
				return;
			}

			if (!reader.is_inline(field))
			{
				reader.resolve_pointer(field);
				return;
			}

			reader.align(7);
			const auto technique = reader.read<MaterialTechniqueHeader>();
			reader.read<MaterialPass>(technique->passCount);
			field = reinterpret_cast<MaterialTechnique*>(technique);

			for (unsigned short i = 0; i < field->hdr.passCount; i++)
			{
				read_technique_pass(reader, &field->passArray[i]);
			}

			reader.read_string(field->hdr.name);
		}
	}

	void techset::read(zone_reader& reader, MaterialTechniqueSet* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		for (auto& technique : asset->techniques)
		{
			read_technique(reader, technique);
		}
		reader.pop_stream();
	}
}
