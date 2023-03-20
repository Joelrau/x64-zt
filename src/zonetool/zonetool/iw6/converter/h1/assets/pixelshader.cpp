#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
#include "zonetool/iw6/converter/h1/include.hpp"
#include "pixelshader.hpp"

#include "zonetool/h1/assets/pixelshader.hpp"

#define TECHSET_PREFIX "_iw6"s

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace pixelshader
		{
			zonetool::h1::MaterialPixelShader* convert(MaterialPixelShader* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::MaterialPixelShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				new_asset->prog.loadDef.microCodeCrc = shader::calc_crc32(new_asset->prog.loadDef.program, new_asset->prog.loadDef.programSize);

				new_asset->name = mem->StrDup(asset->name + TECHSET_PREFIX);

				return new_asset;
			}

			void dump(MaterialPixelShader* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IPixelShader::dump(converted_asset);
			}
		}
	}
}