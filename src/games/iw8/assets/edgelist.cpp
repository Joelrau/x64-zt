#include <std_include.hpp>
#include "edgelist.hpp"

namespace zonetool::iw8
{
	void map_edge_list::read(zone_reader& reader, MapEdgeList* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->edges, 31, asset->numEdges);
		reader.read_array(asset->edgeMetadata, 3, asset->numEdges);
		reader.read_array(asset->edgeOctrees, 15, asset->numEdgeOctrees);
		reader.read_array(asset->edgeOctreeNodeSets, 127, asset->numEdgeOctreeNodeSet);
		reader.read_array(asset->edgeIndices, 3, asset->numEdgeIndices);
		reader.read_array(asset->edgeAdjacencyMetadata, 3, asset->numEdgeAdjacencyMetadata);
		reader.read_array(asset->edgeAdjacency, 3, asset->numEdgeAdjacency);
		reader.pop_stream();
	}
}
