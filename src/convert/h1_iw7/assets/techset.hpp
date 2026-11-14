#pragma once

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace techset
		{
			extern std::unordered_map <std::int32_t, std::int32_t> technique_index_map;

			void dump(zonetool::h1::ComputeShader* asset);
			void dump(zonetool::h1::MaterialVertexShader* asset);
			void dump(zonetool::h1::MaterialHullShader* asset);
			void dump(zonetool::h1::MaterialDomainShader* asset);
			void dump(zonetool::h1::MaterialPixelShader* asset);

			zonetool::iw7::MaterialTechniqueSet* convert(zonetool::h1::MaterialTechniqueSet* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::MaterialTechniqueSet* asset);
		}
	}
}