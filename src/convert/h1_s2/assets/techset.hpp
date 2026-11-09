#pragma once

namespace zonetool::h1::converter::s2::techset
{
	struct native_techset
	{
		zonetool::s2::MaterialTechniqueSet* techset;
		std::array<std::int32_t, zonetool::s2::technique_count> sources;
		std::array<std::int32_t, zonetool::s2::technique_count> light_sources;
		std::array<std::int32_t, zonetool::s2::technique_count> state_sources;
	};

	const native_techset* convert_native(const std::string& h1_techset, bool sprite = false);

	void dump(zonetool::s2::MaterialTechniqueSet* asset);
	void finish();
	void clear();
}
