#pragma once

#include "source_include.hpp"

#include <s2/s2.hpp>

namespace zonetool::h1::converter::s2
{
	void add_csv_row(std::string_view type, std::string_view name, bool referenced = false);

	const std::filesystem::path& s2_folder();
	void set_s2_folder(const std::filesystem::path& folder);
	void finish(const std::string& zone_name);

	std::uint32_t convert_surface_type(std::uint32_t h1_type);
	std::uint32_t convert_surface_flags(std::uint32_t h1_flags);
	std::uint64_t convert_surface_type_bits(std::uint64_t h1_bits);

	template <typename T>
	T* make_stub(const char* name, utils::memory::allocator& allocator)
	{
		const auto stub = allocator.allocate<T>();
		stub->name = name;
		return stub;
	}

	template <typename T>
	T* copy_array(const T* data, const std::size_t count, utils::memory::allocator& allocator)
	{
		if (!data || !count)
		{
			return nullptr;
		}

		const auto result = allocator.allocate_array<T>(count);
		std::memcpy(result, data, sizeof(T) * count);
		return result;
	}

	// reinterprets an h1 array as the s2 struct of the same layout
	template <typename To, typename From>
	To* retype(From* data)
	{
		static_assert(sizeof(To) == sizeof(From));
		return reinterpret_cast<To*>(data);
	}

	// the S2 writer reserves a runtime array only when its pointer is set
	template <typename T>
	void mark_runtime(T*& buffer, utils::memory::allocator& allocator)
	{
		buffer = allocator.allocate<T>();
	}

	template <typename T, std::size_t N>
	void mark_runtime(T* (&buffers)[N], utils::memory::allocator& allocator)
	{
		for (auto& buffer : buffers)
		{
			mark_runtime(buffer, allocator);
		}
	}

	// marks the runtime array only when it has elements
	template <typename T>
	void reserve_runtime(T*& buffer, const unsigned int count, utils::memory::allocator& allocator)
	{
		if (count && !buffer)
		{
			buffer = allocator.allocate<T>();
		}
	}

	// h1 keeps the binormal sign in the sign of binormalSign, s2 sets the top two tangent bits when the binormal is -cross(normal, tangent)
	inline std::uint32_t pack_tangent(const std::uint32_t tangent, const float binormal_sign)
	{
		return (tangent & 0x3FFFFFFFu) | (binormal_sign < 0.0f ? 0xC0000000u : 0u);
	}

	inline const char* table_cell(const StringTable* table, const int row, const int column)
	{
		const auto value = table->values[row * table->columnCount + column].string;
		return value ? value : "";
	}

	inline void write_file(const std::string& name, const std::string_view data)
	{
		filesystem::file file(name);
		file.open("wb");
		file.write(data.data(), data.size(), 1);
		file.close();
	}
}
