#include <std_include.hpp>
#include "edgelist.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_edge_list(Walk walk, MapEdgeList& asset)
		{
			walk.array(asset.edges, asset.numEdges, 31);
			walk.array(asset.edgeMetadata, asset.numEdges, 3);
			walk.array(asset.edgeOctrees, asset.numEdgeOctrees, 15);
			walk.array(asset.edgeOctreeNodeSets, asset.numEdgeOctreeNodeSet, 127);
			walk.array(asset.edgeIndices, asset.numEdgeIndices, 3);
			walk.array(asset.edgeAdjacencyMetadata, asset.numEdgeAdjacencyMetadata, 3);
			walk.array(asset.edgeAdjacency, asset.numEdgeAdjacency, 3);
		}
	}

	void map_edge_list::read(zone_reader& reader, MapEdgeList* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_edge_list(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void map_edge_list::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_edge_list(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void map_edge_list::dump(MapEdgeList* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("edgelist\\"s + asset->name + ".edgelistb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_edge_list(dump_walk{dump}, *asset);
		dump.close();
	}

	MapEdgeList* map_edge_list::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("edgelist\\"s + name + ".edgelistb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing edgelist \"%s\"...", name.data());

		auto* asset = read.read_single<MapEdgeList>();
		asset->name = read.read_string();
		walk_edge_list(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void map_edge_list::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<MapEdgeList>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string map_edge_list::name()
	{
		return this->name_;
	}

	std::int32_t map_edge_list::type()
	{
		return ASSET_TYPE_EDGE_LIST;
	}
}
