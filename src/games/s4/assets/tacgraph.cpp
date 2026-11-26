#include <std_include.hpp>
#include "tacgraph.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_search_node(Walk walk, tacpoint_search_node_t& node)
		{
			if (const auto children = walk.array(node.m_ChildNodes, 4, 7))
			{
				for (auto i = 0; i < 4; i++)
				{
					if (const auto child = walk.array(children[i], 1, 7))
					{
						walk_search_node(walk, *child);
					}
				}
			}
		}

		template <typename Walk>
		void walk_graph(Walk walk, TacticalGraph& graph)
		{
			walk.array(graph.m_Points, graph.m_NumPoints, 3);

			if (const auto rows = walk.array(graph.m_VisGraph, graph.m_NumVisRows, 7))
			{
				for (auto i = 0; i < graph.m_NumVisRows; i++)
				{
					walk.array(rows[i].m_Vis, (rows[i].m_End - rows[i].m_Start) / 8 + 1, 0);
				}
			}

			walk_search_node(walk, graph.m_SearchRoot);

			if (const auto list = walk.array(graph.indexList, 1, 7))
			{
				walk.array(list->indices, list->count, 1);
			}

			walk.script_string(graph.name);
		}

		template <typename Walk>
		void walk_tactical_graph(Walk walk, TacticalGraphData& asset)
		{
			if (const auto graphs = walk.array(asset.m_TacGraphs, asset.m_NumTacGraphs, 7))
			{
				for (auto i = 0; i < asset.m_NumTacGraphs; i++)
				{
					walk_graph(walk, graphs[i]);
				}
			}
		}
	}

	namespace
	{
		void read_region_node(zone_reader& reader, AIRegionGraphNode*& field)
		{
			const auto node = reader.read_array(field, 7);
			if (!node)
			{
				return;
			}

			reader.read_array(node->point, 3);
			if (const auto links = reader.read_array(node->links, 7, node->linkCount))
			{
				for (auto i = 0; i < node->linkCount; i++)
				{
					read_region_node(reader, links[i].node);
				}
			}
		}
	}

	void ai_region_graphs::read(zone_reader& reader, AIRegionGraphs* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto points = reader.read_array(asset->points, 7, asset->pointCount))
		{
			for (auto i = 0; i < asset->pointCount; i++)
			{
				reader.read_array(points[i], 3);
			}
		}

		if (const auto graphs = reader.read_array(asset->graphs, 7, asset->graphCount))
		{
			for (auto i = 0; i < asset->graphCount; i++)
			{
				reader.read_script_string(graphs[i].name);
				if (const auto nodes = reader.read_array(graphs[i].nodes, 7, graphs[i].nodeCount))
				{
					for (auto j = 0; j < graphs[i].nodeCount; j++)
					{
						read_region_node(reader, nodes[j]);
					}
				}
			}
		}

		reader.pop_stream();
	}

	void tactical_graph::read(zone_reader& reader, TacticalGraphData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_tactical_graph(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void tactical_graph::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_tactical_graph(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void tactical_graph::dump(TacticalGraphData* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".tacgraph"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_tactical_graph(dump_walk{dump}, *asset);
		dump.close();
	}

	TacticalGraphData* tactical_graph::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".tacgraph"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing tacgraph \"%s\"...", name.data());

		auto* asset = read.read_single<TacticalGraphData>();
		asset->name = read.read_string();
		walk_tactical_graph(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void tactical_graph::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<TacticalGraphData>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void tactical_graph::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_tactical_graph(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string tactical_graph::name()
	{
		return this->name_;
	}

	std::int32_t tactical_graph::type()
	{
		return ASSET_TYPE_TACGRAPH;
	}
}
