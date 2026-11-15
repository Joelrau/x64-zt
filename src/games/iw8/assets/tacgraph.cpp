#include <std_include.hpp>
#include "tacgraph.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_search_node(zone_reader& reader, tacpoint_search_node_t* node)
		{
			if (const auto children = reader.read_array(node->m_ChildNodes, 7, 4))
			{
				for (auto i = 0; i < 4; i++)
				{
					if (const auto child = reader.read_single(children[i], 7))
					{
						read_search_node(reader, child);
					}
				}
			}
		}

		void read_graph(zone_reader& reader, TacticalGraph* graph)
		{
			reader.read_array(graph->m_Points, 3, graph->m_NumPoints);

			const auto row_count = graph->m_NumPoints - 1;
			if (const auto rows = reader.read_array(graph->m_VisGraph, 7, row_count))
			{
				for (auto i = 0; i < row_count; i++)
				{
					reader.read_array(rows[i].m_Vis, 0, (rows[i].m_End - rows[i].m_Start) / 8 + 1);
				}
			}

			read_search_node(reader, &graph->m_SearchRoot);
		}
	}

	void tactical_graph::read(zone_reader& reader, TacticalGraphData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto graphs = reader.read_array(asset->m_TacGraphs, 7, asset->m_NumTacGraphs))
		{
			for (auto i = 0; i < asset->m_NumTacGraphs; i++)
			{
				read_graph(reader, &graphs[i]);
			}
		}
		reader.pop_stream();
	}
}
