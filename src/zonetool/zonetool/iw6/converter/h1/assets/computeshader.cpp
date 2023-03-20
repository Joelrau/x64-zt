#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
#include "zonetool/iw6/converter/h1/include.hpp"
#include "computeshader.hpp"

#include "zonetool/h1/assets/computeshader.hpp"

#define TECHSET_PREFIX "_iw6"s

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace computeshader
		{
			zonetool::h1::ComputeShader* convert(ComputeShader* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::ComputeShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				//memcpy(&new_asset->prog.loadDef.__pad, &asset->prog.loadDef.loadForRenderer, sizeof(short));

				new_asset->name = mem->StrDup(asset->name + TECHSET_PREFIX);

				return new_asset;
			}

			void dump(ComputeShader* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IComputeShader::dump(converted_asset);
			}
		}
	}
}