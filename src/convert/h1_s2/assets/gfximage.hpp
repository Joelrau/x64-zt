#pragma once

namespace zonetool::h1::converter::s2::gfximage
{
	void dump(zonetool::h1::GfxImage* asset);
	std::string dump_solid(const std::string& name, std::uint8_t semantic, const std::array<std::uint8_t, 4>& color);
	std::optional<std::string> dump_sky(zonetool::h1::GfxImage* asset);
	bool is_dumped(const std::string& name);
}
