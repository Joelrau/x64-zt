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
			zonetool::h1::MaterialPixelShader* convert(MaterialPixelShader* asset, zone_memory* mem)
			{
				auto* new_asset = mem->allocate<zonetool::h1::MaterialPixelShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				new_asset->prog.loadDef.microCodeCrc = ::shader::calc_crc32(new_asset->prog.loadDef.program, new_asset->prog.loadDef.programSize);

				new_asset->name = mem->duplicate_string(asset->name + TECHSET_PREFIX);

				return new_asset;
			}

			void dump(MaterialPixelShader* asset, zone_memory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::pixel_shader::dump(converted_asset);
			}
		}
	}
}