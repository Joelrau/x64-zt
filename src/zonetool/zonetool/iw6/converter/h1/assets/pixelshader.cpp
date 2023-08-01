#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "pixelshader.hpp"

#include "zonetool/h1/assets/pixelshader.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace pixelshader
		{
			zonetool::h1::MaterialPixelShader* convert(MaterialPixelShader* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::MaterialPixelShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				new_asset->prog.loadDef.microCodeCrc = ::shader::calc_crc32(new_asset->prog.loadDef.program, new_asset->prog.loadDef.programSize);
				new_asset->name = allocator.duplicate_string(game::add_source_postfix(asset->name, game::iw6));

				return new_asset;
			}

			void dump(MaterialPixelShader* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::pixel_shader::dump(converted_asset);
			}
		}
	}
}