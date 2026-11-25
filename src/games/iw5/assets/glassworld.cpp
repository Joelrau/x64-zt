#include <std_include.hpp>
#include "glassworld.hpp"

namespace zonetool::iw5
{
	void glass_world::read(zone_reader32& reader, GlassWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto data = reader.read_single(asset->g_glassData, 3))
		{
			reader.read_array(data->glassPieces, 3, data->pieceCount);

			if (const auto names = reader.read_array(data->glassNames, 3, data->glassNameCount))
			{
				for (auto i = 0u; i < data->glassNameCount; i++)
				{
					reader.read_string(names[i].nameStr);
					reader.read_array(names[i].pieceIndices, 1, names[i].pieceCount);
				}
			}
		}

		reader.pop_stream();
	}
}
