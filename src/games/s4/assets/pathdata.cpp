#include <std_include.hpp>
#include "pathdata.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_node_tree_array(Walk walk, pathnode_tree_t*& field, std::size_t count);

		template <typename Walk>
		void walk_node_tree_children(Walk walk, pathnode_tree_t& tree)
		{
			if (tree.axis < 0)
			{
				walk.array(tree.u.s.nodes, tree.u.s.nodeCount, 1);
				return;
			}

			for (auto& child : tree.u.child)
			{
				walk_node_tree_array(walk, child, 1);
			}
		}

		template <typename Walk>
		void walk_node_tree_array(Walk walk, pathnode_tree_t*& field, const std::size_t count)
		{
			if (const auto trees = walk.array(field, count, 7))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					walk_node_tree_children(walk, trees[i]);
				}
			}
		}

		template <typename Walk>
		void walk_node(Walk walk, pathnode_constant_t& node)
		{
			walk.script_string(node.targetname);
			walk.script_string(node.script_linkName);
			walk.script_string(node.script_noteworthy);
			walk.script_string(node.target);
			walk.script_string(node.animscript);
			walk.script_string(node.interactable);
			walk.script_string(node.parent.name);
			walk.array(node.Links, node.totalLinkCount, 3);
		}

		template <typename Walk>
		void walk_path_data(Walk walk, PathData& asset)
		{
			if (const auto nodes = walk.array(asset.nodes, asset.nodeCount, 7))
			{
				for (auto i = 0u; i < asset.nodeCount; i++)
				{
					walk_node(walk, nodes[i].constant);
				}
			}

			walk.array(asset.pathVis, asset.visBytes, 0);
			walk_node_tree_array(walk, asset.nodeTree, asset.nodeTreeCount);

			if (const auto groups = walk.array(asset.dynamicNodeGroups, asset.dynamicNodeGroupCount, 7))
			{
				for (auto i = 0; i < asset.dynamicNodeGroupCount; i++)
				{
					walk_node_tree_array(walk, groups[i].nodeTree, groups[i].nodeTreeCount);
				}
			}

			walk.array(asset.pathExposure, asset.exposureBytes, 0);
			walk.array(asset.pathNoPeekVis, asset.noPeekVisBytes, 0);
			walk.array(asset.pathZones, asset.zonesBytes, 0);
			walk.array(asset.pathDynStates, asset.dynStatesBytes, 0);
		}
	}

	void path_data::read(zone_reader& reader, PathData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_path_data(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void path_data::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_path_data(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void path_data::dump(PathData* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".aipaths"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_path_data(dump_walk{dump}, *asset);
		dump.close();
	}

	PathData* path_data::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".aipaths"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing aipaths \"%s\"...", name.data());

		auto* asset = read.read_single<PathData>();
		asset->name = read.read_string();
		walk_path_data(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void path_data::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<PathData>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void path_data::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_path_data(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string path_data::name()
	{
		return this->name_;
	}

	std::int32_t path_data::type()
	{
		return ASSET_TYPE_PATHDATA;
	}
}
