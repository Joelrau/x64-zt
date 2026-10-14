#pragma once

namespace zonetool::s2::converter::h1::material
{
	void dump(zonetool::s2::Material* asset);
	void finish();
	std::optional<std::uint8_t> find_camera_region(const std::string& name);
}
