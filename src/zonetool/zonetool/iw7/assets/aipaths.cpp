#include "std_include.hpp"
#include "aipaths.hpp"

namespace zonetool::iw7
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

#define ADD_SCRIPT_STRING(__field__) \
	{ \
		const auto script_string = read.read_string(); \
		this->add_script_string(&asset->nodes[i].constant.__field__, script_string); \
	} \

#define WRITE_SCRIPT_STRING(__field__) \
	{ \
		data->nodes[i].constant.__field__ = static_cast<scr_string_t>( \
			buf->write_scriptstring(this->get_script_string(&data->nodes[i].constant.__field__))); \
	} \

	std::unordered_map<std::string, unsigned short> waypoint_types =
	{
		{"stand", NODE_CONCEALMENT_STAND},
		{"crouch", NODE_CONCEALMENT_CROUCH},
		{"prone", NODE_CONCEALMENT_PRONE},
	};

	float distance(float* a, float* b)
	{
		return std::sqrtf((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
	}

	pathnode_tree_t* allocate_tree(PathData* asset, zone_memory* mem)
	{
		++asset->nodeTreeCount;
		return mem->allocate<pathnode_tree_t>();
	}

	pathnode_tree_t* build_node_tree(PathData* asset, zone_memory* mem, 
		unsigned short* node_indexes, unsigned int num_nodes)
	{
		if (num_nodes < 4)
		{
			const auto result = allocate_tree(asset, mem);
			result->axis = -1;
			result->u.s.nodeCount = num_nodes;
			result->u.s.nodes = node_indexes;
			return result;
		}

		vec2_t maxs{};
		vec2_t mins{};

		const auto start_node = &asset->nodes[*node_indexes];
		maxs[0] = start_node->constant.vLocalOrigin[0];
		mins[0] = maxs[0];
		maxs[1] = start_node->constant.vLocalOrigin[1];
		mins[1] = maxs[1];

		for (auto i = 1u; i < num_nodes; i++)
		{
			for (auto axis = 0; axis < 2; axis++)
			{
				const auto node = &asset->nodes[node_indexes[i]];
				const auto value = node->constant.vLocalOrigin[axis];
				if (mins[axis] <= value)
				{
					if (value > maxs[axis])
					{
						maxs[axis] = value;
					}
				}
				else
				{
					mins[axis] = value;
				}
			}
		}

		const auto axis = (maxs[1] - mins[1]) > (maxs[0] - mins[0]);
		if ((maxs[axis] - mins[axis]) > 192.f)
		{
			const auto dist = (maxs[axis] + mins[axis]) * 0.5f;
			auto left = 0u;

			for (auto right = num_nodes - 1; ; --right)
			{
				while (dist > asset->nodes[node_indexes[left]].constant.vLocalOrigin[axis])
				{
					++left;
				}

				while (asset->nodes[node_indexes[right]].constant.vLocalOrigin[axis] > dist)
				{
					--right;
				}

				if (left >= right)
				{
					break;
				}

				const auto swap_node = node_indexes[left];
				node_indexes[left] = node_indexes[right];
				node_indexes[right] = swap_node;
				++left;
			}

			while (2 * left < num_nodes &&
				asset->nodes[node_indexes[left]].constant.vLocalOrigin[axis] == dist)
			{
				++left;
			}

			while (2 * left < num_nodes &&
				asset->nodes[node_indexes[left - 1]].constant.vLocalOrigin[axis] == dist)
			{
				--left;
			}

			pathnode_tree_t* child[2]{};
			child[0] = build_node_tree(asset, mem, node_indexes, left);
			child[1] = build_node_tree(asset, mem, &node_indexes[left], num_nodes - left);
			const auto result = allocate_tree(asset, mem);
			result->axis = axis;
			result->dist = dist;
			result->u.child[0] = child[0];
			result->u.child[1] = child[1];
			return result;
		}
		else
		{
			const auto result = allocate_tree(asset, mem);
			result->axis = -1;
			result->u.s.nodeCount = num_nodes;
			result->u.s.nodes = node_indexes;
			return result;
		}
	}


	PathData* parse_from_botwarfare(const std::string& path, 
		const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(path);
		file.open("rb");

		if (!file.get_fp())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing botwarfare waypoints \"%s\"...", path.data());

		auto table = csv::parser(filesystem::get_file_path(path) + path);
		const auto asset = mem->allocate<PathData>();
		asset->name = mem->duplicate_string(name);

		if (table.get_num_rows() <= 0)
		{
			ZONETOOL_FATAL("No rows in csv");
		}

		const auto rows = table.get_rows();
		asset->nodeCount = std::atoi(rows[0]->fields[0]);
		asset->nodes = mem->allocate<pathnode_t>(asset->nodeCount);

		if (table.get_num_rows() < static_cast<int>(asset->nodeCount))
		{
			ZONETOOL_FATAL("Less than asset->nodeCount + 1 (%i) rows", asset->nodeCount);
		}

		for (auto i = 0u; i < asset->nodeCount; i++)
		{
			const auto row = rows[i + 1];
			const auto node = &asset->nodes[i];
			node->constant.type = 1;

			if (row->num_fields < 4)
			{
				ZONETOOL_FATAL("Not enough fields for node num %i (must be origin,links,type,angles,...)", i);
			}

			const auto origin_str = utils::string::split(row->fields[0], ' ');
			if (origin_str.size() == 3)
			{
				node->constant.vLocalOrigin[0] = static_cast<float>(std::atof(origin_str[0].data()));
				node->constant.vLocalOrigin[1] = static_cast<float>(std::atof(origin_str[1].data()));
				node->constant.vLocalOrigin[2] = static_cast<float>(std::atof(origin_str[2].data()));
			}

			if (waypoint_types.contains(row->fields[2]))
			{
				node->constant.type = waypoint_types[row->fields[2]];
			}

			const auto angles_str = utils::string::split(row->fields[3], ' ');
			if (angles_str.size() == 3)
			{
				node->constant.orientation.angles[0] = static_cast<float>(std::atof(angles_str[1].data()));
				node->constant.orientation.angles[1] = static_cast<float>(std::atof(angles_str[0].data()));
				node->constant.orientation.angles[2] = static_cast<float>(std::atof(angles_str[2].data()));
			}

			map_ents::add_entity_string("{\n");
			map_ents::add_entity_string("\"classname\" \"node_pathnode\"\n");
			map_ents::add_entity_string(utils::string::va("\"origin\" \"%f %f %f\"\n",
				node->constant.vLocalOrigin[0],
				node->constant.vLocalOrigin[1],
				node->constant.vLocalOrigin[2]
			));
			map_ents::add_entity_string(utils::string::va("\"angles\" \"%f %f %f\"\n", 
				node->constant.orientation.angles[0],
				node->constant.orientation.angles[1],
				node->constant.orientation.angles[2]
			));
			map_ents::add_entity_string("}\n");

			const auto links_str = utils::string::split(row->fields[1], ' ');
			node->constant.totalLinkCount = static_cast<unsigned short>(links_str.size());
			node->constant.Links = mem->allocate<pathlink_s>(links_str.size());
			for (auto o = 0; o < node->constant.totalLinkCount; o++)
			{
				const auto num = std::atoi(links_str[o].data());
				node->constant.Links[o].nodeNum = static_cast<unsigned short>(num);
			}
		}

		for (auto i = 0u; i < asset->nodeCount; i++)
		{
			const auto node = &asset->nodes[i];
			for (auto o = 0; o < node->constant.totalLinkCount; o++)
			{
				const auto linked_num = node->constant.Links[o].nodeNum;
				if (linked_num >= asset->nodeCount)
				{
					ZONETOOL_FATAL("Node link num out of bounds");
				}

				const auto linked = &asset->nodes[linked_num];
				node->constant.Links[o].negotiationLink = 1;
				node->constant.Links[o].fDist = 
					distance(node->constant.vLocalOrigin, linked->constant.vLocalOrigin);
			}
		}

		const auto node_indexes = mem->allocate<unsigned short>(asset->nodeCount);
		for (auto i = 0u; i < asset->nodeCount; i++)
		{
			node_indexes[i] = static_cast<unsigned short>(i);
		}

		asset->nodeTree = build_node_tree(asset, mem, node_indexes, asset->nodeCount);

		return asset;
	}

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
		}

		asset->pathVis = read.read_array<char>();

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

		asset->pathExposure = read.read_array<char>();
		asset->pathNoPeekVis = read.read_array<char>();
		asset->pathZones = read.read_array<char>();
		asset->pathDynStates = read.read_array<char>();

		return asset;
	}

	void path_data::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
		this->asset_ = parse_from_botwarfare(name, this->name_, mem);
		if (this->asset_)
		{
			return;
		}

		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse aipaths \"%s\"", name.data());
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
		return ASSET_TYPE_PATHDATA;
	}

	void write_node_tree_children(zone_buffer* buf, pathnode_tree_t* data, pathnode_tree_t** dest)
	{
		buf->align(7);
		const auto tree = buf->write(data);
		buf->clear_pointer(dest);

		if (data->axis < 0)
		{
			buf->align(1);
			buf->write(data->u.s.nodes, data->u.s.nodeCount);
			buf->clear_pointer(&tree->u.s.nodes);
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
			buf->clear_pointer(&data->u.s.nodes);
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

	void path_data::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;

		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->nodes)
		{
			buf->align(7);
			auto* dest_nodes = buf->write(data->nodes, data->nodeCount);

			for (auto i = 0u; i < data->nodeCount; i++)
			{
				if (data->nodes[i].constant.Links)
				{
					buf->align(3);
					dest_nodes[i].constant.Links = buf->write(data->nodes[i].constant.Links, data->nodes[i].constant.totalLinkCount);
					buf->clear_pointer(&dest_nodes[i].constant.Links);
				}
			}

			buf->clear_pointer(&dest->nodes);
		}

		if (data->pathVis)
		{
			buf->align(0);
			buf->write(data->pathVis, data->visBytes);
			buf->clear_pointer(&dest->pathVis);
		}

		if (data->nodeTree)
		{
			buf->align(7);
			const auto dest_trees = buf->write(data->nodeTree, data->nodeTreeCount);

			for (auto i = 0; i < data->nodeTreeCount; i++)
			{
				write_node_tree(buf, &data->nodeTree[i], &dest_trees[i]);
			}

			buf->clear_pointer(&dest->nodeTree);
		}

		if (data->dynamicNodeGroups)
		{
			buf->align(7);
			buf->write(data->dynamicNodeGroups, data->dynamicNodeGroupCount);

			for (auto i = 0; i < data->dynamicNodeGroupCount; i++)
			{
				if (data->dynamicNodeGroups[i].nodeTree)
				{
					buf->align(7);
					const auto dest_trees = buf->write(data->dynamicNodeGroups[i].nodeTree,
						data->dynamicNodeGroups[i].nodeTreeCount);
					for (auto o = 0; o < data->dynamicNodeGroups[i].nodeTreeCount; o++)
					{
						write_node_tree(buf, &data->dynamicNodeGroups[i].nodeTree[o], &dest_trees[o]);
					}
					buf->clear_pointer(dest->dynamicNodeGroups[i].nodeTree);
				}
			}

			buf->clear_pointer(&dest->dynamicNodeGroups);
		}

		if (data->pathExposure)
		{
			buf->align(0);
			buf->write(data->pathExposure, data->exposureBytes);
			buf->clear_pointer(dest->pathExposure);
		}

		if (data->pathNoPeekVis)
		{
			buf->align(0);
			buf->write(data->pathNoPeekVis, data->noPeekVisBytes);
			buf->clear_pointer(dest->pathNoPeekVis);
		}

		if (data->pathZones)
		{
			buf->align(0);
			buf->write(data->pathZones, data->zonesBytes);
			buf->clear_pointer(dest->pathZones);
		}

		if (data->pathDynStates)
		{
			buf->align(0);
			buf->write(data->pathDynStates, data->dynStatesBytes);
			buf->clear_pointer(dest->pathDynStates);
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