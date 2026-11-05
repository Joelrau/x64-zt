#pragma once

namespace zonetool::h1::converter::s2::stringtable::lightsets
{
	struct s2_row
	{
		std::string_view name;
		std::string_view h1_name;
		std::string_view fallback;
	};

	void dump(StringTable* asset);
}
