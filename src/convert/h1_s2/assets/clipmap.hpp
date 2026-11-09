#pragma once

namespace zonetool::h1::converter::s2::clipmap
{
	struct vertex_range
	{
		std::uint32_t lowest;
		std::uint32_t highest;
	};

	std::optional<unsigned int> care_package_model(const std::string& map_name);
	zonetool::s2::Bounds care_package_bounds(float padding);
	float care_package_radius();

	void dump(clipMap_t* asset);
	void finish();
}
