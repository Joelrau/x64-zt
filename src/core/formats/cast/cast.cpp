#include <std_include.hpp>
#include "cast.hpp"
#include "../../zonetool.hpp"

#include <utils/string.hpp>

namespace zonetool::cast
{
	namespace
	{
		constexpr std::size_t file_header_size = 0x10;
		constexpr std::size_t node_header_size = 0x18;
		constexpr std::size_t property_header_size = 0x8;

		constexpr std::size_t max_node_depth = 32;

		std::size_t component_size(const property_type type)
		{
			switch (type)
			{
			case property_type::byte_: return 1;
			case property_type::short_: return 2;
			case property_type::integer32: return 4;
			case property_type::integer64: return 8;
			case property_type::float_: return 4;
			case property_type::double_: return 8;
			case property_type::vector2:
			case property_type::vector3:
			case property_type::vector4: return 4;
			default: return 0;
			}
		}

		class stream_reader
		{
		public:
			stream_reader(const std::uint8_t* data, const std::size_t size, std::string name)
				: data_(data)
				, size_(size)
				, name_(std::move(name))
			{
			}

			[[nodiscard]] std::size_t position() const
			{
				return this->pos_;
			}

			[[nodiscard]] std::size_t remaining() const
			{
				return this->size_ - this->pos_;
			}

			void read_raw(void* dest, const std::size_t size)
			{
				this->require(size);
				std::memcpy(dest, this->data_ + this->pos_, size);
				this->pos_ += size;
			}

			void skip(const std::size_t size)
			{
				this->require(size);
				this->pos_ += size;
			}

			template <typename T>
			T read()
			{
				T value{};
				this->read_raw(&value, sizeof(T));
				return value;
			}

			std::string read_string()
			{
				const auto start = this->pos_;
				while (this->pos_ < this->size_ && this->data_[this->pos_] != 0)
				{
					this->pos_++;
				}

				if (this->pos_ >= this->size_)
				{
					this->fail("unterminated string");
				}

				std::string result(reinterpret_cast<const char*>(this->data_ + start), this->pos_ - start);
				this->pos_++;
				return result;
			}

			std::string read_fixed_string(const std::size_t size)
			{
				this->require(size);
				std::string result(reinterpret_cast<const char*>(this->data_ + this->pos_), size);
				this->pos_ += size;
				return result;
			}

			[[noreturn]] void fail(const std::string& reason) const
			{
				throw cast_error(utils::string::va("%s: %s (at offset 0x%llX of 0x%llX)",
					this->name_.data(), reason.data(), this->pos_, this->size_));
			}

			void require(const std::size_t size) const
			{
				if (size > this->size_ || this->pos_ > this->size_ - size)
				{
					this->fail(utils::string::va("truncated file, tried to read %llu bytes", size));
				}
			}

		private:
			const std::uint8_t* data_;
			std::size_t size_;
			std::size_t pos_ = 0;
			std::string name_;
		};

		property_type parse_property_type(const std::string& identifier)
		{
			if (identifier.size() != 2)
			{
				return property_type::unknown;
			}

			if (identifier[1] == 0)
			{
				switch (identifier[0])
				{
				case 'b': return property_type::byte_;
				case 'h': return property_type::short_;
				case 'i': return property_type::integer32;
				case 'l': return property_type::integer64;
				case 'f': return property_type::float_;
				case 'd': return property_type::double_;
				case 's': return property_type::string_;
				default: break;
				}
			}
			else if (identifier[1] == 'v')
			{
				switch (identifier[0])
				{
				case '2': return property_type::vector2;
				case '3': return property_type::vector3;
				case '4': return property_type::vector4;
				default: break;
				}
			}

			return property_type::unknown;
		}

		void read_property(stream_reader& reader, node& target)
		{
			reader.require(property_header_size);

			const auto identifier = reader.read_fixed_string(2);
			const auto name_size = reader.read<std::uint16_t>();
			const auto array_length = reader.read<std::uint32_t>();

			property prop{};
			prop.name = reader.read_fixed_string(name_size);
			prop.type = parse_property_type(identifier);

			if (prop.type == property_type::unknown)
			{
				reader.fail(utils::string::va("unsupported property type 0x%02X%02X for property \"%s\"",
					static_cast<std::uint8_t>(identifier[1]), static_cast<std::uint8_t>(identifier[0]),
					prop.name.data()));
			}

			if (prop.type == property_type::string_)
			{
				prop.string_value = reader.read_string();
				prop.element_count = 1;
			}
			else
			{
				const auto element_size = component_size(prop.type) * prop.components();
				const auto total_size = static_cast<std::size_t>(array_length) * element_size;

				reader.require(total_size);

				prop.element_count = array_length;
				prop.data.resize(total_size);
				if (total_size)
				{
					reader.read_raw(prop.data.data(), total_size);
				}
			}

			target.properties.insert_or_assign(prop.name, std::move(prop));
		}

		std::unique_ptr<node> read_node(stream_reader& reader, file& result, const node* parent,
			const std::size_t depth)
		{
			if (depth > max_node_depth)
			{
				reader.fail("node hierarchy is nested too deeply");
			}

			const auto node_start = reader.position();
			reader.require(node_header_size);

			auto current = std::make_unique<node>();
			current->parent = parent;
			current->id = reader.read<std::uint32_t>();

			const auto node_size = reader.read<std::uint32_t>();
			current->hash = reader.read<std::uint64_t>();

			const auto property_count = reader.read<std::uint32_t>();
			const auto child_count = reader.read<std::uint32_t>();

			if (node_size < node_header_size)
			{
				reader.fail(utils::string::va("node declares an invalid size of %u bytes", node_size));
			}

			if (node_size - node_header_size > reader.remaining())
			{
				reader.fail("node extends past the end of the file");
			}

			const auto node_end = node_start + node_size;
			const auto node_body = node_size - node_header_size;

			if (property_count > node_body / property_header_size)
			{
				reader.fail(utils::string::va("node declares %u properties which do not fit into %llu bytes",
					property_count, node_body));
			}

			if (child_count > node_body / node_header_size)
			{
				reader.fail(utils::string::va("node declares %u children which do not fit into %llu bytes",
					child_count, node_body));
			}

			for (std::uint32_t i = 0; i < property_count; i++)
			{
				if (reader.position() >= node_end)
				{
					reader.fail("node properties extend past the end of the node");
				}
				read_property(reader, *current);
			}

			current->children.reserve(child_count);
			for (std::uint32_t i = 0; i < child_count; i++)
			{
				if (reader.position() >= node_end)
				{
					reader.fail("node children extend past the end of the node");
				}
				current->children.emplace_back(read_node(reader, result, current.get(), depth + 1));
			}

			if (reader.position() > node_end)
			{
				reader.fail("node contents extend past the end of the node");
			}

			reader.skip(node_end - reader.position());

			result.nodes_by_hash.emplace(current->hash, current.get());
			return current;
		}
	}

	std::size_t property::components() const
	{
		switch (this->type)
		{
		case property_type::vector2: return 2;
		case property_type::vector3: return 3;
		case property_type::vector4: return 4;
		case property_type::unknown: return 0;
		default: return 1;
		}
	}

	std::size_t property::value_count() const
	{
		return this->element_count * this->components();
	}

	bool property::is_integer() const
	{
		switch (this->type)
		{
		case property_type::byte_:
		case property_type::short_:
		case property_type::integer32:
		case property_type::integer64:
			return true;
		default:
			return false;
		}
	}

	bool property::is_float() const
	{
		switch (this->type)
		{
		case property_type::float_:
		case property_type::double_:
		case property_type::vector2:
		case property_type::vector3:
		case property_type::vector4:
			return true;
		default:
			return false;
		}
	}

	float property::as_float(const std::size_t index) const
	{
		if (index >= this->value_count())
		{
			throw cast_error(utils::string::va("property \"%s\": value %llu is out of range",
				this->name.data(), index));
		}

		if (this->data.empty())
		{
			return 0.0f;
		}

		const auto* base = this->data.data() + index * component_size(this->type);
		switch (this->type)
		{
		case property_type::float_:
		case property_type::vector2:
		case property_type::vector3:
		case property_type::vector4:
		{
			float value{};
			std::memcpy(&value, base, sizeof(value));
			return value;
		}
		case property_type::double_:
		{
			double value{};
			std::memcpy(&value, base, sizeof(value));
			return static_cast<float>(value);
		}
		default:
			return static_cast<float>(this->as_integer(index));
		}
	}

	std::uint64_t property::as_integer(const std::size_t index) const
	{
		if (index >= this->value_count())
		{
			throw cast_error(utils::string::va("property \"%s\": value %llu is out of range",
				this->name.data(), index));
		}

		if (this->data.empty())
		{
			return 0;
		}

		const auto* base = this->data.data() + index * component_size(this->type);
		switch (this->type)
		{
		case property_type::byte_:
			return *base;
		case property_type::short_:
		{
			std::uint16_t value{};
			std::memcpy(&value, base, sizeof(value));
			return value;
		}
		case property_type::integer32:
		{
			std::uint32_t value{};
			std::memcpy(&value, base, sizeof(value));
			return value;
		}
		case property_type::integer64:
		{
			std::uint64_t value{};
			std::memcpy(&value, base, sizeof(value));
			return value;
		}
		case property_type::float_:
		case property_type::double_:
		case property_type::vector2:
		case property_type::vector3:
		case property_type::vector4:
			return static_cast<std::uint64_t>(this->as_float(index));
		default:
			return 0;
		}
	}

	const property* node::get_property(const std::string& name) const
	{
		const auto itr = this->properties.find(name);
		return itr == this->properties.end() ? nullptr : &itr->second;
	}

	const property* node::get_property(const std::string& name, const property_type type) const
	{
		const auto* prop = this->get_property(name);
		return (prop && prop->type == type) ? prop : nullptr;
	}

	std::string node::get_string(const std::string& name, const std::string& fallback) const
	{
		const auto* prop = this->get_property(name, property_type::string_);
		return prop ? prop->string_value : fallback;
	}

	float node::get_float(const std::string& name, const float fallback) const
	{
		const auto* prop = this->get_property(name);
		if (!prop || prop->value_count() < 1)
		{
			return fallback;
		}
		return prop->as_float(0);
	}

	std::uint64_t node::get_integer(const std::string& name, const std::uint64_t fallback) const
	{
		const auto* prop = this->get_property(name);
		if (!prop || prop->value_count() < 1)
		{
			return fallback;
		}
		return prop->as_integer(0);
	}

	bool node::get_bool(const std::string& name, const bool fallback) const
	{
		const auto* prop = this->get_property(name);
		if (!prop || prop->value_count() < 1)
		{
			return fallback;
		}
		return prop->as_integer(0) >= 1;
	}

	bool node::get_vec3(const std::string& name, float* out) const
	{
		const auto* prop = this->get_property(name);
		if (!prop || prop->value_count() < 3)
		{
			return false;
		}

		out[0] = prop->as_float(0);
		out[1] = prop->as_float(1);
		out[2] = prop->as_float(2);
		return true;
	}

	bool node::get_vec4(const std::string& name, float* out) const
	{
		const auto* prop = this->get_property(name);
		if (!prop || prop->value_count() < 4)
		{
			return false;
		}

		out[0] = prop->as_float(0);
		out[1] = prop->as_float(1);
		out[2] = prop->as_float(2);
		out[3] = prop->as_float(3);
		return true;
	}

	std::vector<const node*> node::children_of_type(const std::uint32_t type) const
	{
		std::vector<const node*> result;
		for (const auto& child : this->children)
		{
			if (child->id == type)
			{
				result.emplace_back(child.get());
			}
		}
		return result;
	}

	const node* node::first_child_of_type(const std::uint32_t type) const
	{
		for (const auto& child : this->children)
		{
			if (child->id == type)
			{
				return child.get();
			}
		}
		return nullptr;
	}

	const node* file::node_by_hash(const std::uint64_t hash) const
	{
		const auto itr = this->nodes_by_hash.find(hash);
		return itr == this->nodes_by_hash.end() ? nullptr : itr->second;
	}

	std::vector<const node*> file::all_of_type(const std::uint32_t type) const
	{
		std::vector<const node*> result;

		const std::function<void(const node*)> walk = [&](const node* current)
		{
			if (current->id == type)
			{
				result.emplace_back(current);
			}
			for (const auto& child : current->children)
			{
				walk(child.get());
			}
		};

		for (const auto& root : this->roots)
		{
			walk(root.get());
		}

		return result;
	}

	namespace
	{
		float import_scale = 1.0f;
		std::string import_root;

		std::unordered_set<std::string>& ignored_bones()
		{
			static std::unordered_set<std::string> bones;
			return bones;
		}
	}

	void add_ignored_bone(const std::string& bone)
	{
		ignored_bones().emplace(utils::string::to_lower(bone));
	}

	void clear_ignored_bones()
	{
		ignored_bones().clear();
	}

	bool is_bone_ignored(const std::string& bone)
	{
		const auto& bones = ignored_bones();
		return bones.find(utils::string::to_lower(bone)) != bones.end();
	}

	void set_import_root(const std::string& bone)
	{
		import_root = bone;
	}

	const std::string& get_import_root()
	{
		return import_root;
	}

	float parse_import_scale(const std::string& value)
	{
		if (value == "inch" || value == "in") return 1.0f;
		if (value == "cm") return 1.0f / 2.54f;
		if (value == "mm") return 1.0f / 25.4f;
		if (value == "m") return 1000.0f / 25.4f;

		const auto scale = static_cast<float>(std::atof(value.data()));
		return scale > 0.0f ? scale : 0.0f;
	}

	void set_import_scale(const float scale)
	{
		import_scale = scale;
	}

	float get_import_scale()
	{
		return import_scale;
	}

	void reset_import_settings()
	{
		import_scale = 1.0f;
		import_root.clear();
		clear_ignored_bones();
	}

	std::unique_ptr<file> parse(const std::uint8_t* data, const std::size_t size, const std::string& display_name)
	{
		stream_reader reader(data, size, display_name);
		reader.require(file_header_size);

		const auto magic = reader.read<std::uint32_t>();
		if (magic != cast_magic)
		{
			reader.fail(utils::string::va("not a cast file (magic 0x%08X, expected 0x%08X)", magic, cast_magic));
		}

		auto result = std::make_unique<file>();
		result->version = reader.read<std::uint32_t>();
		if (result->version > cast_max_version)
		{
			reader.fail(utils::string::va("unsupported cast version %u (this build supports up to version %u)",
				result->version, cast_max_version));
		}

		const auto root_count = reader.read<std::uint32_t>();
		reader.read<std::uint32_t>();

		result->roots.reserve(root_count);
		for (std::uint32_t i = 0; i < root_count; i++)
		{
			result->roots.emplace_back(read_node(reader, *result, nullptr, 0));
		}

		return result;
	}

	std::unique_ptr<file> load(const std::string& path)
	{
		filesystem::file source(path);
		source.open("rb");
		if (!source.get_fp())
		{
			return nullptr;
		}

		const auto size = source.size();
		auto bytes = source.read_bytes(size);
		source.close();

		if (bytes.size() < file_header_size)
		{
			throw cast_error(utils::string::va("%s: file is too small to be a cast file", path.data()));
		}

		return parse(bytes.data(), bytes.size(), path);
	}
}
