#pragma once

namespace zonetool::s2::converter::h1::gfximage
{
	struct strip
	{
		std::size_t face;
		std::size_t y;
		std::size_t height;
	};

	zonetool::h1::IMG_TS convert_semantic(std::uint8_t semantic);
	void dump(zonetool::s2::GfxImage* asset);
	std::string dump_sky_cube(zonetool::s2::GfxImage* asset, float rotation_cos, float rotation_sin, int rotation_degrees);
	std::optional<std::string> find_sky_cube(const std::string& name);
	bool is_dumped(const std::string& name);
	bool dump_used(const std::string& name);
	std::optional<std::string> dump_native_used(const std::string& name);
	bool is_srgb_image(const std::string& name);
}
