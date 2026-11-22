#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"

#include <iw7/assets/mapents.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace mapents
	{
		zonetool::iw7::MapEnts* convert(zonetool::iw5::MapEnts* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw7::MapEnts>();

			new_asset->name = asset->name.get();
			new_asset->entityString = asset->entityString.get();
			new_asset->numEntityChars = asset->numEntityChars;

			return new_asset;
		}

		void dump(zonetool::iw5::MapEnts* asset)
		{
			utils::memory::allocator allocator;
			zonetool::iw7::map_ents::dump(convert(asset, allocator));
		}
	}
}
