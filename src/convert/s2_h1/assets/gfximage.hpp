#pragma once

namespace zonetool::s2::converter::h1::gfximage
{
	void dump(zonetool::s2::GfxImage* asset);
	std::string dump_sky_cube(zonetool::s2::GfxImage* asset, float rotation_cos, float rotation_sin, int rotation_degrees);
	std::optional<std::string> find_sky_cube(const std::string& name);
	bool is_dumped(const std::string& name);
}
