#pragma once

#include "source_include.hpp"

#include <h1/h1.hpp>

namespace zonetool::s2::converter::h1
{
	void add_csv_row(std::string_view type, std::string_view name, bool referenced = false);

	std::uint32_t convert_surface_type(std::uint32_t s2_type);
	std::uint32_t convert_surface_flags(std::uint32_t s2_flags);

	inline void write_file(const std::string& name, const std::string_view data)
	{
		filesystem::file file(name);
		file.open("wb");
		file.write(data.data(), data.size(), 1);
		file.close();
	}
}
