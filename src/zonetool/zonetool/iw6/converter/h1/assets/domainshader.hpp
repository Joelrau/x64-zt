#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace domainshader
		{
			zonetool::h1::MaterialDomainShader* convert(MaterialDomainShader* asset, utils::memory::allocator& allocator);
			void dump(MaterialDomainShader* asset);
		}
	}
}