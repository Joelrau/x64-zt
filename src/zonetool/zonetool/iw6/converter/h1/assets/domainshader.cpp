#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "domainshader.hpp"

#include "zonetool/h1/assets/domainshader.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace domainshader
		{
			zonetool::h1::MaterialDomainShader* convert(MaterialDomainShader* asset, zone_memory* mem)
			{
				auto* new_asset = mem->allocate<zonetool::h1::MaterialDomainShader>();

				new_asset->prog.loadDef.program = asset->prog.loadDef.program;
				new_asset->prog.loadDef.programSize = asset->prog.loadDef.programSize;
				//memcpy(&new_asset->prog.loadDef.__pad, &asset->prog.loadDef.loadForRenderer, sizeof(short));

				new_asset->name = mem->duplicate_string(asset->name + TECHSET_PREFIX);

				return new_asset;
			}

			void dump(MaterialDomainShader* asset, zone_memory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::domain_shader::dump(converted_asset);
			}
		}
	}
}