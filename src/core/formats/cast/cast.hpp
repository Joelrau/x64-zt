/*
	shamelessly copied from https://github.com/dtzxporter/cast
*/

#pragma once

namespace zonetool::cast
{
	constexpr std::uint32_t cast_magic = 0x74736163;
	constexpr std::uint32_t cast_max_version = 1;

	enum node_id : std::uint32_t
	{
		id_root = 0x746F6F72,
		id_model = 0x6C646F6D,
		id_mesh = 0x6873656D,
		id_hair = 0x72696168,
		id_blend_shape = 0x68736C62,
		id_skeleton = 0x6C656B73,
		id_bone = 0x656E6F62,
		id_ik_handle = 0x64686B69,
		id_constraint = 0x74736E63,
		id_animation = 0x6D696E61,
		id_curve = 0x76727563,
		id_curve_mode_override = 0x564F4D43,
		id_notification_track = 0x6669746E,
		id_material = 0x6C74616D,
		id_file = 0x656C6966,
		id_color = 0x726C6F63,
		id_instance = 0x74736E69,
		id_metadata = 0x6174656D,
	};

	enum class property_type : std::uint16_t
	{
		unknown = 0,
		byte_ = 'b',
		short_ = 'h',
		integer32 = 'i',
		integer64 = 'l',
		float_ = 'f',
		double_ = 'd',
		string_ = 's',
		vector2,
		vector3,
		vector4,
	};

	class cast_error : public std::runtime_error
	{
	public:
		explicit cast_error(const std::string& message)
			: std::runtime_error(message)
		{
		}
	};

	class property
	{
	public:
		std::string name;
		property_type type = property_type::unknown;
		std::size_t element_count = 0;
		std::vector<std::uint8_t> data;
		std::string string_value;

		[[nodiscard]] std::size_t components() const;
		[[nodiscard]] std::size_t value_count() const;
		[[nodiscard]] bool is_integer() const;
		[[nodiscard]] bool is_float() const;

		[[nodiscard]] float as_float(std::size_t index) const;
		[[nodiscard]] std::uint64_t as_integer(std::size_t index) const;
	};

	class node
	{
	public:
		std::uint32_t id = 0;
		std::uint64_t hash = 0;
		const node* parent = nullptr;

		std::vector<std::unique_ptr<node>> children;
		std::unordered_map<std::string, property> properties;

		[[nodiscard]] const property* get_property(const std::string& name) const;
		[[nodiscard]] const property* get_property(const std::string& name, property_type type) const;

		[[nodiscard]] std::string get_string(const std::string& name, const std::string& fallback = {}) const;
		[[nodiscard]] float get_float(const std::string& name, float fallback = 0.0f) const;
		[[nodiscard]] std::uint64_t get_integer(const std::string& name, std::uint64_t fallback = 0) const;
		[[nodiscard]] bool get_bool(const std::string& name, bool fallback = false) const;
		[[nodiscard]] bool get_vec3(const std::string& name, float* out) const;
		[[nodiscard]] bool get_vec4(const std::string& name, float* out) const;

		[[nodiscard]] std::vector<const node*> children_of_type(std::uint32_t type) const;
		[[nodiscard]] const node* first_child_of_type(std::uint32_t type) const;
	};

	class file
	{
	public:
		std::uint32_t version = 0;
		std::vector<std::unique_ptr<node>> roots;
		std::unordered_map<std::uint64_t, const node*> nodes_by_hash;

		[[nodiscard]] const node* node_by_hash(std::uint64_t hash) const;
		[[nodiscard]] std::vector<const node*> all_of_type(std::uint32_t type) const;
	};

	std::unique_ptr<file> parse(const std::uint8_t* data, std::size_t size, const std::string& display_name);

	std::unique_ptr<file> load(const std::string& path);

	void set_import_scale(float scale);
	float get_import_scale();
	float parse_import_scale(const std::string& value);

	void set_import_root(const std::string& bone);
	const std::string& get_import_root();

	void add_ignored_bone(const std::string& bone);
	void clear_ignored_bones();
	bool is_bone_ignored(const std::string& bone);

	void reset_import_settings();
}
