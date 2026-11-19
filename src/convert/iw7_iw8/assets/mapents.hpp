#pragma once

#include <iw8/assets/mapents.hpp>

namespace zonetool::iw7::converter::iw8::mapents
{
	void convert_triggers(const MapTriggers& source, zonetool::iw8::MapTriggers& converted, utils::memory::allocator& allocator);

	void dump(MapEnts* asset);
}
