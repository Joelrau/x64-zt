#include "std_include.hpp"
#include "aipaths.hpp"

namespace zonetool::h1
{
	void IAIPaths::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* IAIPaths::get_script_string(scr_string_t* ptr)
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

	void write_node_tree_children(ZoneBuffer* buf, pathnode_tree_t* data, pathnode_tree_t** dest)
	{
		buf->align(3);
		const auto tree = buf->write(data);
		ZoneBuffer::clear_pointer(dest);

		if (data->axis < 0)
		{
			buf->align(1);
			buf->write(data->u.s.nodes, data->u.s.nodeCount);
			ZoneBuffer::clear_pointer(&tree->u.s.nodes);
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

	void write_node_tree(ZoneBuffer* buf, pathnode_tree_t* data, pathnode_tree_t* dest)
	{
		if (data->axis < 0)
		{
			buf->align(1);
			buf->write(data->u.s.nodes, data->u.s.nodeCount);
			ZoneBuffer::clear_pointer(&data->u.s.nodes);
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
		this->add_script_string(&asset->nodes[i].constant.__field__, script_string); \
	} \

#define WRITE_SCRIPT_STRING(__field__) \
	{ \
		data->nodes[i].constant.__field__ = static_cast<scr_string_t>( \
			buf->write_scriptstring(this->get_script_string(&data->nodes[i].constant.__field__))); \
	} \


	std::unordered_map<std::string, unsigned short> waypoint_types =
	{
		{"stand", 13},
		{"crouch", 14},
		{"prone", 15},
	};

	float distance(float* a, float* b)
	{
		return std::sqrtf((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]));
	}

	pathnode_tree_t* allocate_tree(PathData* asset, ZoneMemory* mem)
	{
		++asset->nodeTreeCount;
		return reinterpret_cast<pathnode_tree_t*>(
			::h1::game::Hunk_AllocAlignInternal(sizeof(pathnode_tree_t), 4));
	}

	pathnode_tree_t* build_node_tree(PathData* asset, ZoneMemory* mem, 
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
		const std::string& name, ZoneMemory* mem)
	{
		auto file = filesystem::file(path);
		file.open("rb");

		if (!file.get_fp())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing botwarfare waypoints \"%s\"...", path.data());

		auto table = csv::parser(filesystem::get_file_path(path) + path);
		const auto asset = mem->Alloc<PathData>();
		asset->name = mem->StrDup(name);

		if (table.get_num_rows() <= 0)
		{
			ZONETOOL_FATAL("No rows in csv");
		}

		const auto rows = table.get_rows();
		asset->nodeCount = std::atoi(rows[0]->fields[0]);
		asset->nodes = mem->Alloc<pathnode_t>(asset->nodeCount);

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
				node->constant.___u9.angles[0] = static_cast<float>(std::atof(angles_str[1].data()));
				node->constant.___u9.angles[1] = static_cast<float>(std::atof(angles_str[0].data()));
				node->constant.___u9.angles[2] = static_cast<float>(std::atof(angles_str[2].data()));
			}

			IMapEnts::add_entity_string("{\n");
			IMapEnts::add_entity_string("\"classname\" \"node_pathnode\"\n");
			IMapEnts::add_entity_string(utils::string::va("\"origin\" \"%f %f %f\"\n",
				node->constant.vLocalOrigin[0],
				node->constant.vLocalOrigin[1],
				node->constant.vLocalOrigin[2]
			));
			IMapEnts::add_entity_string(utils::string::va("\"angles\" \"%f %f %f\"\n", 
				node->constant.___u9.angles[0],
				node->constant.___u9.angles[1],
				node->constant.___u9.angles[2]
			));
			IMapEnts::add_entity_string("}\n");

			const auto links_str = utils::string::split(row->fields[1], ' ');
			node->constant.totalLinkCount = static_cast<unsigned short>(links_str.size());
			node->constant.Links = mem->Alloc<pathlink_s>(links_str.size());
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

		const auto node_indexes = mem->Alloc<unsigned short>(asset->nodeCount);
		for (auto i = 0u; i < asset->nodeCount; i++)
		{
			node_indexes[i] = static_cast<unsigned short>(i);
		}

		asset->nodeTree = build_node_tree(asset, mem, node_indexes, asset->nodeCount);

		return asset;
	}

	PathData* IAIPaths::parse(const std::string& name, ZoneMemory* mem)
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

	void IAIPaths::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
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

	void IAIPaths::prepare(ZoneBuffer* buf, ZoneMemory* mem)
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

	void IAIPaths::load_depending(IZone* zone)
	{
	}

	std::string IAIPaths::name()
	{
		return this->name_;
	}

	std::int32_t IAIPaths::type()
	{
		return ASSET_TYPE_AIPATHS;
	}

	void IAIPaths::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;

		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->nodes)
		{
			buf->align(3);
			auto* dest_nodes = buf->write(data->nodes, data->nodeCount);

			for (auto i = 0u; i < data->nodeCount; i++)
			{
				if (data->nodes[i].constant.Links)
				{
					buf->align(3);
					dest_nodes[i].constant.Links = buf->write(data->nodes[i].constant.Links, data->nodes[i].constant.totalLinkCount);
					ZoneBuffer::clear_pointer(&dest_nodes[i].constant.Links);
				}
			}

			ZoneBuffer::clear_pointer(&dest->nodes);
		}

		if (data->pathVis)
		{
			buf->align(0);
			buf->write(data->pathVis, data->visBytes);
			ZoneBuffer::clear_pointer(&dest->pathVis);
		}

		if (data->nodeTree)
		{
			buf->align(3);
			const auto dest_trees = buf->write(data->nodeTree, data->nodeTreeCount);

			for (auto i = 0; i < data->nodeTreeCount; i++)
			{
				write_node_tree(buf, &data->nodeTree[i], &dest_trees[i]);
			}

			ZoneBuffer::clear_pointer(&dest->nodeTree);
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
					ZoneBuffer::clear_pointer(dest->dynamicNodeGroups[i].nodeTree);
				}
			}

			ZoneBuffer::clear_pointer(&dest->dynamicNodeGroups);
		}

		if (data->pathExposure)
		{
			buf->align(0);
			buf->write(data->pathExposure, data->exposureBytes);
			ZoneBuffer::clear_pointer(dest->pathExposure);
		}

		if (data->pathNoPeekVis)
		{
			buf->align(0);
			buf->write(data->pathNoPeekVis, data->noPeekVisBytes);
			ZoneBuffer::clear_pointer(dest->pathNoPeekVis);
		}

		if (data->pathZones)
		{
			buf->align(0);
			buf->write(data->pathZones, data->zonesBytes);
			ZoneBuffer::clear_pointer(dest->pathZones);
		}

		if (data->pathDynStates)
		{
			buf->align(0);
			buf->write(data->pathDynStates, data->dynStatesBytes);
			ZoneBuffer::clear_pointer(dest->pathDynStates);
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
	void IAIPaths::dump(PathData* asset)
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