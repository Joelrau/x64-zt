#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class material
	{
	public:
		static constexpr std::size_t alignment = 15;
		static void read(auto& reader, Material* asset);
		static void dump(Material* asset);

		static bool is_placeholder(const Material* asset)
		{
			return asset->name.value >> 63;
		}
	};
}
