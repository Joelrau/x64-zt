#pragma once

namespace havok::tagfile
{
	enum type_kind : std::uint32_t
	{
		kind_void = 0,
		kind_opaque = 1,
		kind_bool = 2,
		kind_string = 3,
		kind_int = 4,
		kind_float = 5,
		kind_pointer = 6,
		kind_record = 7,
		kind_array = 8,
	};

	enum type_format : std::uint32_t
	{
		format_kind_mask = 0x1F,
		format_fixed_size = 0x20,
		format_signed = 0x200,
		format_int8 = 0x2000,
		format_int16 = 0x4000,
		format_int32 = 0x8000,
		format_int64 = 0x10000,
	};

	enum type_body_flags : std::uint32_t
	{
		body_format = 0x1,
		body_subtype = 0x2,
		body_version = 0x4,
		body_size = 0x8,
		body_type_flags = 0x10,
		body_members = 0x20,
		body_interfaces = 0x40,
		body_attribute = 0x80,
	};

	enum item_flags : std::uint32_t
	{
		item_object = 0x10,
		item_array = 0x20,
	};

	struct type_member
	{
		std::string name;
		std::uint32_t flags;
		std::uint32_t offset;
		std::uint32_t type;
	};

	struct type_info
	{
		std::string name;
		std::vector<std::pair<std::string, std::uint32_t>> templates;
		std::uint32_t parent;
		std::uint32_t format;
		std::uint32_t subtype;
		std::uint32_t version;
		std::uint32_t size;
		std::uint32_t alignment;
		std::uint32_t body_flags;
		std::uint32_t type_flags;
		std::uint32_t raw_member_count;
		std::uint32_t hash;
		bool has_hash;
		std::vector<type_member> members;
	};

	struct item
	{
		std::uint32_t type;
		std::uint32_t flags;
		std::uint32_t offset;
		std::uint32_t count;
	};

	class file;

	class value
	{
	public:
		value() = default;
		value(const file* owner, std::uint32_t type, std::size_t offset);

		bool is_null() const;
		std::string_view type_name() const;

		value operator[](std::string_view member) const;
		value operator[](std::size_t index) const;
		std::size_t size() const;
		value deref() const;
		std::string_view string() const;

		std::int64_t as_int() const;
		double as_float() const;
		bool as_bool() const;

	private:
		struct element_range
		{
			std::uint32_t type;
			std::size_t offset;
			std::size_t count;
		};

		element_range elements() const;

		const file* owner_{};
		std::uint32_t type_{};
		std::size_t offset_{};
	};

	class file
	{
	public:
		explicit file(std::string_view data);

		value root() const;

		const type_info& type(std::uint32_t index) const;
		const type_info& resolve(std::uint32_t index) const;
		std::uint32_t type_size(std::uint32_t index) const;
		const type_member* find_member(std::uint32_t type, std::string_view name) const;
		std::optional<std::uint32_t> find_template(std::uint32_t type, std::string_view name) const;
		const item& get_item(std::uint64_t index) const;
		std::string_view data() const;

	private:
		std::vector<type_info> types_;
		std::vector<item> items_;
		std::string_view data_;
	};

	class writer
	{
	public:
		struct reference
		{
			std::uint32_t item;
			std::uint32_t offset;
		};

		explicit writer(std::vector<type_info> types);

		std::uint32_t type_id(std::string_view name) const;
		std::uint32_t type_size(std::uint32_t type) const;

		reference add_object(std::uint32_t type, const void* data);
		reference add_array(std::uint32_t element_type, const void* data, std::size_t count);
		void set_pointer(std::size_t field_offset, std::uint32_t field_type, std::size_t item);
		std::string finish(std::uint32_t root_item);

	private:
		std::vector<type_info> types_;
		std::vector<item> items_;
		std::string data_;
		std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> patches_;
	};
}
