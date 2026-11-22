#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace mapents
	{
		zonetool::iw5::MapEnts* convert(zonetool::iw4::MapEnts* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw5::MapEnts>();

			COPY_POINTER_RAW(name);
			COPY_POINTER_RAW(entityString);
			COPY_VALUE(numEntityChars);

			static_assert(sizeof(new_asset->trigger) == sizeof(asset->trigger));
			std::memcpy(&new_asset->trigger, &asset->trigger, sizeof(new_asset->trigger));

			return new_asset;
		}
	}
}
