#include <std_include.hpp>
#include "physcollmap.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_brush_wrapper(zone_reader32& reader, BrushWrapper* wrapper)
		{
			auto& brush = wrapper->brush;
			if (const auto sides = reader.read_array(brush.sides, 3, brush.numsides))
			{
				for (auto i = 0; i < brush.numsides; i++)
				{
					reader.read_single(sides[i].plane, 3);
				}
			}

			reader.read_array(brush.baseAdjacentSide, 0, wrapper->totalEdgeCount);
			reader.read_array(wrapper->planes, 3, brush.numsides);
		}
	}

	void phys_collmap::read(zone_reader32& reader, PhysCollmap* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto geoms = reader.read_array(asset->geoms, 3, asset->count))
		{
			for (auto i = 0u; i < asset->count; i++)
			{
				if (const auto wrapper = reader.read_single(geoms[i].brushWrapper, 3))
				{
					read_brush_wrapper(reader, wrapper);
				}
			}
		}

		reader.pop_stream();
	}
}
