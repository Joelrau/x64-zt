#pragma once

namespace zonetool::s2
{
	template <typename S>
	class name_only
	{
	public:
		static void read(zone_reader& reader, S* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.pop_stream();
		}
	};
}
