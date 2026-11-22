#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace mapents
	{
		zonetool::iw4::MapEnts* convert(zonetool::iw3::MapEnts* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::MapEnts>();

			COPY_POINTER_RAW(name);
			COPY_POINTER_RAW(entityString);
			COPY_VALUE(numEntityChars);

			return new_asset;
		}
	}
}
