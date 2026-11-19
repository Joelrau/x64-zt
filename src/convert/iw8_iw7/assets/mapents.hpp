#pragma once

namespace zonetool::iw8::converter::iw7::mapents
{
	void convert_triggers(const MapTriggers& source, zonetool::iw7::MapTriggers& converted, utils::memory::allocator& allocator);
	void dump(MapEnts* asset);
}
