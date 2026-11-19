#include <std_include.hpp>

#include "cast_writer.hpp"
#include "../../zonetool.hpp"

namespace zonetool::cast
{
	namespace
	{
		constexpr std::size_t file_header_size = 0x10;
		constexpr std::size_t node_header_size = 0x18;
		constexpr std::size_t property_header_size = 0x08;

		template <typename T> void append(std::vector<std::uint8_t>& out, const T& value)
		{
			const auto* bytes = reinterpret_cast<const std::uint8_t*>(&value);
			out.insert(out.end(), bytes, bytes + sizeof(T));
		}
	}

	writer_node::writer_node(const node_id id, const std::uint64_t hash, std::uint64_t* counter)
		: id_(id)
		, hash_(hash)
		, counter_(counter)
	{
	}

	writer_node& writer_node::add_child(const node_id id)
	{
		const auto hash = this->counter_ ? (*this->counter_)++ : 0;
		this->children_.emplace_back(std::make_unique<writer_node>(id, hash, this->counter_));
		return *this->children_.back();
	}

	void writer_node::add_property(const char* identifier, const std::string& name,
		const std::uint32_t element_count, const void* data, const std::size_t size)
	{
		property_data prop{};
		prop.identifier[0] = identifier[0];
		prop.identifier[1] = identifier[1];
		prop.name = name;
		prop.element_count = element_count;
		prop.data.resize(size);
		if (size)
		{
			std::memcpy(prop.data.data(), data, size);
		}

		this->properties_.emplace_back(std::move(prop));
	}

	void writer_node::add_string(const std::string& name, const std::string& value)
	{
		std::vector<std::uint8_t> bytes(value.begin(), value.end());
		bytes.emplace_back(0);
		this->add_property("s\0", name, 1, bytes.data(), bytes.size());
	}

	void writer_node::add_byte(const std::string& name, const std::uint8_t value)
	{
		this->add_property("b\0", name, 1, &value, sizeof(value));
	}

	void writer_node::add_short(const std::string& name, const std::uint16_t value)
	{
		this->add_property("h\0", name, 1, &value, sizeof(value));
	}

	void writer_node::add_integer32(const std::string& name, const std::uint32_t value)
	{
		this->add_property("i\0", name, 1, &value, sizeof(value));
	}

	void writer_node::add_integer64(const std::string& name, const std::uint64_t value)
	{
		this->add_property("l\0", name, 1, &value, sizeof(value));
	}

	void writer_node::add_float(const std::string& name, const float value)
	{
		this->add_property("f\0", name, 1, &value, sizeof(value));
	}

	void writer_node::add_integer_array(const std::string& name, const std::vector<std::uint32_t>& values)
	{
		std::uint32_t largest = 0;
		for (const auto value : values)
		{
			largest = std::max(largest, value);
		}

		const auto count = static_cast<std::uint32_t>(values.size());

		if (largest <= 0xFF)
		{
			std::vector<std::uint8_t> packed(values.begin(), values.end());
			this->add_property("b\0", name, count, packed.data(), packed.size());
		}
		else if (largest <= 0xFFFF)
		{
			std::vector<std::uint16_t> packed(values.begin(), values.end());
			this->add_property("h\0", name, count, packed.data(), packed.size() * sizeof(std::uint16_t));
		}
		else
		{
			this->add_property("i\0", name, count, values.data(), values.size() * sizeof(std::uint32_t));
		}
	}

	void writer_node::add_float_array(const std::string& name, const std::vector<float>& values)
	{
		this->add_property("f\0", name, static_cast<std::uint32_t>(values.size()),
			values.data(), values.size() * sizeof(float));
	}

	void writer_node::add_vector_array(const char* identifier, const std::string& name,
		const std::vector<float>& values, const std::size_t components)
	{
		const auto count = static_cast<std::uint32_t>(values.size() / components);
		this->add_property(identifier, name, count, values.data(), values.size() * sizeof(float));
	}

	void writer_node::add_vector2_array(const std::string& name, const std::vector<float>& values)
	{
		this->add_vector_array("2v", name, values, 2);
	}

	void writer_node::add_vector3_array(const std::string& name, const std::vector<float>& values)
	{
		this->add_vector_array("3v", name, values, 3);
	}

	void writer_node::add_vector4_array(const std::string& name, const std::vector<float>& values)
	{
		this->add_vector_array("4v", name, values, 4);
	}

	std::size_t writer_node::size() const
	{
		auto result = node_header_size;

		for (const auto& prop : this->properties_)
		{
			result += property_header_size + prop.name.size() + prop.data.size();
		}

		for (const auto& child : this->children_)
		{
			result += child->size();
		}

		return result;
	}

	void writer_node::serialize(std::vector<std::uint8_t>& out) const
	{
		append(out, static_cast<std::uint32_t>(this->id_));
		append(out, static_cast<std::uint32_t>(this->size()));
		append(out, this->hash_);
		append(out, static_cast<std::uint32_t>(this->properties_.size()));
		append(out, static_cast<std::uint32_t>(this->children_.size()));

		for (const auto& prop : this->properties_)
		{
			out.emplace_back(static_cast<std::uint8_t>(prop.identifier[0]));
			out.emplace_back(static_cast<std::uint8_t>(prop.identifier[1]));
			append(out, static_cast<std::uint16_t>(prop.name.size()));
			append(out, prop.element_count);

			out.insert(out.end(), prop.name.begin(), prop.name.end());
			out.insert(out.end(), prop.data.begin(), prop.data.end());
		}

		for (const auto& child : this->children_)
		{
			child->serialize(out);
		}
	}

	writer::writer()
	{
		this->root_ = std::make_unique<writer_node>(id_root, 0, &this->counter_);
	}

	std::vector<std::uint8_t> writer::serialize() const
	{
		std::vector<std::uint8_t> out;
		out.reserve(file_header_size + this->root_->size());

		append(out, static_cast<std::uint32_t>(cast_magic));
		append(out, static_cast<std::uint32_t>(cast_max_version));
		append(out, static_cast<std::uint32_t>(1));
		append(out, static_cast<std::uint32_t>(0));

		this->root_->serialize(out);
		return out;
	}

	bool writer::write(const std::string& path) const
	{
		const auto bytes = this->serialize();

		filesystem::file output(path);
		output.open("wb");
		if (!output.get_fp())
		{
			return false;
		}

		output.write(bytes.data(), bytes.size(), 1);
		output.close();
		return true;
	}
}
