#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "vertexshader.hpp"

#include "zonetool/h1/assets/vertexshader.hpp"

#include <utils/cryptography.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace vertexshader
		{
			zonetool::h1::MaterialVertexShader* convert(MaterialVertexShader* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::MaterialVertexShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				new_asset->prog.loadDef.microCodeCrc = utils::cryptography::crc32::compute(new_asset->prog.loadDef.program, new_asset->prog.loadDef.programSize);
				new_asset->name = allocator.duplicate_string(game::add_source_postfix(asset->name, game::iw6));

				return new_asset;
			}

			void dump(MaterialVertexShader* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::vertex_shader::dump(converted_asset);
			}
		}
	}
}
