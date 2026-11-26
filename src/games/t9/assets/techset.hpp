#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class techset
	{
	public:
		static void read(auto& reader, MaterialTechniqueSet* asset);
		static void dump(MaterialTechniqueSet* asset);

		static bool is_placeholder(const MaterialTechniqueSet* asset)
		{
			return asset->name.value >> 63;
		}
	};
}
