#pragma once

namespace zonetool::s2
{
	class splines
	{
	public:
		static void read(zone_reader& reader, Spline* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto defs = reader.read_array(asset->splines, 7, asset->splineCount))
			{
				for (auto i = 0; i < asset->splineCount; i++)
				{
					reader.read_array(defs[i].points, 3, defs[i].pointCount);
					reader.read_array(defs[i].data, 0, defs[i].dataSize);
					reader.read_script_string(defs[i].name);
				}
			}
			reader.pop_stream();
		}
	};
}
