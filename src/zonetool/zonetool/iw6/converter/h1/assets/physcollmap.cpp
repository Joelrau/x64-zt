#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "physcollmap.hpp"

#include "zonetool/h1/assets/physcollmap.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physcollmap
		{
			zonetool::h1::PhysCollmap* convert(PhysCollmap* asset, utils::memory::allocator& allocator)
			{
				return nullptr;
			}

			void dump(PhysCollmap* asset)
			{
				//utils::memory::allocator allocator;
				//auto* converted_asset = convert(asset, allocator);
				//zonetool::h1::phys_collmap::dump(converted_asset);
			}
		}
	}
}