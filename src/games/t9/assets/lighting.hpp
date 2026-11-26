#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class lighting
	{
	public:
		static void read(auto& reader, Lighting* asset);
		static void dump(Lighting* asset);
	};

	class wind_def
	{
	public:
		static constexpr std::size_t alignment = 15;

		static void read(auto&, WindDef*)
		{
		}
	};
}
