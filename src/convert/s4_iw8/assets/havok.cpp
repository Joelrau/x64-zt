#include <std_include.hpp>

#include "../include.hpp"
#include "havok.hpp"
#include "physics.hpp"

#include <iw8/zone/loader.hpp>

#include <io/game_files.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::s4::converter::iw8::havok
{
	namespace
	{
		namespace tag = ::havok::tagfile;
		namespace target = zonetool::iw8;

		using type_list = std::vector<tag::type_info>;

		const tag::type_info& resolve(const type_list& types, std::uint32_t type)
		{
			while ((types.at(type).format & tag::format_kind_mask) == tag::kind_void && types[type].parent)
			{
				type = types[type].parent;
			}

			return types[type];
		}

		std::uint32_t kind_of(const type_list& types, const std::uint32_t type)
		{
			return resolve(types, type).format & tag::format_kind_mask;
		}

		std::uint32_t size_of(const type_list& types, std::uint32_t type)
		{
			while (!types.at(type).size && types[type].parent)
			{
				type = types[type].parent;
			}

			return types[type].size;
		}

		bool is_fixed_array(const type_list& types, const std::uint32_t type)
		{
			const auto& info = resolve(types, type);
			return (info.format & tag::format_kind_mask) == tag::kind_array && (info.format & tag::format_fixed_size);
		}

		std::string full_name(const type_list& types, const std::uint32_t type)
		{
			const auto& info = types.at(type);
			if (info.templates.empty())
			{
				return info.name;
			}

			auto name = info.name + "<";
			for (const auto& [template_name, value] : info.templates)
			{
				name += template_name.starts_with('t') ? full_name(types, value) : std::to_string(value);
				name += ",";
			}

			name.back() = '>';
			return name;
		}

		std::optional<std::uint32_t> find_template(const type_list& types, std::uint32_t type, const std::string_view name)
		{
			for (; type; type = types[type].parent)
			{
				for (const auto& [template_name, value] : types.at(type).templates)
				{
					if (template_name == name)
					{
						return value;
					}
				}
			}

			return {};
		}

		std::uint32_t element_type(const type_list& types, const std::uint32_t type)
		{
			const auto& info = resolve(types, type);
			return info.subtype ? info.subtype : find_template(types, type, "tT").value_or(0);
		}

		std::uint32_t fixed_count(const type_list& types, const std::uint32_t type)
		{
			return find_template(types, type, "vN").value_or(resolve(types, type).format >> 8);
		}

		const tag::type_member* find_member(const type_list& types, std::uint32_t type, const std::string_view name)
		{
			for (; type; type = types[type].parent)
			{
				for (const auto& member : types.at(type).members)
				{
					if (member.name == name)
					{
						return &member;
					}
				}
			}

			return nullptr;
		}

		std::vector<const tag::type_member*> get_members(const type_list& types, std::uint32_t type)
		{
			std::vector<const tag::type_member*> members;
			for (; type; type = types[type].parent)
			{
				for (const auto& member : types.at(type).members)
				{
					members.push_back(&member);
				}
			}

			return members;
		}

		bool is_plain(const std::uint32_t kind)
		{
			return kind == tag::kind_bool || kind == tag::kind_int || kind == tag::kind_float || kind == tag::kind_opaque;
		}

		template <typename T>
		void put(std::vector<std::byte>& out, const std::size_t offset, const T& value)
		{
			std::memcpy(out.data() + offset, &value, sizeof(T));
		}

		template <typename Node>
		std::vector<float> collect_points(const Node* nodes, const std::size_t count)
		{
			std::vector<float> points;
			const std::function<void(std::uint32_t)> add_node = [&](const std::uint32_t index)
			{
				const auto& node = nodes[index];
				for (auto i = 0; i < 4; i++)
				{
					if (node.data[i] == 0xFFFFFFFF || node.lx[i] > node.hx[i])
					{
						continue;
					}

					if (!node.isLeaf)
					{
						if (node.data[i] < count)
						{
							add_node(node.data[i]);
						}

						continue;
					}

					const auto instance = 0x3F000000u | node.data[i];
					points.insert(points.end(), {node.lx[i] + node.hx[i], node.ly[i] + node.hy[i], node.lz[i] + node.hz[i], std::bit_cast<float>(instance)});
				}
			};
			add_node(1);

			return points;
		}

		void get_bounds(const std::vector<compound_child>& children, const std::span<const std::uint32_t> items, vec3_t min, vec3_t max)
		{
			for (auto axis = 0; axis < 3; axis++)
			{
				min[axis] = std::numeric_limits<float>::max();
				max[axis] = -std::numeric_limits<float>::max();
				for (const auto item : items)
				{
					min[axis] = std::min(min[axis], children[item].min[axis]);
					max[axis] = std::max(max[axis], children[item].max[axis]);
				}
			}
		}

		target::hkcdSimdTreeNode make_tree_node(const bool leaf)
		{
			target::hkcdSimdTreeNode node{};
			for (auto slot = 0; slot < 4; slot++)
			{
				node.lx[slot] = node.ly[slot] = node.lz[slot] = std::numeric_limits<float>::max();
				node.hx[slot] = node.hy[slot] = node.hz[slot] = -std::numeric_limits<float>::max();
				node.data[slot] = leaf ? 0xFFFFFFFF : 0;
			}

			node.isLeaf = leaf;
			return node;
		}

		void set_tree_slot(target::hkcdSimdTreeNode& node, const std::uint32_t slot, const vec3_t min, const vec3_t max, const std::uint32_t data)
		{
			node.lx[slot] = min[0];
			node.ly[slot] = min[1];
			node.lz[slot] = min[2];
			node.hx[slot] = max[0];
			node.hy[slot] = max[1];
			node.hz[slot] = max[2];
			node.data[slot] = data;
		}

		void build_tree_node(std::vector<target::hkcdSimdTreeNode>& nodes, const std::uint32_t index, const std::vector<compound_child>& children,
			const std::span<std::uint32_t> items)
		{
			vec3_t min, max;
			get_bounds(children, items, min, max);
			const auto extent = [&](const int axis) { return max[axis] - min[axis]; };
			const auto axis = extent(0) >= extent(1) && extent(0) >= extent(2) ? 0 : extent(1) >= extent(2) ? 1 : 2;
			std::sort(items.begin(), items.end(), [&](const std::uint32_t a, const std::uint32_t b)
			{
				return children[a].min[axis] + children[a].max[axis] < children[b].min[axis] + children[b].max[axis];
			});

			const auto group_count = std::min<std::size_t>(4, (items.size() + 3) / 4);
			const auto first_child = static_cast<std::uint32_t>(nodes.size());
			nodes.resize(nodes.size() + group_count);

			for (auto group = 0u; group < group_count; group++)
			{
				const auto begin = items.size() * group / group_count;
				const auto end = items.size() * (group + 1) / group_count;
				const auto group_items = items.subspan(begin, end - begin);
				const auto child = first_child + group;

				vec3_t group_min, group_max;
				get_bounds(children, group_items, group_min, group_max);
				set_tree_slot(nodes[index], group, group_min, group_max, child);

				nodes[child] = make_tree_node(group_items.size() <= 4);
				if (group_items.size() > 4)
				{
					build_tree_node(nodes, child, children, group_items);
					continue;
				}

				for (auto slot = 0u; slot < group_items.size(); slot++)
				{
					const auto& item = children[group_items[slot]];
					set_tree_slot(nodes[child], slot, item.min, item.max, group_items[slot]);
				}
			}
		}

		const tag::file& source_compendium()
		{
			static const auto compendium = []
			{
				const auto* library = static_cast<const PhysicsLibrary*>(db_find_asset(ASSET_TYPE_PHYSICSLIBRARY, "physicsglobaltypecompendium"));
				if (!library)
				{
					throw std::runtime_error("the S4 physicsglobaltypecompendium is not loaded");
				}

				return tag::file(std::string_view(library->havokData, library->havokDataSize));
			}();

			return compendium;
		}

		std::string load_target_compendium()
		{
			xfile::oodle::load(iw8_folder());
			auto previous_files = game_files::exchange(game_files::open_casc(iw8_folder()));
			const auto _ = gsl::finally([&previous_files] { game_files::set(std::move(previous_files)); });

			const auto path = game_files::find_zone("code_post_gfx");
			if (!path)
			{
				throw std::runtime_error("the IW8 zone code_post_gfx is missing");
			}

			const auto database = db_create();
			db_scope scope(*database);
			const auto zone = target::load_zone(*path);
			for (const auto& asset : zone->assets)
			{
				const auto* library = static_cast<const target::PhysicsLibrary*>(asset.header);
				if (!asset.referenced && asset.type == target::ASSET_TYPE_PHYSICSLIBRARY && library->isGlobalTypeCompendium)
				{
					return std::string(library->havokData, library->havokDataSize);
				}
			}

			throw std::runtime_error("the IW8 zone code_post_gfx has no type compendium");
		}

		const tag::file& target_compendium()
		{
			static const auto data = load_target_compendium();
			static const tag::file compendium(data);
			return compendium;
		}
	}

	transcoder::transcoder(const tag::file& source, const type_list& target_types, tag::writer& writer, const std::string_view asset_name)
		: source_(source)
		, source_types_(source.types())
		, target_types_(target_types)
		, writer_(writer)
		, asset_name_(asset_name)
	{
		for (auto i = 1u; i < target_types.size(); i++)
		{
			this->target_names_.try_emplace(full_name(target_types, i), i);
		}
	}

	void transcoder::merge_tiles(const tile_shape_map& tiles)
	{
		this->tiles_ = &tiles;
	}

	void transcoder::clear_root_user_data()
	{
		this->clear_root_user_data_ = true;
	}

	std::uint32_t transcoder::convert_root()
	{
		const auto& root = this->source_.get_item(1);
		return this->convert_object({root.type, root.offset});
	}

	template <typename T>
	T transcoder::read(const std::size_t offset) const
	{
		const auto data = this->source_.data();
		if (offset + sizeof(T) > data.size())
		{
			throw std::runtime_error("havok: read out of range");
		}

		T value{};
		std::memcpy(&value, data.data() + offset, sizeof(T));
		return value;
	}

	std::uint32_t transcoder::add_pointer_array(const std::uint32_t array_type, const std::uint32_t item)
	{
		if (!item)
		{
			return 0;
		}

		const auto element = element_type(this->target_types_, array_type);
		const std::vector<std::byte> bytes(size_of(this->target_types_, element), std::byte{0});
		const auto reference = this->writer_.add_array(element, bytes.data(), 1);
		this->writer_.set_pointer(reference.offset, element, item);
		return reference.item;
	}

	template <typename T>
	std::uint32_t transcoder::add_value_array(const std::uint32_t array_type, const std::vector<T>& values)
	{
		return this->writer_.add_array(element_type(this->target_types_, array_type), values.data(), values.size()).item;
	}

	std::int64_t transcoder::read_int(const std::uint32_t type, const std::size_t offset) const
	{
		switch (size_of(this->source_types_, type))
		{
		case 1:
			return this->read<std::int8_t>(offset);
		case 2:
			return this->read<std::int16_t>(offset);
		case 4:
			return this->read<std::int32_t>(offset);
		default:
			return this->read<std::int64_t>(offset);
		}
	}

	std::optional<source_object> transcoder::read_pointer(const std::uint32_t type, const std::size_t offset) const
	{
		if (resolve(this->source_types_, type).name == "hkRelPtr")
		{
			const auto relative = this->read<std::int64_t>(offset);
			if (!relative)
			{
				return {};
			}

			const auto target_offset = offset + relative;
			const auto* item = this->source_.find_item(target_offset);
			return source_object{item ? item->type : element_type(this->source_types_, type), target_offset};
		}

		const auto index = this->read<std::uint64_t>(offset);
		if (!index)
		{
			return {};
		}

		const auto& item = this->source_.get_item(index);
		return source_object{item.type, item.offset};
	}

	source_range transcoder::read_range(const std::uint32_t type, const std::size_t offset) const
	{
		const auto* offset_member = find_member(this->source_types_, type, "offset");
		const auto* size_member = find_member(this->source_types_, type, "size");
		if (offset_member && size_member)
		{
			const auto count = this->read_int(size_member->type, offset + size_member->offset);
			if (count <= 0)
			{
				return {};
			}

			const auto field = offset + offset_member->offset;
			const auto target_offset = field + this->read_int(offset_member->type, field);
			const auto* item = this->source_.find_item(target_offset);
			return {item ? item->type : element_type(this->source_types_, type), target_offset, static_cast<std::size_t>(count)};
		}

		if (is_fixed_array(this->source_types_, type))
		{
			return {element_type(this->source_types_, type), offset, fixed_count(this->source_types_, type)};
		}

		const auto index = size_of(this->source_types_, type) == sizeof(std::uint32_t)
			? this->read<std::uint32_t>(offset)
			: this->read<std::uint64_t>(offset);
		if (!index)
		{
			return {};
		}

		const auto& item = this->source_.get_item(index);
		return {item.type, item.offset, item.count};
	}

	std::optional<source_object> transcoder::find_source_member(const std::uint32_t type, const std::size_t offset, const std::string_view name) const
	{
		if (const auto* member = find_member(this->source_types_, type, name))
		{
			return source_object{member->type, offset + member->offset};
		}

		if (const auto* hull = find_member(this->source_types_, type, "hull"))
		{
			if (const auto* member = find_member(this->source_types_, hull->type, name))
			{
				return source_object{member->type, offset + hull->offset + member->offset};
			}
		}

		return {};
	}

	std::optional<std::uint32_t> transcoder::find_target_type(const std::string_view name) const
	{
		const auto found = this->target_names_.find(std::string(name == "hknpConvexShape" ? "hknpConvexPolytopeShape" : name));
		if (found == this->target_names_.end())
		{
			return {};
		}

		return found->second;
	}

	std::vector<std::int32_t> transcoder::read_int_array(const std::uint32_t type, const std::size_t offset, const std::string_view name) const
	{
		std::vector<std::int32_t> values;
		if (const auto member = this->find_source_member(type, offset, name))
		{
			const auto range = this->read_range(member->type, member->offset);
			const auto size = size_of(this->source_types_, range.type);
			for (auto i = 0u; i < range.count; i++)
			{
				values.push_back(static_cast<std::int32_t>(this->read_int(range.type, range.offset + i * size)));
			}
		}

		return values;
	}

	std::vector<std::array<float, 4>> transcoder::read_vector_array(const std::uint32_t type, const std::size_t offset, const std::string_view name) const
	{
		std::vector<std::array<float, 4>> values;
		if (const auto member = this->find_source_member(type, offset, name))
		{
			const auto range = this->read_range(member->type, member->offset);
			const auto size = size_of(this->source_types_, range.type);
			for (auto i = 0u; i < range.count; i++)
			{
				values.push_back(this->read<std::array<float, 4>>(range.offset + i * size));
			}
		}

		return values;
	}

	member_location transcoder::locate_target_member(const std::uint32_t type, const std::initializer_list<std::string_view> path) const
	{
		member_location location{0, type};
		for (const auto name : path)
		{
			const auto* member = find_member(this->target_types_, location.type, name);
			if (!member)
			{
				throw std::runtime_error(std::format("havok: IW8 type \"{}\" has no member \"{}\"", this->target_types_[location.type].name, name));
			}

			location.offset += member->offset;
			location.type = member->type;
		}

		return location;
	}

	void transcoder::fill(const std::uint32_t source_type, const std::size_t source_offset, const std::uint32_t target_type,
		std::vector<std::byte>& out, const std::size_t base, std::vector<pointer_fixup>& fixups)
	{
		const auto source_kind = kind_of(this->source_types_, source_type);
		const auto target_kind = kind_of(this->target_types_, target_type);

		if (target_kind == tag::kind_record)
		{
			this->fill_record(source_type, source_offset, target_type, out, base, fixups);
		}
		else if (target_kind == tag::kind_array)
		{
			this->fill_array(source_type, source_offset, target_type, out, base, fixups);
		}
		else if (target_kind == tag::kind_pointer)
		{
			if (source_kind != tag::kind_pointer)
			{
				return;
			}

			if (const auto object = this->read_pointer(source_type, source_offset))
			{
				fixups.push_back({base, target_type, [this, object = *object] { return this->convert_object(object); }});
			}
		}
		else if (target_kind == tag::kind_string)
		{
			const auto index = source_kind == tag::kind_string ? this->read<std::uint64_t>(source_offset) : 0;
			if (!index)
			{
				return;
			}

			const auto& item = this->source_.get_item(index);
			fixups.push_back({base, target_type, [this, text = std::string(this->source_.data().substr(item.offset, item.count))]
			{
				return this->writer_.add_array(*this->find_target_type("char"), text.data(), text.size()).item;
			}});
		}
		else if (source_kind == tag::kind_record)
		{
			const auto members = get_members(this->source_types_, source_type);
			if (members.size() == 1)
			{
				this->fill(members.front()->type, source_offset + members.front()->offset, target_type, out, base, fixups);
			}
		}
		else if (is_plain(source_kind) && is_plain(target_kind))
		{
			const auto size = std::min(size_of(this->source_types_, source_type), size_of(this->target_types_, target_type));
			std::memcpy(out.data() + base, this->source_.data().data() + source_offset, size);
		}
	}

	void transcoder::fill_record(const std::uint32_t source_type, const std::size_t source_offset, const std::uint32_t target_type,
		std::vector<std::byte>& out, const std::size_t base, std::vector<pointer_fixup>& fixups)
	{
		if (kind_of(this->source_types_, source_type) != tag::kind_record)
		{
			const auto members = get_members(this->target_types_, target_type);
			if (members.size() == 1)
			{
				this->fill(source_type, source_offset, members.front()->type, out, base + members.front()->offset, fixups);
			}

			return;
		}

		for (auto type = target_type; type; type = this->target_types_[type].parent)
		{
			for (const auto& member : this->target_types_[type].members)
			{
				if (this->fill_changed_member(this->target_types_[type].name, member, source_type, source_offset, out, base + member.offset, fixups))
				{
					continue;
				}

				if (const auto source_member = this->find_source_member(source_type, source_offset, member.name))
				{
					this->fill(source_member->type, source_member->offset, member.type, out, base + member.offset, fixups);
				}
			}
		}
	}

	void transcoder::fill_array(const std::uint32_t source_type, const std::size_t source_offset, const std::uint32_t target_type,
		std::vector<std::byte>& out, const std::size_t base, std::vector<pointer_fixup>& fixups)
	{
		const auto source_kind = kind_of(this->source_types_, source_type);
		const auto target_element = element_type(this->target_types_, target_type);

		if (!is_fixed_array(this->target_types_, target_type))
		{
			if (source_kind != tag::kind_array)
			{
				return;
			}

			if (const auto range = this->read_range(source_type, source_offset); range.count)
			{
				fixups.push_back({base, target_type, [this, range, target_element] { return this->convert_array(range, target_element); }});
			}

			return;
		}

		const auto target_size = size_of(this->target_types_, target_element);
		const auto target_count = fixed_count(this->target_types_, target_type);
		if (source_kind == tag::kind_array)
		{
			const auto range = this->read_range(source_type, source_offset);
			const auto source_size = size_of(this->source_types_, range.type);
			for (auto i = 0u; i < std::min<std::size_t>(range.count, target_count); i++)
			{
				this->fill(range.type, range.offset + i * source_size, target_element, out, base + i * target_size, fixups);
			}
		}
		else if (source_kind == tag::kind_record)
		{
			const auto members = get_members(this->source_types_, source_type);
			for (auto i = 0u; i < std::min<std::size_t>(members.size(), target_count); i++)
			{
				this->fill(members[i]->type, source_offset + members[i]->offset, target_element, out, base + i * target_size, fixups);
			}
		}
	}

	bool transcoder::fill_changed_member(const std::string_view owner, const tag::type_member& member, const std::uint32_t source_type,
		const std::size_t source_offset, std::vector<std::byte>& out, const std::size_t base, std::vector<pointer_fixup>& fixups)
	{
		if (owner == "hkReferencedObject")
		{
			return true;
		}

		if (owner == "hknpShape" && member.name == "userData" && this->clear_root_user_data_ && source_offset == this->source_.get_item(1).offset)
		{
			return true;
		}

		if (owner.ends_with("ShapeTagData") && member.name == "collisionFilterInfo")
		{
			const auto contents = this->find_source_member(source_type, source_offset, "collisionFilterInfo");
			put(out, base, physics::convert_contents(this->read<std::uint32_t>(contents->offset)));
			return true;
		}

		if (owner == "HavokPhysicsShapeList" && this->tiles_)
		{
			return this->fill_merged_shape_list(member, source_type, source_offset, base, fixups);
		}

		if (owner.starts_with("hkBitFieldBase") && member.name == "storage")
		{
			this->fill(source_type, source_offset, member.type, out, base, fixups);
			return true;
		}

		if (owner == "hknpShape" && member.name == "type")
		{
			const auto type = this->find_source_member(source_type, source_offset, "type");
			put(out, base, static_cast<std::uint8_t>(this->read<std::uint8_t>(type->offset) + 1)); // IW8 has CONVEX_POLYTOPE at 1
			return true;
		}

		if (owner == "hknpShape" && member.name == "properties")
		{
			const auto list = this->find_source_member(source_type, source_offset, "properties");
			const auto entries = this->find_source_member(list->type, list->offset, "properties");
			if (const auto range = this->read_range(entries->type, entries->offset); range.count)
			{
				fixups.push_back({base, member.type, [this, range] { return this->convert_properties(range); }});
			}

			return true;
		}

		if (owner == "hknpConvexShape" && member.name == "vertices")
		{
			const auto vertices = this->find_source_member(source_type, source_offset, "vertices");
			if (const auto range = this->read_range(vertices->type, vertices->offset); range.count)
			{
				const auto target_element = element_type(this->target_types_, member.type);
				fixups.push_back({base, member.type, [this, range, target_element] { return this->convert_hull_vertices(range, target_element); }});
			}

			return true;
		}

		if (owner == "hknpConvexPolytopeShape" && member.name == "connectivity")
		{
			const auto vertex_edges = this->find_source_member(source_type, source_offset, "vertexEdges");
			const auto face_links = this->find_source_member(source_type, source_offset, "faceLinks");
			const auto vertex_edge_range = this->read_range(vertex_edges->type, vertex_edges->offset);
			const auto face_link_range = this->read_range(face_links->type, face_links->offset);
			const auto vertices = this->find_source_member(source_type, source_offset, "vertices");
			const auto vertex_count = this->read_range(vertices->type, vertices->offset).count;
			if (vertex_edge_range.count && face_link_range.count)
			{
				fixups.push_back({base, member.type, [this, vertex_edge_range, face_link_range, vertex_count]
				{
					return this->convert_hull_connectivity(vertex_edge_range, face_link_range, vertex_count);
				}});

				return true;
			}

			const auto faces = this->find_source_member(source_type, source_offset, "faces");
			const auto indices = this->find_source_member(source_type, source_offset, "indices");
			const auto face_range = this->read_range(faces->type, faces->offset);
			const auto index_range = this->read_range(indices->type, indices->offset);
			if (vertex_count && face_range.count && index_range.count)
			{
				fixups.push_back({base, member.type, [this, face_range, index_range, vertex_count]
				{
					return this->build_hull_connectivity(face_range, index_range, vertex_count);
				}});
			}

			return true;
		}

		if (owner == "hknpShapeInstance" && member.name == "transform")
		{
			this->fill_instance_transform(source_offset, out, base);
			return true;
		}

		if (owner == "hknpShapeInstance" && member.name == "scale")
		{
			const auto& instance = *reinterpret_cast<const hknpShapeInstance*>(this->source_.data().data() + source_offset);
			put(out, base, instance.scale);
			put(out, base + sizeof(vec3_t), 1.0f);
			return true;
		}

		if (owner == "hknpCompoundShapeSimdTree" && member.name == "points")
		{
			const auto nodes = this->find_source_member(source_type, source_offset, "nodes");
			if (const auto range = this->read_range(nodes->type, nodes->offset); range.count > 1)
			{
				const auto target_element = element_type(this->target_types_, member.type);
				fixups.push_back({base, member.type, [this, range, target_element] { return this->build_compound_points(range, target_element); }});
			}

			return true;
		}

		if (owner == "hknpPhysicsSystemData" && member.name == "microStepMultiplier")
		{
			put(out, base, static_cast<std::uint8_t>(1));
			return true;
		}

		return false;
	}

	bool transcoder::fill_merged_shape_list(const tag::type_member& member, const std::uint32_t source_type, const std::size_t source_offset,
		const std::size_t base, std::vector<pointer_fixup>& fixups)
	{
		const auto add_ints = [&](std::vector<std::int32_t> values)
		{
			fixups.push_back({base, member.type, [this, type = member.type, values = std::move(values)] { return this->add_value_array(type, values); }});
			return true;
		};

		if (member.name == "shapes")
		{
			fixups.push_back({base, member.type, [this, type = member.type, source_type, source_offset]
			{
				return this->add_pointer_array(type, this->build_world_compound(source_type, source_offset));
			}});
			return true;
		}

		if (member.name == "shapeIndices")
		{
			return add_ints({-1});
		}

		if (member.name == "vertCounts" || member.name == "triCounts" || member.name == "convexCounts")
		{
			const auto values = this->read_int_array(source_type, source_offset, member.name);
			return add_ints({std::accumulate(values.begin(), values.end(), 0)});
		}

		if (member.name == "shapeContents")
		{
			const auto values = this->read_int_array(source_type, source_offset, member.name);
			const auto merge = [](const std::int32_t merged, const std::int32_t contents)
			{
				return merged | static_cast<std::int32_t>(physics::convert_contents(contents));
			};

			return add_ints({std::accumulate(values.begin(), values.end(), 0, merge)});
		}

		if (member.name == "minMaxes")
		{
			const auto bounds = this->read_vector_array(source_type, source_offset, member.name);
			const auto limit = std::numeric_limits<float>::max();
			std::vector<std::array<float, 4>> merged{{limit, limit, limit, 0.0f}, {-limit, -limit, -limit, 0.0f}};
			for (auto i = 0u; i + 1 < bounds.size(); i += 2)
			{
				for (auto axis = 0; axis < 3; axis++)
				{
					merged[0][axis] = std::min(merged[0][axis], bounds[i][axis]);
					merged[1][axis] = std::max(merged[1][axis], bounds[i + 1][axis]);
				}
			}

			fixups.push_back({base, member.type, [this, type = member.type, merged] { return this->add_value_array(type, merged); }});
			return true;
		}

		return false;
	}

	std::uint32_t transcoder::convert_object(const source_object& object)
	{
		if (const auto found = this->converted_.find(object.offset); found != this->converted_.end())
		{
			return found->second;
		}

		const auto name = full_name(this->source_types_, object.type);
		const auto target_type = this->find_target_type(name);
		if (!target_type)
		{
			ZONETOOL_WARNING("havok data \"%s\": IW8 has no type \"%s\", the object is dropped", this->asset_name_.data(), name.data());
			return 0;
		}

		std::vector<std::byte> bytes(size_of(this->target_types_, *target_type), std::byte{0});
		std::vector<pointer_fixup> fixups;
		this->fill(object.type, object.offset, *target_type, bytes, 0, fixups);

		const auto reference = this->writer_.add_object(*target_type, bytes.data());
		this->converted_[object.offset] = reference.item;
		this->apply(reference.offset, fixups);
		return reference.item;
	}

	std::uint32_t transcoder::convert_array(const source_range& range, const std::uint32_t target_element)
	{
		const auto source_size = size_of(this->source_types_, range.type);
		const auto target_size = size_of(this->target_types_, target_element);
		if (!source_size || !target_size)
		{
			return 0;
		}

		std::vector<std::byte> bytes(target_size * range.count, std::byte{0});
		std::vector<pointer_fixup> fixups;
		if (source_size == target_size && is_plain(kind_of(this->source_types_, range.type)) && is_plain(kind_of(this->target_types_, target_element)))
		{
			std::memcpy(bytes.data(), this->source_.data().data() + range.offset, bytes.size());
		}
		else
		{
			for (auto i = 0u; i < range.count; i++)
			{
				this->fill(range.type, range.offset + i * source_size, target_element, bytes, i * target_size, fixups);
			}
		}

		const auto reference = this->writer_.add_array(target_element, bytes.data(), range.count);
		this->apply(reference.offset, fixups);
		return reference.item;
	}

	std::uint32_t transcoder::convert_properties(const source_range& properties)
	{
		const auto list_type = *this->find_target_type("hkRefCountedProperties");
		const auto* entries = find_member(this->target_types_, list_type, "entries");

		const std::vector<std::byte> bytes(size_of(this->target_types_, list_type), std::byte{0});
		const auto reference = this->writer_.add_object(list_type, bytes.data());
		const auto item = this->convert_array(properties, element_type(this->target_types_, entries->type));
		this->writer_.set_pointer(reference.offset + entries->offset, entries->type, item);
		return reference.item;
	}

	std::uint32_t transcoder::convert_hull_vertices(const source_range& vertices, const std::uint32_t target_element)
	{
		const auto source_size = size_of(this->source_types_, vertices.type);
		const auto padded_count = (vertices.count + 3) & ~std::size_t{3};

		std::vector<std::byte> bytes(padded_count * 16, std::byte{0});
		for (auto i = 0u; i < padded_count; i++)
		{
			const auto index = std::min<std::size_t>(i, vertices.count - 1);
			std::memcpy(bytes.data() + i * 16, this->source_.data().data() + vertices.offset + index * source_size, 12);
			put(bytes, i * 16 + 12, static_cast<std::uint32_t>(0x3F000000 | index));
		}

		return this->writer_.add_array(target_element, bytes.data(), padded_count).item;
	}

	std::uint32_t transcoder::add_hull_connectivity(const std::uint32_t vertex_edges, const std::uint32_t face_links)
	{
		const auto type = *this->find_target_type("hknpConvexPolytopeShape::Connectivity");
		const std::vector<std::byte> bytes(size_of(this->target_types_, type), std::byte{0});
		const auto reference = this->writer_.add_object(type, bytes.data());

		for (const auto& [name, item] : {std::pair{"vertexEdges", vertex_edges}, std::pair{"faceLinks", face_links}})
		{
			if (item)
			{
				const auto* member = find_member(this->target_types_, type, name);
				this->writer_.set_pointer(reference.offset + member->offset, member->type, item);
			}
		}

		return reference.item;
	}

	std::uint32_t transcoder::convert_hull_connectivity(const source_range& vertex_edges, const source_range& face_links, const std::size_t vertex_count)
	{
		const auto* first_edge = reinterpret_cast<const hknpConvexHullEdge*>(this->source_.data().data() + vertex_edges.offset);
		std::vector<hknpConvexHullEdge> padded_edges(first_edge, first_edge + vertex_edges.count);
		padded_edges.resize(std::max(vertex_edges.count, (vertex_count + 3) & ~std::size_t{3}), padded_edges.back());

		const auto* member = find_member(this->target_types_, *this->find_target_type("hknpConvexPolytopeShape::Connectivity"), "faceLinks");
		const auto edge_type = element_type(this->target_types_, member->type);
		const auto vertex_edge_item = this->writer_.add_array(edge_type, padded_edges.data(), padded_edges.size()).item;
		return this->add_hull_connectivity(vertex_edge_item, this->convert_array(face_links, edge_type));
	}

	std::uint32_t transcoder::build_hull_connectivity(const source_range& faces, const source_range& indices, const std::size_t vertex_count)
	{
		const auto* face_list = reinterpret_cast<const hknpConvexHullFace*>(this->source_.data().data() + faces.offset);
		const auto* index_list = reinterpret_cast<const std::uint8_t*>(this->source_.data().data() + indices.offset);
		const auto get_vertex = [&](const hknpConvexHullFace& face, const unsigned int edge)
		{
			return index_list[face.firstIndex + edge % face.numIndices];
		};

		std::map<std::pair<std::uint8_t, std::uint8_t>, hknpConvexHullEdge> edges;
		for (auto f = 0u; f < faces.count; f++)
		{
			for (auto e = 0u; e < face_list[f].numIndices; e++)
			{
				edges[{get_vertex(face_list[f], e), get_vertex(face_list[f], e + 1)}] = {static_cast<unsigned short>(f), static_cast<unsigned char>(e), 0};
			}
		}

		const auto padded_count = (vertex_count + 3) & ~std::size_t{3};
		std::vector<hknpConvexHullEdge> vertex_edges(padded_count);
		std::vector<hknpConvexHullEdge> face_links(indices.count, {0xFFFF, 0xFF, 0});
		for (auto f = 0u; f < faces.count; f++)
		{
			for (auto e = 0u; e < face_list[f].numIndices; e++)
			{
				const auto start = get_vertex(face_list[f], e);
				const auto opposite = edges.find({get_vertex(face_list[f], e + 1), start});
				if (opposite != edges.end() && face_list[f].firstIndex + e < face_links.size())
				{
					face_links[face_list[f].firstIndex + e] = opposite->second;
				}

				if (start < vertex_count)
				{
					vertex_edges[start] = {static_cast<unsigned short>(f), static_cast<unsigned char>(e), 0};
				}
			}
		}

		for (auto i = vertex_count; i < padded_count; i++)
		{
			vertex_edges[i] = vertex_edges[vertex_count - 1];
		}

		const auto* member = find_member(this->target_types_, *this->find_target_type("hknpConvexPolytopeShape::Connectivity"), "faceLinks");
		const auto edge_type = element_type(this->target_types_, member->type);
		const auto vertex_edge_item = this->writer_.add_array(edge_type, vertex_edges.data(), vertex_edges.size()).item;
		const auto face_link_item = this->writer_.add_array(edge_type, face_links.data(), face_links.size()).item;
		return this->add_hull_connectivity(vertex_edge_item, face_link_item);
	}

	std::uint32_t transcoder::build_compound_points(const source_range& nodes, const std::uint32_t target_element)
	{
		const auto* node_list = reinterpret_cast<const hkcdSimdTreeNode*>(this->source_.data().data() + nodes.offset);
		if (node_list[1].isLeaf)
		{
			return 0;
		}

		const auto points = collect_points(node_list, nodes.count);
		return points.empty() ? 0 : this->writer_.add_array(target_element, points.data(), points.size() / 4).item;
	}

	std::uint32_t transcoder::convert_tile(const std::string_view data, const std::uint32_t index)
	{
		const tag::file tile(data, &source_compendium());
		transcoder converter(tile, this->target_types_, this->writer_, std::format("{} tile {}", this->asset_name_, index));
		converter.clear_root_user_data();
		return converter.convert_root();
	}

	std::uint32_t transcoder::build_world_compound(const std::uint32_t source_type, const std::size_t source_offset)
	{
		const auto shapes = this->find_source_member(source_type, source_offset, "shapes");
		const auto shape_range = this->read_range(shapes->type, shapes->offset);
		const auto shape_size = size_of(this->source_types_, shape_range.type);
		const auto indices = this->read_int_array(source_type, source_offset, "shapeIndices");
		const auto bounds = this->read_vector_array(source_type, source_offset, "minMaxes");
		const auto tri_counts = this->read_int_array(source_type, source_offset, "triCounts");
		const auto convex_counts = this->read_int_array(source_type, source_offset, "convexCounts");

		std::vector<compound_child> children;
		auto next_shape = 0u;
		for (auto i = 0u; i < indices.size() && 2 * i + 1 < bounds.size(); i++)
		{
			std::uint32_t shape = 0;
			if (indices[i] < 0)
			{
				const auto object = next_shape < shape_range.count ? this->read_pointer(shape_range.type, shape_range.offset + next_shape++ * shape_size) : std::nullopt;
				shape = object ? this->convert_object(*object) : 0;
			}
			else if (const auto tile = this->tiles_->find(static_cast<std::uint32_t>(indices[i])); tile != this->tiles_->end())
			{
				shape = this->convert_tile(tile->second, tile->first);
			}

			if (!shape)
			{
				ZONETOOL_WARNING("havok data \"%s\": world shape %u (index %d) is not converted", this->asset_name_.data(), i, indices[i]);
				continue;
			}

			auto& child = children.emplace_back(compound_child{shape});
			std::memcpy(child.min, bounds[2 * i].data(), sizeof(child.min));
			std::memcpy(child.max, bounds[2 * i + 1].data(), sizeof(child.max));
		}

		if (children.empty())
		{
			return 0;
		}

		const auto estimated_shape_keys = std::accumulate(tri_counts.begin(), tri_counts.end(), 0) + std::accumulate(convex_counts.begin(), convex_counts.end(), 0);
		return this->add_compound(children, estimated_shape_keys);
	}

	std::uint32_t transcoder::add_compound(const std::vector<compound_child>& children, const std::int32_t estimated_shape_keys)
	{
		const auto type = *this->find_target_type("hknpCompoundShape");
		const auto offset_of = [&](const std::initializer_list<std::string_view> path) { return this->locate_target_member(type, path).offset; };

		std::vector<std::uint32_t> items(children.size());
		std::iota(items.begin(), items.end(), 0u);
		vec3_t min, max;
		get_bounds(children, items, min, max);
		const auto radius = 0.5f * std::sqrt((max[0] - min[0]) * (max[0] - min[0]) + (max[1] - min[1]) * (max[1] - min[1]) + (max[2] - min[2]) * (max[2] - min[2]));

		std::vector<std::byte> bytes(size_of(this->target_types_, type), std::byte{0});
		put(bytes, offset_of({"flags"}), static_cast<std::uint16_t>(4)); // IS_COMPOSITE
		put(bytes, offset_of({"type"}), static_cast<std::uint8_t>(0x0C)); // COMPOUND
		put(bytes, offset_of({"numShapeKeyBits"}), static_cast<std::uint8_t>(std::bit_width(children.size())));
		put(bytes, offset_of({"dispatchType"}), static_cast<std::uint8_t>(3)); // COMPOSITE
		put(bytes, offset_of({"shapeTagCodecInfo"}), 0xFFFFFFFFu);
		put(bytes, offset_of({"instances", "firstFree"}), -1);
		put(bytes, offset_of({"instances", "numAllocated"}), static_cast<std::uint32_t>(children.size()));
		put(bytes, offset_of({"aabb", "min"}), std::array{min[0], min[1], min[2], 0.0f});
		put(bytes, offset_of({"aabb", "max"}), std::array{max[0], max[1], max[2], 0.0f});
		put(bytes, offset_of({"boundingRadius"}), radius);
		put(bytes, offset_of({"estimatedNumShapeKeys"}), estimated_shape_keys);
		put(bytes, offset_of({"boundingVolumeData", "simdTree", "isCompact"}), true);
		put(bytes, offset_of({"boundingVolumeData", "type"}), static_cast<std::uint8_t>(2)); // SIMD_TREE
		const auto reference = this->writer_.add_object(type, bytes.data());

		std::vector<target::hknpShapeInstance> instances(children.size());
		for (auto i = 0u; i < instances.size(); i++)
		{
			auto& instance = instances[i];
			instance.transform[0][0] = 1.0f;
			instance.transform[0][3] = std::bit_cast<float>(0x3F000040u);
			instance.transform[1][1] = 1.0f;
			instance.transform[2][2] = 1.0f;
			instance.transform[3][3] = std::bit_cast<float>(0x3F000000u | i);
			std::fill(std::begin(instance.scale), std::end(instance.scale), 1.0f);
			instance.shapeTag = 0xFFFF;
			instance.destructionTag = 0xFFFF;
		}

		const auto elements = this->locate_target_member(type, {"instances", "elements"});
		const auto element = element_type(this->target_types_, elements.type);
		const auto* shape = find_member(this->target_types_, element, "shape");
		const auto instance_list = this->writer_.add_array(element, instances.data(), instances.size());
		for (auto i = 0u; i < children.size(); i++)
		{
			this->writer_.set_pointer(instance_list.offset + i * sizeof(target::hknpShapeInstance) + shape->offset, shape->type, children[i].shape);
		}
		this->writer_.set_pointer(reference.offset + elements.offset, elements.type, instance_list.item);

		std::vector<target::hkcdSimdTreeNode> nodes{make_tree_node(false), make_tree_node(false)};
		build_tree_node(nodes, 1, children, items);
		const auto node_member = this->locate_target_member(type, {"boundingVolumeData", "simdTree", "nodes"});
		const auto node_list = this->writer_.add_array(element_type(this->target_types_, node_member.type), nodes.data(), nodes.size());
		this->writer_.set_pointer(reference.offset + node_member.offset, node_member.type, node_list.item);

		const auto points = collect_points(nodes.data(), nodes.size());
		const auto point_member = this->locate_target_member(type, {"boundingVolumeData", "simdTree", "points"});
		const auto point_list = this->writer_.add_array(element_type(this->target_types_, point_member.type), points.data(), points.size() / 4);
		this->writer_.set_pointer(reference.offset + point_member.offset, point_member.type, point_list.item);
		return reference.item;
	}

	std::size_t transcoder::get_array_index(const std::size_t offset) const
	{
		for (const auto& item : this->source_.items())
		{
			const auto size = item.type ? static_cast<std::size_t>(size_of(this->source_types_, item.type)) : 0;
			if (size && offset >= item.offset && offset < item.offset + size * item.count)
			{
				return (offset - item.offset) / size;
			}
		}

		return 0;
	}

	void transcoder::fill_instance_transform(const std::size_t source_offset, std::vector<std::byte>& out, const std::size_t base) const
	{
		const auto& instance = *reinterpret_cast<const hknpShapeInstance*>(this->source_.data().data() + source_offset);
		const auto x = instance.rotation[0];
		const auto y = instance.rotation[1];
		const auto z = instance.rotation[2];
		const auto w = instance.rotation[3];
		const auto leaf_index = instance.leafIndex == 0xFFFF ? this->get_array_index(source_offset) : instance.leafIndex;

		const float transform[4][4]
		{
			{1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y + z * w), 2.0f * (x * z - y * w), std::bit_cast<float>(0x3F000000u | instance.flags)},
			{2.0f * (x * y - z * w), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z + x * w), 0.0f},
			{2.0f * (x * z + y * w), 2.0f * (y * z - x * w), 1.0f - 2.0f * (x * x + y * y), 0.0f},
			{instance.translation[0], instance.translation[1], instance.translation[2], std::bit_cast<float>(0x3F000000u | static_cast<std::uint32_t>(leaf_index))},
		};
		std::memcpy(out.data() + base, transform, sizeof(transform));
	}

	void transcoder::apply(const std::size_t base, const std::vector<pointer_fixup>& fixups)
	{
		for (const auto& fixup : fixups)
		{
			if (const auto item = fixup.make())
			{
				this->writer_.set_pointer(base + fixup.offset, fixup.type, item);
			}
		}
	}

	namespace
	{
		std::optional<std::string> convert_tagfile(const std::string_view data, const std::string_view asset_name, const bool inline_types,
			const tile_shape_map* tiles)
		{
			try
			{
				const auto& types = target_compendium().types();
				tag::writer writer(types);
				if (!inline_types)
				{
					std::vector<std::uint32_t> type_map(types.size());
					std::iota(type_map.begin(), type_map.end(), 0u);
					writer.use_compendium(0xC16B2E49324C3540, std::move(type_map)); // physicsglobaltypecompendium
				}

				const tag::file source(data, &source_compendium());
				transcoder converter(source, types, writer, asset_name);
				if (tiles)
				{
					converter.merge_tiles(*tiles);
				}

				const auto root = converter.convert_root();
				if (!root)
				{
					return {};
				}

				return writer.finish(root);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_WARNING("havok data \"%s\" is not converted: %s", asset_name.data(), e.what());
				return {};
			}
		}
	}

	std::optional<std::string> convert_physics_data(const std::string_view data, const std::string_view asset_name)
	{
		return convert_tagfile(data, asset_name, false, nullptr);
	}

	std::optional<std::string> convert_shape_list(const std::string_view data, const std::string_view asset_name)
	{
		return convert_tagfile(data, asset_name, true, nullptr);
	}

	std::optional<std::string> convert_world_shape_list(const std::string_view data, const tile_shape_map& tiles, const std::string_view asset_name)
	{
		return convert_tagfile(data, asset_name, true, &tiles);
	}
}
