#pragma once

#include <formats/havok_tagfile.hpp>

namespace zonetool::s4::converter::iw8::havok
{
	struct source_object
	{
		std::uint32_t type;
		std::size_t offset;
	};

	struct source_range
	{
		std::uint32_t type;
		std::size_t offset;
		std::size_t count;
	};

	struct pointer_fixup
	{
		std::size_t offset;
		std::uint32_t type;
		std::function<std::uint32_t()> make;
	};

	struct member_location
	{
		std::size_t offset;
		std::uint32_t type;
	};

	struct compound_child
	{
		std::uint32_t shape;
		vec3_t min;
		vec3_t max;
	};

	using tile_shape_map = std::unordered_map<std::uint32_t, std::string_view>;

	class transcoder
	{
	public:
		transcoder(const ::havok::tagfile::file& source, const std::vector<::havok::tagfile::type_info>& target_types,
			::havok::tagfile::writer& writer, std::string_view asset_name);

		void merge_tiles(const tile_shape_map& tiles);
		void clear_root_user_data();
		std::uint32_t convert_root();

	private:
		template <typename T> T read(std::size_t offset) const;
		std::int64_t read_int(std::uint32_t type, std::size_t offset) const;
		std::optional<source_object> read_pointer(std::uint32_t type, std::size_t offset) const;
		source_range read_range(std::uint32_t type, std::size_t offset) const;
		std::vector<std::int32_t> read_int_array(std::uint32_t type, std::size_t offset, std::string_view name) const;
		std::vector<std::array<float, 4>> read_vector_array(std::uint32_t type, std::size_t offset, std::string_view name) const;
		std::optional<source_object> find_source_member(std::uint32_t type, std::size_t offset, std::string_view name) const;
		std::optional<std::uint32_t> find_target_type(std::string_view name) const;
		member_location locate_target_member(std::uint32_t type, std::initializer_list<std::string_view> path) const;

		void fill(std::uint32_t source_type, std::size_t source_offset, std::uint32_t target_type,
			std::vector<std::byte>& out, std::size_t base, std::vector<pointer_fixup>& fixups);
		void fill_record(std::uint32_t source_type, std::size_t source_offset, std::uint32_t target_type,
			std::vector<std::byte>& out, std::size_t base, std::vector<pointer_fixup>& fixups);
		void fill_array(std::uint32_t source_type, std::size_t source_offset, std::uint32_t target_type,
			std::vector<std::byte>& out, std::size_t base, std::vector<pointer_fixup>& fixups);
		bool fill_changed_member(std::string_view owner, const ::havok::tagfile::type_member& member, std::uint32_t source_type,
			std::size_t source_offset, std::vector<std::byte>& out, std::size_t base, std::vector<pointer_fixup>& fixups);
		bool fill_merged_shape_list(const ::havok::tagfile::type_member& member, std::uint32_t source_type, std::size_t source_offset,
			std::size_t base, std::vector<pointer_fixup>& fixups);

		std::uint32_t convert_object(const source_object& object);
		std::uint32_t convert_array(const source_range& range, std::uint32_t target_element);
		std::uint32_t convert_properties(const source_range& properties);
		std::uint32_t convert_hull_vertices(const source_range& vertices, std::uint32_t target_element);
		std::uint32_t convert_hull_connectivity(const source_range& vertex_edges, const source_range& face_links, std::size_t vertex_count);
		std::uint32_t build_hull_connectivity(const source_range& faces, const source_range& indices, std::size_t vertex_count);
		std::uint32_t add_hull_connectivity(std::uint32_t vertex_edges, std::uint32_t face_links);
		std::uint32_t build_compound_points(const source_range& nodes, std::uint32_t target_element);
		std::uint32_t convert_tile(std::string_view data, std::uint32_t index);
		std::uint32_t build_world_compound(std::uint32_t source_type, std::size_t source_offset);
		std::uint32_t add_compound(const std::vector<compound_child>& children, std::int32_t estimated_shape_keys);
		std::uint32_t add_pointer_array(std::uint32_t array_type, std::uint32_t item);
		template <typename T> std::uint32_t add_value_array(std::uint32_t array_type, const std::vector<T>& values);
		void fill_instance_transform(std::size_t source_offset, std::vector<std::byte>& out, std::size_t base) const;
		std::size_t get_array_index(std::size_t offset) const;
		void apply(std::size_t base, const std::vector<pointer_fixup>& fixups);

		const ::havok::tagfile::file& source_;
		const std::vector<::havok::tagfile::type_info>& source_types_;
		const std::vector<::havok::tagfile::type_info>& target_types_;
		::havok::tagfile::writer& writer_;
		std::string asset_name_;
		std::unordered_map<std::string, std::uint32_t> target_names_;
		std::unordered_map<std::size_t, std::uint32_t> converted_;
		const tile_shape_map* tiles_{};
		bool clear_root_user_data_{};
	};

	std::optional<std::string> convert_physics_data(std::string_view data, std::string_view asset_name);
	std::optional<std::string> convert_shape_list(std::string_view data, std::string_view asset_name);
	std::optional<std::string> convert_world_shape_list(std::string_view data, const tile_shape_map& tiles, std::string_view asset_name);
}
