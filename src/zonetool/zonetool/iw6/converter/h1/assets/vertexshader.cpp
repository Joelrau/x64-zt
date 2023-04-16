#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "vertexshader.hpp"

#include "zonetool/h1/assets/vertexshader.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace vertexshader
		{
			zonetool::h1::MaterialVertexShader* convert(MaterialVertexShader* asset, zone_memory* mem)
			{
				auto* new_asset = mem->allocate<zonetool::h1::MaterialVertexShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				new_asset->prog.loadDef.microCodeCrc = ::shader::calc_crc32(new_asset->prog.loadDef.program, new_asset->prog.loadDef.programSize);

				new_asset->name = mem->duplicate_string(asset->name + TECHSET_PREFIX);

				return new_asset;
			}

			void dump(MaterialVertexShader* asset, zone_memory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::vertex_shader::dump(converted_asset);
			}
		}
	}
}