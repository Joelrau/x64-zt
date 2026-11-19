#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace techset
		{
			void dump(zonetool::s1::ComputeShader* asset);
			void dump(zonetool::s1::MaterialVertexShader* asset);
			void dump(zonetool::s1::MaterialHullShader* asset);
			void dump(zonetool::s1::MaterialDomainShader* asset);
			void dump(zonetool::s1::MaterialPixelShader* asset);

			zonetool::h1::MaterialTechniqueSet* convert(MaterialTechniqueSet* asset, utils::memory::allocator& mem);
			void dump(MaterialTechniqueSet* asset);
		}
	}
}
