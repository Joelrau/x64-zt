#include <std_include.hpp>
#include "tacgraph.hpp"

namespace zonetool::iw8
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

			const auto row_count = graph.m_NumPoints - 1;
			if (const auto rows = walk.array(graph.m_VisGraph, row_count, 7))
			{
				for (auto i = 0; i < row_count; i++)
				{
					walk.array(rows[i].m_Vis, (rows[i].m_End - rows[i].m_Start) / 8 + 1, 0);
				}
			}

			walk_search_node(walk, graph.m_SearchRoot);
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
		walk_tactical_graph(parse_walk{read}, *asset);
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

	std::string tactical_graph::name()
	{
		return this->name_;
	}

	std::int32_t tactical_graph::type()
	{
		return ASSET_TYPE_TACGRAPH;
	}
}
