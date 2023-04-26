#include "std_include.hpp"
#include "aipaths.hpp"

namespace zonetool::h2
{
	void path_data::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* path_data::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	void parse_node_tree_children(assetmanager::reader& read, pathnode_tree_t** node_tree)
	{
		const auto tree = read.read_single<pathnode_tree_t>();
		*node_tree = tree;

		if (tree->axis < 0)
		{
			tree->u.s.nodes = read.read_array<unsigned short>();
		}
		else
		{
			for (auto i = 0; i < 2; i++)
			{
				parse_node_tree_children(read, &tree->u.child[i]);
			}
		}
	}

	void parse_node_tree(assetmanager::reader& read, pathnode_tree_t* node_tree)
	{
		if (node_tree->axis < 0)
		{
			node_tree->u.s.nodes = read.read_array<unsigned short>();
		}
		else
		{
			for (auto i = 0; i < 2; i++)
			{
				parse_node_tree_children(read, &node_tree->u.child[i]);
			}
		}
	}

	void write_node_tree_children(zone_buffer* buf, pathnode_tree_t* data, pathnode_tree_t** dest)
	{
		buf->align(3);
		const auto tree = buf->write(data);
		zone_buffer::clear_pointer(dest);

		if (data->axis < 0)
		{
			buf->align(1);
			buf->write(data->u.s.nodes, data->u.s.nodeCount);
			zone_buffer::clear_pointer(&tree->u.s.nodes);
		}
		else
		{
			for (auto i = 0; i < 2; i++)
			{
				if (data->u.child[i])
				{
					write_node_tree_children(buf, data->u.child[i], &tree->u.child[i]);
				}
			}
		}
	}

	void write_node_tree(zone_buffer* buf, pathnode_tree_t* data, pathnode_tree_t* dest)
	{
		if (data->axis < 0)
		{
			buf->align(1);
			buf->write(data->u.s.nodes, data->u.s.nodeCount);
			zone_buffer::clear_pointer(&data->u.s.nodes);
		}
		else
		{
			for (auto i = 0; i < 2; i++)
			{
				if (data->u.child[i])
				{
					write_node_tree_children(buf, data->u.child[i], &dest->u.child[i]);
				}
			}
		}
	}

#define ADD_SCRIPT_STRING(__field__) \
	{ \
		const auto script_string = read.read_string(); \
		if (asset->nodes[i].constant.__field__) \
		{ \
			this->add_script_string(&asset->nodes[i].constant.__field__, script_string); \
		} \
	} \

#define WRITE_SCRIPT_STRING(__field__) \
	{ \
		data->nodes[i].constant.__field__ = static_cast<scr_string_t>( \
			buf->write_scriptstring(this->get_script_string(&data->nodes[i].constant.__field__))); \
	} \

	PathData* path_data::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = name + ".aipaths"s;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing aipaths \"%s\"...", name.data());

		const auto asset = read.read_single<PathData>();
		asset->name = read.read_string();

		asset->nodes = read.read_array<pathnode_t>();

		for (auto i = 0u; i < asset->nodeCount; i++)
		{
			ADD_SCRIPT_STRING(targetname);
			ADD_SCRIPT_STRING(script_linkName);
			ADD_SCRIPT_STRING(script_noteworthy);
			ADD_SCRIPT_STRING(target);
			ADD_SCRIPT_STRING(animscript);

			if (!asset->parentIndexResolved)
			{
				ADD_SCRIPT_STRING(parent.name);
			}

			asset->nodes[i].constant.Links = read.read_array<pathlink_s>();
			ADD_SCRIPT_STRING(unk);
		}

		asset->pathVis = read.read_array<unsigned char>();

		asset->nodeTree = read.read_array<pathnode_tree_t>();
		for (auto i = 0; i < asset->nodeTreeCount; i++)
		{
			parse_node_tree(read, &asset->nodeTree[i]);
		}

		asset->dynamicNodeGroups = read.read_array<PathDynamicNodeGroup>();
		for (auto i = 0; i < asset->dynamicNodeGroupCount; i++)
		{
			asset->dynamicNodeGroups[i].nodeTree = read.read_array<pathnode_tree_t>();
			for (auto o = 0; o < asset->dynamicNodeGroups[i].nodeTreeCount; o++)
			{
				parse_node_tree(read, &asset->dynamicNodeGroups[i].nodeTree[o]);
			}
		}

		asset->pathExposure = read.read_array<unsigned char>();
		asset->pathNoPeekVis = read.read_array<unsigned char>();
		asset->pathZones = read.read_array<unsigned char>();
		asset->pathDynStates = read.read_array<unsigned char>();

		return asset;
	}

	void path_data::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).aipaths;
		}
	}

	void path_data::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		for (auto i = 0u; i < data->nodeCount; i++)
		{
			WRITE_SCRIPT_STRING(targetname);
			WRITE_SCRIPT_STRING(script_linkName);
			WRITE_SCRIPT_STRING(script_noteworthy);
			WRITE_SCRIPT_STRING(target);
			WRITE_SCRIPT_STRING(animscript);

			if (!data->parentIndexResolved)
			{
				WRITE_SCRIPT_STRING(parent.name);
			}

			WRITE_SCRIPT_STRING(unk);
		}
	}

	void path_data::load_depending(zone_base* zone)
	{
	}

	std::string path_data::name()
	{
		return this->name_;
	}

	std::int32_t path_data::type()
	{
		return ASSET_TYPE_AIPATHS;
	}

	void path_data::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->nodes)
		{
			buf->align(3);
			buf->write(data->nodes, data->nodeCount);

			for (auto i = 0u; i < data->nodeCount; i++)
			{
				buf->align(3);
				buf->write(data->nodes[i].constant.Links, data->nodes[i].constant.totalLinkCount);
				zone_buffer::clear_pointer(&dest->nodes[i].constant.Links);
			}

			zone_buffer::clear_pointer(&dest->nodes);
		}

		if (data->pathVis)
		{
			buf->align(0);
			buf->write(data->pathVis, data->visBytes);
			zone_buffer::clear_pointer(&dest->pathVis);
		}

		if (data->nodeTree)
		{
			buf->align(3);
			const auto dest_trees = buf->write(data->nodeTree, data->nodeTreeCount);

			for (auto i = 0; i < data->nodeTreeCount; i++)
			{
				write_node_tree(buf, &data->nodeTree[i], &dest_trees[i]);
			}

			zone_buffer::clear_pointer(&dest->nodeTree);
		}

		if (data->dynamicNodeGroups)
		{
			buf->align(3);
			buf->write(data->dynamicNodeGroups, data->dynamicNodeGroupCount);

			for (auto i = 0; i < data->dynamicNodeGroupCount; i++)
			{
				if (data->dynamicNodeGroups[i].nodeTree)
				{
					buf->align(3);
					const auto dest_trees = buf->write(data->dynamicNodeGroups[i].nodeTree, 
						data->dynamicNodeGroups[i].nodeTreeCount);
					for (auto o = 0; o < data->dynamicNodeGroups[i].nodeTreeCount; o++)
					{
						write_node_tree(buf, &data->dynamicNodeGroups[i].nodeTree[o], &dest_trees[o]);
					}
					zone_buffer::clear_pointer(dest->dynamicNodeGroups[i].nodeTree);
				}
			}

			zone_buffer::clear_pointer(&dest->dynamicNodeGroups);
		}

		if (data->pathExposure)
		{
			buf->align(0);
			buf->write(data->pathExposure, data->exposureBytes);
			zone_buffer::clear_pointer(dest->pathExposure);
		}

		if (data->pathNoPeekVis)
		{
			buf->align(0);
			buf->write(data->pathNoPeekVis, data->noPeekVisBytes);
			zone_buffer::clear_pointer(dest->pathNoPeekVis);
		}

		if (data->pathZones)
		{
			buf->align(0);
			buf->write(data->pathZones, data->zonesBytes);
			zone_buffer::clear_pointer(dest->pathZones);
		}

		if (data->pathDynStates)
		{
			buf->align(0);
			buf->write(data->pathDynStates, data->dynStatesBytes);
			zone_buffer::clear_pointer(dest->pathDynStates);
		}

		buf->pop_stream();
	}

	void dump_node_tree_children(assetmanager::dumper& dumper, pathnode_tree_t* node_tree)
	{
		dumper.dump_single(node_tree);

		if (node_tree->axis < 0)
		{
			dumper.dump_array(node_tree->u.s.nodes, node_tree->u.s.nodeCount);
		}
		else
		{
			for (auto i = 0; i < 2; i++)
			{
				dump_node_tree_children(dumper, node_tree->u.child[i]);
			}
		}
	}

	void dump_node_tree(assetmanager::dumper& dumper, pathnode_tree_t* node_tree)
	{
		if (node_tree->axis < 0)
		{
			dumper.dump_array(node_tree->u.s.nodes, node_tree->u.s.nodeCount);
		}
		else
		{
			for (auto i = 0; i < 2; i++)
			{
				dump_node_tree_children(dumper, node_tree->u.child[i]);
			}
		}
	}
	void path_data::dump(PathData* asset)
	{
		const auto path = asset->name + ".aipaths"s;

		assetmanager::dumper dumper;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);
		dumper.dump_string(asset->name);

		dumper.dump_array(asset->nodes, asset->nodeCount);
		for (auto i = 0u; i < asset->nodeCount; i++)
		{
			dumper.dump_string(SL_ConvertToString(asset->nodes[i].constant.targetname));
			dumper.dump_string(SL_ConvertToString(asset->nodes[i].constant.script_linkName));
			dumper.dump_string(SL_ConvertToString(asset->nodes[i].constant.script_noteworthy));
			dumper.dump_string(SL_ConvertToString(asset->nodes[i].constant.target));
			dumper.dump_string(SL_ConvertToString(asset->nodes[i].constant.animscript));

			if (!asset->parentIndexResolved)
			{
				dumper.dump_string(SL_ConvertToString(asset->nodes[i].constant.parent.name));
			}

			dumper.dump_array(asset->nodes[i].constant.Links, asset->nodes[i].constant.totalLinkCount);
			dumper.dump_string(SL_ConvertToString(asset->nodes[i].constant.unk));
		}

		dumper.dump_array(asset->pathVis, asset->visBytes);

		dumper.dump_array(asset->nodeTree, asset->nodeTreeCount);
		for (auto i = 0; i < asset->nodeTreeCount; i++)
		{
			dump_node_tree(dumper, &asset->nodeTree[i]);
		}

		dumper.dump_array(asset->dynamicNodeGroups, asset->dynamicNodeGroupCount);
		for (auto i = 0; i < asset->dynamicNodeGroupCount; i++)
		{
			dumper.dump_array(asset->dynamicNodeGroups[i].nodeTree, 
				asset->dynamicNodeGroups[i].nodeTreeCount);
			for (auto o = 0; o < asset->dynamicNodeGroups[i].nodeTreeCount; o++)
			{
				dump_node_tree(dumper, &asset->dynamicNodeGroups[i].nodeTree[o]);
			}
		}

		dumper.dump_array(asset->pathExposure, asset->exposureBytes);
		dumper.dump_array(asset->pathNoPeekVis, asset->noPeekVisBytes);
		dumper.dump_array(asset->pathZones, asset->zonesBytes);
		dumper.dump_array(asset->pathDynStates, asset->dynStatesBytes);

		dumper.close();
	}
}