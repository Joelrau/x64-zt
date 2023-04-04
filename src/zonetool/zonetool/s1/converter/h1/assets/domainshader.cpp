#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "domainshader.hpp"

#include "zonetool/h1/assets/domainshader.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace domainshader
		{
			zonetool::h1::MaterialDomainShader* convert(MaterialDomainShader* asset, zone_memory* mem)
			{
				auto* new_asset = mem->allocate<zonetool::h1::MaterialDomainShader>();

				std::memcpy(new_asset, asset, sizeof(MaterialDomainShader));
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