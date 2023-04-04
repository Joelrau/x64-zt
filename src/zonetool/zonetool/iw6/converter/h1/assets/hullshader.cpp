#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "hullshader.hpp"

#include "zonetool/h1/assets/hullshader.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace hullshader
		{
			zonetool::h1::MaterialHullShader* convert(MaterialHullShader* asset, zone_memory* mem)
			{
				auto* new_asset = mem->allocate<zonetool::h1::MaterialHullShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				//memcpy(&new_asset->prog.loadDef.__pad, &asset->prog.loadDef.loadForRenderer, sizeof(short));

				new_asset->name = mem->duplicate_string(asset->name + TECHSET_PREFIX);

				return new_asset;
			}

			void dump(MaterialHullShader* asset, zone_memory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::hull_shader::dump(converted_asset);
			}
		}
	}
}