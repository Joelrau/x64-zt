#pragma once

#include <t9/t9.hpp>
#include <iw8/iw8.hpp>

namespace zonetool::t9::converter::iw8
{
	void add_csv_row(std::string_view type, std::string_view name);
	bool mark_dumped(zonetool::iw8::XAssetType type, std::string_view name);
	const std::filesystem::path& iw8_folder();
}
