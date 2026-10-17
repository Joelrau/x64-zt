#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		bool is_leaf(const pathnode_tree_t* tree)
		{
			return tree->axis < 0;
		}

		unsigned int path_zone_count(const PathData* asset)
		{
			return asset->zoneCount ? asset->nodeCount : 0;
		}

		void read_node_tree(zone_reader& reader, pathnode_tree_t* tree)
		{
			if (is_leaf(tree))
			{
				reader.read_array(tree->u.s.nodes, 1, tree->u.s.nodeCount);
				return;
			}

			for (auto& child : tree->u.child)
			{
				if (const auto child_tree = reader.read_single(child, 7))
				{
					read_node_tree(reader, child_tree);
				}
			}
		}

		void read_node_trees(zone_reader& reader, pathnode_tree_t*& trees, const int count)
		{
			if (const auto data = reader.read_array(trees, 7, count))
			{
				for (auto i = 0; i < count; i++)
				{
					read_node_tree(reader, &data[i]);
				}
			}
		}

		void parse_node_tree(assetmanager::reader& read, zone_memory* mem, pathnode_tree_t* tree)
		{
			if (is_leaf(tree))
			{
				parse_array(read, mem, tree->u.s.nodes, tree->u.s.nodeCount);
				return;
			}

			for (auto& child : tree->u.child)
			{
				if (const auto child_tree = parse_array(read, mem, child, 1))
				{
					parse_node_tree(read, mem, child_tree);
				}
			}
		}

		void parse_node_trees(assetmanager::reader& read, zone_memory* mem, pathnode_tree_t*& trees, const int count)
		{
			if (const auto data = parse_array(read, mem, trees, count))
			{
				for (auto i = 0; i < count; i++)
				{
					parse_node_tree(read, mem, &data[i]);
				}
			}
		}

		void write_node_tree(zone_buffer* buf, pathnode_tree_t* tree, pathnode_tree_t* dest)
		{
			if (is_leaf(tree))
			{
				write_array(buf, 1, tree->u.s.nodes, tree->u.s.nodeCount, dest->u.s.nodes);
				return;
			}

			for (auto i = 0u; i < std::size(tree->u.child); i++)
			{
				if (const auto dest_child = write_array(buf, 7, tree->u.child[i], 1, dest->u.child[i]))
				{
					write_node_tree(buf, tree->u.child[i], dest_child);
				}
			}
		}

		void write_node_trees(zone_buffer* buf, pathnode_tree_t* trees, const int count, pathnode_tree_t*& dest_field)
		{
			if (const auto dest = write_array(buf, 7, trees, count, dest_field))
			{
				for (auto i = 0; i < count; i++)
				{
					write_node_tree(buf, &trees[i], &dest[i]);
				}
			}
		}

		void dump_node_tree(assetmanager::dumper& write, pathnode_tree_t* tree)
		{
			if (is_leaf(tree))
			{
				write.dump_array(tree->u.s.nodes, tree->u.s.nodeCount);
				return;
			}

			for (auto* child : tree->u.child)
			{
				write.dump_single(child);
				if (child)
				{
					dump_node_tree(write, child);
				}
			}
		}

		void dump_node_trees(assetmanager::dumper& write, pathnode_tree_t* trees, const int count)
		{
			write.dump_array(trees, count);
			for (auto i = 0; trees && i < count; i++)
			{
				dump_node_tree(write, &trees[i]);
			}
		}
	}

	void path_data::read(zone_reader& reader, PathData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto nodes = reader.read_array(asset->nodes, 7, asset->nodeCount))
		{
			for (auto i = 0u; i < asset->nodeCount; i++)
			{
				auto* node = &nodes[i].constant;
				reader.read_script_string(node->targetname);
				reader.read_script_string(node->script_linkName);
				reader.read_script_string(node->script_noteworthy);
				reader.read_script_string(node->target);
				reader.read_script_string(node->animscript);
				if (!asset->parentIndexResolved)
				{
					reader.read_script_string(node->parentName);
				}
				reader.read_array(node->Links, 3, node->totalLinkCount);
			}
		}

		reader.read_array(asset->pathVis, 0, asset->visBytes);
		read_node_trees(reader, asset->nodeTree, asset->nodeTreeCount);
		if (const auto groups = reader.read_array(asset->dynamicNodeGroups, 7, asset->dynamicNodeGroupCount))
		{
			for (auto i = 0; i < asset->dynamicNodeGroupCount; i++)
			{
				read_node_trees(reader, groups[i].nodeTree, groups[i].nodeTreeCount);
			}
		}
		reader.read_array(asset->pathExposure, 0, asset->exposureBytes);
		reader.read_array(asset->pathNoPeekVis, 0, asset->noPeekVisBytes);
		reader.read_array(asset->pathZones, 1, path_zone_count(asset));
		reader.read_array(asset->pathDynStates, 0, asset->dynStatesBytes);

		reader.pop_stream();
	}

	PathData* path_data::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_PATHDATA, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing aipaths \"%s\"...", name.data());

		auto* asset = read.read_single<PathData>();
		asset->name = read.read_string();

		if (const auto nodes = parse_array(read, mem, asset->nodes, asset->nodeCount))
		{
			for (auto i = 0u; i < asset->nodeCount; i++)
			{
				auto* node = &nodes[i].constant;
				node->targetname = parse_script_string(read);
				node->script_linkName = parse_script_string(read);
				node->script_noteworthy = parse_script_string(read);
				node->target = parse_script_string(read);
				node->animscript = parse_script_string(read);
				if (!asset->parentIndexResolved)
				{
					node->parentName = parse_script_string(read);
				}
				parse_array(read, mem, node->Links, node->totalLinkCount);
			}
		}

		parse_array(read, mem, asset->pathVis, asset->visBytes);
		parse_node_trees(read, mem, asset->nodeTree, asset->nodeTreeCount);
		if (const auto groups = parse_array(read, mem, asset->dynamicNodeGroups, asset->dynamicNodeGroupCount))
		{
			for (auto i = 0; i < asset->dynamicNodeGroupCount; i++)
			{
				parse_node_trees(read, mem, groups[i].nodeTree, groups[i].nodeTreeCount);
			}
		}
		parse_array(read, mem, asset->pathExposure, asset->exposureBytes);
		parse_array(read, mem, asset->pathNoPeekVis, asset->noPeekVisBytes);
		parse_array(read, mem, asset->pathZones, path_zone_count(asset));
		parse_array(read, mem, asset->pathDynStates, asset->dynStatesBytes);
		read.close();

		return asset;
	}

	void path_data::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		script_string_preparer prepare(buf);

		for (auto i = 0u; data->nodes && i < data->nodeCount; i++)
		{
			auto* node = &data->nodes[i].constant;
			prepare(node->targetname);
			prepare(node->script_linkName);
			prepare(node->script_noteworthy);
			prepare(node->target);
			prepare(node->animscript);
			if (!data->parentIndexResolved)
			{
				prepare(node->parentName);
			}
		}
	}

	void path_data::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);

		if (const auto dest_nodes = write_array(buf, 7, data->nodes, data->nodeCount, dest->nodes))
		{
			for (auto i = 0u; i < data->nodeCount; i++)
			{
				auto* node = &data->nodes[i].constant;
				write_array(buf, 3, node->Links, node->totalLinkCount, dest_nodes[i].constant.Links);
			}
		}

		write_array(buf, 0, data->pathVis, data->visBytes, dest->pathVis);
		write_node_trees(buf, data->nodeTree, data->nodeTreeCount, dest->nodeTree);
		if (const auto dest_groups = write_array(buf, 7, data->dynamicNodeGroups, data->dynamicNodeGroupCount, dest->dynamicNodeGroups))
		{
			for (auto i = 0; i < data->dynamicNodeGroupCount; i++)
			{
				auto* group = &data->dynamicNodeGroups[i];
				write_node_trees(buf, group->nodeTree, group->nodeTreeCount, dest_groups[i].nodeTree);
			}
		}
		write_array(buf, 0, data->pathExposure, data->exposureBytes, dest->pathExposure);
		write_array(buf, 0, data->pathNoPeekVis, data->noPeekVisBytes, dest->pathNoPeekVis);
		write_array(buf, 1, data->pathZones, path_zone_count(data), dest->pathZones);
		write_array(buf, 0, data->pathDynStates, data->dynStatesBytes, dest->pathDynStates);

		buf->pop_stream();
	}

	void path_data::dump(PathData* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_PATHDATA, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->nodes, asset->nodeCount);
		for (auto i = 0u; asset->nodes && i < asset->nodeCount; i++)
		{
			auto* node = &asset->nodes[i].constant;
			dump_script_string(write, node->targetname);
			dump_script_string(write, node->script_linkName);
			dump_script_string(write, node->script_noteworthy);
			dump_script_string(write, node->target);
			dump_script_string(write, node->animscript);
			if (!asset->parentIndexResolved)
			{
				dump_script_string(write, node->parentName);
			}
			write.dump_array(node->Links, node->totalLinkCount);
		}

		write.dump_array(asset->pathVis, asset->visBytes);
		dump_node_trees(write, asset->nodeTree, asset->nodeTreeCount);
		write.dump_array(asset->dynamicNodeGroups, asset->dynamicNodeGroupCount);
		for (auto i = 0; asset->dynamicNodeGroups && i < asset->dynamicNodeGroupCount; i++)
		{
			dump_node_trees(write, asset->dynamicNodeGroups[i].nodeTree, asset->dynamicNodeGroups[i].nodeTreeCount);
		}
		write.dump_array(asset->pathExposure, asset->exposureBytes);
		write.dump_array(asset->pathNoPeekVis, asset->noPeekVisBytes);
		write.dump_array(asset->pathZones, path_zone_count(asset));
		write.dump_array(asset->pathDynStates, asset->dynStatesBytes);
		write.close();
	}
}
