#pragma once

namespace zonetool::h1::converter::s2::material
{
	struct native_material
	{
		zonetool::s2::Material* material;
		zonetool::s2::material_state* states;
	};

	struct minimap_variant
	{
		const char* suffix;
		const char* techset;
		std::uint8_t sort_key;
		std::uint32_t blend;
		const char* alpha_map;
	};

	std::uint8_t convert_sort_key(std::uint8_t sort_key);

	void dump(zonetool::h1::Material* asset);
	bool dump_used(const char* name);
	const char* dump_sprite(const zonetool::h1::Material* asset);
	void dump_minimap(const std::string& minimap);

	void finish();
}
