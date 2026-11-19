#pragma once

#include <iw8/iw8.hpp>
#include <iw7/iw7.hpp>

namespace zonetool::iw8::converter::iw7
{
	void add_csv_row(std::string_view type, std::string_view name);
	bool mark_dumped(XAssetType type, std::string_view name);
	XModel* find_transient_model(const char* name);
	void release_transient_zones();
}
