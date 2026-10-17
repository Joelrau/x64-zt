#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace techset
		{
			extern std::unordered_map <std::int32_t, std::int32_t> technique_index_map;

			zonetool::h1::MaterialTechniqueSet* convert(MaterialTechniqueSet* asset, utils::memory::allocator& allocator);
			void dump_statebits_map(const std::string& techset, const std::string& material, GfxStateBits* map, unsigned char count);
			void dump(MaterialTechniqueSet* asset);
		}
	}
}