#include <std_include.hpp>
#include "physicsdebugdata.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_points(zone_reader& reader, PhysicsDebugPoints& points)
		{
			reader.read_array(points.points, 3, points.count);
			reader.read_array(points.unknown0, 0, points.count);
			reader.read_array(points.unknown1, 3, points.count);
		}
	}

	void physics_debug_data::read(zone_reader& reader, PhysicsDebugData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		for (auto& list : asset->set.lists)
		{
			if (const auto shapes = reader.read_array(list.shapes, 7, list.count))
			{
				for (auto i = 0; i < list.count; i++)
				{
					reader.read_array(shapes[i].vertices, 3, shapes[i].vertexCount);
					reader.read_array(shapes[i].indices, 1, shapes[i].indexCount);
					reader.read_array(shapes[i].unknown, 3, shapes[i].unknownCount);
				}
			}
		}

		read_points(reader, asset->points0);
		read_points(reader, asset->points1);
		reader.pop_stream();
	}
}
