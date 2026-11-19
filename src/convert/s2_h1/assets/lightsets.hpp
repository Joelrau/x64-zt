#pragma once

namespace zonetool::s2::converter::h1::lightsets
{
	struct h1_row
	{
		std::string_view name;
		std::string_view value;
	};

	void dump(zonetool::s2::StringTable* asset);
}
