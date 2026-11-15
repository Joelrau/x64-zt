#include <std_include.hpp>
#include "glassworld.hpp"

namespace zonetool::iw8
{
	void read_spatial_tree(zone_reader& reader, SpatialPartition_Tree*& field)
	{
		if (const auto tree = reader.read_single(field, 7))
		{
			reader.read_array(tree->tree, 0, tree->treeSize);
			reader.read_array(tree->alwaysList, 3, tree->alwaysListLength);
		}
	}

	void glass_world::read(zone_reader& reader, GlassWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto data = reader.read_single(asset->g_glassData, 7))
		{
			reader.read_array(data->glassPieces, 3, data->pieceCount);

			if (const auto names = reader.read_array(data->glassNames, 7, data->glassNameCount))
			{
				for (auto i = 0u; i < data->glassNameCount; i++)
				{
					reader.read_string(names[i].nameStr);
					reader.read_script_string(names[i].name);
					reader.read_array(names[i].pieceIndices, 1, names[i].pieceCount);
				}
			}

			read_spatial_tree(reader, data->spatialTree);
		}
		reader.pop_stream();
	}
}
