#pragma once

#include "cast.hpp"

namespace zonetool::cast
{
	class writer_node
	{
	public:
		writer_node(node_id id, std::uint64_t hash, std::uint64_t* counter);

		writer_node& add_child(node_id id);
		[[nodiscard]] std::uint64_t hash() const { return this->hash_; }

		void add_string(const std::string& name, const std::string& value);
		void add_byte(const std::string& name, std::uint8_t value);
		void add_short(const std::string& name, std::uint16_t value);
		void add_integer32(const std::string& name, std::uint32_t value);
		void add_integer64(const std::string& name, std::uint64_t value);
		void add_float(const std::string& name, float value);

		void add_integer_array(const std::string& name, const std::vector<std::uint32_t>& values);

		void add_float_array(const std::string& name, const std::vector<float>& values);

		void add_vector2_array(const std::string& name, const std::vector<float>& values);
		void add_vector3_array(const std::string& name, const std::vector<float>& values);
		void add_vector4_array(const std::string& name, const std::vector<float>& values);

		[[nodiscard]] std::size_t size() const;
		void serialize(std::vector<std::uint8_t>& out) const;

	private:
		struct property_data
		{
			char identifier[2]{};
			std::string name;
			std::uint32_t element_count = 0;
			std::vector<std::uint8_t> data;
		};

		void add_property(const char* identifier, const std::string& name, std::uint32_t element_count,
			const void* data, std::size_t size);
		void add_vector_array(const char* identifier, const std::string& name,
			const std::vector<float>& values, std::size_t components);

		node_id id_;
		std::uint64_t hash_;
		std::uint64_t* counter_;
		std::vector<property_data> properties_;
		std::vector<std::unique_ptr<writer_node>> children_;
	};

	class writer
	{
	public:
		writer();

		writer_node& root() { return *this->root_; }

		[[nodiscard]] std::vector<std::uint8_t> serialize() const;

		bool write(const std::string& path) const;

	private:
		std::uint64_t counter_ = 1;
		std::unique_ptr<writer_node> root_;
	};
}
