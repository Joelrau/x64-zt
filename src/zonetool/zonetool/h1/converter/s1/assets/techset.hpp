#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace techset
		{
			void dump(zonetool::h1::ComputeShader* asset);
			void dump(zonetool::h1::MaterialVertexShader* asset);
			void dump(zonetool::h1::MaterialHullShader* asset);
			void dump(zonetool::h1::MaterialDomainShader* asset);
			void dump(zonetool::h1::MaterialPixelShader* asset);

			zonetool::s1::MaterialTechniqueSet* convert(MaterialTechniqueSet* asset, utils::memory::allocator& mem);
			void dump(MaterialTechniqueSet* asset);
		}
	}
}
