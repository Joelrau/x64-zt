#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace domainshader
		{
			zonetool::h1::MaterialDomainShader* convert(MaterialDomainShader* asset, ZoneMemory* mem);
			void dump(MaterialDomainShader* asset, ZoneMemory* mem);
		}
	}
}