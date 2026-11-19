#pragma once

namespace zonetool::h1::converter::s2::mapents
{
	zonetool::s2::MapTriggers convert_triggers(const MapTriggers& trigger, utils::memory::allocator& allocator);
	void dump(MapEnts* asset);
}
