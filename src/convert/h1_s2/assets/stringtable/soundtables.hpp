#pragma once

namespace zonetool::h1::converter::s2::stringtable::soundtables
{
	struct map_reverb_preset
	{
		const char* name;
		std::array<float, 24> values;
	};

	void dump(StringTable* asset);
}
