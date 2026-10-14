#pragma once

namespace zonetool::s2
{
	class clip_map
	{
	public:
		static void read(zone_reader& reader, clipMap_t* asset);
		static void read_info(zone_reader& reader, ClipInfo* info);
	};
}
