#pragma once

namespace zonetool::h1::converter::s2::material
{
	std::uint8_t convert_sort_key(std::uint8_t sort_key);

	void dump(zonetool::h1::Material* asset);
	void dump_minimap(const std::string& minimap);
	void finish();
}
