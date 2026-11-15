#pragma once

#include <iw7/iw7.hpp>
#include <iw8/iw8.hpp>

namespace zonetool::iw7::converter::iw8
{
	void add_csv_row(std::string_view type, std::string_view name);
	bool mark_dumped(XAssetType type, std::string_view name);
	const std::filesystem::path& iw8_folder();
}
