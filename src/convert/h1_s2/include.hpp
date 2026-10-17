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

	inline void* name_stub(const char* name, utils::memory::allocator& allocator)
	{
		const auto stub = allocator.allocate<const char*>();
		*stub = name;
		return stub;
	}
}
