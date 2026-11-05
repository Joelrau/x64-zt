#pragma once

namespace zonetool::s2::converter::h1::comworld
{
	void dump(zonetool::s2::ComWorld* asset);
	std::uint16_t get_light_env(std::vector<std::uint16_t> lights);
	void finish();
}
