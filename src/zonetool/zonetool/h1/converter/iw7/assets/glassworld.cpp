#include <std_include.hpp>

#include "zonetool/h1/converter/iw7/include.hpp"
#include "glassworld.hpp"
#include "zonetool/iw7/assets/glassworld.hpp"

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace glass_world
		{
			zonetool::iw7::GlassWorld* convert(zonetool::h1::GlassWorld* asset, utils::memory::allocator& allocator)
			{
				return reinterpret_cast<zonetool::iw7::GlassWorld*>(asset);
			}

			void dump(zonetool::h1::GlassWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::iw7::glass_world::dump(converted_asset);
			}
		}
	}
}
